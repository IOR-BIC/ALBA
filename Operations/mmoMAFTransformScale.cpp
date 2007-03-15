/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMAFTransformScale.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi
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


#include "mmoMAFTransformScale.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mmgGui.h"
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
enum MAF_TRANSFORM_ID
{
	ID_SHOW_GIZMO = MINID,
  ID_RESET,
//  ID_AUX_REF_SYS,
  ID_ENABLE_SCALING,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoMAFTransformScale);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoMAFTransformScale::mmoMAFTransformScale(const wxString &label) :
mmoTransformInterface(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  m_GizmoScale              = NULL;
//  m_GuiTransform            = NULL;
  m_GuiSaveRestorePose      = NULL;
//  m_GuiTransformTextEntries = NULL;
}
//----------------------------------------------------------------------------
mmoMAFTransformScale::~mmoMAFTransformScale()
//----------------------------------------------------------------------------
{
  cppDEL(m_GizmoScale);
//  cppDEL(m_GuiTransform);
  cppDEL(m_GuiSaveRestorePose);
//  cppDEL(m_GuiTransformTextEntries);
}
//----------------------------------------------------------------------------
bool mmoMAFTransformScale::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme!=NULL && vme->IsMAFType(mafVME) && !vme->IsA("mafVMERoot") && !vme->IsA("mafVMEExternalData"));
}
//----------------------------------------------------------------------------
mafOp* mmoMAFTransformScale::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoMAFTransformScale(m_Label);
}

//----------------------------------------------------------------------------
void mmoMAFTransformScale::OpRun()
//----------------------------------------------------------------------------
{
  // progress bar stuff
  wxBusyInfo wait("creating gui...");

  assert(m_Input);
  m_CurrentTime = ((mafVME *)m_Input)->GetTimeStamp();

  m_NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
  m_OldAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();

  CreateGui();
  ShowGui();
}

//----------------------------------------------------------------------------
void mmoMAFTransformScale::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // perform different actions depending on sender
  // process events: 
  if (maf_event->GetSender() == this->m_Gui) // from this operation gui
  {
    OnEventThis(maf_event); 
    return;
  }
  /*else if (maf_event->GetSender() == m_GuiTransform) // from gui transform
  {
    OnEventGuiTransform(maf_event);
  }*/
  else if (maf_event->GetSender() == m_GizmoScale) // from scaling gizmo
  {
    OnEventGizmoScale(maf_event);
  }
  else if (maf_event->GetSender() == this->m_GuiSaveRestorePose) // from save/restore gui
  {
    OnEventGuiSaveRestorePose(maf_event); 
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
/*  else if (maf_event->GetSender() == this->m_GuiTransformTextEntries)
  {
    OnEventGuiTransformTextEntries(maf_event);
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }*/
  else
  {
    // if no one can handle this event send it to the operation listener
    mafEventMacro(*maf_event); 
  }	
}
	  
//----------------------------------------------------------------------------
void mmoMAFTransformScale::OpDo()
//----------------------------------------------------------------------------
{
  mmoTransformInterface::OpDo();
}
//----------------------------------------------------------------------------
void mmoMAFTransformScale::OpUndo()
//----------------------------------------------------------------------------
{  
	((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
}
//----------------------------------------------------------------------------
void mmoMAFTransformScale::OpStop(int result)
//----------------------------------------------------------------------------
{  
  // progress bar stuff
  wxBusyInfo wait("destroying gui...");

  m_GizmoScale->Show(false);
  cppDEL(m_GizmoScale);

//  m_GuiTransform->DetachInteractorFromVme();

  // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
  // error when operation tab is selected after the operation has ended
  mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  mafEventMacro(mafEvent(this,result));  
}

//----------------------------------------------------------------------------
void mmoMAFTransformScale::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_RESET:
  	{ 
      Reset();
	  }
    break;
    // move this to opgui; both gizmos and gui should know ref sys
    /*case ID_AUX_REF_SYS:
    {
      mafString s;
      s << "Choose VME ref sys";
			mafEvent e(this,VME_CHOOSE, &s);
			mafEventMacro(e);
      SetRefSysVME(mafVME::SafeDownCast(e.GetVme()));
    }
    break;*/

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
void mmoMAFTransformScale::OnEventGizmoScale(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      m_NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
      // update gui 
//      m_GuiTransformTextEntries->SetAbsPose(&m_NewAbsMatrix);
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
void mmoMAFTransformScale::OnEventGuiTransform(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from mafGuiTransformMouse
    {
      PostMultiplyEventMatrix(maf_event);

      // update gizmos positions
      if (m_GizmoScale) m_GizmoScale->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

//      m_GuiTransform->SetRefSys(m_RefSysVME);      
//      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}             

//----------------------------------------------------------------------------
void mmoMAFTransformScale::OnEventGuiSaveRestorePose(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from m_GuiSaveRestorePose
    {
      // update gizmos positions
      //m_GizmoScale->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
//      m_GuiTransform->SetRefSys(m_RefSysVME);

      // update gui 
//      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
      
      m_NewAbsMatrix = *(m_RefSysVME->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}


//----------------------------------------------------------------------------
void mmoMAFTransformScale::OnEventGuiTransformTextEntries(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSFORM: // from m_GuiTransformTextEntries
      {
        mafMatrix absPose;
        absPose = *(e->GetMatrix());
        absPose.SetTimeStamp(m_CurrentTime);

        // update gizmos positions if refsys is local
        if (m_RefSysVME == mafVME::SafeDownCast(m_Input))
        {      
          m_GizmoScale->SetAbsPose(&absPose);
//          m_GuiTransform->SetRefSys(m_RefSysVME);
        }

        m_NewAbsMatrix = absPose;
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
void mmoMAFTransformScale::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);

  m_Gui->Divider(2);
  m_Gui->Label("gizmo interaction", true);
  m_Gui->Label("left mouse: interact through gizmo");

  //---------------------------------
  // Transform Gui
  //---------------------------------
  // create the transform Gui
//  m_GuiTransform = new mafGuiTransformMouse(mafVME::SafeDownCast(m_Input), this);

  // add transform gui to operation
  //m_Gui->AddGui(m_GuiTransform->GetGui());

  //---------------------------------
  // Text transform Gui
  //---------------------------------
  // create the transform Gui
  //m_GuiTransformTextEntries = new mafGuiTransformTextEntries(mafVME::SafeDownCast(m_Input), this);

  // add transform Gui to operation
  //m_Gui->AddGui(m_GuiTransformTextEntries->GetGui());

  //---------------------------------
  // Scaling Gizmo Gui
  //---------------------------------
  m_GizmoScale = new mafGizmoScale(mafVME::SafeDownCast(m_Input), this);
  m_GizmoScale->Show(false);

  // add scaling gizmo gui to operation
  m_Gui->AddGui(m_GizmoScale->GetGui());

  //---------------------------------
  // Store/Restore position Gui
  //---------------------------------
  m_GuiSaveRestorePose = new mafGuiSaveRestorePose(mafVME::SafeDownCast(m_Input), this);
  
  // add Gui to operation
  m_Gui->AddGui(m_GuiSaveRestorePose->GetGui());

  //--------------------------------- 
  //m_Gui->Divider(2);

  //m_Gui->Label("auxiliary ref sys", true);
	//m_Gui->Button(ID_AUX_REF_SYS,"choose");
	if(this->m_RefSysVME == NULL)
  {
    SetRefSysVME(mafVME::SafeDownCast(m_Input));
    m_RefSysVMEName = m_Input->GetName();
  }
  m_Gui->Label("refsys name: ",&m_RefSysVMEName);

  m_Gui->Divider(2);
  m_Gui->Label("preferences", true);
  m_Gui->Label("scale handling when done");
  wxString scaleHandling[2] = {"discard", "apply to data"};
  m_Gui->Combo(ID_SHOW_GIZMO,"",&m_EnableScaling,2,scaleHandling);


  m_Gui->Divider(2);
  m_Gui->Button(ID_RESET,"reset","","Cancel the transformation.");

	m_Gui->OkCancel(); 
  m_Gui->Label("");
  //--------------------------------- 

  m_Gui->Update();

	m_GizmoScale->SetRefSys(m_RefSysVME);
	m_GizmoScale->Show(true);


	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mmoMAFTransformScale::Reset()
//----------------------------------------------------------------------------
{
  ((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);  
//  m_GuiTransformTextEntries->Reset();
  SetRefSysVME(mafVME::SafeDownCast(m_Input)); 
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mmoMAFTransformScale::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  // plugged components set their refsys;
  /*
  this should cycle on all plugged components => improve in order to use base class
  SetRefSys on mafGuiTransformInterface pointer
  */

  // change isa refsys
//  m_GuiTransform->SetRefSys(m_RefSysVME);
  
  // change gscale refsys
  m_GizmoScale->SetRefSys(m_RefSysVME);  
  // change gui transform text entries refsys
//  m_GuiTransformTextEntries->SetRefSys(m_RefSysVME);

  m_RefSysVMEName = m_RefSysVME->GetName();
  assert(m_Gui);
  m_Gui->Update();
}
