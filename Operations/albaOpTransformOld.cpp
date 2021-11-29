/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformOld
 Authors: Stefano Perticoni 
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaOpTransformOld.h"

#include <wx/busyinfo.h>

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGizmoScale.h"
#include "albaGUITransformMouse.h"
#include "albaGUISaveRestorePose.h"
#include "albaGUITransformTextEntries.h"

#include "albaInteractorGenericMouse.h"

#include "albaSmartPointer.h"
#include "albaTransform.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkTransform.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum ALBA_TRANSFORM_ID
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
albaCxxTypeMacro(albaOpTransformOld);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpTransformOld::albaOpTransformOld(const wxString &label) :
albaOpTransformInterface(label)
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
albaOpTransformOld::~albaOpTransformOld()
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
bool albaOpTransformOld::Accept(albaVME* vme)
//----------------------------------------------------------------------------
{
	albaEvent e(this,VIEW_SELECTED);
	albaEventMacro(e);
	return (vme!=NULL && vme->IsALBAType(albaVME) && !vme->IsA("albaVMERoot") && !vme->IsA("albaVMEExternalData") /*&& e.GetBool()*/);
}
//----------------------------------------------------------------------------
albaOp* albaOpTransformOld::Copy()   
//----------------------------------------------------------------------------
{
  return new albaOpTransformOld(m_Label);
}

//----------------------------------------------------------------------------
void albaOpTransformOld::OpRun()
//----------------------------------------------------------------------------
{
  // progress bar stuff
  if (!m_TestMode)
  {
  	wxBusyInfo wait("creating gui...");
  }

  assert(m_Input);
  m_CurrentTime = m_Input->GetTimeStamp();

  m_NewAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();
  m_OldAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();

  if (!m_TestMode)
  {
	  CreateGui();
	  ShowGui();
  }
}

//----------------------------------------------------------------------------
void albaOpTransformOld::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // perform different actions depending on sender
  // process events: 
  if (alba_event->GetSender() == this->m_Gui) // from this operation gui
  {
    OnEventThis(alba_event); 
    return;
  }
  else if (alba_event->GetSender() == m_GuiTransform) // from gui transform
  {
    OnEventGuiTransform(alba_event);
  }
  else if (alba_event->GetSender() == m_GizmoTranslate) // from translation gizmo
  {
    OnEventGizmoTranslate(alba_event);
  }
  else if (alba_event->GetSender() == m_GizmoRotate) // from rotation gizmo
  {
    OnEventGizmoRotate(alba_event);
  }
  else if (alba_event->GetSender() == m_GizmoScale) // from scaling gizmo
  {
    OnEventGizmoScale(alba_event);
  }
  else if (alba_event->GetSender() == this->m_GuiSaveRestorePose) // from save/restore gui
  {
    OnEventGuiSaveRestorePose(alba_event); 
		GetLogicManager()->CameraUpdate();
  }
  else if (alba_event->GetSender() == this->m_GuiTransformTextEntries)
  {
    OnEventGuiTransformTextEntries(alba_event);
		GetLogicManager()->CameraUpdate();
  }
  else
  {
    // if no one can handle this event send it to the operation listener
    albaEventMacro(*alba_event); 
  }	
}
	  
//----------------------------------------------------------------------------
void albaOpTransformOld::OpDo()
//----------------------------------------------------------------------------
{
  albaOpTransformInterface::OpDo();
}
//----------------------------------------------------------------------------
void albaOpTransformOld::OpUndo()
//----------------------------------------------------------------------------
{  
	m_Input->SetAbsMatrix(m_OldAbsMatrix);
  m_Input->GetOutput()->Update();

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpTransformOld::OpStop(int result)
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
  albaEventMacro(albaEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  albaEventMacro(albaEvent(this,result));  
}

//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventThis(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  albaEvent e(this,VIEW_SELECTED);
  albaEventMacro(e);

  switch(alba_event->GetId())
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
			GetLogicManager()->CameraUpdate();
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
			GetLogicManager()->CameraUpdate();
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
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetConstraintModality(0,albaInteractorConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetConstraintModality(1,albaInteractorConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetConstraintModality(2,albaInteractorConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetStep(0,m_RotationStep);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetStep(1,m_RotationStep);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetStep(2,m_RotationStep);
        m_GizmoTranslate->SetConstraintModality(0,albaInteractorConstraint::SNAP_STEP);
        m_GizmoTranslate->SetConstraintModality(1,albaInteractorConstraint::SNAP_STEP);
        m_GizmoTranslate->SetConstraintModality(2,albaInteractorConstraint::SNAP_STEP);
        m_GizmoTranslate->SetStep(0,m_TranslationStep);
        m_GizmoTranslate->SetStep(1,m_TranslationStep);
        m_GizmoTranslate->SetStep(2,m_TranslationStep);
      }
      else
      {
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetConstraintModality(0,albaInteractorConstraint::FREE);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetConstraintModality(1,albaInteractorConstraint::FREE);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetConstraintModality(2,albaInteractorConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(0,albaInteractorConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(1,albaInteractorConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(2,albaInteractorConstraint::FREE);
      }
    break;
    // move this to opgui; both gizmos and gui should know ref sys
    case ID_AUX_REF_SYS:
    {
      albaString s;
      s << "Choose VME ref sys";
			albaEvent e(this,VME_CHOOSE, &s);
			albaEventMacro(e);
      SetRefSysVME(e.GetVme());
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
      albaEventMacro(*alba_event);
    }
    break;
  }
}

//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventGizmoTranslate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM:
  	{    
      // post multiplying matrixes coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(alba_event);
      
      if (!m_TestMode)
      {
	      // update gui 
	      m_GuiTransformTextEntries->SetAbsPose(m_Input->GetOutput()->GetAbsMatrix());
      }
	  }
    break;
  
    default:
    {
      albaEventMacro(*alba_event);
    }
  }
}

//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventGizmoRotate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{ 
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      
      // post multiplying matrix coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(alba_event);

      if (!m_TestMode)
      {
	      // update gui 
	      m_GuiTransformTextEntries->SetAbsPose(m_Input->GetOutput()->GetAbsMatrix());
      }
	  }
    break;

    default:
    {
      albaEventMacro(*alba_event);
    }
  }
}

//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventGizmoScale(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{ 
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      m_NewAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();
      if (!m_TestMode)
      {
	      // update gui 
	      m_GuiTransformTextEntries->SetAbsPose(&m_NewAbsMatrix);
				GetLogicManager()->CameraUpdate();
      }
	  }
    break;

    default:
    {
      albaEventMacro(*alba_event);
    }
  }
}


//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventGuiTransform(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM: // from albaGUITransformMouse
    {
      PostMultiplyEventMatrix(alba_event);

      // update gizmos positions
      if (m_GizmoTranslate) m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GizmoRotate) m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GizmoScale) m_GizmoScale->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

      if (!m_TestMode)
      {
	      m_GuiTransform->SetRefSys(m_RefSysVME);      
	      m_GuiTransformTextEntries->SetAbsPose(m_Input->GetOutput()->GetAbsMatrix());
      }
    }
    break;
    default:
    {
      albaEventMacro(*alba_event);
    }
  }
}             

//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventGuiSaveRestorePose(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM: // from m_GuiSaveRestorePose
    {
      // update gizmos positions
      m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GizmoScale->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GuiTransform->SetRefSys(m_RefSysVME);

      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(m_Input->GetOutput()->GetAbsMatrix());
      
      m_NewAbsMatrix = *(m_RefSysVME->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      albaEventMacro(*alba_event);
    }
  }
}


//----------------------------------------------------------------------------
void albaOpTransformOld::OnEventGuiTransformTextEntries(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSFORM: // from m_GuiTransformTextEntries
      {
        albaMatrix absPose;
        absPose = *(e->GetMatrix());
        absPose.SetTimeStamp(m_CurrentTime);

        // update gizmos positions if refsys is local
        if (m_RefSysVME == m_Input)
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
        albaEventMacro(*e);
      }
    }
  }
}

//----------------------------------------------------------------------------
void albaOpTransformOld::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  
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
  m_GuiTransform = new albaGUITransformMouse(m_Input, this);

  // add transform gui to operation
  m_Gui->AddGui(m_GuiTransform->GetGui());

  //---------------------------------
  // Text transform Gui
  //---------------------------------
  // create the transform Gui
  m_GuiTransformTextEntries = new albaGUITransformTextEntries(m_Input, this);

  // add transform Gui to operation
  //m_Gui->AddGui(m_GuiTransformTextEntries->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_TEXT_ENTRIES," Text entries", m_GuiTransformTextEntries->GetGui(), false);

  //---------------------------------
  // Translation Gizmo Gui
  //---------------------------------
	
  // create the gizmos
  m_GizmoTranslate = new albaGizmoTranslate(m_Input, this);
  m_GizmoTranslate->Show(false);

  // add translation gizmo Gui to operation
  //m_Gui->AddGui(m_GizmoTranslate->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_TRANSLATE," Gizmo translate", m_GizmoTranslate->GetGui(), false);
  
  //---------------------------------
  // Rotation Gizmo Gui
  //---------------------------------
  m_GizmoRotate = new albaGizmoRotate(m_Input, this);
  m_GizmoRotate->Show(false);

  // add rotation gizmo Gui to operation
  //m_Gui->AddGui(m_GizmoRotate->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_ROTATE," Gizmo rotate", m_GizmoRotate->GetGui(), false);
  
  //---------------------------------
  // Scaling Gizmo Gui
  //---------------------------------
  m_GizmoScale = new albaGizmoScale(m_Input, this);
  m_GizmoScale->Show(false);

  // add scaling gizmo gui to operation
  //m_Gui->AddGui(m_GizmoScale->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_SCALE," Gizmo scale", m_GizmoScale->GetGui(), false);

  //---------------------------------
  // Store/Restore position Gui
  //---------------------------------
  m_GuiSaveRestorePose = new albaGUISaveRestorePose(m_Input, this);
  
  // add Gui to operation
  //m_Gui->AddGui(m_GuiSaveRestorePose->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_SAVE_POS," Save pose", m_GuiSaveRestorePose->GetGui(), false);

  //--------------------------------- 
  m_Gui->Divider(2);

  m_Gui->Label("auxiliary ref sys", true);
	m_Gui->Button(ID_AUX_REF_SYS,"choose");
	if(this->m_RefSysVME == NULL)
  {
    SetRefSysVME(m_Input);
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
void albaOpTransformOld::Reset()
//----------------------------------------------------------------------------
{
  m_Input->SetAbsMatrix(m_OldAbsMatrix);  
  if (!m_TestMode)
  {
  	m_GuiTransformTextEntries->Reset();
    SetRefSysVME(m_Input); 
  }
  m_NewAbsMatrix = m_OldAbsMatrix;
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpTransformOld::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  // plugged components set their refsys;
  /*
  this should cycle on all plugged components => improve in order to use base class
  SetRefSys on albaGUITransformInterface pointer
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

//----------------------------------------------------------------------------
char ** albaOpTransformOld::GetIcon()
{
#include "pic/MENU_OP_TRANSFORM.xpm"
	return MENU_OP_TRANSFORM_xpm;
}
