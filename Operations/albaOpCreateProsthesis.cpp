/*=========================================================================
Program:   AssemblerPro
Module:    albaOpCreateProsthesis.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpCreateProsthesis.h"
#include "albaDecl.h"
#include "albaVMEProsthesis.h"

#include "albaAbsLogicManager.h"
#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaServiceClient.h"
#include "albaVME.h"
#include "albaVMEProsthesis.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateProsthesis);

//----------------------------------------------------------------------------
albaOpCreateProsthesis::albaOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_ProsthesesDBManager = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateProsthesis::~albaOpCreateProsthesis()
{
}

//----------------------------------------------------------------------------
bool albaOpCreateProsthesis::InternalAccept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
char** albaOpCreateProsthesis::GetIcon()
{
#include "pic/MENU_OP_CREATE_PROSTHESIS.xpm"
	return MENU_OP_CREATE_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* albaOpCreateProsthesis::Copy()
{
	albaOpCreateProsthesis *cp = new albaOpCreateProsthesis(m_Label);
	return cp;
}

//----------------------------------------------------------------------------
void albaOpCreateProsthesis::OpRun()
{
	m_ProsthesesDBManager = GetLogicManager()->GetProsthesesDBManager();
	
	albaVMEProsthesis *proVME = CreateVMEProshesis();
	
	GetLogicManager()->VmeSelect(proVME);
	GetLogicManager()->VmeShow(proVME, true);

	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
albaVMEProsthesis * albaOpCreateProsthesis::CreateVMEProshesis()
{
	albaVMEProsthesis *newVmeProsthesis = NULL;
	albaNEW(newVmeProsthesis);
	newVmeProsthesis->SetName("New Prosthesis");

	std::vector<albaProDBProsthesis *>& prostheses =m_ProsthesesDBManager->GetProstheses();
	if(prostheses.size()>0)
		newVmeProsthesis->SetProsthesis(prostheses.at(0));

	vtkTransform *trans = vtkTransform::New();
	trans->RotateX(90.0);
	trans->Update();
	newVmeProsthesis->SetAbsMatrix(trans->GetMatrix());


	newVmeProsthesis->ReparentTo(m_Input->GetRoot());

	m_Input->GetRoot()->SetLink("VMEProsthesis", newVmeProsthesis);


	return newVmeProsthesis;
}
