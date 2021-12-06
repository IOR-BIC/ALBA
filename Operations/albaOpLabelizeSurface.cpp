/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpLabelizeSurface
 Authors: Matteo Giacomoni
 
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

#include "albaOpLabelizeSurface.h"
#include "albaGUI.h"
#include "mmaMaterial.h"
#include "albaAbsMatrixPipe.h"
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGizmoScale.h"
#include "albaRefSys.h"
#include "albaTransform.h"

#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMEGizmo.h"
#include "albaVMESurfaceEditor.h"

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
#include "vtkALBAClipSurfaceBoundingBox.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpLabelizeSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpLabelizeSurface::albaOpLabelizeSurface(const wxString &label) :
albaOp(label)
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
albaOpLabelizeSurface::~albaOpLabelizeSurface()
//----------------------------------------------------------------------------
{
	for(int i=0;i<m_ResultPolyData.size();i++)
	{
		vtkDEL(m_ResultPolyData[i]);
	}
	m_ResultPolyData.clear();

	albaDEL(m_VmeEditor);
	vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
albaOp* albaOpLabelizeSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpLabelizeSurface(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpLabelizeSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node != NULL && node->IsALBAType(albaVMESurface));
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OpRun()   
//----------------------------------------------------------------------------
{
	GetLogicManager()->VmeShow(m_Input, false);

	albaNEW(m_VmeEditor);
	vtkALBASmartPointer<vtkPolyData> inputOriginalPolydata;
	inputOriginalPolydata->DeepCopy(vtkPolyData::SafeDownCast(albaVMESurface::SafeDownCast(m_Input)->GetOutput()->GetVTKData()));
	int prova=inputOriginalPolydata->GetNumberOfPoints();
	m_VmeEditor->SetData(inputOriginalPolydata,0.0);
	m_VmeEditor->Modified();
	m_VmeEditor->Update();
	m_VmeEditor->ReparentTo(m_Input);

	m_PlaneCreated = false;

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

		vtkALBASmartPointer<vtkDoubleArray> cellScalar;
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

	GetLogicManager()->VmeShow(m_VmeEditor, true);

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
void albaOpLabelizeSurface::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);

	m_Gui->Bool(ID_USE_GIZMO,_("use gizmo"),&m_UseGizmo,1);
	wxString gizmo_name[3] = {"translate","rotate","scale"};
	m_Gui->Combo(ID_CHOOSE_GIZMO,_("gizmo"),&m_GizmoType,3,gizmo_name);

	m_Gui->Double(ID_LABEL_VALUE,_("Label"),&m_LabelValue);
	m_Gui->Lut(ID_LUT,"lut",m_VmeEditor->GetMaterial()->m_ColorLut);
	double b[6];
	m_Input->GetOutput()->GetVMEBounds(b);
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
void albaOpLabelizeSurface::CreateGizmos()
//----------------------------------------------------------------------------
{
	m_ImplicitPlaneGizmo->GetOutput()->GetVTKData()->ComputeBounds();
	m_ImplicitPlaneGizmo->Modified();
	m_ImplicitPlaneGizmo->Update();

	m_GizmoTranslate = new albaGizmoTranslate(m_ImplicitPlaneGizmo, this);
	m_GizmoTranslate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoTranslate->Show(false);
	m_GizmoRotate = new albaGizmoRotate(m_ImplicitPlaneGizmo, this);
	m_GizmoRotate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoRotate->Show(false);
	m_GizmoScale = new albaGizmoScale(m_ImplicitPlaneGizmo, this);
	m_GizmoScale->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoScale->Show(false);
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::AttachInteraction()
//----------------------------------------------------------------------------
{
	albaNEW(m_IsaCompositorWithoutGizmo);

	m_IsaRotate = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_LEFT);
	m_IsaRotate->SetListener(this);
	m_IsaRotate->SetVME(m_ImplicitPlaneGizmo);
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetPivotRefSys()->SetTypeToView();
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
	m_IsaRotate->EnableRotation(true);

	m_IsaTranslate = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_MIDDLE);
	m_IsaTranslate->SetListener(this);
	m_IsaTranslate->SetVME(m_ImplicitPlaneGizmo);
	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetPivotRefSys()->SetTypeToView();
	m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
	m_IsaTranslate->EnableTranslation(true);

	m_IsaChangeArrowWithoutGizmo = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_LEFT_SHIFT);
	m_IsaChangeArrowWithoutGizmo->SetListener(this);
	m_IsaChangeArrowWithoutGizmo->SetVME(m_ImplicitPlaneGizmo);

	m_IsaLabelizeWithoutGizmo = m_IsaCompositorWithoutGizmo->CreateBehavior(MOUSE_LEFT_CONTROL);
	m_IsaLabelizeWithoutGizmo->SetListener(this);
	m_IsaLabelizeWithoutGizmo->SetVME(m_ImplicitPlaneGizmo);

	albaNEW(m_IsaCompositorWithGizmo);

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
void albaOpLabelizeSurface::ShowClipPlane(bool show)
//----------------------------------------------------------------------------
{
	if(show)
	{
		if(m_ClipperPlane == NULL)
		{
			double b[6];
			m_Input->GetOutput()->GetBounds(b);

			// bounding box dim
			double xdim = b[1] - b[0];
			double ydim = b[3] - b[2];
			double zdim = b[5] - b[4];
			
			//m_PlaneWidth = xdim;
			//m_PlaneHeight = ydim;

			m_Input->GetOutput()->GetBounds(b);
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

			albaNEW(m_ImplicitPlaneGizmo);
			m_ImplicitPlaneGizmo->SetData(m_Gizmo->GetOutput());
			m_ImplicitPlaneGizmo->SetName("implicit plane gizmo");
			m_ImplicitPlaneGizmo->ReparentTo(m_Input->GetRoot());

			// position the plane
			albaSmartPointer<albaTransform> currTr;
			currTr->Translate((b[0] + b[1]) / 2, (b[2] + b[3]) / 2,(b[4] + b[5]) / 2 , POST_MULTIPLY);
			currTr->Update();

			albaMatrix mat;
			mat.DeepCopy(&currTr->GetMatrix());
			mat.SetTimeStamp(m_Input->GetTimeStamp());

			m_ImplicitPlaneGizmo->SetAbsMatrix(mat);

			GetLogicManager()->VmeShow(m_ImplicitPlaneGizmo, true);
		}
		mmaMaterial *material = m_ImplicitPlaneGizmo->GetMaterial();
		material->m_Prop->SetOpacity(0.5);
		material->m_Opacity = material->m_Prop->GetOpacity();

		if(!m_PlaneCreated)
		{
			material->m_Prop->SetColor(0.2,0.2,0.8);
			material->m_Prop->GetDiffuseColor(material->m_Diffuse);
			AttachInteraction();
			m_PlaneCreated = true;
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

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (alba_event->GetSender() == this->m_Gui) // from this operation gui
	{
		OnEventThis(alba_event); 
	}
	else if(alba_event->GetSender() == this->m_GizmoTranslate)
	{
		OnEventGizmoTranslate(alba_event);
	}
	else if(alba_event->GetSender() == this->m_GizmoRotate)
	{
		OnEventGizmoRotate(alba_event);
	}
	else if(alba_event->GetSender() == this->m_GizmoScale)
	{
		OnEventGizmoScale(alba_event);
	}
	else
	{
		OnEventGizmoPlane(alba_event);
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OnEventThis(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
		case ID_UNDO:
			{
				Undo();
				GetLogicManager()->CameraUpdate();
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
			albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::SetPlaneDimension(double w,double h)
//----------------------------------------------------------------------------
{
	m_PlaneWidth=w;
	m_PlaneHeight=h;
	SetPlaneDimension();
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::SetPlaneDimension()
//----------------------------------------------------------------------------
{
	if(m_PlaneCreated)
	{
		m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
		m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
		m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
		m_PlaneSource->Update();
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OnEventGizmoPlane(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(alba_event->GetId())
		{
		case ID_TRANSFORM:
			{
				if(e->GetSender()==m_IsaChangeArrowWithGizmo || e->GetSender()==m_IsaChangeArrowWithoutGizmo)
				{
					if(m_Arrow) 
					{
						if(e->GetArg()==albaInteractorGenericMouse::MOUSE_DOWN)
						{
							m_LabelInside= m_LabelInside ? 0 : 1;
							m_Gui->Update();
							m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
							GetLogicManager()->CameraUpdate();
						}
					}
				}
				else if(e->GetSender()==m_IsaLabelizeWithGizmo || e->GetSender()==m_IsaLabelizeWithoutGizmo)
				{
					if(e->GetArg()==albaInteractorGenericMouse::MOUSE_DOWN)
					{
						Labelize();
						GetLogicManager()->CameraUpdate();
					}
				}
				else
				{
					vtkTransform *currTr = vtkTransform::New();
					currTr->PostMultiply();
					currTr->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
					currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
					currTr->Update();

					albaMatrix newAbsMatr;
					newAbsMatr.DeepCopy(currTr->GetMatrix());
					newAbsMatr.SetTimeStamp(m_ImplicitPlaneGizmo->GetTimeStamp());

					// set the new pose to the gizmo
					m_ImplicitPlaneGizmo->SetAbsMatrix(newAbsMatr);
					UpdateISARefSys();

					GetLogicManager()->CameraUpdate();
					currTr->Delete();
				}
			}
			break;
		default:
			{
				albaEventMacro(*alba_event);
			}
		}
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::Undo()
//----------------------------------------------------------------------------
{
	if(m_ResultPolyData.size()>1)
	{
		vtkDEL(m_ResultPolyData[m_ResultPolyData.size()-1]);
		m_ResultPolyData.pop_back();
		((albaVMESurface*)m_VmeEditor)->SetData((vtkPolyData*)m_ResultPolyData[m_ResultPolyData.size()-1],m_Input->GetTimeStamp());
		
		((albaVMESurface*)m_VmeEditor)->InvokeEvent(this,VME_OUTPUT_DATA_UPDATE);
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());

	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	GetLogicManager()->VmeShow(m_VmeEditor, false);
	m_VmeEditor->ReparentTo(NULL);

	if(m_ImplicitPlaneGizmo)
	{
		m_ImplicitPlaneGizmo->SetBehavior(NULL);
		GetLogicManager()->VmeRemove(m_ImplicitPlaneGizmo);
	}
	albaDEL(m_ImplicitPlaneGizmo);

	vtkDEL(m_Gizmo);
	vtkDEL(m_ArrowShape);
	vtkDEL(m_PlaneSource);
	vtkDEL(m_ClipperBoundingBox);
	vtkDEL(m_Arrow);
	vtkDEL(m_ClipperPlane);

	albaDEL(m_IsaCompositorWithoutGizmo);
	albaDEL(m_IsaCompositorWithGizmo);

	if(m_GizmoTranslate)
		m_GizmoTranslate->Show(false);
	if(m_GizmoRotate)
		m_GizmoRotate->Show(false);
	if(m_GizmoScale)
		m_GizmoScale->Show(false);
	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
	cppDEL(m_GizmoScale);

	GetLogicManager()->VmeShow(m_Input, true);

	if(!m_TestMode)
		HideGui();

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OpDo()
//----------------------------------------------------------------------------
{
	if(m_VmeEditor)
	{
		albaVMESurface::SafeDownCast(m_Input)->SetData(vtkPolyData::SafeDownCast(m_VmeEditor->GetOutput()->GetVTKData()),((albaVMESurface*)m_Input)->GetTimeStamp());
		albaSmartPointer<mmaMaterial> mat;
		mat->DeepCopy(m_VmeEditor->GetMaterial());
		albaVMESurface::SafeDownCast(m_Input)->GetSurfaceOutput()->SetMaterial(mat);
		albaVMESurface::SafeDownCast(m_Input)->GetSurfaceOutput()->Update();
		albaVMESurface::SafeDownCast(m_Input)->GetOutput()->Update();
		GetLogicManager()->VmeVisualModeChanged(m_Input);
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OpUndo()
//----------------------------------------------------------------------------
{
	albaVMESurface::SafeDownCast(m_Input)->SetData(m_OriginalPolydata,((albaVMESurface*)m_Input)->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::Labelize()
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkTransform> rotate;
	rotate->RotateX(180);

	vtkALBASmartPointer<vtkTransformPolyDataFilter> before_transform_plane;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_plane;
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

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_input;
	transform_data_input->SetTransform(m_VmeEditor->GetAbsMatrixPipe()->GetVTKTransform());
	transform_data_input->SetInput((vtkPolyData *)m_VmeEditor->GetOutput()->GetVTKData());
	transform_data_input->Update();

	m_ClipperBoundingBox->SetInput(transform_data_input->GetOutput());
	m_ClipperBoundingBox->SetMask(transform_plane->GetOutput());
	m_ClipperBoundingBox->SetClipInside(1);
	m_ClipperBoundingBox->Update();

	vtkALBASmartPointer<vtkPolyData> newPolyData1;
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

	vtkALBASmartPointer<vtkPolyData> newPolyData2;
	newPolyData2->DeepCopy(m_ClipperBoundingBox->GetOutput());
	newPolyData2->Update();

	vtkALBASmartPointer<vtkAppendPolyData> append;
	append->SetInput(newPolyData1);
	append->AddInput(newPolyData2);
	append->Update();


	vtkALBASmartPointer<vtkCleanPolyData> clean;
	clean->SetInput(append.GetPointer()->GetOutput());
	clean->Update();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_output;
	transform_data_output->SetTransform(m_VmeEditor->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
	transform_data_output->SetInput(clean->GetOutput());
	transform_data_output->Update();

	int result=(m_VmeEditor)->SetData(transform_data_output->GetOutput(),m_VmeEditor->GetTimeStamp());

	if(result==ALBA_OK)
	{
		vtkPolyData *poly;
		vtkNEW(poly);
		poly->DeepCopy(transform_data_output->GetOutput());
		poly->Update();
		m_ResultPolyData.push_back(poly);
	}

	((albaVMESurface*)m_VmeEditor)->Modified();
	((albaVMESurface*)m_VmeEditor)->Update();

	((albaVMESurface*)m_VmeEditor)->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

	/*m_Gui->Enable(ID_UNDO,m_ResultPolyData.size()>1);
	m_Gui->Enable(wxOK,m_ResultPolyData.size()>1);*/
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::ChangeGizmo()
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
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::PostMultiplyEventMatrix(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_ImplicitPlaneGizmo->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		albaMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(0.0);

		if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
		{
			// move vme
			m_ImplicitPlaneGizmo->SetAbsMatrix(absPose);
			// update matrix for OpDo()
			//m_NewAbsMatrix = absPose;
		} 
		GetLogicManager()->CameraUpdate();

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OnEventGizmoTranslate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OnEventGizmoRotate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::OnEventGizmoScale(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{ 
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{ 
		GetLogicManager()->CameraUpdate();
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpLabelizeSurface::SetLutEditor(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
	if(m_VmeEditor)
	{
		m_VmeEditor->GetMaterial()->m_ColorLut->DeepCopy(lut);
		m_VmeEditor->GetMaterial()->UpdateProp();
		m_VmeEditor->Update();
	}
}
