/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewArbitraryOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi
 
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
	m_SlicerResetMatrix[0] = m_SlicerResetMatrix[1] = m_SlicerResetMatrix[2] = NULL;
	m_CurrentVolume   = NULL;
	m_Slicer[0] = m_Slicer[1] = m_Slicer[2] = NULL;
	m_CameraToSlicer[0] = m_CameraToSlicer[1] = m_CameraToSlicer[2] = NULL;
	m_VolumeVTKDataCenterABSCoords[0] = m_VolumeVTKDataCenterABSCoords[1] = m_VolumeVTKDataCenterABSCoords[2] = 0.0;
	m_VolumeVTKDataCenterABSCoordinatesReset[0] = m_VolumeVTKDataCenterABSCoordinatesReset[1] = m_VolumeVTKDataCenterABSCoordinatesReset[2] = 0.0;
	m_CameraConeVME[0] = m_CameraConeVME[1] = m_CameraConeVME[2] = NULL;
	for (int i = X; i <= Z; i++)
	{
		m_CameraPositionForReset[i][0] = m_CameraPositionForReset[i][1] = m_CameraPositionForReset[i][2] = 0;
		m_CameraViewUpForReset[i][0] = m_CameraViewUpForReset[i][1] = m_CameraViewUpForReset[i][2] = 0;
	}

	m_SkipCameraUpdate = 0;
	m_EnableGPU = false; 
	m_IsShowingSlicerGizmo = false;
}
//----------------------------------------------------------------------------
albaViewArbitraryOrthoSlice::~albaViewArbitraryOrthoSlice()
{
	for(int i=X;i<=Z;i++)
	{
		albaDEL(m_SlicerResetMatrix[i]);
		delete(m_CameraToSlicer[i]);
	}
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::PackageView()
{
	m_View3d = new albaViewVTK("",CAMERA_PERSPECTIVE,true,false,0,false,m_AxesType);
	m_View3d->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", MUTEX);
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
	m_ViewSlice[v]->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", NON_VISIBLE);

	m_ViewSlice[v]->PlugVisualPipe("albaVMEImage", "albaPipeBox", NON_VISIBLE);
	m_ViewSlice[v]->PlugVisualPipe("albaVMESegmentationVolume", "albaPipeVolumeOrthoSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEPolyline", "albaPipePolylineSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEPolylineSpline", "albaPipePolylineSlice");
	m_ViewSlice[v]->PlugVisualPipe("albaVMEMeter", "albaPipePolyline");
	m_ViewSlice[v]->PlugVisualPipe("medVMEMuscleWrapper", "albaPipeSurfaceSlice");

	PlugChildView(m_ViewSlice[v]);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::VmeShow(albaVME *vme, bool show)
{
	m_SkipCameraUpdate++;
	if ((m_IsShowingSlicerGizmo && vme->IsA("albaVMEGizmo")) || 
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
				double surfaceOriginTranslated[3];
				double normal[3];
				((albaViewSlice*)m_ChildViewList[view])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
				surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
				surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
				surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

				pipeSlice->SetSlice(surfaceOriginTranslated, normal);

				albaPipeMeshSlice* meshPipe = albaPipeMeshSlice::SafeDownCast(nodePipe);
				if(meshPipe)
					meshPipe->SetFlipNormalOff();
			}
		}
	}

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

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);

	m_SkipCameraUpdate--;
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
	for (int i = 0; i < 3; i++)
	{
		if (gizmo == m_GizmoRT[i]->m_GizmoCrossTranslate || gizmo == m_GizmoRT[i]->m_GizmoCrossRotate)
			return i;
	}
	return -1;
}

//----------------------------------------------------------------------------
int albaViewArbitraryOrthoSlice::IsGizmoTranslate(void *gizmo)
{
	for (int i = 0; i < 3; i++)
	{
		if (gizmo == m_GizmoRT[i]->m_GizmoCrossTranslate)
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEventGizmoTranslate(vtkMatrix4x4 *matrix, int planeSkip)
{

	vtkCamera *zViewCamera = ((albaViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera();
	
	for (int i = 0; i < 3; i++)
	{
		if (i != planeSkip)
		{
			PostMultiplyEventMatrixToGizmoCross(matrix, m_GizmoRT[i]);
			PostMultiplyEventMatrixToSlicer(matrix, i);
		}
	}

	//compute the incremental translation
	vtkTransform *tr;
	vtkNEW(tr);
	tr->PostMultiply();
	tr->SetMatrix(matrix);
	tr->Update();
	double translation[3];
	tr->GetPosition(translation);

	albaLogMessage("Translation: x:%f, y:%f, z:%f", translation[0], translation[1], translation[2]);

	//increase the translation
	m_VolumeVTKDataCenterABSCoords[0] += translation[0];
	m_VolumeVTKDataCenterABSCoords[1] += translation[1];
	m_VolumeVTKDataCenterABSCoords[2] += translation[2];
	vtkDEL(tr);

	SetSlices();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnEventGizmoRotate(vtkMatrix4x4 *matrix, int planeSkip)
{
	// roll the camera based on gizmo
	vtkALBASmartPointer<vtkTransform> tr;
	tr->SetMatrix(matrix);
	
	for (int i = 0; i < 3; i++)
	{
		if (i != planeSkip)
		{
			PostMultiplyEventMatrixToGizmoCross(matrix, m_GizmoRT[i]);
			PostMultiplyEventMatrixToSlicer(matrix, i);
			m_ChildViewList[AsixToView(i)]->GetRWI()->GetCamera()->ApplyTransform(tr);
		}
	}

	SetSlices();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::SetSlices()
{
	//update the normal of the cutter plane of the surface
	albaVME *root = m_CurrentVolume->GetRoot();
	for (int view = Z_VIEW; view <= Y_VIEW; view++)
	{
		double surfaceOriginTranslated[3];
		double normal[3];
		((albaViewSlice*)m_ChildViewList[view])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		albaVMEIterator *iter = root->NewIterator();
		for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		{
			albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(m_ChildViewList[view]->GetNodePipe(node));
			if (pipeSlice)
				pipeSlice->SetSlice(surfaceOriginTranslated, normal);
		}
		iter->Delete();
	}
	CameraUpdate();
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
		case ID_UPDATE_LUT:
			UpdateSlicersLUT();
		break;
		case ID_GPUENABLED:
			SetEnableGPU();
		break;
		default:
			albaViewCompound::OnEvent(alba_event);
		}
	}
}

void albaViewArbitraryOrthoSlice::SetEnableGPU()
{
	for (int i = X; i <= Z; i++)
		if (m_Slicer) m_Slicer[i]->SetEnableGPU(m_EnableGPU);
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
	m_Gui->Bool(ID_GPUENABLED, "Enable GPU Acceleration", &m_EnableGPU, 1, "Enable GPU Accelerarion");
	m_Gui->Divider();

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);
	m_Gui->Update();
		
	EnableWidgets( (m_CurrentVolume != NULL) );
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::VmeRemove(albaVME *vme)
{
	if (m_CurrentVolume && vme == m_CurrentVolume) 
	{
		HideVolume();
		m_CurrentVolume = NULL;
	}

	Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicers(vtkMatrix4x4 *matrix)
{  
	for (int i = 0; i < 3; i++)
		PostMultiplyEventMatrixToSlicer(matrix, i);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicer(vtkMatrix4x4 *matrix, int slicerAxis)
{  
	// handle incoming transform event...
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_Slicer[slicerAxis]->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(matrix);
		tr->Update();

		albaMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(m_Slicer[slicerAxis]->GetTimeStamp());

		// ... and update the slicer with the new abs pose
		m_Slicer[slicerAxis]->SetAbsMatrix(absPose);

		// clean up
		tr->Delete();
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
void albaViewArbitraryOrthoSlice::SetSlicerMatrix(albaMatrix* matrix, int axis)
{
	m_GizmoRT[axis]->SetAbsPose(matrix);

	RestoreCameraParametersForAllSubviews();
	CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::CreateGuiView()
{
	m_GuiView = new albaGUI(this);

	//m_GuiView->Label("");
	m_LutSlider = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500,24);
	m_LutSlider->SetMinSize(wxSize(500,24));
	EnableWidgets(m_CurrentVolume != NULL);
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

	mmaMaterial *currentSurfaceMaterial = m_Slicer[Z]->GetMaterial();
	m_ColorLUT = currentSurfaceMaterial->m_ColorLut;
	assert(m_ColorLUT);
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);

	m_Slicer[Y]->GetMaterial()->m_ColorLut->SetRange((long)sr[0],(long)sr[1]);
	m_Slicer[Z]->GetMaterial()->m_ColorLut->SetRange((long)sr[0],(long)sr[1]);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::ShowVolume( albaVME * vme, bool show )
{
	if (m_CurrentVolume == vme) return;

	m_CurrentVolume = vme;

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
	localToABSTPDF->GetOutput()->GetCenter(m_VolumeVTKDataCenterABSCoords);
	localToABSTPDF->GetOutput()->GetCenter(m_VolumeVTKDataCenterABSCoordinatesReset);

	vtkTransform *transformReset;
	vtkNEW(transformReset);
	transformReset->Identity();
	transformReset->Translate(m_VolumeVTKDataCenterABSCoordinatesReset);
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

		albaNEW(m_SlicerResetMatrix[i]);
		m_SlicerResetMatrix[i]->DeepCopy(slicerTransform->GetMatrix());
	}

	ShowSlicers(vme, show);

	vtkDEL(points);
	vtkDEL(sliceCenterLocalCoordsPolydata);
	vtkDEL(sliceCenterLocalCoordsToABSCoordsTransform);
	vtkDEL(localToABSTPDF);
	vtkDEL(transformReset);

	VolumeWindowing(vme);
	UpdateSlicersLUT();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::HideVolume()
{
	OnReset();

	EnableWidgets(false);
	for(int i=X; i<=Z; i++)
	{
		VmeShow(m_CameraConeVME[i], false);
		VmeShow(m_Slicer[i], false);
		m_CameraConeVME[i]->ReparentTo(NULL);
		m_GizmoRT[i]->Show(false);

		m_CameraToSlicer[i]->SetVme(NULL);
		m_Slicer[i]->SetBehavior(NULL);
		m_Slicer[i]->ReparentTo(NULL);
		albaDEL(m_CameraConeVME[i]);
		albaDEL(m_Slicer[i]);
		albaDEL(m_SlicerResetMatrix[i]);
	}

	m_CurrentVolume = NULL;
	m_ColorLUT = NULL;
	m_LutWidget->SetLut(m_ColorLUT);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnReset()
{
	for (int i = X; i <= Z; i++)
	{
		m_GizmoRT[i]->SetAbsPose(m_SlicerResetMatrix[i]);
		m_Slicer[i]->SetAbsMatrix(*m_SlicerResetMatrix[i]);
	}

	RestoreCameraParametersForAllSubviews();

	CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnLUTRangeModified()
{
	albaVME *vme = GetSceneGraph()->GetSelectedVme();

	if( m_CurrentVolume)
		UpdateSlicersLUT();
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::OnLUTChooser()
{
	double *sr;

	if(m_CurrentVolume ) {
		sr = m_ColorLUT->GetRange();
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);

		m_Slicer[X]->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_Slicer[X]->GetMaterial()->m_ColorLut->Modified();
		m_Slicer[Y]->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_Slicer[Y]->GetMaterial()->m_ColorLut->Modified();

		CameraUpdate();
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

	EnableWidgets((m_CurrentVolume != NULL));
	// register sliced volume
	m_InputVolume = albaVMEVolumeGray::SafeDownCast(vmeVolume);
	assert(m_InputVolume);

	for (int i = X; i <= Z; i++)
	{
		if(m_Slicer[i]==NULL)
			albaNEW(m_Slicer[i]);
		m_Slicer[i]->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
		m_Slicer[i]->SetName(slicerNames[i]);
		m_Slicer[i]->ReparentTo(vmeVolume);
		m_Slicer[i]->SetAbsMatrix(*m_SlicerResetMatrix[i]);
		m_Slicer[i]->SetSlicedVMELink(vmeVolume);
		m_Slicer[i]->SetUpdateVTKPropertiesFromMaterial(false);
		m_Slicer[i]->GetMaterial()->m_ColorLut->DeepCopy(albaVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
		m_Slicer[i]->SetEnableGPU(m_EnableGPU);
		m_Slicer[i]->Update();

		m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_Slicer[i], show);
		albaPipeSurfaceTextured *pipeSlice = albaPipeSurfaceTextured::SafeDownCast(m_ChildViewList[PERSPECTIVE_VIEW]->GetNodePipe(m_Slicer[i]));
		if(pipeSlice)
			pipeSlice->SetActorPicking(false);

		m_ChildViewList[AsixToView(i)]->VmeShow(m_Slicer[i], show);
		pipeSlice = albaPipeSurfaceTextured::SafeDownCast(m_ChildViewList[AsixToView(i)]->GetNodePipe(m_Slicer[i]));
		if (pipeSlice)
			pipeSlice->SetActorPicking(false);

		BuildCameraConeVME(i);
		m_CameraToSlicer[i] = new albaAttachCamera(m_Gui, ((albaViewVTK*)m_ChildViewList[AsixToView(i)])->m_Rwi, this);
		m_CameraToSlicer[i]->SetStartingMatrix(m_Slicer[i]->GetOutput()->GetAbsMatrix());
		m_CameraToSlicer[i]->SetVme(m_Slicer[Z]);
		m_CameraToSlicer[i]->EnableAttachCamera();

	}

	ResetCameraToSlices();
		
	for (int i = X; i <= Z; i++)
	{
		if(m_GizmoRT[i]==NULL)
		{
			m_GizmoRT[i] = new albaGizmoCrossRotateTranslate();
			m_GizmoRT[i]->Create(m_Slicer[i], this, true, i);
		}

		m_GizmoRT[i]->SetName(gizmoNames[i]);
		m_GizmoRT[i]->SetInput(m_Slicer[i]);
		m_GizmoRT[i]->SetRefSys(m_Slicer[i]);
		m_GizmoRT[i]->SetAbsPose(m_SlicerResetMatrix[i]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GREW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTAEW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTPEW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GRNS, gizmoColors[i][1]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTANS, gizmoColors[i][1]);
		m_GizmoRT[i]->SetColor(albaGizmoCrossRotateTranslate::GTPNS, gizmoColors[i][1]);
		m_GizmoRT[i]->Show(true);
		m_Slicer[i]->SetVisibleToTraverse(false);
	}

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
	m_CurrentVolume->GetOutput()->GetVMELocalBounds(b);

	double p1[3] = { b[0], b[2], b[4] };
	double p2[3] = { b[1], b[3], b[5] };
	double coneRadius = sqrt(vtkMath::Distance2BetweenPoints(p1, p2)) / 10.0;

	vtkConeSource *cameraConeSource = vtkConeSource::New();
	cameraConeSource->SetCenter(0, 0, b[(side * 2) + 1] / 2 + coneRadius / 2);
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
	m_CameraConeVME[side]->SetData(cameraConeSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_CameraConeVME[side]->SetVisibleToTraverse(false);

	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetColor(col[side]);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetAmbient(1);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetDiffuse(0);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetSpecular(0);
	m_CameraConeVME[side]->GetMaterial()->m_Prop->SetOpacity(0.2);

	// default slicer matrix rotation component is identity when the input volume has identity pose matrix
	if (m_CameraConeVME[side]->GetParent() != m_Slicer[side])
		m_CameraConeVME[side]->ReparentTo(m_Slicer[side]);

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
void albaViewArbitraryOrthoSlice::PostMultiplyEventMatrixToGizmoCross(vtkMatrix4x4 * matrix, albaGizmoCrossRotateTranslate *targetGizmo )
{

	vtkTransform *tr1 = vtkTransform::New();
	tr1->PostMultiply();
	tr1->SetMatrix(targetGizmo->GetAbsPose()->GetVTKMatrix());
	tr1->Concatenate(matrix);
	tr1->Update();

	albaMatrix absPose;
	absPose.DeepCopy(tr1->GetMatrix());
	absPose.SetTimeStamp(m_GizmoRT[1]->GetAbsPose()->GetTimeStamp());

	targetGizmo->SetAbsPose(&absPose);

	vtkDEL(tr1);
}
//----------------------------------------------------------------------------
void albaViewArbitraryOrthoSlice::ResetCameraToSlices()
{
	for(int i=0;i<=Z;i++)
		((albaViewVTK*)m_ChildViewList[AsixToView(i)])->CameraReset(m_Slicer[i]);
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
void albaViewArbitraryOrthoSlice::UpdateSlicers(int axis)
{
	m_Slicer[axis]->SetAbsMatrix(m_Slicer[axis]->GetAbsMatrixPipe()->GetMatrix());
	m_Slicer[axis]->GetSurfaceOutput()->GetVTKData()->Modified();
	m_Slicer[axis]->GetSurfaceOutput()->GetVTKData()->Update();
}
//----------------------------------------------------------------------------
albaPipeSurface * albaViewArbitraryOrthoSlice::GetPipe(int inView, albaVMESurface *inSurface)
{
	return (albaPipeSurface *)((m_ChildViewList[inView])->GetNodePipe(inSurface));
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
void albaViewArbitraryOrthoSlice::UpdateSlicersLUT()
{
	double low, hi;
	m_LutSlider->GetSubRange(&low,&hi);
	m_ColorLUT->SetTableRange(low,hi);

	for (int i = X; i <= Z; i++)
	{
		albaVMEOutputSurface *surfaceOutputSlicer = albaVMEOutputSurface::SafeDownCast(m_Slicer[i]->GetOutput());
		surfaceOutputSlicer->Update();
		surfaceOutputSlicer->GetMaterial()->m_ColorLut->SetTableRange(low, hi);
	}

	CameraUpdate();
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
	GLO_CurrentVolume = m_CurrentVolume;
	albaString title = _("Choose RefSys");
	albaEvent e;
	e.SetId(VME_CHOOSE);
	e.SetPointer(&albaViewArbitraryOrthoSlice::AcceptRefSys);
	e.SetString(&title);
	albaEventMacro(e);
	albaVME *n = e.GetVme();
	if (n != NULL)
	{
		albaMatrix *refSysMatrix = n->GetOutput()->GetAbsMatrix();
		albaMatrix *slicerMatrix = GetSlicerMatrix();

		//We need to find the roto-translation matrix that moves current slicer matrix to the refsys absolute matrix 
		//We can use the following formula to find the B matrix:
		//((AB)^?1)*(A) = B^?1

		//In this formula AB is the target, the refsys matrix in this case
		//A is the current slicer matrix
		//and B is the transform from current slicer matrix to the refsys

		albaMatrix translation, rotation;

		//Calculating (AB)^?1
		albaMatrix ABinverse;
		ABinverse.DeepCopy(refSysMatrix);
		ABinverse.Invert();
		
		//Multiplying (AB)^?1 to A to obtain B^-1
		albaTransform Binverse;
		Binverse.SetMatrix(ABinverse);
		Binverse.Concatenate(*slicerMatrix, false);

		//Obtaing the final matrix by inverting Binverse, (B^-1)^-1 = B
		albaMatrix finalMatrix = Binverse.GetMatrix();
		finalMatrix.Invert();

		//OneventGizmoRotate manages both rotations and translations
		OnEventGizmoRotate(finalMatrix.GetVTKMatrix(), -1);
	}
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
	pipeSlice = GetViewSlice(axis)->GetNodePipe(m_Slicer[axis]);

	return pipeSlice;
}
