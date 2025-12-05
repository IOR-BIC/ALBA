/*=========================================================================
Program:   Alba
Module:    albaOpTransformAtoB.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2024 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the alba must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpTransformAtoB.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMERefSys.h"
#include "albaVMEOutput.h"
#include "albaTransformFrame.h"



albaVME* GLO_TransformAtoBInput;
albaVME* GLO_TransformAtoBBase;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpTransformAtoB);




//----------------------------------------------------------------------------
albaOpTransformAtoB::albaOpTransformAtoB(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}

//----------------------------------------------------------------------------
albaOpTransformAtoB::~albaOpTransformAtoB()
{
}

//----------------------------------------------------------------------------
bool albaOpTransformAtoB::AcceptA(albaVME* node)
{
	return node != GLO_TransformAtoBInput;
}

//----------------------------------------------------------------------------
bool albaOpTransformAtoB::AcceptB(albaVME* node)
{
	return (node != GLO_TransformAtoBInput && node != GLO_TransformAtoBBase);
}

//----------------------------------------------------------------------------
bool albaOpTransformAtoB::InternalAccept(albaVME *node)
{
	bool accepted = false;

	accepted = !node->IsA("lhpVMESurfaceScalarVarying") \
		&& !node->IsA("albaVMEMeter") \
		&& !node->IsA("medVMELabeledVolume") \
		&& !node->IsA("albaVMEHelicalAxis");

	if (accepted == false)
	{
		return false;
	}
	else
	{
		return (node != NULL && node->IsALBAType(albaVME) && !node->IsA("albaVMERoot")
			&& !node->IsA("albaVMEExternalData") && !node->IsA("albaVMERefSys")) ||
			(node != NULL && node->IsALBAType(albaVMERefSys) && albaVMERefSys::SafeDownCast(node)->IsMovable());
	}
}


//----------------------------------------------------------------------------
char** albaOpTransformAtoB::GetIcon()
{
#include "pic/MENU_OP_TRANSFORM_A2B.xpm"
	return MENU_OP_TRANSFORM_A2B_xpm;
}

//----------------------------------------------------------------------------
albaOp* albaOpTransformAtoB::Copy()
{
	albaOpTransformAtoB *cp = new albaOpTransformAtoB(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpTransformAtoB::OpRun()
{

	GLO_TransformAtoBInput = m_Input;

	m_InputMatrix.DeepCopy(m_Input->GetOutput()->GetAbsMatrix());

	albaString s = "Choose VME Base Transform VME";
	albaEvent e(this, VME_CHOOSE, &s);
	e.SetPointer(&AcceptA);
	albaEventMacro(e);

	if (e.GetVme() == NULL)
	{
		OpStop(OP_RUN_CANCEL);
		return;
	}
	m_BaseVME= GLO_TransformAtoBBase = e.GetVme();

	albaString s2 = "Choose VME Target Transform VME";
	albaEvent e2(this, VME_CHOOSE, &s);
	e2.SetPointer(&AcceptB);
	albaEventMacro(e2);

	if (e2.GetVme() == NULL)
	{
		OpStop(OP_RUN_CANCEL);
		return;
	}

	m_TargerVME = e2.GetVme();

	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
void albaOpTransformAtoB::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpTransformAtoB::OpDo()
{

	// Express absPose in RefSysVME ref sys
	albaTransformFrame* mflTr = albaTransformFrame::New();
	mflTr->SetInput(&m_InputMatrix);
	mflTr->SetTargetFrame(m_BaseVME->GetOutput()->GetAbsMatrix());
	mflTr->Update();

	albaMatrix resultMatrix;

	albaMatrix::Multiply4x4(*m_TargerVME->GetOutput()->GetAbsMatrix(), mflTr->GetMatrix(), resultMatrix);

	m_Input->SetAbsMatrix(resultMatrix);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpTransformAtoB::OpUndo()
{
	m_Input->SetAbsMatrix(m_InputMatrix);

	GetLogicManager()->CameraUpdate();

}

