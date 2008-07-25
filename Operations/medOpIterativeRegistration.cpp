/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpIterativeRegistration.cpp,v $
Language:  C++
Date:      $Date: 2008-07-25 11:12:22 $
Version:   $Revision: 1.3 $
Authors:   Stefano Perticoni - porting Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "mafDefines.h" 
#include "medDefines.h"

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/busyinfo.h>

#include "medOpIterativeRegistration.h"
#include "medGUILandmark.h"


#include "mafEvent.h" 

#include "mafGUI.h" 
#include "mafVME.h"
#include "mafVMELandmarkCloud.h" 
//#include "mflMatrixPipeDirectCinematic.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMELandmark.h"
#include "mafTransform.h"

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
mafCxxTypeMacro(medOpIterativeRegistration);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpIterativeRegistration::medOpIterativeRegistration(wxString label) 
: mafOp(label)
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
medOpIterativeRegistration::~medOpIterativeRegistration( ) 
//----------------------------------------------------------------------------
{
  vtkDEL(m_UndoSourceAbsPose);  
  vtkDEL(m_RegistrationMatrix);
}
//----------------------------------------------------------------------------
mafOp* medOpIterativeRegistration::Copy()
//----------------------------------------------------------------------------
{
  return (new medOpIterativeRegistration(m_Label));
}
//----------------------------------------------------------------------------
bool medOpIterativeRegistration::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  if (!node) return false;
  return (node->IsA("mafVMEVolumeGray") || node->IsA("mafVMESurface"));
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
void medOpIterativeRegistration::OpRun()   
//----------------------------------------------------------------------------
{
  // progress bar stuff
  wxString progress_string("creating gui...");
  wxBusyInfo wait(progress_string.c_str());

  m_UndoSourceAbsPose = vtkMatrix4x4::New();
  m_RegistrationMatrix = vtkMatrix4x4::New();

  m_SourceVME = mafVME::SafeDownCast(m_Input);
  m_RegistrationMatrix->DeepCopy(m_SourceVME->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());

  // store source abs pose for undo
  m_UndoSourceAbsPose->DeepCopy(m_SourceVME->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());   

  assert(m_Input);
//  m_CurrentTime = m_Input->GetCurrentTime();
  m_CurrentTime = mafVME::SafeDownCast(m_Input)->GetTimeStamp();
  
  
  m_SourceVmeName = m_Input->GetName();  
  CreateGui();
}
//----------------------------------------------------------------------------
void medOpIterativeRegistration::OpDo()
//----------------------------------------------------------------------------
{
  //m_SourceVME->SetAbsPose(m_RegistrationMatrix, m_CurrentTime);
  double pos[3], rot[3];
  mafTransform::GetPosition(m_RegistrationMatrix,pos);
  mafTransform::GetOrientation(m_RegistrationMatrix,rot);
  m_SourceVME->SetAbsPose(pos,rot ,m_CurrentTime);    
}
//----------------------------------------------------------------------------
void medOpIterativeRegistration::OpUndo()
//----------------------------------------------------------------------------
{
  double pos[3], rot[3];
  mafTransform::GetPosition(m_UndoSourceAbsPose,pos);
  mafTransform::GetOrientation(m_UndoSourceAbsPose,rot);
  if (m_SourceVME) m_SourceVME->SetAbsPose(pos,rot, m_CurrentTime);  
}
//----------------------------------------------------------------------------
void medOpIterativeRegistration::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // perform different actions depending on sender
  // process events: 
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
      mafEventMacro(*e); 
    }	
  }
  
}

//----------------------------------------------------------------------------
void medOpIterativeRegistration::OpStop(int result)
//----------------------------------------------------------------------------
{   
  // progress bar stuff
  wxString progress_string("destroying gui...");
  wxBusyInfo wait(progress_string.c_str());

  cppDEL(m_GuiLandmark[SOURCE]);
  cppDEL(m_GuiLandmark[TARGET]);
   
  // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
  // error when operation tab is selected after the operation has ended
  /*mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  cppDEL(m_Gui);*/
  HideGui();


  mafEventMacro(mafEvent(this,result));   
}


void medOpIterativeRegistration::CreateGui() 
{
   // create Gui
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
 
  m_Gui->Label("source vme:", true);
  m_Gui->Label(&m_SourceVmeName, false, true);
   

  m_Gui->Divider(2); 
  m_Gui->Button(ID_CHOOSE_TARGET,  "choose target", "", "choose registration target vme");
  
  
  m_Gui->Label("target vme:", true);

  m_Gui->Label(&m_TargetVmeName, false, true); 
  m_Gui->Divider(2); 
  m_Gui->Label("mouse interaction", true);
  m_Gui->Label("left mouse: pick landmark");
  m_Gui->Label("middle mouse: translate landmark");
  m_Gui->Label("middle mouse + ctrl: translate and snap");
  m_Gui->Divider(2);

  m_Gui->Button(ID_REGISTER,"register","","perform rigid registration");
  m_Gui->Enable(ID_REGISTER, false);
   
  // source gui
  m_Gui->Divider(2);
  m_Gui->Label("source landmark:", true);
  m_GuiLandmark[SOURCE] = new medGUILandmark(m_SourceVME, this);
  m_GuiLandmark[SOURCE]->SetLMCloudName("source_lc");
  m_Gui->AddGui(m_GuiLandmark[SOURCE]->GetGui());

  // target gui
  m_Gui->Divider(2);
  m_Gui->Label("target landmark:", true);
  m_GuiLandmark[TARGET] = new medGUILandmark(m_TargetVME, this);
  m_GuiLandmark[TARGET]->SetLMCloudName("target_lc");
  m_Gui->AddGui(m_GuiLandmark[TARGET]->GetGui());

  m_Gui->Divider(2);
  m_Gui->Divider();
  m_Gui->Divider();
  
	m_Gui->OkCancel(); 
  m_Gui->Label("");

  m_Gui->Update(); 

  ShowGui();
}

void medOpIterativeRegistration::OnRegister(mafEventBase *maf_event)
{ 
  Register();

  m_Gui->Enable(ID_REGISTER,false); 
  m_GuiLandmark[SOURCE]->SetGUIStatusToPick();
  m_GuiLandmark[TARGET]->SpawnLMOn();
  m_GuiLandmark[TARGET]->SetGUIStatusToPick();    
  mafEventMacro(mafEvent(this, CAMERA_UPDATE)); 
}

void medOpIterativeRegistration::OnEventThis(mafEventBase *maf_event)
{ 
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
  {
    case ID_CHOOSE_TARGET:
    {
      mafString title = _("Choose target vme");
      mafEvent e(this,VME_CHOOSE,&title,(long)&medGUILandmark::VmeAccept);
      mafEventMacro(e); 
      m_TargetVME = mafVME::SafeDownCast(e.GetVme());
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
      mafEventMacro(*e);
    } 
  } 
  }
}

void medOpIterativeRegistration::OnEventGuiSource(mafEventBase *maf_event)
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
  {
    case VME_PICKED:
    {
      if (m_GuiLandmark[TARGET]->GetGuiStatus() == medGUILandmark::ENABLED)
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
      mafEventMacro(*e);
    } 
  }
  }
}

void medOpIterativeRegistration::OnEventGuiTarget(mafEventBase *maf_event)
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
  {
    case VME_PICKED:
    {
      if (m_GuiLandmark[SOURCE]->GetGuiStatus() == medGUILandmark::ENABLED)
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
      mafEventMacro(*e);
    } 
  }

  }
}

//----------------------------------------------------------------------------
int medOpIterativeRegistration::Register() 
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> sourcePoints; 
  vtkMAFSmartPointer<vtkPoints> targetPoints; 

	sourcePoints->Reset();
	targetPoints->Reset();
  
  mafVMELandmarkCloud *sourceLC = m_GuiLandmark[SOURCE]->GetLandmarkCLoud();
  mafVMELandmarkCloud *targetLC = m_GuiLandmark[TARGET]->GetLandmarkCLoud();
    
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
      mafVMELandmark::SafeDownCast(sourceLC->GetChild(i))->GetPoint(sourcePos); //was GetPosition
			sourcePoints->InsertNextPoint(sourcePos);

      
      double targetPos[3]; 
      mafVMELandmark::SafeDownCast(targetLC->GetChild(i))->GetPoint(targetPos); //was GetPosition
			targetPoints->InsertNextPoint(targetPos);
 
		} 
  }    
  
 	vtkMAFSmartPointer<vtkLandmarkTransform> RegisterTransform; 
	RegisterTransform->SetSourceLandmarks(sourcePoints);	
	RegisterTransform->SetTargetLandmarks(targetPoints);	
  RegisterTransform->SetModeToRigidBody();
  RegisterTransform->Update();

  vtkMatrix4x4 *t_matrix;
  vtkNEW(t_matrix);
	//postmultiply the registration matrix by the abs matrix of the target to position the
	//registered  at the correct position in the space
  mafMatrix mat;
  targetLC->GetOutput()->GetAbsMatrix(mat,m_CurrentTime);
 
	vtkMatrix4x4::Multiply4x4(mat.GetVTKMatrix(),RegisterTransform->GetMatrix(),t_matrix); 

  m_SourceVME->SetTimeStamp(m_CurrentTime); //was SetCurrentTime

  double pos[3], rot[3];
  mafTransform::GetPosition(t_matrix,pos);
  mafTransform::GetOrientation(t_matrix,rot);
  m_SourceVME->SetAbsPose(pos,rot, m_CurrentTime);   
  //opdo
  m_RegistrationMatrix->DeepCopy(t_matrix);
  vtkDEL(t_matrix);

  return 0;
  }

 