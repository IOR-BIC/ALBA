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
#include "albaBehaviorPicker.h"
#include "mmaMaterial.h"
#include "vtkProperty.h"
#include <vector>


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

 

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpIterativeRegistration);
//----------------------------------------------------------------------------

// sides of the registration
enum REGISTRATION_SIDE
{
  SOURCE = 0,
  TARGET,
};

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

  for (int side = SOURCE; side <= TARGET; side++)
  {
    m_Picker[side] = NULL;
    m_OldBehavior[side] = NULL;
    m_Cloud[side] = NULL;
    m_Landmark[side] = NULL;
  }
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
    else if (e->GetId() == VME_PICKED && e->GetSender() == m_Picker[SOURCE])
    {
      OnVmePicked(SOURCE, e);
    }
    else if (e->GetId() == VME_PICKED && e->GetSender() == m_Picker[TARGET])
    {
      OnVmePicked(TARGET, e);
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

  Clear();
   
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

  m_Gui->Button(ID_REGISTER,"Register","","Perform rigid registration");
  m_Gui->Enable(ID_REGISTER, false);
  // a left click on the source and on the target VME places their landmarks
  AttachPicker(SOURCE, m_SourceVME);

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

  // the next picks create a new pair of landmarks
  m_Landmark[SOURCE] = NULL;
  m_Landmark[TARGET] = NULL;
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
			e.SetPointer(&albaOpIterativeRegistration::VmeAccept);
      albaEventMacro(e); 
      m_TargetVME = e.GetVme();
      if (m_TargetVME)
      {
        AttachPicker(TARGET, m_TargetVME);
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

//----------------------------------------------------------------------------
void albaOpIterativeRegistration::AttachPicker(int side, albaVME *vme)
//----------------------------------------------------------------------------
{
  albaNEW(m_Picker[side]);
  m_Picker[side]->SetListener(this);
  m_OldBehavior[side] = vme->GetBehavior();
  vme->SetBehavior(m_Picker[side]);
}
//----------------------------------------------------------------------------
void albaOpIterativeRegistration::OnVmePicked(int side, albaEvent *e)
//----------------------------------------------------------------------------
{
  albaVME *vme = side == SOURCE ? m_SourceVME : m_TargetVME;

  // new landmark abs position from event
  double absPosition[3] = {0, 0, 0};
  ((vtkPoints *)e->GetVtkObj())->GetPoint(0, absPosition);

  if (m_Cloud[side] == NULL)
  {
    albaNEW(m_Cloud[side]);
    m_Cloud[side]->SetName(side == SOURCE ? "source_lc" : "target_lc");

    double b[6];
    vme->GetOutput()->GetVTKData()->GetBounds(b);
    double maxBound = MAX(MAX(fabs(b[1] - b[0]), fabs(b[3] - b[2])), fabs(b[5] - b[4]));
    m_Cloud[side]->SetRadius(maxBound / 60);

    // the landmarks are half transparent and the picking goes through them, on the VME
    m_Cloud[side]->GetMaterial()->m_Opacity = 0.5;
    m_Cloud[side]->GetMaterial()->m_Prop->SetOpacity(0.5);
    m_Cloud[side]->SetPickable(false);

    m_Cloud[side]->ReparentTo(vme);
    GetLogicManager()->VmeAdd(m_Cloud[side]);
    GetLogicManager()->VmeShow(m_Cloud[side], true);
  }

  if (m_Landmark[side] == NULL)
  {
    wxString name("lm_");
    name << m_Cloud[side]->GetNumberOfLandmarks();
    albaNEW(m_Landmark[side]);
    m_Landmark[side]->SetName(name.ToAscii());
    m_Landmark[side]->ReparentTo(m_Cloud[side]);
    m_Landmark[side]->Update();
    m_Landmark[side]->SetAbsPose(absPosition[0], absPosition[1], absPosition[2], 0, 0, 0);
    GetLogicManager()->VmeAdd(m_Landmark[side]);
    m_Landmark[side]->Delete(); // the cloud keeps it
  }
  else
    m_Landmark[side]->SetAbsPose(absPosition[0], absPosition[1], absPosition[2], 0, 0, 0);

  GetLogicManager()->VmeShow(m_Landmark[side], true);

  // a pair of landmarks can be registered
  if (m_Landmark[SOURCE] && m_Landmark[TARGET])
  {
    m_Gui->Enable(ID_REGISTER, true);
    m_Gui->Update();
  }
  GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpIterativeRegistration::Clear()
//----------------------------------------------------------------------------
{
  albaVME *vmes[2] = {m_SourceVME, m_TargetVME};
  for (int side = SOURCE; side <= TARGET; side++)
  {
    if (m_Picker[side])
      vmes[side]->SetBehavior(m_OldBehavior[side]);
    albaDEL(m_Picker[side]);

    if (m_Cloud[side])
    {
      std::vector<albaVME *> landmarks;
      for (int i = 0; i < m_Cloud[side]->GetNumberOfChildren(); i++)
        landmarks.push_back(m_Cloud[side]->GetChild(i));
      for (int i = 0; i < landmarks.size(); i++)
      {
        GetLogicManager()->VmeShow(landmarks[i], false);
        GetLogicManager()->VmeRemove(landmarks[i]);
      }
      GetLogicManager()->VmeShow(m_Cloud[side], false);
      GetLogicManager()->VmeRemove(m_Cloud[side]);
      albaDEL(m_Cloud[side]);
    }
    m_Landmark[side] = NULL;
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
  
  albaVMELandmarkCloud *sourceLC = m_Cloud[SOURCE];
  albaVMELandmarkCloud *targetLC = m_Cloud[TARGET];
    
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

 