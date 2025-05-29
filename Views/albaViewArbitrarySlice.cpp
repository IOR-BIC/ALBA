/*=========================================================================

Program: ALBA
Module: albaViewArbitrarySlice
Authors: Eleonora Mambrini , Stefano Perticoni, Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include "albaDecl.h"
#include "albaViewArbitrarySlice.h"
#include "albaGUI.h"
#include "albaViewSlice.h"
#include "albaVme.h"
#include "albaVMESlicer.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaPipeImage3D.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipeSurface.h"
#include "albaPipeSurfaceTextured.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaSceneGraph.h"
#include "albaEvent.h"
#include "albaAbsMatrixPipe.h"
#include "albaAttachCamera.h"
#include "albaInteractorGenericMouse.h"
#include "albaTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEIterator.h"
#include "albaGUILutPreset.h"
#include "albaVMEOutputSurface.h"
#include "albaAttribute.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaPipeMesh.h"
#include "albaPipeMeshSlice.h"
#include "albaPipePolylineGraphEditor.h"

#include "vtkTransform.h"
#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "albaTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkPolyDataNormals.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "albaPipeSlice.h"
#include "albaPipeVolumeArbSlice.h"
#include "albaGUIPicButton.h"
#include "albaRefSys.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipeMeshSlice.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewArbitrarySlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum ARBITRARY_SUBVIEW_ID
{
	ARBITRARY_VIEW = 0,
	SLICE_VIEW,
};
enum GIZMO_TYPE_ID
{
	GIZMO_TRANSLATE = 0,
	GIZMO_ROTATE,
};
enum AXIS_ID
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
};

//----------------------------------------------------------------------------
albaViewArbitrarySlice::albaViewArbitrarySlice(wxString label, bool show_ruler)
	: albaViewCompoundWindowing(label, 1, 2)
{
	m_ViewArbitrary   = NULL;
	m_ViewSlice       = NULL;
	m_GizmoTranslate  = NULL;
	m_GizmoRotate     = NULL;
	m_MatrixReset     = NULL;
	m_InputVolume   = NULL;
	m_GuiGizmos       = NULL;
	m_AttachCamera    = NULL;

	m_SliceCenterSurface[0] = 0.0;
	m_SliceCenterSurface[1] = 0.0;
	m_SliceCenterSurface[2] = 0.0;
	m_SliceCenterSurface[3] = 1.0;


	m_SliceCenterSurfaceReset[0] = 0.0;
	m_SliceCenterSurfaceReset[1] = 0.0;
	m_SliceCenterSurfaceReset[2] = 0.0;
	m_SliceCenterSurfaceReset[3] = 1.0;

	m_TypeGizmo = GIZMO_TRANSLATE;

	albaNEW(m_SlicingMatrix);

	m_AllSurface = 0;
	m_Border = 1;

	m_TrilinearInterpolationOn = true;
	m_CameraFollowGizmo = false;
	m_EnableGPU = true;
	m_SkipCameraUpdate = 0;
}
//----------------------------------------------------------------------------
albaViewArbitrarySlice::~albaViewArbitrarySlice()
{
	m_MatrixReset   = NULL;
	m_InputVolume = NULL;
	m_ColorLUT      = NULL;

	albaDEL(m_SlicingMatrix);
}

//----------------------------------------------------------------------------
char ** albaViewArbitrarySlice::GetIcon()
{
#include "pic/VIEW_ARBITRARY.xpm"
	return VIEW_ARBITRARY_xpm;
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::PackageView()
{
	m_ViewArbitrary = new albaViewVTK("", CAMERA_PERSPECTIVE);
	m_ViewArbitrary->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeArbSlice", MUTEX);
	m_ViewArbitrary->PlugVisualPipe("albaVMELabeledVolume", "albaPipeBox", MUTEX);

	// Create And Plug Slice View
	m_ViewSlice = new albaViewVTK("", CAMERA_OS_Z);
	m_ViewSlice->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeArbSlice", MUTEX);
	m_ViewSlice->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
	m_ViewSlice->PlugVisualPipe("albaVMEGizmo", "albaPipeGizmo", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("albaVMEPointCloud", "albaPipeBox", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMERefSys", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");
	
	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSlice);
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VmeShow(albaVME *vme, bool show)
{
	m_SkipCameraUpdate++;

	vme->Update();
	if (show == false && vme->GetOutput()->IsA("albaVMEOutputVolume"))
		OnReset();

	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(vme, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(vme, show);

	if (show)
	{
		if (vme != m_InputVolume)
		{
			if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
			{
				double sliceCenterVolumeReset[4];
				sliceCenterVolumeReset[3] = 1;

				m_InputVolume = albaVMEVolumeGray::SafeDownCast(vme);

				// get the VTK volume
				vtkDataSet *data = vme->GetOutput()->GetVTKData();
				if (data == NULL)
					return;
				data->Update();
				//Get center of Volume to can the reset
				data->GetCenter(sliceCenterVolumeReset);

				albaTransform::GetOrientation(vme->GetAbsMatrixPipe()->GetMatrix(), m_SliceAngleReset);
				//Compute the center of Volume in absolute coordinate, to center the surface and gizmo
				vtkTransform *transform;
				vtkNEW(transform);
				transform->SetMatrix(vme->GetOutput()->GetMatrix()->GetVTKMatrix());
				transform->Update();
				transform->MultiplyPoint(sliceCenterVolumeReset, m_SliceCenterSurface);
				transform->MultiplyPoint(sliceCenterVolumeReset, m_SliceCenterSurfaceReset);

				//Create a matrix to permit the reset of the gizmos
				vtkTransform *TransformReset;
				vtkNEW(TransformReset);
				TransformReset->Translate(m_SliceCenterSurfaceReset);
				TransformReset->RotateX(m_SliceAngleReset[0]);
				TransformReset->RotateY(m_SliceAngleReset[1]);
				TransformReset->RotateZ(m_SliceAngleReset[2]);
				TransformReset->Update();
				albaNEW(m_MatrixReset);
				m_MatrixReset->SetVTKMatrix(TransformReset->GetMatrix());


				m_AttachCamera = new albaAttachCamera(m_Gui, ((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->m_Rwi, this);

				m_AttachCamera->DisableAttachCamera();
				((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraSet(CAMERA_OS_Z);
				m_AttachCamera->SetStartingMatrix(m_MatrixReset->GetVTKMatrix());
				m_SlicingMatrix->DeepCopy(m_MatrixReset);
				m_AttachCamera->SetAttachedMatrix(m_SlicingMatrix->GetVTKMatrix());
				m_AttachCamera->EnableAttachCamera();

				((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_InputVolume);


				albaPipeVolumeArbSlice* pipeVolSlice = albaPipeVolumeArbSlice::SafeDownCast(m_ChildViewList[SLICE_VIEW]->GetNodePipe(m_InputVolume));
				if (pipeVolSlice)
					pipeVolSlice->SetEnableSliceViewCorrection(true);

				CreateGizmos();

				SetEnableGPU();
				SetSlices();

				m_Gui->FitGui();
				m_Gui->Update();

				vtkDEL(transform);
				vtkDEL(TransformReset);
			}
			else
			{
				albaPipe *  nodePipe = ((albaViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(vme);
				albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(nodePipe);
				if (pipeSlice)
				{
					double normal[3];
					GetSlicingNormal(normal);

					pipeSlice->SetSlice(m_SliceCenterSurface, normal);
				}

				SetBorder(nodePipe);

				albaPipeMeshSlice *pipeSliceViewMesh = albaPipeMeshSlice::SafeDownCast(nodePipe);
				if (pipeSliceViewMesh) pipeSliceViewMesh->SetFlipNormalOff();
			}
		}
	}
	else//if show=false
	{
		if(vme->GetOutput()->IsA("albaVMEOutputVolume"))
		{
			if (m_AttachCamera == NULL)
				return;

			m_AttachCamera->SetAttachedMatrix(NULL);

			DestroyGizmos();

			delete m_AttachCamera;
			m_AttachCamera = NULL;
			albaDEL(m_MatrixReset);

			m_InputVolume = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
	}

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);
	
	m_SkipCameraUpdate--;
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::GetSlicingNormal(double * normal)
{
	((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VmeSelect(albaVME *node, bool select)
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeSelect(node, select);
	m_ChildViewList[SLICE_VIEW]->VmeSelect(node, select);

	if (!m_AllSurface)
	{
		albaPipe* nodePipe = m_ChildViewList[SLICE_VIEW]->GetNodePipe(node);

		albaPipeSurfaceSlice *surfPipe = albaPipeSurfaceSlice::SafeDownCast(nodePipe);
		if (surfPipe)
			m_Border = surfPipe->GetThickness();

		albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(nodePipe);
		if (meshPipe)
			m_Border = meshPipe->GetThickness();

		if (m_Gui)
			m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VmeRemove(albaVME *vme)
{
	if (m_InputVolume && vme == m_InputVolume)
	{
		m_AttachCamera->SetAttachedMatrix(NULL);

		DestroyGizmos();

		m_InputVolume->RemoveObserver(this);
		m_InputVolume = NULL;
	}

	Superclass::VmeRemove(vme);
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEvent(albaEventBase *alba_event)
{
	if (alba_event->GetSender() == this->m_Gui || alba_event->GetSender() == this->m_LutSlider) // from this view gui
	{
		OnEventThis(alba_event); 
	}
	else if (alba_event->GetSender() == m_GizmoTranslate) // from translation gizmo
	{
		OnEventGizmoTranslate(alba_event);
	}
	else if (alba_event->GetSender() == m_GizmoRotate) // from rotation gizmo
	{
		OnEventGizmoRotate(alba_event);
	}
	else
	{
		switch (alba_event->GetId())
		{
		case ID_GIZMO_TRANSLATE:
			SetGizmo(GIZMO_TRANSLATE);
			break;

		case ID_GIZMO_ROTATE:
			SetGizmo(GIZMO_ROTATE);
			break;

		default:
			// if no one can handle this event send it to the operation listener
			Superclass::OnEvent(alba_event);
			break;
		}
	}	
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEventGizmoTranslate(albaEventBase *alba_event)
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    

			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event,false);

			albaEvent *e = albaEvent::SafeDownCast(alba_event);

			//compute the incremental translation
			vtkTransform *tr;
			vtkNEW(tr);
			tr->PostMultiply();
			tr->SetMatrix(e->GetMatrix()->GetVTKMatrix());
			tr->Update();
			double translation[3];
			tr->GetPosition(translation);

			//increase the translation
			m_SliceCenterSurface[0]+=translation[0];
			m_SliceCenterSurface[1]+=translation[1];
			m_SliceCenterSurface[2]+=translation[2];

			//change the position of rotation gizmos
			vtkTransform *TransformReset;
			vtkNEW(TransformReset);
			TransformReset->Identity();
			TransformReset->Translate(m_SliceCenterSurface);
			TransformReset->RotateX(m_SliceAngleReset[0]);
			TransformReset->RotateY(m_SliceAngleReset[1]);
			TransformReset->RotateZ(m_SliceAngleReset[2]);
			TransformReset->Update();
				
			SetSlices();
					
			CameraUpdate();
			vtkDEL(tr);
			vtkDEL(TransformReset);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEventGizmoRotate(albaEventBase *alba_event)
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);

			SetSlices();

			CameraUpdate();
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEventThis(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
		case ID_COMBO_GIZMOS:
			SetGizmo(m_TypeGizmo);
			break;
		case ID_RANGE_MODIFIED:
			OnLUTRangeModified();
			break;
		case ID_LUT_CHOOSER:
			OnLUTChooser();
			break;
		case ID_RESET:
			OnReset();
			break;
		case ID_TRILINEAR_INTERPOLATION_ON:
			TrilinearInterpolationOn();
		break;
		case ID_GPUENABLED:
			SetEnableGPU();
			break;
		case ID_ALL_SURFACE:
		case ID_BORDER_CHANGE:
		{
			OnEventSetThickness();
		}
		break;

		default:
			albaViewCompound::OnEvent(alba_event);
		}
	}
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::SetGizmo(int typeGizmo)
{
	if (m_InputVolume)
	{
		if (typeGizmo == GIZMO_TRANSLATE)
		{
			m_GizmoRotate->Show(false);
			m_GizmoTranslate->SetAbsPose(m_GizmoRotate->GetAbsPose(), 0);
			m_GizmoTranslate->Show(true);
		}
		else if (typeGizmo == GIZMO_ROTATE)
		{
			m_GizmoTranslate->Show(false);
			m_GizmoRotate->SetAbsPose(m_GizmoTranslate->GetAbsPose(), 0);
			m_GizmoRotate->Show(true);
		}

		m_TypeGizmo = typeGizmo;
		m_Gui->Update();
	}
	CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnReset()
{
	if (m_MatrixReset == NULL)
		return;

	m_GizmoRotate->SetAbsPose(m_MatrixReset);
	m_GizmoTranslate->SetAbsPose(m_MatrixReset);
	m_SlicingMatrix->DeepCopy(m_MatrixReset);

	m_SliceCenterSurface[0] = m_SliceCenterSurfaceReset[1];
	m_SliceCenterSurface[1] = m_SliceCenterSurfaceReset[1];
	m_SliceCenterSurface[2] = m_SliceCenterSurfaceReset[2];

	m_AttachCamera->UpdateCameraMatrix();
	//update because I need to refresh the normal of the camera
	SetSlices();
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::SetSlices()
{
	//update the normal of the cutter plane of the surface
	double normal[3];
	GetSlicingNormal(normal);
	albaVME *root = m_InputVolume->GetRoot();
	albaVMEIterator *iter = root->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		if (node != m_InputVolume)
		{
			albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
			if (pipeSlice)
				pipeSlice->SetSlice(m_SliceCenterSurface, normal);
		}
	}
	iter->Delete();


	albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_InputVolume));
	if (pipeSlice)
		pipeSlice->SetSlice(m_SliceCenterSurface, normal);
	
	pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_InputVolume));
	if (pipeSlice)
		pipeSlice->SetSlice(m_SliceCenterSurface, normal);

	CameraUpdate();
}

//----------------------------------------------------------------------------
albaView *albaViewArbitrarySlice::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;
	albaViewArbitrarySlice *v = new albaViewArbitrarySlice(m_Label);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	for (int i=0;i<m_PluggedChildViewList.size();i++)
	{
		v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
	}
	v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
albaGUI* albaViewArbitrarySlice::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER, "Lut", m_ColorLUT);

	m_Gui->Divider();
	m_Gui->Bool(ID_TRILINEAR_INTERPOLATION_ON, "Interpolation", &m_TrilinearInterpolationOn, 1);
	m_Gui->Divider();

	m_Gui->Divider();
	m_Gui->Bool(ID_GPUENABLED, "Enable GPU Acceleration", &m_EnableGPU, 1, "Enable GPU Acceleration");
	m_Gui->Divider();


	m_Gui->Bool(ID_ALL_SURFACE, "All Surface", &m_AllSurface);
	m_Gui->FloatSlider(ID_BORDER_CHANGE, "Border", &m_Border, 1.0, 5.0);


	m_Gui->Divider(1);

	wxString Text[2]={_("Translation"),_("Rotation")};
	m_Gui->Radio(ID_COMBO_GIZMOS, "Gizmo", &m_TypeGizmo, 2, Text);

	//button to reset at the start position
	m_Gui->Button(ID_RESET,_("Reset"),"");

	m_Gui->Divider();
	m_Gui->Bool(ID_CAMERA_FOLLOW_GIZMO, "Camera follow gizmos", &m_CameraFollowGizmo, 1, "Camera follow gizmos");
	m_Gui->Divider();

	m_Gui->Update();

	EnableWidgets(m_InputVolume);
	return m_Gui;
}


//----------------------------------------------------------------------------
void albaViewArbitrarySlice::PostMultiplyEventMatrix(albaEventBase *alba_event, int isRotation)
{  
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{

		albaMatrix rotation;
		vtkMatrix4x4 * matrix = e->GetMatrix()->GetVTKMatrix();
		
		rotation.CopyRotation(matrix);
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_SlicingMatrix->GetVTKMatrix());
		if (isRotation || m_CameraFollowGizmo)
			tr->Concatenate(matrix);
		else
			tr->Concatenate(rotation.GetVTKMatrix());


		albaMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(m_InputVolume->GetTimeStamp());

		
		m_SlicingMatrix->DeepCopy(&absPose);
		m_AttachCamera->UpdateCameraMatrix(); 

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::CameraUpdate()
{
	if (!m_SkipCameraUpdate)
		for (int i = 0; i < m_NumOfChildView; i++)
		{
			m_ChildViewList[i]->CameraUpdate();
		}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::CreateGuiView()
{
	m_GuiView = new albaGUI(this);

	wxBoxSizer *mainVertSizer = new wxBoxSizer(wxHORIZONTAL);
	m_LutSlider = new albaGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(500, 24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500, 24);
	m_LutSlider->SetMinSize(wxSize(500, 24));
	EnableWidgets(m_InputVolume != NULL);

	//
	albaGUIPicButton *button1 = new albaGUIPicButton(m_GuiView, "GIZMO_TRANSLATE_ICON", ID_GIZMO_TRANSLATE, this);
	button1->SetListener(this);
	button1->SetToolTip("Translate");

	albaGUIPicButton *button2 = new albaGUIPicButton(m_GuiView, "GIZMO_ROTATE_ICON", ID_GIZMO_ROTATE, this);
	button2->SetListener(this);
	button2->SetToolTip("Rotate");

	mainVertSizer->Add(button1);
	mainVertSizer->Add(button2);
	mainVertSizer->Add(m_LutSlider, wxEXPAND);

	m_GuiView->Add(mainVertSizer, 1, wxEXPAND);
	m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::EnableWidgets(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_RESET, enable);
		m_Gui->Enable(ID_COMBO_GIZMOS, enable);
		m_Gui->Enable(ID_LUT_CHOOSER, enable);
 		m_Gui->Enable(ID_GIZMO_TRANSLATE, enable);
 		m_Gui->Enable(ID_GIZMO_ROTATE, enable);
		m_Gui->FitGui();
		m_Gui->Update();
	}

	m_LutSlider->Enable(enable);
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VolumeWindowing(albaVME *volume)
{
	double sr[2];
	vtkDataSet *data = volume->GetOutput()->GetVTKData();
	data->Update();
	data->GetScalarRange(sr);

	mmaVolumeMaterial *currentVolumeMaterial = ((albaVMEOutputVolume *)m_InputVolume->GetOutput())->GetMaterial();
	m_ColorLUT = currentVolumeMaterial->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentVolumeMaterial->GetTableRange()[0],(long)currentVolumeMaterial->GetTableRange()[1]);
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEventSetThickness()
{
	if (m_AllSurface)
	{
		albaVME *vme = GetViewArbitrary()->GetSceneGraph()->GetSelectedVme();
		albaVME *root = vme->GetRoot();
		SetThicknessForAllSurfaceSlices(root);
	}
	else
	{
		albaVME *node = GetViewArbitrary()->GetSceneGraph()->GetSelectedVme();
		albaSceneNode *SN = this->GetSceneGraph()->Vme2Node(node);
		albaPipe *p = m_ChildViewList[SLICE_VIEW]->GetNodePipe(node);
		SetBorder(p);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::SetThicknessForAllSurfaceSlices(albaVME *root)
{
	albaVMEIterator *iter = root->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		//view slice 0 is enough because the property will be applied on all slices.
		albaView *view = m_ChildViewList[SLICE_VIEW];
		albaPipe *p = view->GetNodePipe(node);
		SetBorder(p);
	}
	iter->Delete();
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::SetBorder(albaPipe * p)
{
	albaPipeSurfaceSlice *surfPipe = albaPipeSurfaceSlice::SafeDownCast(p);
	if (surfPipe) surfPipe->SetThickness(m_Border);

	albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(p);
	if (meshPipe) meshPipe->SetThickness(m_Border);
}


//-------------------------------------------------------------------------
void albaViewArbitrarySlice::CreateGizmos()
{
	if (m_InputVolume == NULL)
	{
		albaLogMessage("Generate - Current volume = NULL");
		return;
	}

	albaLogMessage("Generate");

	//m_Slicer->SetUpdateVTKPropertiesFromMaterial(false);
	// Create the Gizmos
	m_GizmoTranslate = new albaGizmoTranslate(m_InputVolume, this);
	m_GizmoTranslate->SetInput(m_InputVolume);
	m_GizmoTranslate->SetRefSys(m_InputVolume);
	m_GizmoTranslate->SetAbsPose(m_MatrixReset);
	m_GizmoTranslate->SetStep(X_AXIS, 1.0);
	m_GizmoTranslate->SetStep(Y_AXIS, 1.0);
	m_GizmoTranslate->SetStep(Z_AXIS, 1.0);
/*	m_GizmoTranslate->SetConstraintModality(X_AXIS, albaInteractorConstraint::BOUNDS);
	m_GizmoTranslate->SetConstraintModality(Y_AXIS, albaInteractorConstraint::BOUNDS);
	m_GizmoTranslate->SetConstraintModality(Z_AXIS, albaInteractorConstraint::BOUNDS);*/
	m_GizmoTranslate->Show(true);

	m_GizmoRotate = new albaGizmoRotate(m_InputVolume, this);
	m_GizmoRotate->SetInput(m_InputVolume);
	m_GizmoRotate->SetRefSys(m_InputVolume);
	m_GizmoRotate->SetAbsPose(m_MatrixReset);
	m_GizmoRotate->Show(false);

	m_TypeGizmo = GIZMO_TRANSLATE;

	//Create the Gizmos' Gui
	if (m_GuiGizmos == NULL)
	{
		m_GuiGizmos = new albaGUI(this);
		m_Gui->AddGui(m_GuiGizmos);

		m_GuiGizmos->AddGui(m_GizmoTranslate->GetGui());
		m_GuiGizmos->AddGui(m_GizmoRotate->GetGui());
	}

	m_GuiGizmos->Update();
	m_GuiGizmos->FitGui();

	m_Gui->Update();
	m_Gui->FitGui();
	m_Gui->Fit();
	m_Gui->FitInside();
}
//-------------------------------------------------------------------------
void albaViewArbitrarySlice::DestroyGizmos()
{
	if (m_InputVolume == NULL)
	{
		albaLogMessage("Destroy - Current volume = NULL");
		return;
	}
	albaLogMessage("Destroy");

	// Remove Gizmos
	m_Gui->Remove(m_GuiGizmos);
	m_Gui->Update();
	m_Gui->FitGui();

	m_GizmoTranslate->Show(false);
	cppDEL(m_GizmoTranslate);

	m_GizmoRotate->Show(false);
	cppDEL(m_GizmoRotate);

	cppDEL(m_GuiGizmos);

	m_Gui->Update();
}

//----------------------------------------------------------------------------
albaMatrix* albaViewArbitrarySlice::GetSlicerMatrix()
{
	return m_SlicingMatrix; 
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::SetRestoreTagToVME(albaVME *vme)
{
	albaTagItem tag("ArbSliceMtr", (double *)m_SlicingMatrix, 16);
	vme->GetTagArray()->SetTag(tag);
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::RestoreFromVME(albaVME* vme)
{
	albaTagItem *tag = vme->GetTagArray()->GetTag("ArbSliceMtr");
	
	int n = 0;
	for (int j = 0; j < 4; j++)
		for (int k = 0; k < 4; k++)
		{
			m_SlicingMatrix->SetElement(j, k, tag->GetComponentAsDouble(n));
			n++;
		}

	SetSlices();
	CameraUpdate();
}

//----------------------------------------------------------------------------
albaViewVTK * albaViewArbitrarySlice::GetViewArbitrary()
{
	return (albaViewVTK*)m_ChildViewList[ARBITRARY_VIEW];
}
//----------------------------------------------------------------------------
albaViewVTK * albaViewArbitrarySlice::GetViewSlice()
{
	return (albaViewVTK*)m_ChildViewList[SLICE_VIEW];
}
//----------------------------------------------------------------------------
albaPipe* albaViewArbitrarySlice::GetPipeSlice() 
{
	albaPipe *pipeSlice = NULL;
	pipeSlice = GetViewSlice()->GetNodePipe(m_InputVolume);

	return pipeSlice;
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::SetEnableGPU()
{
	for (int i = ARBITRARY_VIEW; i <= SLICE_VIEW; i++)
	{
		albaPipeVolumeArbSlice *pipeSlice = albaPipeVolumeArbSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[i])->GetNodePipe(m_InputVolume));
		if (pipeSlice)
			pipeSlice->SetEnableGPU(m_EnableGPU);
	}
		
	CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnLUTRangeModified()
{
	if (m_InputVolume)
	{
		double low, hi;
		m_LutSlider->GetSubRange(&low, &hi);
		m_ColorLUT->SetTableRange(low, hi);
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnLUTChooser()
{
	if (m_InputVolume)
	{
		double *sr;

		sr = m_ColorLUT->GetRange();
		m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::TrilinearInterpolationOn()
{
	if (m_InputVolume)
	{
		for (int i = 0; i < m_NumOfChildView; i++)
		{
			albaPipeVolumeArbSlice *p = albaPipeVolumeArbSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[i])->GetNodePipe(m_InputVolume));
			if (p)
			{
				p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
			}
		}
		this->CameraUpdate();
	}
}