/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewArbitraryOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi
 
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

#include "albaGizmoCrossRotateTranslate.h"
#include "albaGUI.h"
#include "albaDecl.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewSlice.h"
#include "albaVme.h"
#include "albaVMESlicer.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaPipeSlice.h"
#include "albaVMEVolumeGray.h"

#include "albaEvent.h"
#include "albaAttachCamera.h"
#include "albaInteractorGenericMouse.h"
#include "albaVMESlicer.h"
#include "albaTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEIterator.h"
#include "albaVMEOutputSurface.h"
#include "albaAttribute.h"
#include "albaGUILutSlider.h"
#include "albaGUILutSwatch.h"
#include "albaViewSlice.h"

#include "vtkTransform.h"
#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkDataSetAttributes.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkConeSource.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkCellPicker.h"
#include "albaTransformFrame.h"
#include "albaVMEGizmo.h"
#include "albaGizmoInterface.h"
#include "albaDataPipe.h"
#include "vtkImageData.h"
#include "vtkDataSetWriter.h"
#include "vtkUnsignedShortArray.h"
#include "albaRWIBase.h"
#include "wx\busyinfo.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkSphereSource.h"
#include "vtkALBASmartPointer.h"
#include "albaPipeMeshSlice.h"
#include "albaPipeSurfaceTextured.h"
#include "albaPipeVolumeArbOrthoSlice.h"
#include "albaPipeVolumeArbSlice.h"
#include "albaPipeSurfaceSlice.h"

albaCxxTypeMacro(albaViewArbitraryOrthoSlice);

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum ARBITRARY_SUBVIEW_ID
{
	PERSPECTIVE_VIEW = 0,
	Z_VIEW = 1,
	X_VIEW = 2,
	Y_VIEW = 3,
	NUMBER_OF_SUBVIEWS = 4,
};

#define AsixToView(a) ( (a) == X ? X_VIEW : ((a) == Y ? Y_VIEW : Z_VIEW))

albaVME *GLO_CurrentVolume;

//----------------------------------------------------------------------------
albaViewArbitraryOrthoSlice::albaViewArbitraryOrthoSlice(wxString label, albaAxes::AXIS_TYPE_ENUM axesType) : albaViewCompoundWindowing(label, 2, 2)
{
	m_AxesType = axesType;
	m_InputVolume = NULL;
	m_GizmoRT[0] = m_GizmoRT[1] = m_GizmoRT[2] = NULL;
	m_View3d   = NULL;
	m_ViewSlice[0] = m_ViewSlice[1] = m_ViewSlice[2] = NULL;
	m_SlicingResetMatrix[0] = m_SlicingResetMatrix[1] = m_SlicingResetMatrix[2] = NULL;
	m_InputVolume   = NULL;
	m_CameraToSlicer[0] = m_CameraToSlicer[1] = m_CameraToSlicer[2] = NULL;
	m_SlicingOrigin[0] = m_SlicingOrigin[1] = m_SlicingOrigin[2] = 0.0;
	m_SlicingOriginGUI[0] = m_SlicingOriginGUI[1] = m_SlicingOriginGUI[2] = 0.0;
	m_SlicingOriginReset[0] = m_SlicingOriginReset[1] = m_SlicingOriginReset[2] = 0.0;
	m_CameraConeVME[0] = m_CameraConeVME[1] = m_CameraConeVME[2] = NULL;
	for (int i = X; i <= Z; i++)
	{
		m_CameraPositionForReset[i][0] = m_CameraPositionForReset[i][1] = m_CameraPositionForReset[i][2] = 0;
		m_CameraViewUpForReset[i][0] = m_CameraViewUpForReset[i][1] = m_CameraViewUpForReset[i][2] = 0;
	}

	m_AllSurface = 0;
	m_Border = 1;

	m_SkipCameraUpdate = 0;
	m_EnableGPU = true; 
	m_CameraFollowGizmo = false;
	m_IsShowingSlicerGizmo = false;
	for (int i = X; i <= Z; i++)
	{
		albaNEW(m_SlicingMatrix[i]);
	}
}
//----------------------------------------------------------------------------
albaViewArbitraryOrthoSlice::~albaViewArbitraryOrthoSlice()
{
	m_InputVolume = NULL;
	m_ColorLUT = NULL;

	for (int i = X; i <= Z; i++)
	{
		albaDEL(m_SlicingResetMatrix[i]);
		albaDEL(m_SlicingMatrix[i]);
		delete(m_CameraToSlicer[i]);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::PackageView()
{
	m_View3d = new albaViewVTK("",CAMERA_PERSPECTIVE,true,false,0,false,m_AxesType);
	m_View3d->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeArbOrthoSlice", MUTEX);
	m_View3d->PlugVisualPipe("albaVMEGizmo", "albaPipeGizmo", NON_VISIBLE);
	PlugChildView(m_View3d);

	CreateAndPlugSliceView(2);
	CreateAndPlugSliceView(0);
	CreateAndPlugSliceView(1);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::CreateAndPlugSliceView(int v)
{
	m_ViewSlice[v] = new albaViewVTK("", CAMERA_OS_X + v, true, false, 0, false, m_AxesType);
	m_ViewSlice[v]->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeArbSlice", MUTEX);

	m_ViewSlice[v]->PlugVisualPipe("albaVMEImage", "albaPipeBox", NON_VISIBLE);
	m_ViewSlice[v]->PlugVisualPipe("albaVMESegmentationVolume", "albaPipeVolumeOrthoSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEPointCloud", "albaPipeBox", NON_VISIBLE);
	m_ViewSlice[v]->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEPolyline", "albaPipePolylineSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEPolylineSpline", "albaPipePolylineSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEMeter", "albaPipePolyline");
	m_ViewSlice[v]->PlugVisualPipe("medVMEMuscleWrapper", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEProsthesis", "albaPipeSurfaceSlice");
	
	PlugChildView(m_ViewSlice[v]);
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::VmeShow(albaVME *vme, bool show)
{
	m_SkipCameraUpdate++;

	if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
	{
		if (show)
		{
			ShowVolume(vme, show);
			StoreCameraParametersForAllSubviews();
		}
		else//if show==false
		{
			HideVolume();
		}
	}
	else if ((m_IsShowingSlicerGizmo && vme->IsA("albaVMEGizmo")) || 
		albaString("AxisRotationFeedbackGizmo").Equals(vme->GetName()) ||
		albaString("PlaneTranslationFeedbackGizmo").Equals(vme->GetName()))
	{
		for (int i = 0; i <= Z; i++)
		{
			if (BelongsToNormalGizmo(vme, i))
			{
				m_ChildViewList[AsixToView(i)]->VmeShow(vme, show);
				m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
				break;
			}
		}
	}
	else
	{
		m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
		m_ChildViewList[Z_VIEW]->VmeShow(vme, show);
		m_ChildViewList[X_VIEW]->VmeShow(vme, show);
		m_ChildViewList[Y_VIEW]->VmeShow(vme, show);


		for (int view = Z_VIEW; view <= Y_VIEW; view++)
		{
			albaPipe* nodePipe = m_ChildViewList[view]->GetNodePipe(vme);
			albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(nodePipe);
			if (pipeSlice)
			{
				double normal[3];
				((albaViewSlice*)m_ChildViewList[view])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
				pipeSlice->SetSlice(m_SlicingOrigin, normal);

				SetBorder(nodePipe);

				albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(nodePipe);
				if (meshPipe) meshPipe->SetFlipNormalOff();
			}
		}
	}
	
	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);

	m_SkipCameraUpdate--;
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::VmeSelect(albaVME *node, bool select)
{
	m_ChildViewList[PERSPECTIVE_VIEW]->VmeSelect(node, select);



	if (!m_AllSurface)
	{
		albaPipe* nodePipe = m_ChildViewList[Z_VIEW]->GetNodePipe(node);

		albaPipeSurfaceSlice *surfPipe = albaPipeSurfaceSlice::SafeDownCast(nodePipe);
		if (surfPipe) 
			m_Border = surfPipe->GetThickness();

		albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(nodePipe);
		if (meshPipe) 
			m_Border = meshPipe->GetThickness();
		
		if(m_Gui)
			m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::VmeRemove(albaVME *vme)
{
	if (m_InputVolume && vme == m_InputVolume)
	{
		HideVolume();
		m_InputVolume = NULL;
	}

	Superclass::VmeRemove(vme);
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::UpdateConesPosition()
{
	for (int i = X; i <= Z; i++)
		m_CameraConeVME[i]->SetAbsMatrix(*m_GizmoRT[i]->GetAbsPose());
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnSlicingOrigin()
{
	
	if (m_InputVolume) {
		double translation[3];

		for (int i = 0; i < 3; i++)
		{
			translation[i] = m_SlicingOriginGUI[i] - m_SlicingOrigin[i];
		}
		albaMatrix transMatrix;
		albaTransform::Translate(transMatrix, translation, true);

		OnEventGizmoTranslate(transMatrix.GetVTKMatrix(), -1);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::CameraUpdate()
{
	if (!m_SkipCameraUpdate)
		Superclass::CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEvent(albaEventBase *alba_event)
{
	assert(alba_event);

	if (alba_event->GetSender() == this->m_Gui || alba_event->GetSender() == this->m_LutSlider) // from this view gui
	{
		OnEventThis(alba_event); 
	}
	else if (alba_event->GetId() == ID_TRANSFORM)
	{	
		albaEvent *e = albaEvent::SafeDownCast(alba_event);
		vtkMatrix4x4 *  matrix= e->GetMatrix()->GetVTKMatrix();
		void * sender = alba_event->GetSender();
		int plane = GetGizmoPlane(sender);

		if (plane >= 0)
		{
			if (IsGizmoTranslate(sender))
				OnEventGizmoTranslate(matrix, plane);
			else 
				OnEventGizmoRotate(matrix, plane);
		}
		else
			albaEventMacro(*alba_event); 
	}
	else
	{
		Superclass::OnEvent(alba_event);
	}
}
//----------------------------------------------------------------------------
int albaViewArbitraryOrthoSlice::GetGizmoPlane(void *gizmo)
{
	for (int i = X; i <= Z; i++)
	{
		if (gizmo == m_GizmoRT[i]->m_GizmoCrossTranslate || gizmo == m_GizmoRT[i]->m_GizmoCrossRotate)
			return i;
	}
	return -1;
}

//----------------------------------------------------------------------------
int albaViewArbitraryOrthoSlice::IsGizmoTranslate(void *gizmo)
{
	for (int i = X; i <= Z; i++)
	{
		if (gizmo == m_GizmoRT[i]->m_GizmoCrossTranslate)
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEventGizmoTranslate(vtkMatrix4x4 *matrix, int planeSkip)
{
	double translation[3];

	for (int i = X; i <= Z; i++)
	{
		if (i != planeSkip)
			PostMultiplyEventMatrix(matrix,i,false);
	}

	UpdateConesPosition();

	albaTransform::GetPosition(matrix, translation);

	//increase the translation
	m_SlicingOrigin[0] += translation[0];
	m_SlicingOrigin[1] += translation[1];
	m_SlicingOrigin[2] += translation[2];
	
	SetSlices();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEventGizmoRotate(vtkMatrix4x4 *matrix, int planeSkip)
{
	for (int i = X; i <= Z; i++)
	{
		if (i != planeSkip)
			PostMultiplyEventMatrix(matrix, i);
	}

	UpdateConesPosition();

	SetSlices();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::SetSlices()
{
	albaPipeVolumeArbOrthoSlice *pipeOrthoSlice = albaPipeVolumeArbOrthoSlice::SafeDownCast(m_ChildViewList[PERSPECTIVE_VIEW]->GetNodePipe(m_InputVolume));

	if (pipeOrthoSlice == NULL)
		return;
	
	albaVME *root = m_InputVolume->GetRoot();
		
	for (int i = X; i <= Z; i++)
	{
		m_SlicingOriginGUI[i] = m_SlicingOrigin[i];

		double normal[3];
		((albaViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		albaVMEIterator *iter = root->NewIterator();
		for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		{
			if (node != m_InputVolume)
			{
				albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[AsixToView(i)])->GetNodePipe(node));
				if (pipeSlice)
					pipeSlice->SetSlice(m_SlicingOrigin, normal);
			}
		}
		iter->Delete();


		albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[AsixToView(i)])->GetNodePipe(m_InputVolume));
		if (pipeSlice)
			pipeSlice->SetSlice(m_SlicingOrigin, normal);

		pipeOrthoSlice->SetSlice(i, m_SlicingOrigin, normal);
	}
	CameraUpdate();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEventThis(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
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
		case ID_LOAD_FROM_REFSYS:
			OnLoadFromRefsys();
		break;
		case ID_GPUENABLED:
			SetEnableGPU();
		break;
		case ID_SLICING_ORIGIN:
			OnSlicingOrigin();
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
void albaViewArbitraryOrthoSlice::SetEnableGPU()
{
	for (int i = X; i <= Z; i++)
	{
		albaPipeVolumeArbSlice *pipeSlice = albaPipeVolumeArbSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[AsixToView(i)])->GetNodePipe(m_InputVolume));
		if (pipeSlice)
			pipeSlice->SetEnableGPU(m_EnableGPU);
	}

	albaPipeVolumeArbOrthoSlice *pipeSlice = albaPipeVolumeArbOrthoSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_InputVolume));
	if (pipeSlice)
		pipeSlice->SetEnableGPU(m_EnableGPU);

	CameraUpdate();
}

//----------------------------------------------------------------------------
albaView *albaViewArbitraryOrthoSlice::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;
	albaViewArbitraryOrthoSlice *v = new albaViewArbitraryOrthoSlice(m_Label);
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
albaGUI* albaViewArbitraryOrthoSlice::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	m_Gui->Label("");
	m_Gui->Button(ID_RESET,_("Reset slices"),"");
	m_Gui->Button(ID_LOAD_FROM_REFSYS, _("Load from Refsys"), "");

	m_Gui->Divider();
	m_Gui->Bool(ID_GPUENABLED, "Enable GPU Acceleration", &m_EnableGPU, 1, "Enable GPU Acceleration");
	
	m_Gui->Divider();

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);

	m_Gui->Divider();
	m_Gui->Divider();
	m_Gui->Label("Slicing Origin:", true);
	m_Gui->Double(ID_SLICING_ORIGIN, "X:", &m_SlicingOriginGUI[0]);
	m_Gui->Double(ID_SLICING_ORIGIN, "Y:", &m_SlicingOriginGUI[1]);
	m_Gui->Double(ID_SLICING_ORIGIN, "Z:", &m_SlicingOriginGUI[2]);

	m_Gui->Bool(ID_ALL_SURFACE, "All Surface", &m_AllSurface);
	m_Gui->FloatSlider(ID_BORDER_CHANGE, "Border", &m_Border, 1.0, 5.0);


	m_Gui->Divider();
	m_Gui->Divider();

	m_Gui->Divider();
	m_Gui->Bool(ID_CAMERA_FOLLOW_GIZMO, "Camera follow gizmos", &m_CameraFollowGizmo, 1, "Camera follow gizmos");
	m_Gui->Divider();

	m_Gui->Update();
		
	EnableWidgets( (m_InputVolume != NULL) );
	return m_Gui;
}


//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEventSetThickness()
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
		albaPipe *p = GetViewSlice(0)->GetNodePipe(node);
		SetBorder(p);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::SetThicknessForAllSurfaceSlices(albaVME *root)
{
	albaVMEIterator *iter = root->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		//view slice 0 is enough because the property will be applied on all slices.
			albaViewVTK *view = GetViewSlice(0);
			albaPipe *p = view->GetNodePipe(node);
			SetBorder(p);
	}
	iter->Delete();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::SetBorder(albaPipe * p)
{
	albaPipeSurfaceSlice *surfPipe = albaPipeSurfaceSlice::SafeDownCast(p);
	if (surfPipe) surfPipe->SetThickness(m_Border);

	albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(p);
	if (meshPipe) meshPipe->SetThickness(m_Border);
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::UpdateSubviewsCamerasToFaceSlices()
{
	for (int i = X; i <= Z ; i++)
	{
		if (m_CameraToSlicer[i] != NULL)
			m_CameraToSlicer[i]->UpdateCameraMatrix();
	}

	CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::ShowGizmos(bool show)
{
	m_IsShowingSlicerGizmo = show;
	for (int i = X; i <= Z; i++)
	{
		if(m_GizmoRT[i])
		m_GizmoRT[i]->Show(show);
	}
	m_IsShowingSlicerGizmo = false;
}

//----------------------------------------------------------------------------
char ** albaViewArbitraryOrthoSlice::GetIcon()
{
#include "pic/VIEW_ARB_ORTHO.xpm"
	return VIEW_ARB_ORTHO_xpm;
}

//----------------------------------------------------------------------------
albaMatrix* albaViewArbitraryOrthoSlice::GetSlicerMatrix(int axis)
{
	return m_GizmoRT[axis]->GetAbsPose();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::SetRestoreTagToVME(albaVME *vme)
{
	albaMatrix *matrix;

	for (int i = 0; i < 3; i++)
	{
		//Slicing Stuff
		albaString tagName;
		tagName.Printf("ArbOrthoSliceMtr%d", i);
		matrix = GetSlicerMatrix(i);
		albaTagItem tag(tagName, (double *)matrix->GetElements(), 16);
		vme->GetTagArray()->SetTag(tag);


		//Camera Stuff
		double values[3],scale;
		vtkCamera* camera = ((albaViewSlice*)m_ChildViewList[i+1])->GetRWI()->GetCamera();
		
		tagName.Printf("ArbOrthoSliceCamPos%d", i);
		camera->GetPosition(values);
		albaTagItem tagPos(tagName, values, 3);
		vme->GetTagArray()->SetTag(tagPos);

		tagName.Printf("ArbOrthoSliceCamFP%d", i);
		camera->GetFocalPoint(values);
		albaTagItem tagFP(tagName, values, 3);
		vme->GetTagArray()->SetTag(tagFP);

		tagName.Printf("ArbOrthoSliceCamUp%d", i);
		camera->GetViewUp(values);
		albaTagItem tagUP(tagName, values, 3);
		vme->GetTagArray()->SetTag(tagUP);

		tagName.Printf("ArbOrthoSliceCamScale%d", i);
		scale=camera->GetParallelScale();
		albaTagItem tagScale(tagName, scale);
		vme->GetTagArray()->SetTag(tagScale);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::RestoreFromVME(albaVME* vme)
{
	if (vme == NULL) return;
	
	//Slicing Stuff
	for (int i = 0; i < 3; i++)
	{
		albaString tagName;
		tagName.Printf("ArbOrthoSliceMtr%d", i);
		albaTagItem *tag = vme->GetTagArray()->GetTag(tagName);

		if (tag == NULL)
			return;

			albaMatrix mtr;
		int n = 0;
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				mtr.SetElement(j, k, tag->GetComponentAsDouble(n));
				n++;
			}

		m_GizmoRT[i]->SetAbsPose(&mtr);
		m_SlicingMatrix[i]->DeepCopy(m_GizmoRT[i]->GetAbsPose());
		m_CameraToSlicer[i]->UpdateCameraMatrix();

	

	}

	albaTransform::GetPosition(*m_GizmoRT[0]->GetAbsPose(), m_SlicingOrigin);
	UpdateConesPosition();
	SetSlices();

	
	//Camera Stuff
	CameraUpdate();

	for (int i = 0; i < 3; i++)
	{
		albaString tagName;
		double values[3];
		vtkCamera* camera = ((albaViewSlice*)m_ChildViewList[i+1])->GetRWI()->GetCamera();

		tagName.Printf("ArbOrthoSliceCamPos%d", i);
		albaTagItem *tag = vme->GetTagArray()->GetTag(tagName);
		if (tag)
		{
			for (int j = 0; j < 3; j++)
				values[j] = tag->GetComponentAsDouble(j);
			camera->SetPosition(values);
		}

		tagName.Printf("ArbOrthoSliceCamFP%d", i);
		tag = vme->GetTagArray()->GetTag(tagName);
		if (tag)
		{
			for (int j = 0; j < 3; j++)
				values[j] = tag->GetComponentAsDouble(j);
			camera->SetFocalPoint(values);
		}

		tagName.Printf("ArbOrthoSliceCamUp%d", i);
		tag = vme->GetTagArray()->GetTag(tagName);
		if (tag)
		{
			for (int j = 0; j < 3; j++)
				values[j] = tag->GetComponentAsDouble(j);
			camera->SetViewUp(values);
		}

		tagName.Printf("ArbOrthoSliceCamScale%d", i);
		tag = vme->GetTagArray()->GetTag(tagName);
		if (tag)
		{
			double  scale = tag->GetValueAsDouble();
			camera->SetParallelScale(scale);
		}
	}

	CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::CreateGuiView()
{
	m_GuiView = new albaGUI(this);

	//m_GuiView->Label("");
	m_LutSlider = new albaGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(500, 24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500, 24);
	m_LutSlider->SetMinSize(wxSize(500, 24));
	EnableWidgets(m_InputVolume != NULL);
	m_GuiView->Add(m_LutSlider);
	m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::EnableWidgets(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_RESET, enable);
		m_Gui->Enable(ID_LUT_CHOOSER, enable);
		m_Gui->Enable(ID_SHOW_GIZMO, enable);
		m_Gui->Enable(ID_SLICING_ORIGIN, enable);
		m_Gui->FitGui();
		m_Gui->Update();
	}
	 
	m_LutSlider->Enable(enable);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::VolumeWindowing(albaVME *volume)
{
	double sr[2];
	vtkDataSet *data = volume->GetOutput()->GetVTKData();
	data->Update();
	data->GetScalarRange(sr);

	mmaVolumeMaterial *currentVolumeMaterial = ((albaVMEOutputVolume *)m_InputVolume->GetOutput())->GetMaterial();
	m_ColorLUT = currentVolumeMaterial->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0], (long)sr[1]);
	m_LutSlider->SetSubRange((long)currentVolumeMaterial->GetTableRange()[0], (long)currentVolumeMaterial->GetTableRange()[1]);
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::ShowVolume( albaVME * vme, bool show )
{
	if (m_InputVolume == vme) return;

	if (m_InputVolume)
		GetLogicManager()->VmeShow(m_InputVolume, false);

	m_InputVolume = albaVMEVolumeGray::SafeDownCast(vme);

	wxBusyInfo wait("please wait");

	for (int i = X; i <= Z; i++)
	{
		albaViewVTK *xView = ((albaViewVTK *)(m_ChildViewList[AsixToView(i)]));
		assert(xView);
		// fuzzy picking
		xView->GetPicker2D()->SetTolerance(0.03);
	}
	
		
	double sr[2],volumeVTKDataCenterLocalCoords[3];

	EnableWidgets(true);

	vtkDataSet *volumeVTKData = vme->GetOutput()->GetVTKData();
	volumeVTKData->Update();
	volumeVTKData->GetCenter(volumeVTKDataCenterLocalCoords);
	volumeVTKData->GetScalarRange(sr);
	
	albaTransform::GetOrientation(vme->GetAbsMatrixPipe()->GetMatrix(),m_VolumeVTKDataABSOrientation);

	// Compute the center of Volume in absolute coordinates
	// Needed to position the surface and the gizmo
	vtkPoints *points;
	vtkNEW(points);
	points->InsertNextPoint(volumeVTKDataCenterLocalCoords);

	vtkPolyData *sliceCenterLocalCoordsPolydata=vtkPolyData::New();
	sliceCenterLocalCoordsPolydata->SetPoints(points);

	vtkTransform *sliceCenterLocalCoordsToABSCoordsTransform;
	vtkNEW(sliceCenterLocalCoordsToABSCoordsTransform);
	sliceCenterLocalCoordsToABSCoordsTransform->Identity();
	sliceCenterLocalCoordsToABSCoordsTransform->SetMatrix(vme->GetOutput()->GetMatrix()->GetVTKMatrix());
	sliceCenterLocalCoordsToABSCoordsTransform->Update();

	vtkTransformPolyDataFilter *localToABSTPDF;
	vtkNEW(localToABSTPDF);
	localToABSTPDF->SetInput(sliceCenterLocalCoordsPolydata);
	localToABSTPDF->SetTransform(sliceCenterLocalCoordsToABSCoordsTransform);
	localToABSTPDF->Update();
	localToABSTPDF->GetOutput()->GetCenter(m_SlicingOrigin);
	localToABSTPDF->GetOutput()->GetCenter(m_SlicingOriginGUI);
	localToABSTPDF->GetOutput()->GetCenter(m_SlicingOriginReset);

	vtkTransform *transformReset;
	vtkNEW(transformReset);
	transformReset->Identity();
	transformReset->Translate(m_SlicingOriginReset);
	transformReset->RotateZ(m_VolumeVTKDataABSOrientation[2]);
	transformReset->RotateX(m_VolumeVTKDataABSOrientation[0]);
	transformReset->RotateY(m_VolumeVTKDataABSOrientation[1]);
	transformReset->Update();


	vtkALBASmartPointer<vtkTransform> slicerTransform;
	for (int i = X; i <= Z; i++)
	{
		slicerTransform->SetMatrix(transformReset->GetMatrix());
		if(i==X)
			slicerTransform->RotateY(89.999);
		else if(i==Y)
			slicerTransform->RotateX(90);
		slicerTransform->Update();

		albaNEW(m_SlicingResetMatrix[i]);
		m_SlicingResetMatrix[i]->DeepCopy(slicerTransform->GetMatrix());
		m_SlicingMatrix[i]->DeepCopy(m_SlicingResetMatrix[i]);
	}

	ShowSlicers(vme, show);

	vtkDEL(points);
	vtkDEL(sliceCenterLocalCoordsPolydata);
	vtkDEL(sliceCenterLocalCoordsToABSCoordsTransform);
	vtkDEL(localToABSTPDF);
	vtkDEL(transformReset);

	VolumeWindowing(vme);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::HideVolume()
{
	OnReset();

	EnableWidgets(false);
	
	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_InputVolume, false);
	for(int i=X; i<=Z; i++)
	{
		VmeShow(m_CameraConeVME[i], false);
		m_ChildViewList[AsixToView(i)]->VmeShow(m_InputVolume, false);
		m_CameraConeVME[i]->ReparentTo(NULL);
		m_GizmoRT[i]->Show(false);

		m_CameraToSlicer[i]->SetVme(NULL);
		cppDEL(m_GizmoRT[i]);
		albaDEL(m_CameraConeVME[i]);
		albaDEL(m_SlicingResetMatrix[i]);
	}

	m_InputVolume = NULL;
	m_ColorLUT = NULL;
	m_LutWidget->SetLut(m_ColorLUT);
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnReset()
{
	for (int i = X; i <= Z; i++)
	{
		m_GizmoRT[i]->SetAbsPose(m_SlicingResetMatrix[i]);
		m_SlicingMatrix[i]->DeepCopy(m_SlicingResetMatrix[i]);
		m_SlicingOriginGUI[i] = m_SlicingOrigin[i] = m_SlicingOriginReset[i];
	}

	RestoreCameraParametersForAllSubviews();
	SetSlices();
	UpdateConesPosition();

	CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnLUTRangeModified()
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
void albaViewArbitraryOrthoSlice::OnLUTChooser()
{
	if (m_InputVolume)
	{
		double *sr;

		sr = m_ColorLUT->GetRange();
		m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);
	}
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::ShowSlicers(albaVME * vmeVolume, bool show)
{
	m_IsShowingSlicerGizmo = true;
	char slicerNames[3][10] = { "m_SlicerX","m_SlicerY","m_SlicerZ" };
	char gizmoNames[3][13] = { "m_GizmoXView","m_GizmoYView","m_GizmoZView" };
	enum albaGizmoCrossRotateTranslate::COLOR gizmoColors[3][2] = { { albaGizmoCrossRotateTranslate::GREEN, albaGizmoCrossRotateTranslate::BLUE },
																																 { albaGizmoCrossRotateTranslate::BLUE,  albaGizmoCrossRotateTranslate::RED },
																																 { albaGizmoCrossRotateTranslate::GREEN, albaGizmoCrossRotateTranslate::RED } };

	EnableWidgets((m_InputVolume != NULL));
	// register sliced volume
	m_InputVolume = albaVMEVolumeGray::SafeDownCast(vmeVolume);
	assert(m_InputVolume);

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_InputVolume, show);
	
	for (int i = X; i <= Z; i++)
	{
		int currentView = AsixToView(i);
		m_ChildViewList[currentView]->VmeShow(m_InputVolume, show);
		m_CameraToSlicer[i] = new albaAttachCamera(m_Gui, ((albaViewVTK*)m_ChildViewList[currentView])->m_Rwi, this);
		m_CameraToSlicer[i]->SetStartingMatrix(m_SlicingResetMatrix[i]);
		m_CameraToSlicer[i]->SetAttachedMatrix(m_SlicingMatrix[i]->GetVTKMatrix());
		m_CameraToSlicer[i]->SetAttachedMatrix(m_SlicingMatrix[i]->GetVTKMatrix());
		m_CameraToSlicer[i]->SetAttachedMatrix(m_SlicingMatrix[i]->GetVTKMatrix());
		m_CameraToSlicer[i]->SetAttachedMatrix(m_SlicingMatrix[i]->GetVTKMatrix());
		m_CameraToSlicer[i]->EnableAttachCamera();

		albaPipeVolumeArbSlice* pipeVolSlice = albaPipeVolumeArbSlice::SafeDownCast(m_ChildViewList[currentView]->GetNodePipe(m_InputVolume));
		if (pipeVolSlice)
			pipeVolSlice->SetEnableSliceViewCorrection(true);
		
		BuildCameraConeVME(i);
	}

	SetEnableGPU();
	ResetCameraToSlices();
	SetSlices();
		
	for (int i = X; i <= Z; i++)
	{
		if(m_GizmoRT[i]==NULL)
		{
			m_GizmoRT[i] = new albaGizmoCrossRotateTranslate();
			m_GizmoRT[i]->Create(m_InputVolume, this, true, i);
		}

		m_GizmoRT[i]->SetName(gizmoNames[i]);
		m_GizmoRT[i]->SetInput(m_InputVolume);
		m_GizmoRT[i]->SetRefSys(m_InputVolume);
		m_GizmoRT[i]->SetAbsPose(m_SlicingResetMatrix[i]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GREW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTAEW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTPEW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GRNS, gizmoColors[i][1]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTANS, gizmoColors[i][1]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTPNS, gizmoColors[i][1]);
		m_GizmoRT[i]->Show(true);
	}

	UpdateConesPosition();
	UpdateSubviewsCamerasToFaceSlices();
	CreateViewCameraNormalFeedbackActors();

	m_IsShowingSlicerGizmo = false;
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::BuildCameraConeVME(int side)
{
	char coneNames[3][12] = { "ConeXCamera","ConeYCamera","ConeZCamera" };
	double col[3][3] = { { 1,0,0 },{ 0,1,0 },{ 0,0,1 } };
	double b[6];
	m_InputVolume->GetOutput()->GetVMELocalBounds(b);

	double p1[3] = { b[0], b[2], b[4] };
	double p2[3] = { b[1], b[3], b[5] };
	double coneRadius = sqrt(vtkMath::Distance2BetweenPoints(p1, p2)) / 10.0;

	vtkConeSource *cameraConeSource = vtkConeSource::New();
	cameraConeSource->SetCenter(0, 0, coneRadius*2.0);
	cameraConeSource->SetResolution(20);
	cameraConeSource->SetDirection(0, 0, -1);

	cameraConeSource->SetRadius(coneRadius);
	cameraConeSource->SetHeight(coneRadius);

	cameraConeSource->CappingOn();
	cameraConeSource->Update();

	if (m_CameraConeVME[side] == NULL)
		albaNEW(m_CameraConeVME[side]);

	// DEBUG
	m_CameraConeVME[side]->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_CameraConeVME[side]->SetName(coneNames[side]);
	m_CameraConeVME[side]->SetData(cameraConeSource->GetOutput(), m_InputVolume->GetTimeStamp());
	m_CameraConeVME[side]->SetVisibleToTraverse(false);

	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetColor(col[side]);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetAmbient(1);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetDiffuse(0);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetSpecular(0);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetOpacity(0.2);

	// default slicer matrix rotation component is identity when the input volume has identity pose matrix
	if (m_CameraConeVME[side]->GetParent() != m_InputVolume)
		m_CameraConeVME[side]->ReparentTo(m_InputVolume);

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_CameraConeVME[side], true);

	albaPipeSurface *pipeY = (albaPipeSurface *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_CameraConeVME[side]);
	pipeY->SetActorPicking(false);

	cameraConeSource->Delete();
}
//----------------------------------------------------------------------------
bool albaViewArbitraryOrthoSlice::BelongsToNormalGizmo( albaVME * vme, int side)
{
	char nameVect[][13] = { "m_GizmoXView", "m_GizmoYView", "m_GizmoZView" };
	char *name=nameVect[side];

	albaVMEGizmo *gizmo = albaVMEGizmo::SafeDownCast(vme);

	albaObserver *mediator = NULL;
	mediator = gizmo->GetMediator();
	
	albaGizmoInterface *gizmoMediator = NULL;
	gizmoMediator = dynamic_cast<albaGizmoInterface *>(mediator);
	if (gizmoMediator && gizmoMediator->GetName().Equals(name))
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::PostMultiplyEventMatrix(vtkMatrix4x4 * matrix, int slicerAxis, int isRotation)
{
	vtkTransform *tr1 = vtkTransform::New();
	tr1->PostMultiply();
	tr1->SetMatrix(m_GizmoRT[slicerAxis]->GetAbsPose()->GetVTKMatrix());
	tr1->Concatenate(matrix);
	tr1->Update();

	albaMatrix absPose;
	absPose.DeepCopy(tr1->GetMatrix());
	absPose.SetTimeStamp(m_GizmoRT[slicerAxis]->GetAbsPose()->GetTimeStamp());
	
	// ... and update the slicer with the new abs pose
	
	m_GizmoRT[slicerAxis]->SetAbsPose(&absPose);
	if (m_CameraFollowGizmo)
	{
		m_SlicingMatrix[slicerAxis]->DeepCopy(m_GizmoRT[slicerAxis]->GetAbsPose());
	}
	else
	{
		albaMatrix rotation;
		rotation.CopyRotation(matrix);
		vtkTransform *tr2 = vtkTransform::New();
		tr2->PostMultiply();
		tr2->SetMatrix(m_SlicingMatrix[slicerAxis]->GetVTKMatrix());
		if (isRotation)
			tr2->Concatenate(matrix);
		else
			tr2->Concatenate(rotation.GetVTKMatrix());

		tr2->Update();

		m_SlicingMatrix[slicerAxis]->DeepCopy(tr2->GetMatrix());
 	}

	m_CameraToSlicer[slicerAxis]->UpdateCameraMatrix();

	vtkDEL(tr1);
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::ResetCameraToSlices()
{
	for(int i=0;i<=Z;i++)
		((albaViewVTK*)m_ChildViewList[AsixToView(i)])->CameraReset(m_InputVolume);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::StoreCameraParametersForAllSubviews()
{
	for (int i = 0; i <= Z; i++)
	{
		((albaViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetPosition(m_CameraPositionForReset[i]);
		((albaViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetFocalPoint(m_CameraFocalPointForReset[i]);
		((albaViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetViewUp(m_CameraViewUpForReset[i]);
	}
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::RestoreCameraParametersForAllSubviews()
{
	for (int i = 0; i <= Z; i++)
	{
		vtkCamera *xViewCamera = ((albaViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera();
		xViewCamera->SetPosition(m_CameraPositionForReset[i]);
		xViewCamera->SetFocalPoint(m_CameraFocalPointForReset[i]);
		xViewCamera->SetViewUp(m_CameraViewUpForReset[i]);
	}
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::CreateViewCameraNormalFeedbackActors()
{
	double col[3][3] = { { 1,0,0 },{ 0,1,0 },{ 0,0,1 } };
	
	for (int i = X; i <= Z; i++)
	{
		int size[2];
		this->GetWindow()->GetSize(&size[0], &size[1]);
		vtkSphereSource *ss = vtkSphereSource::New();

		ss->SetRadius(size[0] / 20.0);
		ss->SetCenter(0, 0, 0);
		ss->SetThetaResolution(1);
		ss->Update();

		vtkCoordinate *coord = vtkCoordinate::New();
		coord->SetCoordinateSystemToDisplay();
		coord->SetValue(size[0] - 1, size[1] - 1, 0);

		vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
		pdmd->SetInput(ss->GetOutput());
		pdmd->SetTransformCoordinate(coord);

		vtkProperty2D *pd = vtkProperty2D::New();
		pd->SetDisplayLocationToForeground();
		pd->SetLineWidth(4);
		pd->SetColor(col[i]);
		pd->SetOpacity(0.2);

		vtkActor2D *border = vtkActor2D::New();
		border->SetMapper(pdmd);
		border->SetProperty(pd);
		border->SetPosition(1, 1);

		((albaViewVTK*)(m_ChildViewList[AsixToView(i)]))->m_Rwi->m_RenFront->AddActor2D(border);

		vtkDEL(ss);
		vtkDEL(coord);
		vtkDEL(pdmd);
		vtkDEL(pd);
		vtkDEL(border);
	}
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::UpdateWindowing(bool enable,albaVME *vme)
{
	if(vme->GetOutput() && vme->GetOutput()->IsA("albaVMEOutputVolume") && enable)
		VolumeWindowing(vme);
}

//----------------------------------------------------------------------------
bool albaViewArbitraryOrthoSlice::AcceptRefSys(albaVME *node)
{
	if (GLO_CurrentVolume == NULL || !node->IsA("albaVMERefSys"))
		return false;

	for (int i = 0; i < 3; i++)
	{
		albaString tagName;
		tagName.Printf("ArbOrthoSliceMtr%d", i);
		if (node->GetTagArray()->GetTag(tagName) == NULL)
			return false;
	}

	double bounds[6];
	GLO_CurrentVolume->GetOutput()->GetBounds(bounds);
	double pos[3];
	albaTransform::GetPosition(*node->GetOutput()->GetAbsMatrix(), pos);

	return ((pos[0] >= bounds[0]) && (pos[0] <= bounds[1]) && \
		(pos[1] >= bounds[2]) && (pos[1] <= bounds[3]) && \
		(pos[2] >= bounds[4]) && (pos[2] <= bounds[5]));
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnLoadFromRefsys()
{
	GLO_CurrentVolume = m_InputVolume;
	albaString title = _("Choose RefSys");
	albaEvent e;
	e.SetId(VME_CHOOSE);
	e.SetPointer(&albaViewArbitraryOrthoSlice::AcceptRefSys);
	e.SetString(&title);
	albaEventMacro(e);
	albaVME *n = e.GetVme();
	
	if (n != NULL)
		RestoreFromVME(n);
}
//----------------------------------------------------------------------------
albaViewVTK * albaViewArbitraryOrthoSlice::GetViewArbitrary()
{
	return (albaViewVTK*)m_ChildViewList[PERSPECTIVE_VIEW];
}
//----------------------------------------------------------------------------
albaViewVTK * albaViewArbitraryOrthoSlice::GetViewSlice(int axis)
{
	return (albaViewVTK*)m_ChildViewList[AsixToView(axis)];
}
//----------------------------------------------------------------------------
albaPipe* albaViewArbitraryOrthoSlice::GetPipeSlice(int axis)
{
	albaPipe *pipeSlice = NULL;
	pipeSlice = GetViewSlice(axis)->GetNodePipe(m_InputVolume);

	return pipeSlice;
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::SetSlicingOrigin(double* origin)
{
	m_SlicingOriginGUI[0] = origin[0];
	m_SlicingOriginGUI[1] = origin[1];
	m_SlicingOriginGUI[2] = origin[2];

	OnSlicingOrigin();
}
