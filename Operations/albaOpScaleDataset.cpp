/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScaleDataset
 Authors: Daniele Giunchi , Stefano Perticoni
 
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


#include "albaOpScaleDataset.h"

#include <albaGUIBusyInfo.h>

#include "albaDecl.h"
#include "albaGUI.h"
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
  ID_RESET,
  ID_ENABLE_SCALING,
  ID_AUX_REF_SYS,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpScaleDataset);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpScaleDataset::albaOpScaleDataset(const wxString &label) :
albaOpTransformInterface(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  m_GizmoScale              = NULL;
  m_GuiSaveRestorePose      = NULL;
}
//----------------------------------------------------------------------------
albaOpScaleDataset::~albaOpScaleDataset()
//----------------------------------------------------------------------------
{
  cppDEL(m_GizmoScale);
  cppDEL(m_GuiSaveRestorePose);
}
//----------------------------------------------------------------------------
bool albaOpScaleDataset::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{
	return (vme!=NULL && vme->IsALBAType(albaVME) && !vme->IsA("albaVMERoot") && !vme->IsA("albaVMEExternalData"));
}
//----------------------------------------------------------------------------
albaOp* albaOpScaleDataset::Copy()   
//----------------------------------------------------------------------------
{
  return new albaOpScaleDataset(m_Label);
}

//----------------------------------------------------------------------------
void albaOpScaleDataset::OpRun()
//----------------------------------------------------------------------------
{
  albaGUIBusyInfo wait("creating gui...",m_TestMode);
  
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
void albaOpScaleDataset::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (alba_event->GetSender() == this->m_Gui) // from this operation gui
  {
    OnEventThis(alba_event); 
    return;
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
  else
  {
    albaEventMacro(*alba_event); 
  }	
}
	  
//----------------------------------------------------------------------------
void albaOpScaleDataset::OpDo()
//----------------------------------------------------------------------------
{
  albaOpTransformInterface::OpDo();
}
//----------------------------------------------------------------------------
void albaOpScaleDataset::OpUndo()
//----------------------------------------------------------------------------
{  
	m_Input->SetAbsMatrix(m_OldAbsMatrix);
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpScaleDataset::OpStop(int result)
//----------------------------------------------------------------------------
{  
	albaGUIBusyInfo wait("destroying gui...", m_TestMode);

  m_GizmoScale->Show(false);
  cppDEL(m_GizmoScale);

  albaEventMacro(albaEvent(this,result));  
}

//----------------------------------------------------------------------------
void albaOpScaleDataset::OnEventThis(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
	{
    case ID_RESET:
  	{ 
      Reset();
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

	case ID_AUX_REF_SYS:
	{
		albaString s;
		s << "Choose VME ref sys";
		albaEvent e(this,VME_CHOOSE, &s);
		albaEventMacro(e);
		SetRefSysVME(e.GetVme());
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
void albaOpScaleDataset::OnEventGizmoScale(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{ 
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      m_NewAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();
			GetLogicManager()->CameraUpdate();
	  }
    break;

    default:
    {
      albaEventMacro(*alba_event);
    }
  }
}


//----------------------------------------------------------------------------
void albaOpScaleDataset::OnEventGuiSaveRestorePose(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
	{
    case ID_TRANSFORM: // from m_GuiSaveRestorePose
    {   
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
void albaOpScaleDataset::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);

	m_Gui->HintBox(NULL, "Left mouse: interact through gizmo", "Gizmo interaction");

  //---------------------------------
  // Scaling Gizmo Gui
  //---------------------------------
  m_GizmoScale = new albaGizmoScale(m_Input, this);
  m_GizmoScale->Show(false);

  // add scaling gizmo gui to operation
  m_Gui->AddGui(m_GizmoScale->GetGui());

  //---------------------------------
  // Store/Restore position Gui
  //---------------------------------
  m_GuiSaveRestorePose = new albaGUISaveRestorePose(m_Input, this);
  
  // add Gui to operation
  m_Gui->AddGui(m_GuiSaveRestorePose->GetGui());

  //--------------------------------- 
  m_Gui->Divider(2);
  m_Gui->Label("Auxiliary refsys", true);

	if (this->m_RefSysVME == NULL)
	{
		SetRefSysVME(m_Input);
		m_RefSysVMEName = m_Input->GetName();
	}
	m_Gui->Label(&m_RefSysVMEName);

	m_Gui->Button(ID_AUX_REF_SYS,"Choose");
  m_Gui->Button(ID_RESET,"Reset","","Cancel the transformation.");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

  //--------------------------------- 

  m_Gui->Update();

	m_GizmoScale->SetRefSys(m_RefSysVME);
	m_GizmoScale->Show(true);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpScaleDataset::Reset()
//----------------------------------------------------------------------------
{
  m_Input->SetAbsMatrix(m_OldAbsMatrix);  
  SetRefSysVME(m_Input); 
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpScaleDataset::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  // plugged components set their refsys;
  /*
  this should cycle on all plugged components => improve in order to use base class
  SetRefSys on albaGUITransformInterface pointer
  */
  
  if (!m_TestMode)
  {
	  // change gscale refsys
	  m_GizmoScale->SetRefSys(m_RefSysVME);  
  }

  m_RefSysVMEName = m_RefSysVME->GetName();
  if (!m_TestMode)
  {
	  assert(m_Gui);
	  m_Gui->Update();
  }
}
