/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransform
 Authors: Nicola Vanella, Gianluigi Crimi
 
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

#include "albaOpTransform.h"
#include "albaOpTransformInterface.h"

#include <albaGUIBusyInfo.h>

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUITransformMouse.h"
#include "albaGUIRollOut.h"
#include "albaGUISaveRestorePose.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGizmoScale.h"
#include "albaInteractorGenericMouse.h"
#include "albaSmartPointer.h"
#include "albaTransform.h"
#include "albaTransformFrame.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMELandmark.h"
#include "albaVMEOutput.h"
#include "albaVMESurface.h"
#include "albaVMEPolyline.h"
#include "albaTagItem.h"
#include "albaVMERefSys.h"

#include "vtkTransform.h"
#include "vtkDataSet.h"
#include "vtkOutlineCornerFilter.h"

#include "mmaMaterial.h"
#include "vtkALBALandmarkCloudOutlineCornerFilter.h"

#define EPSILON 1e-9

//----------------------------------------------------------------------------
// widget id's
enum TRANSFORMTEXTENTRIES_ID
{
	ID_SHOW_GIZMO = MINID,
	ID_SELECT_GIZMO_COMBO,
	ID_FLAG_GIZMO_UPDATE,
	ID_SELECT_REF_SYS_COMBO,
	ID_SET_RELATIVE_REF_SYS,
	ID_ARBITRARY_REF_SYS_TRANSLATE,
	ID_ARBITRARY_REF_SYS_ROTATE,
	ID_TEXT_TRANSFORM,
	ID_TEXT_TRANSLATE,
	ID_TEXT_ROTATE,
	ID_TEXT_SCALE,
	ID_RESET,
	ID_LOAD_FROM,
	ID_LOAD_FROM_FILE,
	ID_SAVE_TO_FILE,
	ID_IDENTITY,
};

enum REF_SYS
{
	REF_ABSOLUTE = 0,
	REF_BASE,
	REF_INPUT,
	REF_CENTER,
	REF_RELATIVE,
	REF_RELATIVE_CENTER,
	REF_ARBITRARY,
};

albaVME *GLO_TransformInput;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpTransform);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpTransform::albaOpTransform(const wxString &label)
:albaOpTransformInterface(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_RefSystemMode = REF_CENTER;
	m_ActiveGizmo = TR_GIZMO;

	m_RefSysVMEName = "VME Centroid";
	m_RelativeRefSysVME = NULL;
	m_TransformVME = NULL;

	m_UpdateAfterRelease = false;

	m_TransformEntries[0] = m_TransformEntries[1] = m_TransformEntries[2] = 0;
	m_Position[0] = m_Position[1] = m_Position[2] = 0;
	m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;
	m_Scaling[0] = m_Scaling[1] = m_Scaling[2] = 1;

	m_OriginRefSysPosition[0] = m_OriginRefSysPosition[1] = m_OriginRefSysPosition[2] = 0;
	m_OriginRefSysOrientation[0] = m_OriginRefSysOrientation[1] = m_OriginRefSysOrientation[2] = 0;
		
	m_GizmoTranslate = NULL;
	m_GizmoRotate = NULL;
	m_GizmoScale = NULL;
}
//----------------------------------------------------------------------------
albaOpTransform::~albaOpTransform()
{

}
//----------------------------------------------------------------------------
albaOp* albaOpTransform::Copy()
{
	return new albaOpTransform(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpTransform::InternalAccept(albaVME *node)
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
void albaOpTransform::OpRun()
{
	assert(m_Input);

	GLO_TransformInput = m_Input;

	m_CurrentTime = m_Input->GetTimeStamp();
	m_NewAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();
	m_OldAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();

	// Create aux transform VME
	albaNEW(m_TransformVME);

	if (m_Input->IsA("albaVMELandmark"))
	{
		vtkALBASmartPointer<vtkALBALandmarkCloudOutlineCornerFilter> corner;
		corner->SetInput(m_Input->GetOutput()->GetVTKData());
		corner->Update();
		m_TransformVME->SetData(corner->GetOutput(), m_CurrentTime);
	}
	else
	{
		vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
		corner->SetInput(m_Input->GetOutput()->GetVTKData());
		corner->Update();
		m_TransformVME->SetData(corner->GetOutput(), m_CurrentTime);
	}
	// Set default gray color
	m_TransformVME->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);
	m_TransformVME->SetName("gizmoVme");

	m_TransformVME->GetMaterial()->m_Diffuse[0] = 1.0;
	m_TransformVME->GetMaterial()->m_Diffuse[1] = 1.0;
	m_TransformVME->GetMaterial()->m_Diffuse[2] = 1.0;
	m_TransformVME->GetMaterial()->m_Opacity = 0.50;
	m_TransformVME->GetMaterial()->UpdateProp();

	m_TransformVME->Update();

	m_TransformVME->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));

	m_TransformVME->ReparentTo(m_Input->GetRoot());

	// Create aux VME for Arbitrary transform
	albaNEW(m_ArbitraryRefSysVME);
	albaNEW(m_RelativeCenterRefSysVME);
	albaNEW(m_LocalCenterRefSysVME);
	albaNEW(m_LocalRefSysVME);

	m_LocalRefSysVME->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);

	if (!m_TestMode)
	{
		CreateGui();
		ShowGui();
	}

	SetRefSysVME(m_Input->GetRoot());
	SelectRefSys();
	UpdateTransformTextEntries();
}

//----------------------------------------------------------------------------
void albaOpTransform::OpDo()
{

}

//----------------------------------------------------------------------------
void albaOpTransform::OpUndo()
{
	m_Input->SetAbsMatrix(m_OldAbsMatrix);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpTransform::OpStop(int result)
{
	if (!m_TestMode)
	{
		m_GizmoTranslate->Show(false);
		m_GizmoRotate->Show(false);
		m_GizmoScale->Show(false);

		GetLogicManager()->CameraUpdate();
	}

	m_TransformVME->ReparentTo(NULL);

	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
	cppDEL(m_GizmoScale);

	albaDEL(m_TransformVME);
	albaDEL(m_ArbitraryRefSysVME);
	albaDEL(m_RelativeCenterRefSysVME);
	albaDEL(m_LocalCenterRefSysVME);
	albaDEL(m_LocalRefSysVME);

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void albaOpTransform::CreateGui()
{
	m_Gui = new albaGUI(this);

	// Choose active gizmo
	m_Gui->Label("Choose Transform", true);
	wxString available_gizmos[3] = { "Translate", "Rotate", "Scale" };
	m_Gui->Combo(ID_SELECT_GIZMO_COMBO, "", &m_ActiveGizmo, 3, available_gizmos);
	m_Gui->VectorN(ID_TEXT_TRANSFORM, "", m_TransformEntries);
	m_Gui->Bool(ID_FLAG_GIZMO_UPDATE, "Apply upon mouse release", &m_UpdateAfterRelease, 1);
	m_Gui->Divider();
	m_Gui->Divider(2);
	m_Gui->Divider();

	//---------------------------------
	// Text transform Entries Gui  
	//---------------------------------
	m_Gui->Label("VME Pose", true);
	m_Gui->Divider();

	m_Gui->Label("Current Reference System:");
	m_Gui->Label(&m_RefSysVMEName, false);
	m_Gui->Divider();

	m_Gui->Label("Translations:");
	m_Gui->VectorN(ID_TEXT_TRANSLATE, "", m_Position);
	m_Gui->Label("Rotations:");
	m_Gui->VectorN(ID_TEXT_ROTATE, "", m_Orientation);
	m_Gui->Label("Scaling:");
	m_Gui->VectorN(ID_TEXT_SCALE, "", m_Scaling);
	m_Gui->Divider();

	// Reset Button
	m_Gui->Button(ID_RESET, "Reset", "", "Cancel the transformation.");
	m_Gui->Button(ID_LOAD_FROM, "Load From...", "", "Load matrix from another VME");
	m_Gui->Button(ID_LOAD_FROM_FILE, "Load From file", "", "Load matrix from a csv file");
	m_Gui->Button(ID_SAVE_TO_FILE, "Save to file", "", "Save matrix to a csv file");
	m_Gui->Button(ID_IDENTITY, "Identity", "", "Set Identity Matrix");

	m_Gui->Divider();
	m_Gui->Divider(2);
	m_Gui->Divider();

	//---------------------------------
	// Create the gizmos
	//---------------------------------

	// Translation Gizmo Gui
	m_GizmoTranslate = new albaGizmoTranslate(m_TransformVME, this);
	m_GizmoTranslate->Show(true);

	// Rotation Gizmo Gui
	m_GizmoRotate = new albaGizmoRotate(m_TransformVME, this);
	m_GizmoRotate->Show(false);

	// Scale Gizmo Gui
	m_GizmoScale = new albaGizmoScale(m_TransformVME, this);
	m_GizmoScale->Show(false);


	//---------------------------------
	// Choose active sys Ref
	//---------------------------------

	m_Gui->Label("Choose Reference System", true);
	wxString available_sysRef[7] = { "Absolute", "VME base Reference System","VME Reference System", "VME local centroid", "Relative", "Relative centroid", "Arbitrary" };
	m_Gui->Combo(ID_SELECT_REF_SYS_COMBO, "", &m_RefSystemMode, 7, available_sysRef);
	
	m_Gui->Button(ID_SET_RELATIVE_REF_SYS, "Change");
	m_Gui->Enable(ID_SET_RELATIVE_REF_SYS, false);


	m_Gui->Divider();
	m_Gui->Divider(2);
	m_Gui->Divider();

	//---------------------------------
	// Text Ref sys Origin
	//---------------------------------

	m_Gui->Label("Reference System", true);

	m_Gui->Label("Origin:");
	m_Gui->VectorN(ID_ARBITRARY_REF_SYS_TRANSLATE, "", m_OriginRefSysPosition);
	m_Gui->Label("Orientation:");
	m_Gui->VectorN(ID_ARBITRARY_REF_SYS_ROTATE, "", m_OriginRefSysOrientation);
	m_Gui->Enable(ID_ARBITRARY_REF_SYS_TRANSLATE, false);
	m_Gui->Enable(ID_ARBITRARY_REF_SYS_ROTATE, false);

	//---------------------------------

	m_Gui->Divider();
	m_Gui->Divider(2);
	m_Gui->Divider();

	m_Gui->Label("");

	m_Gui->OkCancel();
	m_Gui->Label("");

	//--------------------------------- 
	m_Gui->FitGui();
	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpTransform::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_SELECT_GIZMO_COMBO: // Show the active Gizmo
	{
		m_GizmoTranslate->Show(m_ActiveGizmo == TR_GIZMO);
		m_GizmoRotate->Show(m_ActiveGizmo == ROT_GIZMO);
		m_GizmoScale->Show(m_ActiveGizmo == SCAL_GIZMO);

		if (m_ActiveGizmo == SCAL_GIZMO)
			m_TransformEntries[0] = m_TransformEntries[1] = m_TransformEntries[2] = 1.0;
		else
			m_TransformEntries[0] = m_TransformEntries[1] = m_TransformEntries[2] = 0.0;

		m_Gui->Update();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_FLAG_GIZMO_UPDATE: // Show Transform VME
	{
		GetLogicManager()->VmeShow(m_TransformVME, m_UpdateAfterRelease);
	}
	break;

	case ID_TRANSFORM: // Transform by gizmo
	{
		OnEventTransformGizmo(alba_event);
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_TEXT_TRANSFORM:
	{
		OnEventTransformText();
		UpdateReferenceSystem();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_TEXT_TRANSLATE: // Transform by text entries
	case ID_TEXT_ROTATE:
	case ID_TEXT_SCALE:
	{
		OnEventPoseTextEntries(alba_event);
		UpdateReferenceSystem();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_SELECT_REF_SYS_COMBO: // Select Ref sys
	{
		SelectRefSys();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_SET_RELATIVE_REF_SYS: // Change Relative Ref Sys
	{
		ChooseRelativeRefSys();
		SelectRefSys();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;
	
	case ID_ARBITRARY_REF_SYS_TRANSLATE: // Move Arbitrary/Local Centroid Ref Sys VME
	case ID_ARBITRARY_REF_SYS_ROTATE:
	{
		if (m_RefSystemMode == REF_CENTER)
		{
			double oldOrientation[3];
			double oldPosition[3];
			m_RefSysVME->GetOutput()->GetAbsPose(oldPosition, oldOrientation, m_CurrentTime);

			m_Position[0] += (m_OriginRefSysPosition[0] - oldPosition[0]);
			m_Position[1] += (m_OriginRefSysPosition[1] - oldPosition[1]);
			m_Position[2] += (m_OriginRefSysPosition[2] - oldPosition[2]);
			
			OnEventPoseTextEntries(alba_event);

			SelectRefSys();

			UpdateTransformTextEntries();
			GetLogicManager()->CameraUpdate();
		}
		else if (m_RefSystemMode == REF_ARBITRARY)
		{
			m_ArbitraryRefSysVME->SetAbsPose(m_OriginRefSysPosition[0], m_OriginRefSysPosition[1], m_OriginRefSysPosition[2], m_OriginRefSysOrientation[0], m_OriginRefSysOrientation[1], m_OriginRefSysOrientation[2], m_CurrentTime);
			SetRefSysVME(m_ArbitraryRefSysVME);
			UpdateTransformTextEntries();
			GetLogicManager()->CameraUpdate();
		}
	}
	break;

	case ID_RESET: // Reset transform
	{
		Reset();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_IDENTITY: // Set identity
	{
		Identity();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_LOAD_FROM: // load from another VME
	{
		LoadFrom();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_LOAD_FROM_FILE: // load from file
	{
		LoadFromFile();
		UpdateTransformTextEntries();
		GetLogicManager()->CameraUpdate();
	}
	break;

	case ID_SAVE_TO_FILE: // save to file
	{
		SaveToFile();
	}
	break;

	case wxOK: // Apply transform
	{
		OnEventPoseTextEntries(alba_event);
		this->OpStop(OP_RUN_OK);
		return;
	}
	break;

	case wxCANCEL: // Abort operation
	{
		Reset();
		this->OpStop(OP_RUN_CANCEL);
		return;
	}
	break;

	default:
	{
		Superclass::OnEvent(alba_event);
	}
	break;
	}

	if (albaVMELandmark *landmark = albaVMELandmark::SafeDownCast(m_Input))
	{
		landmark->Modified();
		landmark->Update();
	}
}

//----------------------------------------------------------------------------
void albaOpTransform::SelectRefSys()
{
	switch (m_RefSystemMode)
	{
		case REF_ABSOLUTE:
		{
			SetRefSysVME(m_Input->GetRoot());
			m_RefSysVMEName = "Absolute";
		}
		break;
		case REF_BASE:
		{
			SetRefSysVME(m_LocalRefSysVME);
			m_RefSysVMEName = "VME Base";
		}
		break;
		case REF_INPUT:
		{
			SetRefSysVME(m_Input);
			m_RefSysVMEName = "VME";
		}
		break;
		case REF_CENTER:
		{
			UpdateReferenceSystem();

			m_RefSysVMEName = "VME Centroid";
		}
		break;
		case REF_RELATIVE:
		{
			if (m_RelativeRefSysVME == NULL)
				ChooseRelativeRefSys();
			else
				SetRefSysVME(m_RelativeRefSysVME);
			m_RefSysVMEName = m_RelativeRefSysVME->GetName();
		}
		break;
		case REF_RELATIVE_CENTER:
		{
			if (m_RelativeRefSysVME == NULL)
				ChooseRelativeRefSys();
			else
				SetRefSysVME(m_RelativeRefSysVME);

			m_LocalRefSysVME->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);

			// Calculate centroid of VME using bounds 
			double bounds[6];
			m_RelativeRefSysVME->GetOutput()->GetVMEBounds(bounds);
			albaMatrix mtr;
			mtr.CopyRotation(*m_RelativeRefSysVME->GetOutput()->GetAbsMatrix());
			albaTransform tra;
			tra.Translate((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, false);
			tra.Concatenate(mtr, true);
			tra.Update();

			m_RelativeCenterRefSysVME->SetAbsMatrix(tra.GetMatrix());

			SetRefSysVME(m_RelativeCenterRefSysVME);

			m_RefSysVMEName.Printf("Centroid of %s", m_RelativeRefSysVME->GetName());
		}
		break;
		case REF_ARBITRARY:
		{
			SetRefSysVME(m_ArbitraryRefSysVME);
			m_RefSysVMEName = "Arbitrary";
		}
		break;
	}

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_ARBITRARY_REF_SYS_TRANSLATE, m_RefSystemMode == REF_ARBITRARY || m_RefSystemMode == REF_CENTER);
		m_Gui->Enable(ID_ARBITRARY_REF_SYS_ROTATE, m_RefSystemMode == REF_ARBITRARY);
		m_Gui->Enable(ID_TEXT_TRANSLATE, m_RefSystemMode != REF_CENTER);
		m_Gui->Enable(ID_SET_RELATIVE_REF_SYS, m_RefSystemMode == REF_RELATIVE || m_RefSystemMode == REF_RELATIVE_CENTER);
	}
}

//----------------------------------------------------------------------------
void albaOpTransform::ChooseRelativeRefSys()
{
	albaString s;
	s << "Choose VME Reference System";
	albaEvent e(this, VME_CHOOSE, &s);
	e.SetPointer(&AcceptRefSys);
	albaEventMacro(e);

	SetRefSysVME(e.GetVme());

	m_RelativeRefSysVME = (albaVMEPolyline *)m_RefSysVME;
	m_RefSysVMEName = m_RefSysVME->GetName();
}

//----------------------------------------------------------------------------
void albaOpTransform::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		m_GizmoTranslate->SetRefSys(m_RefSysVME);
		m_GizmoRotate->SetRefSys(m_RefSysVME);
		m_GizmoScale->SetRefSys(m_RefSysVME);
	}

	// Update Origin position
	m_RefSysVME->GetOutput()->GetAbsPose(m_OriginRefSysPosition, m_OriginRefSysOrientation, m_CurrentTime);
}

//----------------------------------------------------------------------------
void albaOpTransform::OnEventTransformGizmo(albaEventBase *alba_event)
{
	albaEvent *e = albaEvent::SafeDownCast(alba_event);
	long arg = e->GetArg();

	if (m_ActiveGizmo != SCAL_GIZMO)
		PostMultiplyMatrix(e->GetMatrix());

	if (!m_UpdateAfterRelease || arg == albaInteractorGenericMouse::MOUSE_UP)
	{
		m_Input->SetAbsMatrix(*m_TransformVME->GetOutput()->GetAbsMatrix());
	}

	if (arg == albaInteractorGenericMouse::MOUSE_UP)
	{
		UpdateReferenceSystem();
	}

	UpdateTransformTextEntries();
}

//----------------------------------------------------------------------------
void albaOpTransform::UpdateReferenceSystem()
{
	if (m_RefSystemMode == REF_RELATIVE_CENTER)
	{
		double bounds[6];
		m_RelativeRefSysVME->GetOutput()->GetVMEBounds(bounds);
		albaMatrix mtr;
		mtr.CopyRotation(*m_RelativeRefSysVME->GetOutput()->GetAbsMatrix());
		albaTransform tra;
		tra.Translate((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, false);
		tra.Concatenate(mtr, true);
		tra.Update();

		m_RelativeCenterRefSysVME->SetAbsMatrix(tra.GetMatrix());

		SetRefSysVME(m_RelativeCenterRefSysVME);
	}
	else if (m_RefSystemMode == REF_CENTER)
	{
		double bounds[6];
		double pos[3];
		m_TransformVME->GetOutput()->GetVMEBounds(bounds);
		m_TransformVME->GetOutput()->GetAbsPose(pos, m_OriginRefSysOrientation, m_CurrentTime);
		m_LocalCenterRefSysVME->SetAbsPose((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, m_OriginRefSysOrientation[0], m_OriginRefSysOrientation[1], m_OriginRefSysOrientation[2], m_CurrentTime);

		SetRefSysVME(m_LocalCenterRefSysVME);
	}
	else if (m_RefSystemMode == REF_INPUT)
	{
		SetRefSysVME(m_Input);
	}
}

//----------------------------------------------------------------------------
void albaOpTransform::PostMultiplyMatrix(albaMatrix *matrix)
{
	// handle incoming transform events
	vtkTransform *tr = vtkTransform::New();
	tr->PostMultiply();
	tr->SetMatrix(m_TransformVME->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
	tr->Concatenate(matrix->GetVTKMatrix());
	tr->Update();

	albaMatrix absPose;
	absPose.DeepCopy(tr->GetMatrix());
	absPose.SetTimeStamp(m_CurrentTime);
		
	m_TransformVME->SetAbsMatrix(absPose);
	// update matrix for OpDo()
	m_NewAbsMatrix = absPose;

	GetLogicManager()->CameraUpdate();

	// clean up
	tr->Delete();
}




//----------------------------------------------------------------------------
void albaOpTransform::OnEventTransformText()
{
	albaTransform tran;

	tran.Translate(-m_OriginRefSysPosition[0], -m_OriginRefSysPosition[1], -m_OriginRefSysPosition[2], POST_MULTIPLY);
	
	if (m_ActiveGizmo == SCAL_GIZMO)
	{
		tran.Scale(m_TransformEntries[0], m_TransformEntries[1], m_TransformEntries[2], POST_MULTIPLY);
	}
	else if (m_ActiveGizmo == ROT_GIZMO)
	{
		tran.RotateX(m_TransformEntries[0], POST_MULTIPLY);
		tran.RotateY(m_TransformEntries[1], POST_MULTIPLY);
		tran.RotateZ(m_TransformEntries[2], POST_MULTIPLY);
	}
	else if (m_ActiveGizmo == TR_GIZMO)
	{
		tran.Translate(m_TransformEntries,POST_MULTIPLY);
	}

	tran.Translate(m_OriginRefSysPosition, POST_MULTIPLY);

	PostMultiplyMatrix(tran.GetMatrixPointer());

	m_Input->SetAbsMatrix(*m_TransformVME->GetOutput()->GetAbsMatrix());
		
	if (m_ActiveGizmo == SCAL_GIZMO)
		m_TransformEntries[0] = m_TransformEntries[1] = m_TransformEntries[2] = 1.0;
	else
		m_TransformEntries[0] = m_TransformEntries[1] = m_TransformEntries[2] = 0.0;
}

//----------------------------------------------------------------------------
void albaOpTransform::OnEventPoseTextEntries(albaEventBase *alba_event)
{
	albaTransform tran;
	tran.Scale(m_Scaling[0], m_Scaling[1], m_Scaling[2], POST_MULTIPLY);
	tran.RotateY(m_Orientation[1], POST_MULTIPLY);
	tran.RotateX(m_Orientation[0], POST_MULTIPLY);
	tran.RotateZ(m_Orientation[2], POST_MULTIPLY);
	tran.SetPosition(m_Position);

	// Postmultiply to ref sys abs matrix
	tran.Concatenate(m_RefSysVME->GetOutput()->GetAbsTransform(), POST_MULTIPLY);

	albaMatrix absPose;
	absPose = tran.GetMatrix();
	absPose.SetTimeStamp(m_CurrentTime);

	m_NewAbsMatrix = absPose;

	m_Input->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);

	// Notify the listener about the new abs pose
	albaEvent e2s;
	e2s.SetSender(this);
	e2s.SetMatrix(tran.GetMatrixPointer());
	e2s.SetId(ID_TRANSFORM);
	albaEventMacro(e2s);
}

//----------------------------------------------------------------------------
void albaOpTransform::Reset()
{
	m_Input->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);

	SelectRefSys();
}

//----------------------------------------------------------------------------
void albaOpTransform::Identity()
{
	albaMatrix identityMatr;
	
	m_Input->SetAbsMatrix(identityMatr, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(identityMatr, m_CurrentTime);

	SelectRefSys();
}

//----------------------------------------------------------------------------
void albaOpTransform::LoadFrom()
{
	albaString s;
	s << "Choose VME";
	albaEvent e(this, VME_CHOOSE, &s);
	e.SetPointer(&AcceptRefSys);
	albaEventMacro(e);

	albaMatrix targetMtr;
	if(e.GetVme())
	 targetMtr.DeepCopy(e.GetVme()->GetOutput()->GetAbsMatrix());

	m_Input->SetAbsMatrix(targetMtr, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(targetMtr, m_CurrentTime);

	SelectRefSys();
}

//----------------------------------------------------------------------------
void albaOpTransform::LoadFromFile()
{
	wxString proposed = albaGetLastUserFolder();
	wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = albaGetOpenFile(proposed, wildc).ToAscii();

	albaMatrix loadmatrix;

	if (!f.IsEmpty())
	{
		FILE *inFile;
		inFile = albaTryOpenFile(f.ToAscii(), "r");
		if (inFile == NULL)
		{
			albaMessage("Error: cloud not open file!");
			return;
		}
	
		//Content
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				double value;
				int ret = fscanf(inFile, "%lf;", &value);
				if(ret!=1)
				{
					albaMessage("Error, cloud not read file or wrong file format!");
					fclose(inFile);
					return;
				}
				loadmatrix.SetElement(i, j,value);
			}
		}
		fclose(inFile);
	}

	m_Input->SetAbsMatrix(loadmatrix, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(loadmatrix, m_CurrentTime);

	SelectRefSys();
}

//----------------------------------------------------------------------------
void albaOpTransform::SaveToFile()
{
	wxString proposed = albaGetLastUserFolder();
	proposed += m_Input->GetName();
	proposed += "_matrix.csv";
	wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = albaGetSaveFile(proposed, wildc).ToAscii();

	if (!f.IsEmpty())
	{
		FILE *outFile;
		outFile = albaTryOpenFile(f.ToAscii(), "w");
		if (outFile==NULL)
		{
			albaMessage("Error: cloud not open file!");
			fclose(outFile);
			return;
		}

		albaMatrix saveMtr;
		saveMtr.DeepCopy(m_Input->GetOutput()->GetAbsMatrix());
		

		//Content
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				double value = saveMtr.GetElement(i, j);
				fprintf(outFile, "%lf;", value);
			}
			fprintf(outFile, "\n");
		}
		fclose(outFile);
	}
}


//----------------------------------------------------------------------------
void albaOpTransform::Translate(double x, double y, double z)
{
	m_ActiveGizmo = TR_GIZMO;

	m_TransformEntries[0] = x;
	m_TransformEntries[1] = y;
	m_TransformEntries[2] = z;

	OnEventTransformText();
}
//----------------------------------------------------------------------------
void albaOpTransform::Rotate(double x, double y, double z)
{
	m_ActiveGizmo = ROT_GIZMO;

	m_TransformEntries[0] = x;
	m_TransformEntries[1] = y;
	m_TransformEntries[2] = z;

	OnEventTransformText();
}
//----------------------------------------------------------------------------
void albaOpTransform::Scale(double x, double y, double z)
{
	m_ActiveGizmo = SCAL_GIZMO;

	m_TransformEntries[0] = x;
	m_TransformEntries[1] = y;
	m_TransformEntries[2] = z;

	OnEventTransformText();
}

//----------------------------------------------------------------------------
void albaOpTransform::UpdateTransformTextEntries()
{
	// Express absPose in RefSysVME ref sys
	albaTransformFrame *mflTr = albaTransformFrame::New();
	mflTr->SetInput(m_TransformVME->GetOutput()->GetAbsMatrix());
	mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());

	mflTr->Update();

	// Update gui with new pose: Position, Orientation, Scale
	albaTransform::GetPosition(mflTr->GetMatrix(), m_Position);
	albaTransform::GetOrientation(mflTr->GetMatrix(), m_Orientation);
	albaTransform::GetScale(mflTr->GetMatrix(), m_Scaling);
	
	albaDEL(mflTr);

	if (m_RefSystemMode == REF_BASE)
	{
		m_LocalRefSysVME->GetOutput()->GetAbsPose(m_OriginRefSysPosition, m_OriginRefSysOrientation, m_CurrentTime);
	}
	else
	{
		m_RefSysVME->GetOutput()->GetAbsPose(m_OriginRefSysPosition, m_OriginRefSysOrientation, m_CurrentTime);
	}
	
	//round values near 0 on GUI
	for (int i = 0; i < 3; i++)
	{
		if (fabs(m_Position[i]) < EPSILON)
			m_Position[i] = 0.0;
		if (fabs(m_Orientation[i]) < EPSILON)
			m_Orientation[i] = 0.0;
		if (fabs(m_OriginRefSysPosition[i]) < EPSILON)
			m_OriginRefSysPosition[i] = 0.0;
		if (fabs(m_OriginRefSysOrientation[i]) < EPSILON)
			m_OriginRefSysOrientation[i] = 0.0;
	}
	if (!m_TestMode)
	{
		assert(m_Gui);
		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
bool albaOpTransform::AcceptRefSys(albaVME *node)
{
	return node != GLO_TransformInput;
}

//----------------------------------------------------------------------------
char ** albaOpTransform::GetIcon()
{
#include "pic/MENU_OP_TRANSFORM.xpm"
	return MENU_OP_TRANSFORM_xpm;
}

