/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMAFTransform.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:21:53 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmoMAFTransform.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGizmoScale.h"
#include "mafGuiTransformMouse.h"
#include "mafGuiSaveRestorePose.h"
#include "mafGuiTransformTextEntries.h"

#include "mmiGenericMouse.h"

#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum MAFTRANSFORM_WIDGET_ID
{
	ID_SHOW_GIZMO = MINID,
  ID_CHOOSE_GIZMO_COMBO,
  ID_ROT_SNAP,
  ID_RESET,
  ID_AUX_REF_SYS,
  ID_ENABLE_SCALING,
};
//----------------------------------------------------------------------------
mmoMAFTransform::mmoMAFTransform(wxString label) :
mmoTransformInterface(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  GizmoTranslate          = NULL;
  GizmoRotate             = NULL;
  GizmoScale              = NULL;
  GuiTransform            = NULL;
  GuiSaveRestorePose      = NULL;
  GuiTransformTextEntries = NULL;
}
//----------------------------------------------------------------------------
mmoMAFTransform::~mmoMAFTransform()
//----------------------------------------------------------------------------
{
  cppDEL(GizmoTranslate);
  cppDEL(GizmoRotate);
  cppDEL(GizmoScale);
  cppDEL(GuiTransform);
  cppDEL(GuiSaveRestorePose);
  cppDEL(GuiTransformTextEntries);
}
//----------------------------------------------------------------------------
bool mmoMAFTransform::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme!=NULL && vme->IsMAFType(mafVME) && !vme->IsA("mafVMERoot") && !vme->IsA("mafVMEExternalData"));
}
//----------------------------------------------------------------------------
mafOp* mmoMAFTransform::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoMAFTransform(m_Label);
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OpRun()
//----------------------------------------------------------------------------
{
  // progress bar stuff
  wxBusyInfo wait("creating gui...");

  assert(m_Input);
  ((mafVME *)m_Input)->GetOutput()->GetVTKData()->Update();
  CurrentTime = ((mafVME *)m_Input)->GetTimeStamp();

  NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
  OldAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();

  CreateGui();
  ShowGui();
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // perform different actions depending on sender
  // process events: 
  if (maf_event->GetSender() == this->m_Gui) // from this operation gui
  {
    OnEventThis(maf_event); 
    return;
  }
  else if (maf_event->GetSender() == GuiTransform) // from gui transform
  {
    OnEventGuiTransform(maf_event);
  }
  else if (maf_event->GetSender() == GizmoTranslate) // from translation gizmo
  {
    OnEventGizmoTranslate(maf_event);
  }
  else if (maf_event->GetSender() == GizmoRotate) // from rotation gizmo
  {
    OnEventGizmoRotate(maf_event);
  }
  else if (maf_event->GetSender() == GizmoScale) // from scaling gizmo
  {
    OnEventGizmoScale(maf_event);
  }
  else if (maf_event->GetSender() == this->GuiSaveRestorePose) // from save/restore gui
  {
    OnEventGuiSaveRestorePose(maf_event); 
  }
  else if (maf_event->GetSender() == this->GuiTransformTextEntries)
  {
    OnEventGuiTransformTextEntries(maf_event);
  }
  else
  {
    // if no one can handle this event send it to the operation listener
    mafEventMacro(*maf_event); 
  }	
}
	  
//----------------------------------------------------------------------------
void mmoMAFTransform::OpDo()
//----------------------------------------------------------------------------
{
  mmoTransformInterface::OpDo();
}
//----------------------------------------------------------------------------
void mmoMAFTransform::OpUndo()
//----------------------------------------------------------------------------
{  
	((mafVME *)m_Input)->SetAbsMatrix(OldAbsMatrix);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
}
//----------------------------------------------------------------------------
void mmoMAFTransform::OpStop(int result)
//----------------------------------------------------------------------------
{  
  // progress bar stuff
  wxBusyInfo wait("destroying gui...");

  GizmoTranslate->Show(false);
  cppDEL(GizmoTranslate);

  GizmoRotate->Show(false);
  cppDEL(GizmoRotate);

  GizmoScale->Show(false);
  cppDEL(GizmoScale);

  GuiTransform->DetachInteractorFromVme();

  // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
  // error when operation tab is selected after the operation has ended
  mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  mafEventMacro(mafEvent(this,result));  
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_SHOW_GIZMO:
    {
      // update gizmo choose gui
      m_Gui->Enable(ID_CHOOSE_GIZMO_COMBO, UseGizmo ? true : false);
     
      if (UseGizmo == 0)
      {
        GizmoRotate->Show(false);
        GizmoTranslate->Show(false);
        GizmoScale->Show(false);
        GuiTransform->SetRefSys(RefSysVME);
        GuiTransform->EnableWidgets(true);
      }
      else if (UseGizmo == 1)
      {
        GuiTransform->EnableWidgets(false);

        if (ActiveGizmo == TR_GIZMO)
        {
          GizmoRotate->Show(false);
          GizmoScale->Show(false);
          GizmoTranslate->SetRefSys(RefSysVME);
          GizmoTranslate->Show(true);
        }
        else if (ActiveGizmo == ROT_GIZMO)
        {
          GizmoTranslate->Show(false);
          GizmoScale->Show(false);
          GizmoRotate->SetRefSys(RefSysVME);
          GizmoRotate->Show(true);
        }
        else if (ActiveGizmo == SCAL_GIZMO)
        {
          GizmoTranslate->Show(false);
          GizmoRotate->Show(false);
          GizmoScale->SetRefSys(RefSysVME);
          GizmoScale->Show(true);
        }
      }
    }
    break;
    
    case ID_CHOOSE_GIZMO_COMBO:
    {
      if (ActiveGizmo == TR_GIZMO)
      {
        GizmoRotate->Show(false);
        GizmoScale->Show(false);
        GizmoTranslate->SetRefSys(RefSysVME);
        GizmoTranslate->Show(true);
      }
      else if (ActiveGizmo == ROT_GIZMO)
      {
        GizmoTranslate->Show(false);
        GizmoScale->Show(false);
        GizmoRotate->SetRefSys(RefSysVME);
        GizmoRotate->Show(true);
      }
      else if (ActiveGizmo == SCAL_GIZMO)
      {
        GizmoTranslate->Show(false);
        GizmoRotate->Show(false);
        GizmoScale->SetRefSys(RefSysVME);
        GizmoScale->Show(true);
      }

    }
    break;

    case ID_RESET:
  	{ 
      Reset();
	  }
    break;

    // move this to opgui; both gizmos and gui should know ref sys
    case ID_AUX_REF_SYS:
    {
      mafString s;
      s << "Choose VME ref sys";
			mafEvent e(this,VME_CHOOSE, &s);
			mafEventMacro(e);
      SetRefSysVME(mafVME::SafeDownCast(e.GetVme()));
    }
    break;

    case wxOK:
    {
			this->OpStop(OP_RUN_OK);
      return;
    }
    break;

		case wxCANCEL:
    {
			Reset();
			this->OpStop(OP_RUN_CANCEL);
      return;
    }
    break;

    default:
    {
      mafEventMacro(*maf_event);
    }
    break;
  }
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventGizmoTranslate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{    
      // post multipliying matrixes coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(maf_event);
      
      // update gui 
      GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
	  }
    break;
  
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventGizmoRotate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      
      // post multipliying matrixes coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(maf_event);

      // update gui 
      GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
	  }
    break;

    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventGizmoScale(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
      // update gui 
      GuiTransformTextEntries->SetAbsPose(&NewAbsMatrix);
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	  }
    break;

    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}


//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventGuiTransform(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from mafGuiTransformMouse
    {
      PostMultiplyEventMatrix(maf_event);

      // update gizmos positions
      if (GizmoTranslate) GizmoTranslate->SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());
      if (GizmoRotate) GizmoRotate->SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());
      if (GizmoScale) GizmoScale->SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());

      GuiTransform->SetRefSys(RefSysVME);      
      GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}             

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventGuiSaveRestorePose(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from GuiSaveRestorePose
    {
      // update gizmos positions
      GizmoTranslate->SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());
      GizmoRotate->SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());
      GizmoScale->SetAbsPose(RefSysVME->GetOutput()->GetAbsMatrix());
      GuiTransform->SetRefSys(RefSysVME);

      // update gui 
      GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
      
      NewAbsMatrix = *(RefSysVME->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}


//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventGuiTransformTextEntries(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSFORM: // from GuiTransformTextEntries
      {
        mafMatrix absPose;
        absPose = *(e->GetMatrix());
        absPose.SetTimeStamp(CurrentTime);

        // update gizmos positions if refsys is local
        if (RefSysVME == mafVME::SafeDownCast(m_Input))
        {      
          GizmoTranslate->SetAbsPose(&absPose);
          GizmoRotate->SetAbsPose(&absPose);
          GizmoScale->SetAbsPose(&absPose);
          GuiTransform->SetRefSys(RefSysVME);
        }

        NewAbsMatrix = absPose;
      }
      break;
      default:
      {
        mafEventMacro(*e);
      }
    }
  }
}

//----------------------------------------------------------------------------
void mmoMAFTransform::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  
  // enable/disable gizmo interaction
  m_Gui->Label("interaction modality", true);
  wxString interactionModality[2] = {"mouse", "gizmo"};
  m_Gui->Combo(ID_SHOW_GIZMO,"",&UseGizmo,2,interactionModality);

  m_Gui->Divider(2);
  m_Gui->Label("gizmo interaction", true);
  m_Gui->Label("left mouse: interact through gizmo");

  // choose active gizmo
  wxString available_gizmos[3] = {"translate", "rotate", "scale"};
  m_Gui->Combo(ID_CHOOSE_GIZMO_COMBO, "", &ActiveGizmo, 3, available_gizmos);
  m_Gui->Enable(ID_CHOOSE_GIZMO_COMBO, UseGizmo?true:false);

  //---------------------------------
  // Transform gui
  //---------------------------------
  // create the tranform gui
  GuiTransform = new mafGuiTransformMouse(mafVME::SafeDownCast(m_Input), this);
  GuiTransform->EnableWidgets(true);

  // add transform gui to operation
  m_Gui->AddGui(GuiTransform->GetGui());

  //---------------------------------
  // Text transform gui
  //---------------------------------
  // create the tranform gui
  GuiTransformTextEntries = new mafGuiTransformTextEntries(mafVME::SafeDownCast(m_Input), this);
  GuiTransformTextEntries->EnableWidgets(true);

  // add transform gui to operation
  m_Gui->AddGui(GuiTransformTextEntries->GetGui());

  //---------------------------------
  // Translation Gizmo gui
  //---------------------------------
	
  // create the gizmos
  GizmoTranslate = new mafGizmoTranslate(mafVME::SafeDownCast(m_Input), this);
  GizmoTranslate->Show(false);

  // add translation gizmo gui to operation
  m_Gui->AddGui(GizmoTranslate->GetGui());
  
  //---------------------------------
  // Rotation Gizmo gui
  //---------------------------------
  GizmoRotate = new mafGizmoRotate(mafVME::SafeDownCast(m_Input), this);
  GizmoRotate->Show(false);

  // add rotation gizmo gui to operation
  m_Gui->AddGui(GizmoRotate->GetGui());
  
  //---------------------------------
  // Scaling Gizmo gui
  //---------------------------------
  GizmoScale = new mafGizmoScale(mafVME::SafeDownCast(m_Input), this);
  GizmoScale->Show(false);

  // add scaling gizmo gui to operation
  m_Gui->AddGui(GizmoScale->GetGui());

  //---------------------------------
  // Store/Restore position gui
  //---------------------------------
  GuiSaveRestorePose = new mafGuiSaveRestorePose(mafVME::SafeDownCast(m_Input), this);
  GuiSaveRestorePose->EnableWidgets(true);
  
  // add gui to operation
  m_Gui->AddGui(GuiSaveRestorePose->GetGui());

  //--------------------------------- 
  m_Gui->Divider(2);

  m_Gui->Label("auxiliary ref sys", true);
	m_Gui->Button(ID_AUX_REF_SYS,"choose");
	if(this->RefSysVME == NULL)
  {
    RefSysVME = mafVME::SafeDownCast(m_Input);
    RefSysVMEName = m_Input->GetName();
  }
  m_Gui->Label("refsys name: ",&RefSysVMEName);

  m_Gui->Divider(2);
  m_Gui->Label("preferences", true);
  m_Gui->Label("scale handling when done");
  wxString scaleHandling[2] = {"discard", "apply to data"};
  m_Gui->Combo(ID_SHOW_GIZMO,"",&EnableScaling,2,scaleHandling);


  m_Gui->Divider(2);
  m_Gui->Button(ID_RESET,"reset","","Cancel the transformation.");

	m_Gui->OkCancel(); 
  m_Gui->Label("");
  //--------------------------------- 

  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mmoMAFTransform::Reset()
//----------------------------------------------------------------------------
{
  ((mafVME *)m_Input)->SetAbsMatrix(OldAbsMatrix);  
  GuiTransformTextEntries->Reset();
  SetRefSysVME(mafVME::SafeDownCast(m_Input)); 
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mmoMAFTransform::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  // plugged components set their refsys;
  /*
  this should cycle on all plugged components => improve in order to use base class
  SetRefSys on mafGuiTransformInterface pointer
  */

  // change isa refsys
  GuiTransform->SetRefSys(RefSysVME);
  // change gtranslate refsys
  GizmoTranslate->SetRefSys(RefSysVME);
  // change grotate refsys
  GizmoRotate->SetRefSys(RefSysVME);
  // change grotate refsys
  GizmoScale->SetRefSys(RefSysVME);  
  // change gui transform text entries refsys
  GuiTransformTextEntries->SetRefSys(RefSysVME);

  RefSysVMEName = RefSysVME->GetName();
  assert(m_Gui);
  m_Gui->Update();
}
