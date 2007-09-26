/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMAFTransform.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-26 09:50:57 $
  Version:   $Revision: 1.21 $
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
#include "mmgGui.h"
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
enum MAF_TRANSFORM_ID
{
	ID_SHOW_GIZMO = MINID,
  ID_CHOOSE_GIZMO_COMBO,
  ID_ROTATION_STEP,
  ID_TRANSLATION_STEP,
  ID_ENABLE_STEP,
  ID_ROT_SNAP,
  ID_RESET,
  ID_AUX_REF_SYS,
  ID_ENABLE_SCALING,
  ID_ROLLOUT_TEXT_ENTRIES,
  ID_ROLLOUT_GIZMO_TRANSLATE,
  ID_ROLLOUT_GIZMO_ROTATE,
  ID_ROLLOUT_GIZMO_SCALE,
  ID_ROLLOUT_SAVE_POS
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoMAFTransform);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoMAFTransform::mmoMAFTransform(const wxString &label) :
mmoTransformInterface(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  m_RotationStep    = 10.0;
  m_TranslationStep = 2.0;
  m_EnableStep      = 0;

  m_GizmoTranslate          = NULL;
  m_GizmoRotate             = NULL;
  m_GizmoScale              = NULL;
  m_GuiTransform            = NULL;
  m_GuiSaveRestorePose      = NULL;
  m_GuiTransformTextEntries = NULL;
}
//----------------------------------------------------------------------------
mmoMAFTransform::~mmoMAFTransform()
//----------------------------------------------------------------------------
{
  cppDEL(m_GizmoTranslate);
  cppDEL(m_GizmoRotate);
  cppDEL(m_GizmoScale);
  cppDEL(m_GuiTransform);
  cppDEL(m_GuiSaveRestorePose);
  cppDEL(m_GuiTransformTextEntries);
}
//----------------------------------------------------------------------------
bool mmoMAFTransform::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	mafEvent e(this,VIEW_SELECTED);
	mafEventMacro(e);
	return (vme!=NULL && vme->IsMAFType(mafVME) && !vme->IsA("mafVMERoot") && !vme->IsA("mafVMEExternalData") /*&& e.GetBool()*/);
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
  m_CurrentTime = ((mafVME *)m_Input)->GetTimeStamp();

  m_NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
  m_OldAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();

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
  else if (maf_event->GetSender() == m_GuiTransform) // from gui transform
  {
    OnEventGuiTransform(maf_event);
  }
  else if (maf_event->GetSender() == m_GizmoTranslate) // from translation gizmo
  {
    OnEventGizmoTranslate(maf_event);
  }
  else if (maf_event->GetSender() == m_GizmoRotate) // from rotation gizmo
  {
    OnEventGizmoRotate(maf_event);
  }
  else if (maf_event->GetSender() == m_GizmoScale) // from scaling gizmo
  {
    OnEventGizmoScale(maf_event);
  }
  else if (maf_event->GetSender() == this->m_GuiSaveRestorePose) // from save/restore gui
  {
    OnEventGuiSaveRestorePose(maf_event); 
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else if (maf_event->GetSender() == this->m_GuiTransformTextEntries)
  {
    OnEventGuiTransformTextEntries(maf_event);
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
	((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);
  ((mafVME *)m_Input)->GetOutput()->Update();

  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
}
//----------------------------------------------------------------------------
void mmoMAFTransform::OpStop(int result)
//----------------------------------------------------------------------------
{  
  // progress bar stuff
  wxBusyInfo wait("destroying gui...");

  m_GizmoTranslate->Show(false);
  cppDEL(m_GizmoTranslate);

  m_GizmoRotate->Show(false);
  cppDEL(m_GizmoRotate);

  m_GizmoScale->Show(false);
  cppDEL(m_GizmoScale);

  m_GuiTransform->DetachInteractorFromVme();

  // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
  // error when operation tab is selected after the operation has ended
  mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  mafEventMacro(mafEvent(this,result));  
}

//----------------------------------------------------------------------------
void mmoMAFTransform::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEvent e(this,VIEW_SELECTED);
  mafEventMacro(e);

  switch(maf_event->GetId())
	{
    case ID_SHOW_GIZMO:
    {  
      // update gizmo choose gui
      m_Gui->Enable(ID_CHOOSE_GIZMO_COMBO, m_UseGizmo ? true : false);
      m_Gui->Enable(ID_ROTATION_STEP, m_UseGizmo?true:false);
      m_Gui->Enable(ID_TRANSLATION_STEP, m_UseGizmo?true:false);
      m_Gui->Enable(ID_ENABLE_STEP, m_UseGizmo?true:false);
      
      if (m_UseGizmo == 0 && e.GetBool())
      {
        m_GizmoRotate->Show(false);
        m_GizmoTranslate->Show(false);
        m_GizmoScale->Show(false);
        m_GuiTransform->SetRefSys(m_RefSysVME);
        m_GuiTransform->EnableWidgets(true);
      }
      else if (m_UseGizmo == 1)
      {
        m_GuiTransform->EnableWidgets(false);

        if (m_ActiveGizmo == TR_GIZMO)
        {
          m_GizmoRotate->Show(false);
          m_GizmoScale->Show(false);
          m_GizmoTranslate->SetRefSys(m_RefSysVME);
          m_GizmoTranslate->Show(true && e.GetBool());
        }
        else if (m_ActiveGizmo == ROT_GIZMO)
        {
          m_GizmoTranslate->Show(false);
          m_GizmoScale->Show(false);
          m_GizmoRotate->SetRefSys(m_RefSysVME);
          m_GizmoRotate->Show(true && e.GetBool());
        }
        else if (m_ActiveGizmo == SCAL_GIZMO)
        {
          m_GizmoTranslate->Show(false);
          m_GizmoRotate->Show(false);
          m_GizmoScale->SetRefSys(m_RefSysVME);
          m_GizmoScale->Show(true && e.GetBool());
        }
      }
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));
    }
    break;
    
    case ID_CHOOSE_GIZMO_COMBO:
    {
      if (m_ActiveGizmo == TR_GIZMO)
      {
        m_GizmoRotate->Show(false);
        m_GizmoScale->Show(false);
        m_GizmoTranslate->SetRefSys(m_RefSysVME);
        m_GizmoTranslate->Show(true && e.GetBool());
      }
      else if (m_ActiveGizmo == ROT_GIZMO)
      {
        m_GizmoTranslate->Show(false);
        m_GizmoScale->Show(false);
        m_GizmoRotate->SetRefSys(m_RefSysVME);
        m_GizmoRotate->Show(true && e.GetBool());
      }
      else if (m_ActiveGizmo == SCAL_GIZMO)
      {
        m_GizmoTranslate->Show(false);
        m_GizmoRotate->Show(false);
        m_GizmoScale->SetRefSys(m_RefSysVME);
        m_GizmoScale->Show(true && e.GetBool());
      }
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));
    }
    break;

    case ID_RESET:
  	{ 
      Reset();
	  }
    break;
    case ID_ROTATION_STEP:
      m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetStep(0,m_RotationStep);
      m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetStep(1,m_RotationStep);
      m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetStep(2,m_RotationStep);
    break;
    case ID_TRANSLATION_STEP:
      m_GizmoTranslate->SetStep(0,m_TranslationStep);
      m_GizmoTranslate->SetStep(1,m_TranslationStep);
      m_GizmoTranslate->SetStep(2,m_TranslationStep);
    break;
    case ID_ENABLE_STEP:
      if (m_EnableStep != 0)
      {
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetConstraintModality(0,mmiConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetConstraintModality(1,mmiConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetConstraintModality(2,mmiConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetStep(0,m_RotationStep);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetStep(1,m_RotationStep);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetStep(2,m_RotationStep);
        m_GizmoTranslate->SetConstraintModality(0,mmiConstraint::SNAP_STEP);
        m_GizmoTranslate->SetConstraintModality(1,mmiConstraint::SNAP_STEP);
        m_GizmoTranslate->SetConstraintModality(2,mmiConstraint::SNAP_STEP);
        m_GizmoTranslate->SetStep(0,m_TranslationStep);
        m_GizmoTranslate->SetStep(1,m_TranslationStep);
        m_GizmoTranslate->SetStep(2,m_TranslationStep);
      }
      else
      {
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetConstraintModality(0,mmiConstraint::FREE);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetConstraintModality(1,mmiConstraint::FREE);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetConstraintModality(2,mmiConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(0,mmiConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(1,mmiConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(2,mmiConstraint::FREE);
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
      // post multiplying matrixes coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(maf_event);
      
      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
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
      
      // post multiplying matrix coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(maf_event);

      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
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
      m_NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(&m_NewAbsMatrix);
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
      if (m_GizmoTranslate) m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GizmoRotate) m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GizmoScale) m_GizmoScale->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

      m_GuiTransform->SetRefSys(m_RefSysVME);      
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
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
    case ID_TRANSFORM: // from m_GuiSaveRestorePose
    {
      // update gizmos positions
      m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GizmoScale->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GuiTransform->SetRefSys(m_RefSysVME);

      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
      
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
void mmoMAFTransform::OnEventGuiTransformTextEntries(mafEventBase *maf_event)
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
          m_GizmoTranslate->SetAbsPose(&absPose);
          m_GizmoRotate->SetAbsPose(&absPose);
          m_GizmoScale->SetAbsPose(&absPose);
          m_GuiTransform->SetRefSys(m_RefSysVME);
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
void mmoMAFTransform::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  
  // enable/disable gizmo interaction
  m_Gui->Label("interaction modality", true);
  wxString interactionModality[2] = {"mouse", "gizmo"};
  m_Gui->Combo(ID_SHOW_GIZMO,"",&m_UseGizmo,2,interactionModality);

  m_Gui->Divider(2);
  m_Gui->Label("gizmo interaction", true);
  m_Gui->Label("left mouse: interact through gizmo");

  // choose active gizmo
  wxString available_gizmos[3] = {"translate", "rotate", "scale"};
  m_Gui->Combo(ID_CHOOSE_GIZMO_COMBO, "", &m_ActiveGizmo, 3, available_gizmos);
  m_Gui->Divider(2);
  m_Gui->Label("step parameters:",true);
  m_Gui->Double(ID_TRANSLATION_STEP,"translation",&m_TranslationStep,0.01);
  m_Gui->Double(ID_ROTATION_STEP,"rotation",&m_RotationStep,0.01);
  m_Gui->Bool(ID_ENABLE_STEP,"on/off",&m_EnableStep);
  m_Gui->Enable(ID_CHOOSE_GIZMO_COMBO, m_UseGizmo?true:false);
  m_Gui->Enable(ID_ROTATION_STEP, m_UseGizmo?true:false);
  m_Gui->Enable(ID_TRANSLATION_STEP, m_UseGizmo?true:false);
  m_Gui->Enable(ID_ENABLE_STEP, m_UseGizmo?true:false);

  //---------------------------------
  // Transform Gui
  //---------------------------------
  // create the transform Gui
  m_GuiTransform = new mafGuiTransformMouse(mafVME::SafeDownCast(m_Input), this);

  // add transform gui to operation
  m_Gui->AddGui(m_GuiTransform->GetGui());

  //---------------------------------
  // Text transform Gui
  //---------------------------------
  // create the transform Gui
  m_GuiTransformTextEntries = new mafGuiTransformTextEntries(mafVME::SafeDownCast(m_Input), this);

  // add transform Gui to operation
  //m_Gui->AddGui(m_GuiTransformTextEntries->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_TEXT_ENTRIES," Text entries", m_GuiTransformTextEntries->GetGui(), false);

  //---------------------------------
  // Translation Gizmo Gui
  //---------------------------------
	
  // create the gizmos
  m_GizmoTranslate = new mafGizmoTranslate(mafVME::SafeDownCast(m_Input), this);
  m_GizmoTranslate->Show(false);

  // add translation gizmo Gui to operation
  //m_Gui->AddGui(m_GizmoTranslate->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_TRANSLATE," Gizmo translate", m_GizmoTranslate->GetGui(), false);
  
  //---------------------------------
  // Rotation Gizmo Gui
  //---------------------------------
  m_GizmoRotate = new mafGizmoRotate(mafVME::SafeDownCast(m_Input), this);
  m_GizmoRotate->Show(false);

  // add rotation gizmo Gui to operation
  //m_Gui->AddGui(m_GizmoRotate->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_ROTATE," Gizmo rotate", m_GizmoRotate->GetGui(), false);
  
  //---------------------------------
  // Scaling Gizmo Gui
  //---------------------------------
  m_GizmoScale = new mafGizmoScale(mafVME::SafeDownCast(m_Input), this);
  m_GizmoScale->Show(false);

  // add scaling gizmo gui to operation
  //m_Gui->AddGui(m_GizmoScale->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_SCALE," Gizmo scale", m_GizmoScale->GetGui(), false);

  //---------------------------------
  // Store/Restore position Gui
  //---------------------------------
  m_GuiSaveRestorePose = new mafGuiSaveRestorePose(mafVME::SafeDownCast(m_Input), this);
  
  // add Gui to operation
  //m_Gui->AddGui(m_GuiSaveRestorePose->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_SAVE_POS," Save pose", m_GuiSaveRestorePose->GetGui(), false);

  //--------------------------------- 
  m_Gui->Divider(2);

  m_Gui->Label("auxiliary ref sys", true);
	m_Gui->Button(ID_AUX_REF_SYS,"choose");
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
}

//----------------------------------------------------------------------------
void mmoMAFTransform::Reset()
//----------------------------------------------------------------------------
{
  ((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);  
  m_GuiTransformTextEntries->Reset();
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
  m_GuiTransform->SetRefSys(m_RefSysVME);
  // change gtranslate refsys
  m_GizmoTranslate->SetRefSys(m_RefSysVME);
  // change grotate refsys
  m_GizmoRotate->SetRefSys(m_RefSysVME);
  // change grotate refsys
  m_GizmoScale->SetRefSys(m_RefSysVME);  
  // change gui transform text entries refsys
  m_GuiTransformTextEntries->SetRefSys(m_RefSysVME);

  m_RefSysVMEName = m_RefSysVME->GetName();
  assert(m_Gui);
  m_Gui->Update();
}
