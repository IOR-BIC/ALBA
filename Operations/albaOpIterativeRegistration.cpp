/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpIterativeRegistration
 Authors: Stefano Perticoni - porting Daniele Giunchi
 
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
#include <albaGUIBusyInfo.h>

#include "albaOpIterativeRegistration.h"
#include "albaGUILandmark.h"


#include "albaEvent.h" 

#include "albaGUI.h" 
#include "albaVME.h"
#include "albaVMELandmarkCloud.h" 
//#include "mflMatrixPipeDirectCinematic.h"
#include "albaAbsMatrixPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaVMELandmark.h"
#include "albaTransform.h"

#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"

 
enum 
{
  SOURCE = 0,
  TARGET,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpIterativeRegistration);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpIterativeRegistration::albaOpIterativeRegistration(wxString label) 
: albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

  m_SourceVME = NULL;
  m_TargetVME = NULL;
 
  m_CurrentTime = -1; 

  m_SourceVmeName = "";
  m_TargetVmeName = "";

  m_SourceVmeName = "";
  m_TargetVmeName = "";

  m_UndoSourceAbsPose = NULL;
  m_RegistrationMatrix = NULL;
}
//----------------------------------------------------------------------------
albaOpIterativeRegistration::~albaOpIterativeRegistration( ) 
//----------------------------------------------------------------------------
{
  vtkDEL(m_UndoSourceAbsPose);  
  vtkDEL(m_RegistrationMatrix);
}
//----------------------------------------------------------------------------
albaOp* albaOpIterativeRegistration::Copy()
//----------------------------------------------------------------------------
{
  return (new albaOpIterativeRegistration(m_Label));
}
//----------------------------------------------------------------------------
bool albaOpIterativeRegistration::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  if (!node) return false;
  return (node->IsA("albaVMEVolumeGray") || node->IsA("albaVMESurface"));
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum 
{
  ID_CHOOSE_TARGET = MINID,
  ID_REGISTER, 
};
//----------------------------------------------------------------------------
void albaOpIterativeRegistration::OpRun()   
//----------------------------------------------------------------------------
{
  // progress bar stuff
  wxString progress_string("creating gui...");
  albaGUIBusyInfo wait(progress_string.ToAscii(),m_TestMode);

  m_UndoSourceAbsPose = vtkMatrix4x4::New();
  m_RegistrationMatrix = vtkMatrix4x4::New();

  m_SourceVME = m_Input;
  m_RegistrationMatrix->DeepCopy(m_SourceVME->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());

  // store source abs pose for undo
  m_UndoSourceAbsPose->DeepCopy(m_SourceVME->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());   

  assert(m_Input);
//  m_CurrentTime = m_Input->GetCurrentTime();
  m_CurrentTime = m_Input->GetTimeStamp();
  
  
  m_SourceVmeName = m_Input->GetName();  
  CreateGui();
}
//----------------------------------------------------------------------------
void albaOpIterativeRegistration::OpDo()
//----------------------------------------------------------------------------
{
  //m_SourceVME->SetAbsPose(m_RegistrationMatrix, m_CurrentTime);
  double pos[3], rot[3];
  albaTransform::GetPosition(m_RegistrationMatrix,pos);
  albaTransform::GetOrientation(m_RegistrationMatrix,rot);
  m_SourceVME->SetAbsPose(pos,rot ,m_CurrentTime);    
}
//----------------------------------------------------------------------------
void albaOpIterativeRegistration::OpUndo()
//----------------------------------------------------------------------------
{
  double pos[3], rot[3];
  albaTransform::GetPosition(m_UndoSourceAbsPose,pos);
  albaTransform::GetOrientation(m_UndoSourceAbsPose,rot);
  if (m_SourceVME) m_SourceVME->SetAbsPose(pos,rot, m_CurrentTime);  
}
//----------------------------------------------------------------------------
void albaOpIterativeRegistration::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // perform different actions depending on sender
  // process events: 
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    if (e->GetSender() == this->m_Gui) // from this operation gui
    {
      OnEventThis(e);  
    }
    else if (e->GetSender() == m_GuiLandmark[SOURCE]) // from source gui
    {
      OnEventGuiSource(e);
    }
    else if (e->GetSender() == m_GuiLandmark[TARGET]) // from target gui
    {
      OnEventGuiTarget(e);
    } 
    else
    {
      // if no one can handle this event send it to the operation listener
      albaEventMacro(*e); 
    }	
  }
  
}

//----------------------------------------------------------------------------
void albaOpIterativeRegistration::OpStop(int result)
//----------------------------------------------------------------------------
{   
  // progress bar stuff
  wxString progress_string("destroying gui...");
  albaGUIBusyInfo wait(progress_string.ToAscii(),m_TestMode);

  cppDEL(m_GuiLandmark[SOURCE]);
  cppDEL(m_GuiLandmark[TARGET]);
   
  // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
  // error when operation tab is selected after the operation has ended
  /*albaEventMacro(albaEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  cppDEL(m_Gui);*/
  HideGui();


  albaEventMacro(albaEvent(this,result));   
}

//----------------------------------------------------------------------------
void albaOpIterativeRegistration::CreateGui() 
{
   // create Gui
  m_Gui = new albaGUI(this);
  m_Gui->SetListener(this);
 
  m_Gui->Label("Source vme:", true);
  m_Gui->Label(&m_SourceVmeName, false, true);
   
  m_Gui->Divider(2); 
   
  m_Gui->Label("Target vme:", true);
  m_Gui->Label(&m_TargetVmeName, false, true); 
	m_Gui->Button(ID_CHOOSE_TARGET, "Choose target", "", "Choose registration target vme");
	m_Gui->Divider(2);

	m_Gui->HintBox(NULL, "Left btn: pick landmark\nMiddle btn: translate landmark\nMiddle btn+ctrl: translate and snap","Mouse interaction");

  m_Gui->Button(ID_REGISTER,"Register","","Perform rigid registration");
  m_Gui->Enable(ID_REGISTER, false);
	m_Gui->Divider(2);

  // source gui
  m_Gui->Label("Source landmark:", true);
  m_GuiLandmark[SOURCE] = new albaGUILandmark(m_SourceVME, this);
  m_GuiLandmark[SOURCE]->SetLMCloudName("source_lc");
  m_Gui->AddGui(m_GuiLandmark[SOURCE]->GetGui());

  // target gui
  m_Gui->Divider(2);
  m_Gui->Label("Target landmark:", true);
  m_GuiLandmark[TARGET] = new albaGUILandmark(m_TargetVME, this);
  m_GuiLandmark[TARGET]->SetLMCloudName("target_lc");
  m_Gui->AddGui(m_GuiLandmark[TARGET]->GetGui());

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}

void albaOpIterativeRegistration::OnRegister(albaEventBase *alba_event)
{ 
  Register();

  m_Gui->Enable(ID_REGISTER,false); 
  m_GuiLandmark[SOURCE]->SetGUIStatusToPick();
  m_GuiLandmark[TARGET]->SpawnLMOn();
  m_GuiLandmark[TARGET]->SetGUIStatusToPick();    
	GetLogicManager()->CameraUpdate();
}

void albaOpIterativeRegistration::OnEventThis(albaEventBase *alba_event)
{ 
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
  {
    case ID_CHOOSE_TARGET:
    {
      albaString title = _("Choose target vme");
      albaEvent e(this,VME_CHOOSE,&title);
			e.SetPointer(&albaGUILandmark::VmeAccept);
      albaEventMacro(e); 
      m_TargetVME = e.GetVme();
      if (m_TargetVME)
      {
        m_GuiLandmark[TARGET]->SetInputVME(m_TargetVME);
        m_GuiLandmark[TARGET]->SetGUIStatusToPick();
        m_Gui->Enable(ID_CHOOSE_TARGET, false);
        m_TargetVmeName = m_TargetVME->GetName();
        m_Gui->Update();
      } 
      
    }
    break;

    case ID_REGISTER:
    {
      OnRegister(e);
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
      OpUndo();
      this->OpStop(OP_RUN_CANCEL);
      return;
    }
    break;

    default:
    {
      albaEventMacro(*e);
    } 
  } 
  }
}

void albaOpIterativeRegistration::OnEventGuiSource(albaEventBase *alba_event)
{
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
  {
    case VME_PICKED:
    {
      if (m_GuiLandmark[TARGET]->GetGuiStatus() == albaGUILandmark::ENABLED)
      {
        m_Gui->Enable(ID_REGISTER, true);
        assert(m_GuiLandmark[SOURCE]->GetLandmarkCLoud()->GetNumberOfLandmarks() ==
        m_GuiLandmark[TARGET]->GetLandmarkCLoud()->GetNumberOfLandmarks());
        m_Gui->Update();
      }
    }
    break; 
    default:
    {
      albaEventMacro(*e);
    } 
  }
  }
}

void albaOpIterativeRegistration::OnEventGuiTarget(albaEventBase *alba_event)
{
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
  {
    case VME_PICKED:
    {
      if (m_GuiLandmark[SOURCE]->GetGuiStatus() == albaGUILandmark::ENABLED)
      {
        m_Gui->Enable(ID_REGISTER, true);
        assert(m_GuiLandmark[SOURCE]->GetLandmarkCLoud()->GetNumberOfLandmarks() ==
        m_GuiLandmark[TARGET]->GetLandmarkCLoud()->GetNumberOfLandmarks());
        m_Gui->Update();
      }
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
int albaOpIterativeRegistration::Register() 
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> sourcePoints; 
  vtkALBASmartPointer<vtkPoints> targetPoints; 

	sourcePoints->Reset();
	targetPoints->Reset();
  
  albaVMELandmarkCloud *sourceLC = m_GuiLandmark[SOURCE]->GetLandmarkCLoud();
  albaVMELandmarkCloud *targetLC = m_GuiLandmark[TARGET]->GetLandmarkCLoud();
    
  sourceLC->Update();
  targetLC->Update();
 
	int npSource = sourceLC->GetNumberOfLandmarks();
	int npTarget = targetLC->GetNumberOfLandmarks();

	if (npSource != npTarget) return -1;
  int i;
 
	for(i=0;i<npSource;i++)
	{ 	 
		{
      double sourcePos[3]; 
      albaVMELandmark::SafeDownCast(sourceLC->GetChild(i))->GetPoint(sourcePos); //was GetPosition
			sourcePoints->InsertNextPoint(sourcePos);

      
      double targetPos[3]; 
      albaVMELandmark::SafeDownCast(targetLC->GetChild(i))->GetPoint(targetPos); //was GetPosition
			targetPoints->InsertNextPoint(targetPos);
 
		} 
  }    
  
 	vtkALBASmartPointer<vtkLandmarkTransform> RegisterTransform; 
	RegisterTransform->SetSourceLandmarks(sourcePoints);	
	RegisterTransform->SetTargetLandmarks(targetPoints);	
  RegisterTransform->SetModeToRigidBody();
  RegisterTransform->Update();

  vtkMatrix4x4 *t_matrix;
  vtkNEW(t_matrix);
	//postmultiply the registration matrix by the abs matrix of the target to position the
	//registered  at the correct position in the space
  albaMatrix mat;
  targetLC->GetOutput()->GetAbsMatrix(mat,m_CurrentTime);
 
	vtkMatrix4x4::Multiply4x4(mat.GetVTKMatrix(),RegisterTransform->GetMatrix(),t_matrix); 

  m_SourceVME->SetTimeStamp(m_CurrentTime); //was SetCurrentTime

  double pos[3], rot[3];
  albaTransform::GetPosition(t_matrix,pos);
  albaTransform::GetOrientation(t_matrix,rot);
  m_SourceVME->SetAbsPose(pos,rot, m_CurrentTime);   
  //opdo
  m_RegistrationMatrix->DeepCopy(t_matrix);
  vtkDEL(t_matrix);

  return 0;
  }

 