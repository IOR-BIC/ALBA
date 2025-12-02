/*=========================================================================
Program:   Alba
Module:    albaOpMeasureAxisRotations.cpp
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

#include "albaOpMeasureAxisRotations.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaMatrix.h"
#include "albaVMEOutput.h"
#include "albaVME.h"
#include "albaQuaternion.h"
#include "albaVect3d.h"
#include "vtkMath.h"
#include "albaTransformFrame.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMeasureAxisRotations);

enum AXIS_ROTATION_ID
{
	ID_MODALITY = MINID,
	ID_SELECT_RELATIVE_VME,
	ID_ROTATIONS,
};

enum ROTATION_MODALITIES
{
	MOD_CURRENT,
	MOD_RELATIVE,
};

albaVME* GLO_AxisRotationInput;

//----------------------------------------------------------------------------
albaOpMeasureAxisRotations::albaOpMeasureAxisRotations(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_Modality = MOD_CURRENT;
	m_RelativeVME = NULL;
}

//----------------------------------------------------------------------------
albaOpMeasureAxisRotations::~albaOpMeasureAxisRotations()
{
}

//----------------------------------------------------------------------------
bool albaOpMeasureAxisRotations::InternalAccept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
char** albaOpMeasureAxisRotations::GetIcon()
{

#include "pic/MENU_OP_MEASURE_AXIS_ROTATION.xpm"
	return MENU_OP_MEASURE_AXIS_ROTATION_xpm;
}

//----------------------------------------------------------------------------
albaOp* albaOpMeasureAxisRotations::Copy()
{
	albaOpMeasureAxisRotations *cp = new albaOpMeasureAxisRotations(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::OpRun()
{

	GLO_AxisRotationInput = m_Input;

	if (!m_TestMode)
	{
		CreateGui();
	}
	UpdateRotations();
}
//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::OpDo()
{
}

//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case ID_ROTATIONS:
				UpdateRotations();
				break;
			case ID_MODALITY:
			{
				if (m_Modality == MOD_RELATIVE && m_RelativeVME == NULL)
				{
					ChooseRelativeVME();
					if (m_RelativeVME == NULL)
						m_Modality = MOD_CURRENT;
				}
				m_Gui->Enable(ID_SELECT_RELATIVE_VME, m_Modality == MOD_RELATIVE);
				UpdateRotations();
			}
			break;
			case ID_SELECT_RELATIVE_VME:
			{
				ChooseRelativeVME();
				UpdateRotations();
			}
			break;
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(alba_event);
// 		}
	}
}

//----------------------------------------------------------------------------
bool albaOpMeasureAxisRotations::AcceptRelative(albaVME* node)
{
	return node != GLO_AxisRotationInput;
}

//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::ChooseRelativeVME()
{
	albaString s;
	s << "Choose VME Reference System";
	albaEvent e(this, VME_CHOOSE, &s);
	e.SetPointer(&AcceptRelative);
	albaEventMacro(e);

	if (e.GetVme())
	{
		m_RelativeVME = e.GetVme();
		m_RelativeVMEName = m_RelativeVME->GetName();
	}
}

//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);

	wxString choices[] = { "Current","Relative To..." };

	m_Gui->Radio(ID_MODALITY, "Modality",&m_Modality, 2, choices);

	m_Gui->Button(ID_SELECT_RELATIVE_VME, "Select Target");
	m_Gui->Label(&m_RelativeVMEName);
	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Vector(ID_ROTATIONS, "Rotations:", m_Rotations);


	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Button(wxOK,"Ok");
	m_Gui->Label("");

	ShowGui();

	m_Gui->Enable(ID_SELECT_RELATIVE_VME, false);
}

//----------------------------------------------------------------------------
void albaOpMeasureAxisRotations::UpdateRotations()
{
	albaMatrix mtr;

	if (m_Modality == MOD_RELATIVE && m_RelativeVME != NULL)
	{
		// Express absPose in RefSysVME ref sys
		albaTransformFrame* mflTr = albaTransformFrame::New();
		mflTr->SetInput(m_RelativeVME->GetOutput()->GetAbsMatrix());
		mflTr->SetTargetFrame(m_Input->GetOutput()->GetAbsMatrix());
		mflTr->Update();

		mtr.CopyRotation(mflTr->GetMatrix());
		albaDEL(mflTr);
	}
	else
		mtr.CopyRotation(*m_Input->GetOutput()->GetAbsMatrix());


	albaVect3d axisComp;
	double angle;
	albaQuaternion quaternion(mtr);
	
	quaternion.AngleAxis(angle, axisComp);

	angle *= vtkMath::RadiansToDegrees();
	
	m_Rotations[0] = axisComp[0] * angle;
	m_Rotations[1] = axisComp[1] * angle;
	m_Rotations[2] = axisComp[2] * angle;

	m_Gui->Update();
}
