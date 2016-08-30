/*=========================================================================

 Program: MAF2
 Module: mafOpTransform
 Authors: Nicola Vanella
 
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

#include "mafOpTransform.h"
#include "mafOpTransformInterface.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUITransformMouse.h"
#include "mafGUIRollOut.h"
#include "mafGUISaveRestorePose.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGizmoScale.h"
#include "mafInteractorGenericMouse.h"
#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMELandmark.h"
#include "mafVMEOutput.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafTagItem.h"
#include "mafVMERefSys.h"

#include "vtkTransform.h"
#include "vtkDataSet.h"
#include "vtkOutlineCornerFilter.h"

#include "mmaMaterial.h"

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
	ID_SET_ARBITRARY_REF_SYS,
	ID_TEXT_TRANSLATE,
	ID_TEXT_ROTATE,
	ID_TEXT_SCALE,
	ID_RESET,
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

mafVME *GLO_TransformInput;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpTransform);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpTransform::mafOpTransform(const wxString &label)
:mafOpTransformInterface(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_RefSystemMode = REF_ABSOLUTE;
	m_ActiveGizmo = TR_GIZMO;

	m_RefSysVMEName = "";
	m_RelativeRefSysVME = NULL;
	m_TransformVME = NULL;

	m_UpdateAfterRelease = false;

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
mafOpTransform::~mafOpTransform()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafOp* mafOpTransform::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpTransform(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpTransform::Accept(mafVME *node)
//----------------------------------------------------------------------------
{
	bool accepted = false;

	accepted = !node->IsA("lhpVMESurfaceScalarVarying") \
		&& !node->IsA("mafVMEMeter") \
		&& !node->IsA("medVMELabeledVolume") \
		&& !node->IsA("mafVMEHelicalAxis");

	if (accepted == false)
	{
		return false;
	}
	else
	{
		return (node != NULL && node->IsMAFType(mafVME) && !node->IsA("mafVMERoot")
			&& !node->IsA("mafVMEExternalData") && !node->IsA("mafVMERefSys")) ||
			(node != NULL && node->IsMAFType(mafVMERefSys) && mafVMERefSys::SafeDownCast(node)->IsMovable());
	}
}
//----------------------------------------------------------------------------
void mafOpTransform::OpRun()
//----------------------------------------------------------------------------
{
	assert(m_Input);

	GLO_TransformInput = m_Input;

	m_CurrentTime = m_Input->GetTimeStamp();
	m_NewAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();
	m_OldAbsMatrix = *m_Input->GetOutput()->GetAbsMatrix();

	// Create aux transform VME
	mafNEW(m_TransformVME);

	vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(m_Input->GetOutput()->GetVTKData());
	corner->Update();

	// Set default gray color
	m_TransformVME->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);
	m_TransformVME->SetData(corner->GetOutput(), m_CurrentTime);
	m_TransformVME->SetName("gizmoVme");

	m_TransformVME->GetMaterial()->m_Diffuse[0] = 1.0;
	m_TransformVME->GetMaterial()->m_Diffuse[1] = 1.0;
	m_TransformVME->GetMaterial()->m_Diffuse[2] = 1.0;
	m_TransformVME->GetMaterial()->m_Opacity = 0.50;
	m_TransformVME->GetMaterial()->UpdateProp();

	m_TransformVME->Update();

	m_TransformVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));

	m_TransformVME->ReparentTo(m_Input->GetRoot());

	// Create aux VME for Arbitrary transform
	mafNEW(m_ArbitraryRefSysVME);
	mafNEW(m_RelativeCenterRefSysVME);
	mafNEW(m_LocalCenterRefSysVME);
	mafNEW(m_LocalRefSysVME);

	m_LocalRefSysVME->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);

	if (!m_TestMode)
	{
		CreateGui();
		ShowGui();
	}

	SetRefSysVME(m_Input->GetRoot());
	UpdateTransformTextEntries();
}

//----------------------------------------------------------------------------
void mafOpTransform::OpDo()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafOpTransform::OpUndo()
//----------------------------------------------------------------------------
{
	m_Input->SetAbsMatrix(m_OldAbsMatrix);

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mafOpTransform::OpStop(int result)
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		m_GizmoTranslate->Show(false);
		m_GizmoRotate->Show(false);
		m_GizmoScale->Show(false);

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		mafEventMacro(mafEvent(this, OP_HIDE_GUI, (wxWindow *)m_Gui->GetParent()));
	}

	m_TransformVME->ReparentTo(NULL);

	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
	cppDEL(m_GizmoScale);

	mafDEL(m_TransformVME);
	mafDEL(m_ArbitraryRefSysVME);
	mafDEL(m_RelativeCenterRefSysVME);
	mafDEL(m_LocalCenterRefSysVME);
	mafDEL(m_LocalRefSysVME);

	mafEventMacro(mafEvent(this, result));
}

//----------------------------------------------------------------------------
void mafOpTransform::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);

	// Choose active gizmo
	m_Gui->Label("Choose Transform", true);
	wxString available_gizmos[3] = { "Translate", "Rotate", "Scale" };
	m_Gui->Combo(ID_SELECT_GIZMO_COMBO, "", &m_ActiveGizmo, 3, available_gizmos);
	m_Gui->Bool(ID_FLAG_GIZMO_UPDATE, "Apply upon mouse release", &m_UpdateAfterRelease, 1);

	m_Gui->Divider(2);

	//---------------------------------
	// Text transform Entries Gui  
	//---------------------------------
	m_Gui->Label("Transform Entries", true);
	m_Gui->Double(ID_TEXT_TRANSLATE, "Translate X", &m_Position[0]);
	m_Gui->Double(ID_TEXT_TRANSLATE, "Translate Y", &m_Position[1]);
	m_Gui->Double(ID_TEXT_TRANSLATE, "Translate Z", &m_Position[2]);
	m_Gui->Double(ID_TEXT_ROTATE, "Rotate X", &m_Orientation[0]);
	m_Gui->Double(ID_TEXT_ROTATE, "Rotate Y", &m_Orientation[1]);
	m_Gui->Double(ID_TEXT_ROTATE, "Rotate Z", &m_Orientation[2]);
	m_Gui->Double(ID_TEXT_SCALE, "Scale X", &m_Scaling[0], 0);
	m_Gui->Double(ID_TEXT_SCALE, "Scale Y", &m_Scaling[1], 0);
	m_Gui->Double(ID_TEXT_SCALE, "Scale Z", &m_Scaling[2], 0);

	m_Gui->Divider(2);

	// Reset Button
	m_Gui->Button(ID_RESET, "Reset", "", "Cancel the transformation.");

	//---------------------------------
	// Create the gizmos
	//---------------------------------

	// Translation Gizmo Gui
	m_GizmoTranslate = new mafGizmoTranslate(m_TransformVME, this);
	m_GizmoTranslate->Show(true);

	// Rotation Gizmo Gui
	m_GizmoRotate = new mafGizmoRotate(m_TransformVME, this);
	m_GizmoRotate->Show(false);

	// Scale Gizmo Gui
	m_GizmoScale = new mafGizmoScale(m_TransformVME, this);
	m_GizmoScale->Show(false);

	m_Gui->Divider(2);

	//---------------------------------
	// Choose active sys Ref
	//---------------------------------

	m_Gui->Label("Choose Reference System", true);
	wxString available_sysRef[7] = { "Absolute", "VME base Reference System","VME Reference System", "VME local centroid", "Relative", "Relative centroid", "Arbitrary" };
	m_Gui->Combo(ID_SELECT_REF_SYS_COMBO, "", &m_RefSystemMode, 7, available_sysRef);

	m_Gui->Label("Ref sys:", &m_RefSysVMEName, false);

	m_Gui->Button(ID_SET_RELATIVE_REF_SYS, "Change");
	m_Gui->Enable(ID_SET_RELATIVE_REF_SYS, false);

	m_Gui->Divider(2);

	//---------------------------------
	// Text Ref sys Origin
	//---------------------------------

	m_Gui->Label("Reference System", true);

	m_Gui->Double(ID_SET_ARBITRARY_REF_SYS, "X Pos", &m_OriginRefSysPosition[0]);
	m_Gui->Double(ID_SET_ARBITRARY_REF_SYS, "Y Pos", &m_OriginRefSysPosition[1]);
	m_Gui->Double(ID_SET_ARBITRARY_REF_SYS, "Z Pos", &m_OriginRefSysPosition[2]);
	m_Gui->Double(ID_SET_ARBITRARY_REF_SYS, "X Rot", &m_OriginRefSysOrientation[0]);
	m_Gui->Double(ID_SET_ARBITRARY_REF_SYS, "Y Rot", &m_OriginRefSysOrientation[1]);
	m_Gui->Double(ID_SET_ARBITRARY_REF_SYS, "Z Rot", &m_OriginRefSysOrientation[2]);
	m_Gui->Enable(ID_SET_ARBITRARY_REF_SYS, false);

	//---------------------------------

	m_Gui->Divider(2);
	m_Gui->Label("");

	m_Gui->OkCancel();
	m_Gui->Label("");

	//--------------------------------- 
	m_Gui->FitGui();
	m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafOpTransform::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch (maf_event->GetId())
	{
	case ID_SELECT_GIZMO_COMBO: // Show the active Gizmo
	{
		m_GizmoTranslate->Show(m_ActiveGizmo == TR_GIZMO);
		m_GizmoRotate->Show(m_ActiveGizmo == ROT_GIZMO);
		m_GizmoScale->Show(m_ActiveGizmo == SCAL_GIZMO);

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	break;

	case ID_FLAG_GIZMO_UPDATE: // Show Transform VME
	{
		mafEventMacro(mafEvent(this, VME_SHOW, m_TransformVME, m_UpdateAfterRelease));
	}
	break;

	case ID_TRANSFORM: // Transform by gizmo
	{
		OnEventTransformGizmo(maf_event);
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	break;

	case ID_TEXT_TRANSLATE: // Transform by text entries
	case ID_TEXT_ROTATE:
	case ID_TEXT_SCALE:
	{
		OnEventTransformTextEntries(maf_event);
		if (m_RefSystemMode == REF_INPUT)
		{
			SetRefSysVME(m_Input);
		}
		else if (m_RefSystemMode == REF_CENTER)
		{
			UpdateAndSetLocalCentroidRefSys();
		}
		UpdateTransformTextEntries();
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	break;

	case ID_SELECT_REF_SYS_COMBO: // Select Ref sys
	{
		SelectRefSys();
		UpdateTransformTextEntries();
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	break;

	case ID_SET_RELATIVE_REF_SYS: // Change Relative Ref Sys
	{
		ChooseRelativeRefSys();
		SelectRefSys();
		UpdateTransformTextEntries();
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	break;

	case ID_SET_ARBITRARY_REF_SYS: // Move Arbitrary/Local Centroid Ref Sys VME
	{
		if (m_RefSystemMode == REF_CENTER)
		{
			double oldOrientation[3];
			double oldPosition[3];
			m_RefSysVME->GetOutput()->GetAbsPose(oldPosition, oldOrientation, m_CurrentTime);

			m_Position[0] += (m_OriginRefSysPosition[0] - oldPosition[0]);
			m_Position[1] += (m_OriginRefSysPosition[1] - oldPosition[1]);
			m_Position[2] += (m_OriginRefSysPosition[2] - oldPosition[2]);
			
			OnEventTransformTextEntries(maf_event);

			SelectRefSys();

			UpdateTransformTextEntries();
			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		}
		else if (m_RefSystemMode == REF_ARBITRARY)
		{
			m_ArbitraryRefSysVME->SetAbsPose(m_OriginRefSysPosition[0], m_OriginRefSysPosition[1], m_OriginRefSysPosition[2], m_OriginRefSysOrientation[0], m_OriginRefSysOrientation[1], m_OriginRefSysOrientation[2], m_CurrentTime);
			SetRefSysVME(m_ArbitraryRefSysVME);
			UpdateTransformTextEntries();
			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		}
	}
	break;

	case ID_RESET: // Reset transform
	{
		Reset();
		UpdateTransformTextEntries();
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	break;

	case wxOK: // Apply transform
	{
		OnEventTransformTextEntries(maf_event);
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
		Superclass::OnEvent(maf_event);
	}
	break;
	}

	if (mafVMELandmark *landmark = mafVMELandmark::SafeDownCast(m_Input))
	{
		landmark->Modified();
		landmark->Update();
	}
}

//----------------------------------------------------------------------------
void mafOpTransform::SelectRefSys()
//----------------------------------------------------------------------------
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
			UpdateAndSetLocalCentroidRefSys();

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
			m_RelativeCenterRefSysVME->SetAbsPose((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, 0, 0, 0, m_CurrentTime);

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
		m_Gui->Enable(ID_SET_ARBITRARY_REF_SYS, m_RefSystemMode == REF_ARBITRARY || m_RefSystemMode == REF_CENTER);
		m_Gui->Enable(ID_TEXT_TRANSLATE, m_RefSystemMode != REF_CENTER);
		m_Gui->Enable(ID_SET_RELATIVE_REF_SYS, m_RefSystemMode == REF_RELATIVE || m_RefSystemMode == REF_RELATIVE_CENTER);
	}
}

//----------------------------------------------------------------------------
void mafOpTransform::UpdateAndSetLocalCentroidRefSys()
{
	// Calculate centroid of VME using bounds 
	double bounds[6];
	m_TransformVME->GetOutput()->GetVMEBounds(bounds);
	m_LocalCenterRefSysVME->SetAbsPose((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, 0, 0, 0, m_CurrentTime);
	SetRefSysVME(m_LocalCenterRefSysVME);
}

//----------------------------------------------------------------------------
void mafOpTransform::ChooseRelativeRefSys()
//----------------------------------------------------------------------------
{
	mafString s;
	s << "Choose VME Reference System";
	mafEvent e(this, VME_CHOOSE, &s);
	e.SetArg((long)&AcceptRefSys);
	mafEventMacro(e);

	SetRefSysVME(e.GetVme());

	m_RelativeRefSysVME = (mafVMEPolyline *)m_RefSysVME;
	m_RefSysVMEName = m_RefSysVME->GetName();
}

//----------------------------------------------------------------------------
void mafOpTransform::RefSysVmeChanged()
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
void mafOpTransform::OnEventTransformGizmo(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	mafEvent *e = mafEvent::SafeDownCast(maf_event);
	long arg = e->GetArg();

	if (m_ActiveGizmo != SCAL_GIZMO)
		PostMultiplyEventMatrix(maf_event);

	if (!m_UpdateAfterRelease || arg == mafInteractorGenericMouse::MOUSE_UP)
	{
		m_Input->SetAbsMatrix(*m_TransformVME->GetOutput()->GetAbsMatrix());
	}

	if (arg == mafInteractorGenericMouse::MOUSE_UP)
	{
		if (m_RefSystemMode == REF_RELATIVE_CENTER)
		{
			double bounds[6];
			m_RelativeRefSysVME->GetOutput()->GetVMEBounds(bounds);
			m_RelativeCenterRefSysVME->SetAbsPose((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, 0, 0, 0, m_CurrentTime);

			SetRefSysVME(m_RelativeCenterRefSysVME);
		}
		else if (m_RefSystemMode == REF_CENTER)
		{
			double bounds[6];
			m_TransformVME->GetOutput()->GetVMEBounds(bounds);
			m_LocalCenterRefSysVME->SetAbsPose((bounds[0] + bounds[1]) / 2.0, (bounds[2] + bounds[3]) / 2.0, (bounds[4] + bounds[5]) / 2.0, 0, 0, 0, m_CurrentTime);

			SetRefSysVME(m_LocalCenterRefSysVME);
		}
	}

	m_NewAbsMatrix = *(m_RefSysVME->GetOutput()->GetAbsMatrix());
	UpdateTransformTextEntries();
}

//----------------------------------------------------------------------------
void mafOpTransform::PostMultiplyEventMatrix(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_TransformVME->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		mafMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(m_CurrentTime);

		if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
		{
			// move vme
			m_TransformVME->SetAbsMatrix(absPose);
			// update matrix for OpDo()
			m_NewAbsMatrix = absPose;
		}

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));

		// clean up
		tr->Delete();
	}
}

//----------------------------------------------------------------------------
void mafOpTransform::OnEventTransformTextEntries(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	mafSmartPointer<mafTransform> tran;
	tran->Scale(m_Scaling[0], m_Scaling[1], m_Scaling[2], POST_MULTIPLY);
	tran->RotateY(m_Orientation[1], POST_MULTIPLY);
	tran->RotateX(m_Orientation[0], POST_MULTIPLY);
	tran->RotateZ(m_Orientation[2], POST_MULTIPLY);
	tran->SetPosition(m_Position);

	// Postmultiply to ref sys abs matrix
	tran->Concatenate(m_RefSysVME->GetOutput()->GetAbsTransform(), POST_MULTIPLY);

	mafMatrix absPose;
	absPose = tran->GetMatrix();
	absPose.SetTimeStamp(m_CurrentTime);

	m_NewAbsMatrix = absPose;

	m_Input->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);

	// Notify the listener about the new abs pose
	mafEvent e2s;
	e2s.SetSender(this);
	e2s.SetMatrix(tran->GetMatrixPointer());
	e2s.SetId(ID_TRANSFORM);
	mafEventMacro(e2s);
}

//----------------------------------------------------------------------------
void mafOpTransform::Reset()
//----------------------------------------------------------------------------
{
	m_Input->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);
	m_TransformVME->SetAbsMatrix(m_OldAbsMatrix, m_CurrentTime);

	SelectRefSys();
}

//----------------------------------------------------------------------------
void mafOpTransform::UpdateTransformTextEntries()
//----------------------------------------------------------------------------
{
	// Express absPose in RefSysVME ref sys
	mafTransformFrame *mflTr = mafTransformFrame::New();
	mflTr->SetInput(m_TransformVME->GetOutput()->GetAbsMatrix());
	mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());

	mflTr->Update();

	// Update gui with new pose: Position, Orientation, Scale
	mafTransform::GetPosition(mflTr->GetMatrix(), m_Position);
	mafTransform::GetOrientation(mflTr->GetMatrix(), m_Orientation);
	mafTransform::GetScale(mflTr->GetMatrix(), m_Scaling);
	
	mafDEL(mflTr);

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
bool mafOpTransform::AcceptRefSys(mafVME *node)
{
	return node != GLO_TransformInput;
}

