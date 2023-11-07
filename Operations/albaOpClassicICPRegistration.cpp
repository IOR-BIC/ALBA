/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpClassicICPRegistration
 Authors: Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
 
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
#include <wx/wxprec.h>

#include "albaDecl.h"
#include "albaOp.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "vtkALBASmartPointer.h"

#include "albaOpClassicICPRegistration.h"

#include "albaVME.h"
#include "albaVMELandmarkCloud.h"

#include "albaAbsMatrixPipe.h"
#include "albaVMEItem.h"
#include "albaVMESurface.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "albaVMERoot.h"
#include "albaClassicICPRegistration.h"
#include "vtkTransformPolyDataFilter.h"
#include "albaVMEIterator.h"

//----------------------------------------------------------------------------
albaOpClassicICPRegistration::albaOpClassicICPRegistration(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;

	m_Target						= NULL; 
	m_Registered				= NULL; 

	m_Convergence				= 0.0001;

	m_ReportFilename		= "";	
	m_InputName					= "";
	m_TargetName				= _("none");
	m_CopySubTree = false;
}
//----------------------------------------------------------------------------
albaOpClassicICPRegistration::~albaOpClassicICPRegistration( ) 
//----------------------------------------------------------------------------
{
	albaDEL(m_Registered);
}
//----------------------------------------------------------------------------
albaOp* albaOpClassicICPRegistration::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpClassicICPRegistration(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpClassicICPRegistration::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{
	if(!vme) return false;
	if( vme->IsA("albaVMESurface") || vme->IsA("albaVMESurfaceParametric")) return true;
	if( vme->IsA("albaVMELandmarkCloud") && ((albaVMELandmarkCloud *)vme)->IsRigid() ) return true;
  return false;
};
//----------------------------------------------------------------------------
// wodget id's
//----------------------------------------------------------------------------
enum 
{
	ID_CHOOSE = MINID,
	ID_CONVERGENCE,
	ID_FILE,
};
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::CreateGui()
//----------------------------------------------------------------------------
{
	wxString wildcard = _("Report log (*.log)|*.log");
	wxString dir = albaGetLastUserFolder();
	if(!wxDirExists(dir)) dir = "";
	m_ReportFilename = dir + _("report.log");
	
	m_InputName = m_Input->GetName();
	
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Label(_("Source:"),true);
	m_Gui->Label(&m_InputName);
	m_Gui->Label(_("Target:"),true);
	m_Gui->Label(&m_TargetName);
	m_Gui->Button(ID_CHOOSE,_("Choose target"));
	m_Gui->Bool(-1, "Copy subtree", &m_CopySubTree,true);
	
	m_Gui->Divider(1);
	m_Gui->Double(ID_CONVERGENCE, _("Conv.step"), &m_Convergence, 1.0e-20, 1.0e+20, 10);
	m_Gui->FileSave(ID_FILE,_("Report log"),&m_ReportFilename,wildcard);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK,false);
}
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::OpRun()   
//----------------------------------------------------------------------------
{
	CreateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
		switch(e->GetId())
		{
			case ID_CHOOSE:
				OnChooseTarget();
			break;
			case wxOK:
				OpStop(OP_RUN_OK);           
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);        
			break;
			
			default:
				albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));       
}
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::OpDo()
//----------------------------------------------------------------------------
{
	wxBusyCursor *busyCursor = NULL;
  if (!m_TestMode)
  {
		busyCursor = new wxBusyCursor();
  }
	
  assert( m_Target);
	assert(!m_Registered);

	m_Input->GetOutput()->Update();
  

	albaSmartPointer<albaMatrix> icp_matrix;
	albaSmartPointer<albaMatrix> final_matrix;

	vtkALBASmartPointer<vtkTransform> inputTra,targetTra;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> inputTraFilter,targetTraFilter;
	
	albaMatrix *inputMatr = m_Input->GetOutput()->GetAbsMatrix();
	inputTra->SetMatrix(inputMatr->GetVTKMatrix());
	inputTraFilter->SetInput((vtkPolyData *)m_Input->GetOutput()->GetVTKData());
	inputTraFilter->SetTransform(inputTra);
	inputTraFilter->Update();
	
	albaMatrix *targetMatr = m_Target->GetOutput()->GetAbsMatrix();
	targetTra->SetMatrix(targetMatr->GetVTKMatrix());
	targetTraFilter->SetInput((vtkPolyData *)m_Target->GetOutput()->GetVTKData());
	targetTraFilter->SetTransform(targetTra);
	targetTraFilter->Update();

	

	vtkALBASmartPointer<albaClassicICPRegistration> icp; //to be deleted 
	//albaProgressMacro(icp,"classic ICP - registering");
	icp->SetConvergence(m_Convergence);
	icp->SetSource(inputTraFilter->GetOutput());
	icp->SetTarget(targetTraFilter->GetOutput());
	icp->SetResultsFileName(m_ReportFilename.GetCStr());
	icp->SaveResultsOn();
	icp->Update();
	vtkALBASmartPointer<vtkMatrix4x4> appo_matrix;
	icp->GetMatrix(appo_matrix);
	icp_matrix->SetVTKMatrix(appo_matrix);
   //modified by Stefano 7-11-2004
  double error = icp->GetRegistrationError();

	albaMatrix::Multiply4x4(*icp_matrix, *inputMatr, *final_matrix);

  wxString name = albaString::Format(_("%s registered on %s"),m_Input->GetName(), m_Target->GetName());

	if (m_CopySubTree)
	{
		m_Registered = albaVME::CopyTree(m_Input, m_Input->GetParent());
	}
	else
	{
		m_Registered = m_Input->NewInstance();
		m_Registered->Register(this);
		m_Registered->DeepCopy(m_Input); //not to be deleted, - delete it in the Undo or in destructor
		m_Registered->GetOutput()->Update();
	}
  m_Registered->SetName(name);
	m_Registered->ReparentTo(m_Input->GetParent());
	m_Registered->SetAbsMatrix(*final_matrix);

	albaVMEIterator *iter = m_Registered->NewIterator();

	m_Output = m_Registered;

	GetLogicManager()->CameraUpdate();

  wxString regString;
  regString = _("Registration done!");
  regString << '\n';
  regString << _("ICP registration error: ");
  regString << error;
  if (!m_TestMode)
  {
    wxMessageBox(regString);
  }

	cppDEL(busyCursor);
}
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::OnChooseTarget()   
//----------------------------------------------------------------------------
{
	albaEvent e(this,VME_CHOOSE);
	albaEventMacro(e);
	albaVME *vme = e.GetVme();

	if(!vme) return; // the user choosed cancel - keep previous target
  if(!InternalAccept(vme)) // the user choosed ok     - check if it is a valid vme
	{
    wxString msg = _("target vme must be a non-empty LandmarkCloud or Surface\n please choose another vme \n");
		wxMessageBox(msg,_("incorrect vme type"),wxOK|wxICON_ERROR);
    m_Target				= NULL;
  	m_TargetName		= _("none");
		m_Gui->Enable(wxOK,false);
		m_Gui->Update();
		return;
	}
  SetTarget(vme);

	m_Gui->Enable(wxOK,true);
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaOpClassicICPRegistration::SetTarget(albaVME* node)   
//----------------------------------------------------------------------------
{
  m_Target			= node;
	m_TargetName = m_Target->GetName();
}
