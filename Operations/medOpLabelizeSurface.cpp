/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpLabelizeSurface.cpp,v $
Language:  C++
Date:      $Date: 2007-10-26 13:16:03 $
Version:   $Revision: 1.8 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpLabelizeSurface.h"
#include "mmgGui.h"
#include "mmaMaterial.h"
#include "mafAbsMatrixPipe.h"
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGizmoScale.h"

#include "mafTransform.h"
#include "mafEventSource.h"

#include "mafMatrix.h"
#include "mafNode.h"
#include "mafVMESurface.h"
#include "mafVMEGizmo.h"
#include "medVMESurfaceEditor.h"

#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkArrowSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkProperty.h"
#include "vtkGlyph3D.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkFloatArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkClipSurfaceBoundingBox.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpLabelizeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpLabelizeSurface::medOpLabelizeSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = false;

	m_ImplicitPlaneGizmo = NULL;
	m_ClipperPlane = NULL;
	m_PlaneSource = NULL;
	m_ArrowShape = NULL;
	m_Arrow = NULL;
	m_Gizmo = NULL;
	m_IsaCompositorWithGizmo = NULL;
	m_IsaCompositorWithoutGizmo = NULL;
	m_InputSurface = NULL;
	m_OriginalPolydata = NULL;
	m_ClipperBoundingBox = NULL;

	m_LabelValue = 0.0;

	m_LabelInside = 1;
	m_UseGizmo		= 1;
	m_GizmoType		= GIZMO_TRANSLATE;

	m_PlaneWidth = 0.0;
	m_PlaneHeight = 0.0;

	m_VmeEditor = NULL;

	m_GizmoTranslate=NULL;
	m_GizmoRotate=NULL;
	m_GizmoScale=NULL;

	m_ResultPolyData.clear();
}

//----------------------------------------------------------------------------
medOpLabelizeSurface::~medOpLabelizeSurface()
//----------------------------------------------------------------------------
{
	for(int i=0;i<m_ResultPolyData.size();i++)
	{
		vtkDEL(m_ResultPolyData[i]);
	}
	m_ResultPolyData.clear();

	mafDEL(m_VmeEditor);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
mafOp* medOpLabelizeSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpLabelizeSurface(m_Label);
}

//----------------------------------------------------------------------------
bool medOpLabelizeSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node != NULL && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OpRun()   
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_Input,false));

	mafNEW(m_VmeEditor);
	vtkMAFSmartPointer<vtkPolyData> inputOriginalPolydata;
	inputOriginalPolydata->DeepCopy(vtkPolyData::SafeDownCast(mafVMESurface::SafeDownCast(m_Input)->GetOutput()->GetVTKData()));
	int prova=inputOriginalPolydata->GetNumberOfPoints();
	m_VmeEditor->SetData(inputOriginalPolydata,0.0);
	m_VmeEditor->Modified();
	m_VmeEditor->Update();
	m_VmeEditor->ReparentTo(mafVME::SafeDownCast(m_Input));

	PlaneCreated = false;

	vtkPolyData *inputPolydata=vtkPolyData::SafeDownCast(m_VmeEditor->GetOutput()->GetVTKData());
	inputPolydata->Modified();
	inputPolydata->Update();

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy(inputPolydata);
	m_OriginalPolydata->Modified();
	m_OriginalPolydata->Update();

	if(!inputPolydata->GetCellData()->GetArray("CELL_LABEL"))
	{
		m_VmeEditor->GetMaterial()->m_ColorLut->SetNumberOfTableValues(1);
		m_VmeEditor->GetMaterial()->m_ColorLut->SetTableValue(0,1.0,1.0,1.0);
		m_VmeEditor->GetMaterial()->m_ColorLut->Build();

		vtkMAFSmartPointer<vtkDoubleArray> cellScalar;
		cellScalar->SetName("CELL_LABEL");
		cellScalar->SetNumberOfComponents(1);
		cellScalar->SetNumberOfTuples(inputPolydata->GetNumberOfCells());
		for(int i=0;i<inputPolydata->GetNumberOfCells();i++)
		{
			cellScalar->InsertTuple1(i,0.0);
		}

		inputPolydata->GetCellData()->SetScalars(cellScalar);
		inputPolydata->GetCellData()->Update();

		inputPolydata->Modified();
		inputPolydata->Update();
	}
	else
	{
		inputPolydata->GetCellData()->SetActiveScalars("CELL_LABEL");
		inputPolydata->Modified();
		inputPolydata->Update();
	}

	m_VmeEditor->Modified();
	m_VmeEditor->Update();

	mafEventMacro(mafEvent(this,VME_SHOW,m_VmeEditor,true));

	vtkPolyData *initialData;
	vtkNEW(initialData);
	initialData->DeepCopy(inputPolydata);
	m_ResultPolyData.push_back(initialData);

	vtkNEW(m_ClipperBoundingBox);

	ShowClipPlane(true);

	if(!m_TestMode)
	{
		//m_VmeEditor->GetOutput()->SetMaterial()
		CreateGui();
		ShowGui();

		CreateGizmos();
		//Enable & show Gizmos
		ChangeGizmo();
	}
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum CLIP_SURFACE_ID
{
	ID_LABEL_VALUE = MINID,
	ID_UNDO,
	ID_PLANE_WIDTH,
	ID_PLANE_HEIGHT,
	ID_USE_GIZMO,
	ID_CHOOSE_GIZMO,
	ID_LUT,
};
//----------------------------------------------------------------------------
void medOpLabelizeSurface::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);

	m_Gui->Bool(ID_USE_GIZMO,_("use gizmo"),&m_UseGizmo,1);
	wxString gizmo_name[3] = {"translate","rotate","scale"};
	m_Gui->Combo(ID_CHOOSE_GIZMO,_("gizmo"),&m_GizmoType,3,gizmo_name);

	m_Gui->Double(ID_LABEL_VALUE,_("Label"),&m_LabelValue);
	m_Gui->Lut(ID_LUT,"lut",m_VmeEditor->GetMaterial()->m_ColorLut);
	double b[6];
	((mafVME *)m_Input)->GetOutput()->GetVMEBounds(b);
	// bounding box dim
	m_PlaneWidth = b[1] - b[0];
	m_PlaneHeight = b[3] - b[2];
	//m_Gui->Double(ID_PLANE_WIDTH,_("plane w."),&m_PlaneWidth,0.0);
	//m_Gui->Double(ID_PLANE_HEIGHT,_("plane h."),&m_PlaneHeight,0.0);
	m_Gui->Divider();
	m_Gui->Button(ID_UNDO,_("Undo"));

	m_Gui->Divider(1);
	m_Gui->OkCancel();

	m_Gui->Divider();
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::CreateGizmos()
//----------------------------------------------------------------------------
{
	m_ImplicitPlaneGizmo->GetOutput()->GetVTKData()->ComputeBounds();
	m_ImplicitPlaneGizmo->Modified();
	m_ImplicitPlaneGizmo->Update();

	m_GizmoTranslate = new mafGizmoTranslate(mafVME::SafeDownCast(m_ImplicitPlaneGizmo), this);
	m_GizmoTranslate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoTranslate->Show(false);
	m_GizmoRotate = new mafGizmoRotate(mafVME::SafeDownCast(m_ImplicitPlaneGizmo), this);
	m_GizmoRotate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoRotate->Show(false);
	m_GizmoScale = new mafGizmoScale(mafVME::SafeDownCast(m_ImplicitPlaneGizmo), this);
	m_GizmoScale->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoScale->Show(false);
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::AttachInteraction()
//----------------------------------------------------------------------------
{
	mafNEW(m_IsaCompositorWithoutGizmo);

	m_IsaRotate = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_LEFT);
	m_IsaRotate->SetListener(this);
	m_IsaRotate->SetVME(m_ImplicitPlaneGizmo);
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetPivotRefSys()->SetTypeToView();
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
	m_IsaRotate->EnableRotation(true);

	m_IsaTranslate = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_MIDDLE);
	m_IsaTranslate->SetListener(this);
	m_IsaTranslate->SetVME(m_ImplicitPlaneGizmo);
	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetPivotRefSys()->SetTypeToView();
	m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
	m_IsaTranslate->EnableTranslation(true);

	m_IsaChangeArrowWithoutGizmo = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_LEFT_SHIFT);
	m_IsaChangeArrowWithoutGizmo->SetListener(this);
	m_IsaChangeArrowWithoutGizmo->SetVME(m_ImplicitPlaneGizmo);

	m_IsaLabelizeWithoutGizmo = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_LEFT_CONTROL);
	m_IsaLabelizeWithoutGizmo->SetListener(this);
	m_IsaLabelizeWithoutGizmo->SetVME(m_ImplicitPlaneGizmo);

	mafNEW(m_IsaCompositorWithGizmo);

	m_IsaChangeArrowWithGizmo = m_IsaCompositorWithGizmo->CreateBehavior(MOUSE_LEFT_SHIFT);
	m_IsaChangeArrowWithGizmo->SetListener(this);
	m_IsaChangeArrowWithGizmo->SetVME(m_ImplicitPlaneGizmo);

	m_IsaLabelizeWithGizmo = m_IsaCompositorWithGizmo->CreateBehavior(MOUSE_LEFT_CONTROL);
	m_IsaLabelizeWithGizmo->SetListener(this);
	m_IsaLabelizeWithGizmo->SetVME(m_ImplicitPlaneGizmo);

	if(!m_UseGizmo)
		m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositorWithoutGizmo);
	else
		m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositorWithGizmo);
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::ShowClipPlane(bool show)
//----------------------------------------------------------------------------
{
	if(show)
	{
		if(m_ClipperPlane == NULL)
		{
			double b[6];
			((mafVME *)m_Input)->GetOutput()->GetBounds(b);

			// bounding box dim
			double xdim = b[1] - b[0];
			double ydim = b[3] - b[2];
			double zdim = b[5] - b[4];
			
			//m_PlaneWidth = xdim;
			//m_PlaneHeight = ydim;

			((mafVME *)m_Input)->GetOutput()->GetBounds(b);
			// bounding box dim
			m_PlaneWidth = b[1] - b[0];
			m_PlaneHeight = b[3] - b[2];

			// create the gizmo plane on the z = 0 plane
			vtkNEW(m_PlaneSource);
			m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
			m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
			m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
			m_PlaneSource->Update();

			vtkNEW(m_ClipperPlane);
			m_ClipperPlane->SetOrigin(m_PlaneSource->GetOrigin());
			m_ClipperPlane->SetNormal(m_PlaneSource->GetNormal());

			vtkNEW(m_ArrowShape);
			m_ArrowShape->SetShaftResolution(40);
			m_ArrowShape->SetTipResolution(40);

			vtkNEW(m_Arrow);
			m_Arrow->SetInput(m_PlaneSource->GetOutput());
			m_Arrow->SetSource(m_ArrowShape->GetOutput());
			m_Arrow->SetVectorModeToUseNormal();

			int clip_sign = m_LabelInside ? 1 : -1;
			m_Arrow->SetScaleFactor(clip_sign * abs(zdim/10.0));
			m_Arrow->Update();

			vtkNEW(m_Gizmo);
			m_Gizmo->AddInput(m_PlaneSource->GetOutput());
			m_Gizmo->AddInput(m_Arrow->GetOutput());
			m_Gizmo->Update();

			mafNEW(m_ImplicitPlaneGizmo);
			m_ImplicitPlaneGizmo->SetData(m_Gizmo->GetOutput());
			m_ImplicitPlaneGizmo->SetName("implicit plane gizmo");
			m_ImplicitPlaneGizmo->ReparentTo(mafVME::SafeDownCast(m_Input->GetRoot()));

			// position the plane
			mafSmartPointer<mafTransform> currTr;
			currTr->Translate((b[0] + b[1]) / 2, (b[2] + b[3]) / 2,(b[4] + b[5]) / 2 , POST_MULTIPLY);
			currTr->Update();

			mafMatrix mat;
			mat.DeepCopy(&currTr->GetMatrix());
			mat.SetTimeStamp(((mafVME *)m_Input)->GetTimeStamp());

			m_ImplicitPlaneGizmo->SetAbsMatrix(mat);

			mafEventMacro(mafEvent(this,VME_SHOW,m_ImplicitPlaneGizmo,true));
		}
		mmaMaterial *material = m_ImplicitPlaneGizmo->GetMaterial();
		material->m_Prop->SetOpacity(0.5);
		material->m_Opacity = material->m_Prop->GetOpacity();

		if(!PlaneCreated)
		{
			material->m_Prop->SetColor(0.2,0.2,0.8);
			material->m_Prop->GetDiffuseColor(material->m_Diffuse);
			AttachInteraction();
			PlaneCreated = true;
		}
	}
	else
	{
		if(m_ImplicitPlaneGizmo != NULL)
		{
			mmaMaterial *material = m_ImplicitPlaneGizmo->GetMaterial();
			material->m_Prop->SetOpacity(0);
			material->m_Opacity = material->m_Prop->GetOpacity();
		}
	}

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (maf_event->GetSender() == this->m_Gui) // from this operation gui
	{
		OnEventThis(maf_event); 
	}
	else if(maf_event->GetSender() == this->m_GizmoTranslate)
	{
		OnEventGizmoTranslate(maf_event);
	}
	else if(maf_event->GetSender() == this->m_GizmoRotate)
	{
		OnEventGizmoRotate(maf_event);
	}
	else if(maf_event->GetSender() == this->m_GizmoScale)
	{
		OnEventGizmoScale(maf_event);
	}
	else
	{
		OnEventGizmoPlane(maf_event);
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ID_UNDO:
			{
				Undo();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case ID_PLANE_WIDTH:
		case ID_PLANE_HEIGHT:
			{
				SetPlaneDimension();
			}
			break;
		case ID_USE_GIZMO:
			{
				if(m_IsaCompositorWithoutGizmo && !m_UseGizmo)
					m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositorWithoutGizmo);
				else if(m_IsaCompositorWithGizmo && m_UseGizmo)
					m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositorWithGizmo);


				ChangeGizmo();
			}
			break;
		case ID_CHOOSE_GIZMO:
			{
				ChangeGizmo();
			}
			break;
		case wxOK:
			{
				OpStop(OP_RUN_OK);
			}
			break;
		case wxCANCEL:
			{
				OpStop(OP_RUN_CANCEL);
			}
			break;
		default:
			mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::SetPlaneDimension(double w,double h)
//----------------------------------------------------------------------------
{
	m_PlaneWidth=w;
	m_PlaneHeight=h;
	SetPlaneDimension();
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::SetPlaneDimension()
//----------------------------------------------------------------------------
{
	if(PlaneCreated)
	{
		m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
		m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
		m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
		m_PlaneSource->Update();
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OnEventGizmoPlane(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(maf_event->GetId())
		{
		case ID_TRANSFORM:
			{
				if(e->GetSender()==m_IsaChangeArrowWithGizmo || e->GetSender()==m_IsaChangeArrowWithoutGizmo)
				{
					if(m_Arrow) 
					{
						if(e->GetArg()==mmiGenericMouse::MOUSE_DOWN)
						{
							m_LabelInside= m_LabelInside ? 0 : 1;
							m_Gui->Update();
							m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
							mafEventMacro(mafEvent(this, CAMERA_UPDATE));
						}
					}
				}
				else if(e->GetSender()==m_IsaLabelizeWithGizmo || e->GetSender()==m_IsaLabelizeWithoutGizmo)
				{
					if(e->GetArg()==mmiGenericMouse::MOUSE_DOWN)
					{
						Labelize();
						mafEventMacro(mafEvent(this, CAMERA_UPDATE));
					}
				}
				else
				{
					vtkTransform *currTr = vtkTransform::New();
					currTr->PostMultiply();
					currTr->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
					currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
					currTr->Update();

					mafMatrix newAbsMatr;
					newAbsMatr.DeepCopy(currTr->GetMatrix());
					newAbsMatr.SetTimeStamp(m_ImplicitPlaneGizmo->GetTimeStamp());

					// set the new pose to the gizmo
					m_ImplicitPlaneGizmo->SetAbsMatrix(newAbsMatr);
					UpdateISARefSys();

					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
					currTr->Delete();
				}
			}
			break;
		default:
			{
				mafEventMacro(*maf_event);
			}
		}
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::Undo()
//----------------------------------------------------------------------------
{
	if(m_ResultPolyData.size()>1)
	{
		vtkDEL(m_ResultPolyData[m_ResultPolyData.size()-1]);
		m_ResultPolyData.pop_back();
		((mafVMESurface*)m_VmeEditor)->SetData((vtkPolyData*)m_ResultPolyData[m_ResultPolyData.size()-1],((mafVME*)m_Input)->GetTimeStamp());
		
		((mafVMESurface*)m_VmeEditor)->GetEventSource()->InvokeEvent(m_VmeEditor,VME_OUTPUT_DATA_UPDATE);
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());

	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_VmeEditor,false));
	m_VmeEditor->ReparentTo(NULL);

	if(m_ImplicitPlaneGizmo)
	{
		m_ImplicitPlaneGizmo->SetBehavior(NULL);
		mafEventMacro(mafEvent(this, VME_REMOVE, m_ImplicitPlaneGizmo));
	}
	mafDEL(m_ImplicitPlaneGizmo);

	vtkDEL(m_Gizmo);
	vtkDEL(m_ArrowShape);
	vtkDEL(m_PlaneSource);
	vtkDEL(m_ClipperBoundingBox);
	vtkDEL(m_Arrow);
	vtkDEL(m_ClipperPlane);

	mafDEL(m_IsaCompositorWithoutGizmo);
	mafDEL(m_IsaCompositorWithGizmo);

	if(m_GizmoTranslate)
		m_GizmoTranslate->Show(false);
	if(m_GizmoRotate)
		m_GizmoRotate->Show(false);
	if(m_GizmoScale)
		m_GizmoScale->Show(false);
	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
	cppDEL(m_GizmoScale);

	mafEventMacro(mafEvent(this,VME_SHOW,m_Input,true));

	if(!m_TestMode)
		HideGui();

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OpDo()
//----------------------------------------------------------------------------
{
	if(m_VmeEditor)
	{
		mafVMESurface::SafeDownCast(m_Input)->SetData(vtkPolyData::SafeDownCast(m_VmeEditor->GetOutput()->GetVTKData()),((mafVMESurface*)m_Input)->GetTimeStamp());
		mafSmartPointer<mmaMaterial> mat;
		mat->DeepCopy(m_VmeEditor->GetMaterial());
		mafVMESurface::SafeDownCast(m_Input)->GetSurfaceOutput()->SetMaterial(mat);
		mafVMESurface::SafeDownCast(m_Input)->GetSurfaceOutput()->Update();
		mafVMESurface::SafeDownCast(m_Input)->GetOutput()->Update();
		mafEventMacro(mafEvent(this,VME_SHOW,m_Input,false));
		mafEventMacro(mafEvent(this,VME_SHOW,m_Input,true));
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OpUndo()
//----------------------------------------------------------------------------
{
	mafVMESurface::SafeDownCast(m_Input)->SetData(m_OriginalPolydata,((mafVMESurface*)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::Labelize()
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkTransform> rotate;
	rotate->RotateX(180);

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> before_transform_plane;
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_plane;
	if(m_LabelInside==1)//if clip reverse is necessary rotate plane
	{
		before_transform_plane->SetTransform(rotate);
		before_transform_plane->SetInput(m_PlaneSource->GetOutput());
		before_transform_plane->Update();

		transform_plane->SetInput(before_transform_plane->GetOutput());
	}
	else
		transform_plane->SetInput(m_PlaneSource->GetOutput());


	transform_plane->SetTransform(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform());
	transform_plane->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_data_input;
	transform_data_input->SetTransform(((mafVME*)m_VmeEditor)->GetAbsMatrixPipe()->GetVTKTransform());
	transform_data_input->SetInput((vtkPolyData *)((mafVME *)m_VmeEditor)->GetOutput()->GetVTKData());
	transform_data_input->Update();

	m_ClipperBoundingBox->SetInput(transform_data_input->GetOutput());
	m_ClipperBoundingBox->SetMask(transform_plane->GetOutput());
	m_ClipperBoundingBox->SetClipInside(1);
	m_ClipperBoundingBox->Update();

	vtkMAFSmartPointer<vtkPolyData> newPolyData1;
	newPolyData1->DeepCopy(m_ClipperBoundingBox->GetOutput());
	newPolyData1->Update();

	vtkDoubleArray *cellScalar = vtkDoubleArray::SafeDownCast(newPolyData1->GetCellData()->GetArray("CELL_LABEL"));
	if(cellScalar)
	{
		for(int i=0;i<newPolyData1->GetNumberOfCells();i++)
		{
			cellScalar->SetTuple1(i,m_LabelValue);
		}
		newPolyData1->GetCellData()->Update();
	}

	double range[2];
	newPolyData1->GetScalarRange(range);

	m_ClipperBoundingBox->SetClipInside(0);
	m_ClipperBoundingBox->Modified();
	m_ClipperBoundingBox->Update();

	vtkMAFSmartPointer<vtkPolyData> newPolyData2;
	newPolyData2->DeepCopy(m_ClipperBoundingBox->GetOutput());
	newPolyData2->Update();

	vtkMAFSmartPointer<vtkAppendPolyData> append;
	append->SetInput(newPolyData1);
	append->AddInput(newPolyData2);
	append->Update();


	vtkMAFSmartPointer<vtkCleanPolyData> clean;
	clean->SetInput(append.GetPointer()->GetOutput());
	clean->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_data_output;
	transform_data_output->SetTransform(((mafVME*)m_VmeEditor)->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
	transform_data_output->SetInput(clean->GetOutput());
	transform_data_output->Update();

	int result=(m_VmeEditor)->SetData(transform_data_output->GetOutput(),m_VmeEditor->GetTimeStamp());

	if(result==MAF_OK)
	{
		vtkPolyData *poly;
		vtkNEW(poly);
		poly->DeepCopy(transform_data_output->GetOutput());
		poly->Update();
		m_ResultPolyData.push_back(poly);
	}

	((mafVMESurface*)m_VmeEditor)->Modified();
	((mafVMESurface*)m_VmeEditor)->Update();

	((mafVMESurface*)m_VmeEditor)->GetEventSource()->InvokeEvent(m_VmeEditor,VME_OUTPUT_DATA_UPDATE);

	/*m_Gui->Enable(ID_UNDO,m_ResultPolyData.size()>1);
	m_Gui->Enable(wxOK,m_ResultPolyData.size()>1);*/
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::ChangeGizmo()
//----------------------------------------------------------------------------
{
	m_GizmoTranslate->Show(m_GizmoType==GIZMO_TRANSLATE && m_UseGizmo);
	m_GizmoRotate->Show(m_GizmoType==GIZMO_ROTATE && m_UseGizmo);
	m_GizmoScale->Show(m_GizmoType==GIZMO_SCALE && m_UseGizmo,m_GizmoType==GIZMO_SCALE && m_UseGizmo,false,m_GizmoType==GIZMO_SCALE && m_UseGizmo);

	if(m_UseGizmo)
	{
		switch(m_GizmoType)
		{
		case GIZMO_TRANSLATE:
			m_GizmoTranslate->SetRefSys(m_ImplicitPlaneGizmo);
			break;
		case GIZMO_ROTATE:
			m_GizmoRotate->SetRefSys(m_ImplicitPlaneGizmo);
			break;
		case GIZMO_SCALE:
			m_GizmoScale->SetRefSys(m_ImplicitPlaneGizmo);
			break;
		}
	}
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::PostMultiplyEventMatrix(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(((mafVME *)m_ImplicitPlaneGizmo)->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		mafMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(0.0);

		if (arg == mmiGenericMouse::MOUSE_MOVE)
		{
			// move vme
			((mafVME *)m_ImplicitPlaneGizmo)->SetAbsMatrix(absPose);
			// update matrix for OpDo()
			//m_NewAbsMatrix = absPose;
		} 
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OnEventGizmoTranslate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OnEventGizmoRotate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::OnEventGizmoScale(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{ 
			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void medOpLabelizeSurface::SetLutEditor(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
	if(m_VmeEditor)
	{
		m_VmeEditor->GetMaterial()->m_ColorLut->DeepCopy(lut);
		m_VmeEditor->GetMaterial()->UpdateProp();
		m_VmeEditor->Update();
	}
}