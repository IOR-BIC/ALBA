/*=========================================================================

 Program: MAF2
 Module: mafOpClassicICPRegistration
 Authors: Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/wxprec.h>

#include "mafDecl.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "vtkMAFSmartPointer.h"

#include "mafOpClassicICPRegistration.h"

#include "mafNode.h"
#include "mafVMELandmarkCloud.h"

#include "mafAbsMatrixPipe.h"
#include "mafVMEItem.h"
#include "mafVMESurface.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "mafVMERoot.h"
#include "mafClassicICPRegistration.h"

//----------------------------------------------------------------------------
mafOpClassicICPRegistration::mafOpClassicICPRegistration(wxString label) :
mafOp(label)
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
}
//----------------------------------------------------------------------------
mafOpClassicICPRegistration::~mafOpClassicICPRegistration( ) 
//----------------------------------------------------------------------------
{
	mafDEL(m_Registered);
}
//----------------------------------------------------------------------------
mafOp* mafOpClassicICPRegistration::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpClassicICPRegistration(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpClassicICPRegistration::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	if(!vme) return false;
	if( vme->IsA("mafVMESurface") || vme->IsA("mafVMESurfaceParametric")) return true;
	if( vme->IsA("mafVMELandmarkCloud") && ((mafVMELandmarkCloud *)vme)->IsRigid() ) return true;
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
	ID_HELP,
};
//----------------------------------------------------------------------------
void mafOpClassicICPRegistration::CreateGui()
//----------------------------------------------------------------------------
{
	wxString wildcard = _("Report log (*.log)|*.log");
	wxString dir = mafGetLastUserFolder().c_str();
	if(!wxDirExists(dir)) dir = "";
	m_ReportFilename = dir + _("report.log");
	
	m_InputName = m_Input->GetName();
	
	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	mafEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		m_Gui->Button(ID_HELP, "Help","");	
	}

	m_Gui->Label("");
	m_Gui->Label(_("source:"),true);
	m_Gui->Label(&m_InputName);
	m_Gui->Label("");
	m_Gui->Label(_("target:"),true);
	m_Gui->Label(&m_TargetName);
	m_Gui->Button(ID_CHOOSE,_("choose target"));
	m_Gui->Label("");
	m_Gui->Double(ID_CONVERGENCE,_("conv.step"),&m_Convergence,1.0e-20,1.0e+20,10);
	m_Gui->Label("");
	m_Gui->FileSave(ID_FILE,_("report log"),&m_ReportFilename,wildcard);
	m_Gui->Label("");
	m_Gui->OkCancel();
	m_Gui->Divider();

	m_Gui->Enable(wxOK,false);
}
//----------------------------------------------------------------------------
void mafOpClassicICPRegistration::OpRun()   
//----------------------------------------------------------------------------
{
	CreateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void mafOpClassicICPRegistration::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
			
			case ID_HELP:
				{
					mafEvent helpEvent;
					helpEvent.SetSender(this);
					mafString operationLabel = this->m_Label;
					helpEvent.SetString(&operationLabel);
					helpEvent.SetId(OPEN_HELP_PAGE);
					mafEventMacro(helpEvent);
				}
			break;

			default:
				mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void mafOpClassicICPRegistration::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));       
}
//----------------------------------------------------------------------------
void mafOpClassicICPRegistration::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
	  wxBusyCursor wait;
  }
	
  assert( m_Target);
	assert(!m_Registered);

	((mafVME*)m_Input)->GetOutput()->Update();
  

	mafSmartPointer<mafMatrix> icp_matrix;  
	mafSmartPointer<mafMatrix> final_matrix;
  mafSmartPointer<mafMatrix> target_matrix;
  m_Target->GetOutput()->GetAbsMatrix(*target_matrix,m_Target->GetTimeStamp());
	

	vtkMAFSmartPointer<mafClassicICPRegistration> icp; //to be deleted 
	//mafProgressMacro(icp,"classic ICP - registering");
	icp->SetConvergence(m_Convergence);
	icp->SetSource(((mafVME*)m_Input)->GetOutput()->GetVTKData());
	icp->SetTarget(m_Target->GetOutput()->GetVTKData());
	icp->SetResultsFileName(m_ReportFilename.GetCStr());
	icp->SaveResultsOn();
	icp->Update();
	vtkMAFSmartPointer<vtkMatrix4x4> appo_matrix;
	icp->GetMatrix(appo_matrix);
	icp_matrix->SetVTKMatrix(appo_matrix);
   //modified by Stefano 7-11-2004
  double error = icp->GetRegistrationError();

	target_matrix->Multiply4x4(*target_matrix, *icp_matrix, *final_matrix);

  wxString name = wxString::Format(_("%s registered on %s"),m_Input->GetName(), m_Target->GetName());

  mafNEW(m_Registered);

  if(m_Input->IsMAFType(mafVMESurface))
   {	
     m_Registered->DeepCopy(m_Input); //not to be deleted, - delete it in the Undo or in destructor
     m_Registered->GetOutput()->Update();
   }
  else
   {
     m_Registered->SetData((vtkPolyData*)(((mafVME*)m_Input)->GetOutput()->GetVTKData()),0.0);
     m_Registered->Update();
   }
  m_Registered->SetName(name);
	m_Registered->ReparentTo(m_Target->GetParent());
	m_Registered->SetMatrix(*final_matrix);

	m_Output = m_Registered;

	m_Registered->ReparentTo(m_Input);

  mafVME *sourceVME = mafVME::SafeDownCast(m_Input);

  vtkMAFSmartPointer<vtkTransform> sourceABSPoseInverseTr;
  sourceABSPoseInverseTr->PostMultiply();
  sourceABSPoseInverseTr->SetMatrix((sourceVME->GetAbsMatrixPipe()->GetMatrix()).GetVTKMatrix());
  sourceABSPoseInverseTr->Inverse();
  sourceABSPoseInverseTr->PreMultiply();
  sourceABSPoseInverseTr->Concatenate((m_Registered->GetAbsMatrixPipe()->GetMatrix()).GetVTKMatrix());
  
  mafSmartPointer<mafMatrix> mat;
  mat->SetVTKMatrix(sourceABSPoseInverseTr->GetMatrix());

  m_Registered->SetAbsMatrix(*mat);
  
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

  // modified by Stefano 7-11-2004 (beg)
  // registration error feedback to user
  wxString regString;
  regString = _("Registration done!");
  regString << '\n';
  regString << _("ICP registration error: ");
  regString << error;
  if (!m_TestMode)
  {
    wxMessageBox(regString);
  }
}
//----------------------------------------------------------------------------
void mafOpClassicICPRegistration::OnChooseTarget()   
//----------------------------------------------------------------------------
{
	mafEvent e(this,VME_CHOOSE);
	mafEventMacro(e);
	mafNode *vme = e.GetVme();

	if(!vme) return; // the user choosed cancel - keep previous target
  if(!Accept(vme)) // the user choosed ok     - check if it is a valid vme
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
void mafOpClassicICPRegistration::SetTarget(mafNode* node)   
//----------------------------------------------------------------------------
{
  m_Target			= mafVME::SafeDownCast(node);
	m_TargetName = m_Target->GetName();
}
