/*=========================================================================

 Program: MAF2
 Module: mafViewArbitraryOrthoSlice
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const int BOUND_0=0;
const int BOUND_1=1;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGizmoCrossRotateTranslate.h"
#include "mafGUI.h"
#include "mafDecl.h"
#include "mafViewArbitraryOrthoSlice.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeImage3D.h"
#include "mafPipeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipeSurface.h"
#include "mafPipeSurfaceTextured.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafGizmoCrossTranslate.h"
#include "mafGizmoCrossRotate.h"
#include "mafSceneGraph.h"
#include "mafEvent.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttachCamera.h"
#include "mafInteractorGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEIterator.h"
#include "mafGUILutPreset.h"
#include "mafVMEOutputSurface.h"
#include "mafAttribute.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafPipeMesh.h"
#include "mafPipeMeshSlice.h"
#include "mafPipePolylineGraphEditor.h"

#include "vtkTransform.h"
#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkPolyDataNormals.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkConeSource.h"
#include "vtkProperty.h"
#include "vtkTextProperty.h"
#include "vtkRenderer.h"
#include "vtkCellPicker.h"
#include "mafTransformFrame.h"
#include "mafVMEGizmo.h"
#include "mafGizmoInterface.h"
#include "mafDataPipe.h"
#include <algorithm>
#include "vtkStructuredPoints.h"
#include "vtkDataSetWriter.h"
#include "vtkUnsignedShortArray.h"
#include "vtkShortArray.h"
#include "vtkLineSource.h"
#include "vtkMatrix4x4.h"
#include "vtkBMPWriter.h"
#include "mafRWIBase.h"
#include "vtkPNGWriter.h"
#include "wx\busyinfo.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkOutlineFilter.h"
#include "vtkSphereSource.h"
#include "vtkMAFSmartPointer.h"
#include "vtkTextSource.h"
#include "vtkCaptionActor2D.h"
#include "mafProgressBarHelper.h"

mafCxxTypeMacro(mafViewArbitraryOrthoSlice);

const int MID = 1;
const int PID = 0;


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

enum AXIS_ID
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
};

//----------------------------------------------------------------------------
mafViewArbitraryOrthoSlice::mafViewArbitraryOrthoSlice(wxString label, bool show_ruler) : mafViewCompoundWindowing(label, 2, 2)
{

	m_ThicknessText[RED] = "UNDEFINED_THICKNESS_RED_TEXT";
	m_ThicknessText[GREEN] = "UNDEFINED_THICKNESS_GREEN_TEXT";
	m_ThicknessText[BLUE] = "UNDEFINED_THICKNESS_BLUE_TEXT";

	m_XSlicerPicker = NULL;
	m_YSlicerPicker = NULL;
	m_ZSlicerPicker = NULL;

	m_DebugMode = false;


	m_NumberOfAxialSections[RED] = 3;
	m_NumberOfAxialSections[GREEN] = 3;
	m_NumberOfAxialSections[BLUE] = 3;

	m_FeedbackLineHeight[RED] = 20;
	m_FeedbackLineHeight[GREEN] = 20;
	m_FeedbackLineHeight[BLUE] = 20;

	m_ExportPlanesHeight[RED] = 20;
	m_ExportPlanesHeight[GREEN] = 20;
	m_ExportPlanesHeight[BLUE] = 20;


	m_InputVolume = NULL;
	m_GizmoZView = NULL;
	m_GizmoYView = NULL;
	m_GizmoXView = NULL;

	m_ViewArbitrary   = NULL;

	m_ViewSliceX       = NULL;
	m_ViewSliceY       = NULL;
	m_ViewSliceZ       = NULL;

	m_SlicerXResetMatrix     = NULL;
	m_SlicerYResetMatrix     = NULL;
	m_SlicerZResetMatrix     = NULL;

	m_ShowGizmo = 1;

	m_CurrentVolume   = NULL;
	m_CurrentImage    = NULL;

	m_SlicerY = NULL;
	m_SlicerY = NULL;
	m_SlicerZ          = NULL;
	m_GuiGizmos       = NULL;
	m_ShowGizmo = 1;
	m_AttachCameraToSlicerXInXView    = NULL;
	m_AttachCameraToSlicerYInYView    = NULL;
	m_AttachCameraToSlicerZInZView    = NULL;

	m_CurrentPolylineGraphEditor = NULL;

	m_VolumeVTKDataCenterABSCoords[0] = 0.0;
	m_VolumeVTKDataCenterABSCoords[1] = 0.0;
	m_VolumeVTKDataCenterABSCoords[2] = 0.0;

	m_VolumeVTKDataCenterABSCoordinatesReset[0] = 0.0;
	m_VolumeVTKDataCenterABSCoordinatesReset[1] = 0.0;
	m_VolumeVTKDataCenterABSCoordinatesReset[2] = 0.0;

	m_XCameraConeVME = NULL;
	m_YCameraConeVME = NULL;
	m_ZCameraConeVME = NULL;

	m_XCameraPositionForReset[0] = 0;
	m_XCameraPositionForReset[1] = 0;
	m_XCameraPositionForReset[2] = 0;

	m_YCameraPositionForReset[0] = 0;
	m_YCameraPositionForReset[1] = 0;
	m_YCameraPositionForReset[2] = 0;

	m_ZCameraPositionForReset[0] = 0;
	m_ZCameraPositionForReset[1] = 0;
	m_ZCameraPositionForReset[2] = 0;

	m_XCameraViewUpForReset[0] = 0;
	m_XCameraViewUpForReset[1] = 0;
	m_XCameraViewUpForReset[2] = 0;

	m_YCameraViewUpForReset[0] = 0;
	m_YCameraViewUpForReset[1] = 0;
	m_YCameraViewUpForReset[2] = 0;

	m_ZCameraViewUpForReset[0] = 0;
	m_ZCameraViewUpForReset[1] = 0;
	m_ZCameraViewUpForReset[2] = 0;
}
//----------------------------------------------------------------------------
mafViewArbitraryOrthoSlice::~mafViewArbitraryOrthoSlice()
{
	mafDEL(m_XSlicerPicker);
	mafDEL(m_XSlicerPicker);




	m_SlicerZResetMatrix   = NULL;
	m_CurrentVolume = NULL;
	m_CurrentImage  = NULL;
	m_ColorLUT      = NULL;

	cppDEL(m_GizmoZView);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PackageView()
{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE,true,false,0,false,mafAxes::HEAD);
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
	
	m_ViewArbitrary->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);

	m_ViewSliceX = new mafViewVTK("",CAMERA_OS_X,true,false,0,false,mafAxes::HEAD);
	m_ViewSliceX->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);

	m_ViewSliceY = new mafViewVTK("",CAMERA_OS_Y,true,false,0,false,mafAxes::HEAD);
	m_ViewSliceY->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);

	m_ViewSliceZ = new mafViewVTK("",CAMERA_OS_Z,true,false,0,false,mafAxes::HEAD);
	m_ViewSliceZ->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);

	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSliceZ);
	PlugChildView(m_ViewSliceX);
	PlugChildView(m_ViewSliceY);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::VmeShow(mafVME *vme, bool show)
{
	if (vme->IsA("mafVMEGizmo"))
	{
		if (BelongsToZNormalGizmo(vme))
		{
			m_ChildViewList[Z_VIEW]->VmeShow(vme, show);
			m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
		}
		else if (BelongsToXNormalGizmo(vme))
		{
			m_ChildViewList[X_VIEW]->VmeShow(vme, show);
			m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
		}
		else if (BelongsToYNormalGizmo(vme))
		{
			m_ChildViewList[Y_VIEW]->VmeShow(vme, show);
			m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
		}
		else
		{
			return;
		}
	}

	else
	{
		m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(vme, show);
		m_ChildViewList[Z_VIEW]->VmeShow(vme, show);
		m_ChildViewList[X_VIEW]->VmeShow(vme, show);
		m_ChildViewList[Y_VIEW]->VmeShow(vme, show);
	}

	vme->Update();
	if (show)
	{
		if(vme->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			ShowMafVMEVolume(vme, show);
			StoreCameraParametersForAllSubviews();
		}

		else if(vme->IsA("mafVMESurface") || vme->IsA("mafVMESurfaceParametric") || vme->IsA("mafVMELandmark") || vme->IsA("mafVMELandmarkCloud"))
		{
			// ShowVMESurfacesAndLandmarks(node);

		}
		else if(vme->IsA("mafVMEPolylineEditor"))
		{
			ShowmafVMEPolylineEditor(vme);
		}
		else if(vme->IsA("mafVMEMesh"))
		{
			ShowMafVMEMesh(vme);

		}
		else if(vme->IsA("mafVMEImage"))
		{	
			ShowMafVMEImage(vme);
		}
	}
	else//if show==false
	{
		if(vme->IsA("mafVMEPolylineGraphEditor"))
		{
			m_CurrentPolylineGraphEditor = NULL;
		}

		if(vme->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			HideMafVMEVolume();
		}
		else if(vme->IsA("mafVMEImage")) {
			HideMafVmeImage();
		}
	}

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEvent(mafEventBase *maf_event)
{
	assert(maf_event);

	if (maf_event->GetSender() == this->m_Gui || maf_event->GetSender() == this->m_LutSlider) // from this view gui
	{
		OnEventThis(maf_event); 
	}
	else if (maf_event->GetId() == ID_TRANSFORM)
	{	
		if (maf_event->GetSender() == m_GizmoZView->m_GizmoCrossRotate || maf_event->GetSender() == m_GizmoZView->m_GizmoCrossTranslate)
		{	
			OnEventGizmoCrossRTZNormalView(maf_event);
		}		
		else if (maf_event->GetSender() == m_GizmoYView->m_GizmoCrossRotate || maf_event->GetSender() == m_GizmoYView->m_GizmoCrossTranslate)
		{
			OnEventGizmoCrossRTYNormalView(maf_event);
		}
		else if (maf_event->GetSender() == m_GizmoXView->m_GizmoCrossRotate || maf_event->GetSender() == m_GizmoXView->m_GizmoCrossTranslate)
		{
			OnEventGizmoCrossRTXNormalView(maf_event);
		}
		else
		{
			// if no one can handle this event send it to the operation listener

			mafEventMacro(*maf_event); 
		}	
	}
	else if (maf_event->GetId() == VME_PICKED)
	{
		// wxMessageBox("Hello!");

		mafEvent *event = mafEvent::SafeDownCast(maf_event);
		assert(event);

		double pickedPointCoordinates[3];  
		vtkPoints *pickedPoint = NULL; 
		pickedPoint = (vtkPoints *)event->GetVtkObj();
		pickedPoint->GetPoint(0,pickedPointCoordinates);

		using namespace::std;

		string pickedSliceName;
		if (event->GetSender() == m_XSlicerPicker)
		{
			pickedSliceName = m_XSlicerPicker->GetName();

			mafInteractorCameraPicker *picker = m_XSlicerPicker;
			GenerateMoveEvent(picker, pickedPointCoordinates);

		}
		else if (event->GetSender() == m_YSlicerPicker)
		{
			pickedSliceName = m_YSlicerPicker->GetName();

			mafInteractorCameraPicker *picker = m_YSlicerPicker;
			GenerateMoveEvent(picker, pickedPointCoordinates);

		}
		else if (event->GetSender() == m_ZSlicerPicker)
		{
			pickedSliceName = m_ZSlicerPicker->GetName();

			mafInteractorCameraPicker *picker = m_ZSlicerPicker;
			GenerateMoveEvent(picker, pickedPointCoordinates);
		}

		UpdateSlicersLUT();

		// DEBUG
		std::ostringstream stringStream;
		stringStream << "picked: " << pickedSliceName << std::endl;
		stringStream << "on position: " << pickedPointCoordinates[0] << " " << pickedPointCoordinates[1] << " " << pickedPointCoordinates[2] << std::endl;
		mafLogMessage(stringStream.str().c_str());

	}
	else
	{
		Superclass::OnEvent(maf_event);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossTranslateZNormalView(mafEventBase *maf_event)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			vtkCamera *zViewCamera = ((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera();

			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoYView);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoXView);

			// post multiplying matrices coming from the gizmo to the slicers
			PostMultiplyEventMatrixToSlicers(maf_event);

			mafEvent *e = mafEvent::SafeDownCast(maf_event);

			//compute the incremental translation
			vtkTransform *tr;
			vtkNEW(tr);
			tr->PostMultiply();
			tr->SetMatrix(e->GetMatrix()->GetVTKMatrix());
			tr->Update();
			double translation[3];
			tr->GetPosition(translation);

			//increase the translation
			m_VolumeVTKDataCenterABSCoords[0]+=translation[0];
			m_VolumeVTKDataCenterABSCoords[1]+=translation[1];
			m_VolumeVTKDataCenterABSCoords[2]+=translation[2];


			//for each surface visualized change the center of the cut plane
			assert(m_CurrentVolume);
			mafVME *root=m_CurrentVolume->GetRoot();
			mafVMEIterator *iter = root->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud")|| node->IsA("mafVMERefSys"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated,NULL);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeArbitraryViewMesh && PipeSliceViewMesh)
					{
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,NULL);
					}
				}
			}
			iter->Delete();
			if(m_CurrentPolylineGraphEditor)
			{
				//a surface is visible only if there is a volume in the view
				if(m_CurrentVolume)
				{

					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}

			ChildViewsCameraUpdate();
			vtkDEL(tr);

		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossRotateZNormalView(mafEventBase *maf_event)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoXView);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoYView);

			PostMultiplyEventMatrixToSlicer(maf_event, X);
			PostMultiplyEventMatrixToSlicer(maf_event, Y);

			// roll the camera based on gizmo

			mafEvent *event = mafEvent::SafeDownCast(maf_event);
			assert(event);

			mafString activeGizmoAxis = *(event->GetString());

			vtkMatrix4x4 *mat = event->GetMatrix()->GetVTKMatrix();

			vtkMAFSmartPointer<vtkTransform> tr;
			tr->SetMatrix(mat);

			m_ChildViewList[X_VIEW]->GetRWI()->GetCamera()->ApplyTransform(tr);
			m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera()->ApplyTransform(tr);

			GetLogicManager()->CameraUpdate();

			//update the normal of the cutter plane of the surface
			mafVME *root=m_CurrentVolume->GetRoot();
			mafVMEIterator *iter = root->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated, normal);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewMesh)
					{
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,normal);
					}
				}
			}
			iter->Delete();
			if(m_CurrentPolylineGraphEditor)
			{
				//a surface is visible only if there is a volume in the view
				if(m_CurrentVolume)
				{

					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}

			//      UpdateCutPlanes();
			ChildViewsCameraUpdate();

		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossRotateYNormalView(mafEventBase *maf_event)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoZView);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoXView);

			PostMultiplyEventMatrixToSlicer(maf_event, X);
			PostMultiplyEventMatrixToSlicer(maf_event, Z);

			mafEvent *event = mafEvent::SafeDownCast(maf_event);
			assert(event);

			vtkMatrix4x4 *mat = event->GetMatrix()->GetVTKMatrix();

			vtkMAFSmartPointer<vtkTransform> tr;
			tr->SetMatrix(mat);

			m_ChildViewList[Z_VIEW]->GetRWI()->GetCamera()->ApplyTransform(tr);
			m_ChildViewList[X_VIEW]->GetRWI()->GetCamera()->ApplyTransform(tr);


			//update the normal of the cutter plane of the surface
			mafVME *root=m_CurrentVolume->GetRoot();
			mafVMEIterator *iter = root->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated, normal);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewMesh)
					{
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,normal);
					}
				}
			}
			iter->Delete();
			if(m_CurrentPolylineGraphEditor)
			{
				//a surface is visible only if there is a volume in the view
				if(m_CurrentVolume)
				{

					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}
			
			ChildViewsCameraUpdate();

			GetLogicManager()->CameraUpdate();
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossTranslateYNormalView(mafEventBase *maf_event)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    

			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoZView);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoXView);

			// post multiplying matrices coming from the gizmo to the slicers
			PostMultiplyEventMatrixToSlicers(maf_event);

			mafEvent *e = mafEvent::SafeDownCast(maf_event);

			//compute the incremental translation
			vtkTransform *tr;
			vtkNEW(tr);
			tr->PostMultiply();
			tr->SetMatrix(e->GetMatrix()->GetVTKMatrix());
			tr->Update();
			double translation[3];
			tr->GetPosition(translation);

			//increase the translation
			m_VolumeVTKDataCenterABSCoords[0]+=translation[0];
			m_VolumeVTKDataCenterABSCoords[1]+=translation[1];
			m_VolumeVTKDataCenterABSCoords[2]+=translation[2];


			//for each surface visualized change the center of the cut plane
			assert(m_CurrentVolume);
			mafVME *root=m_CurrentVolume->GetRoot();
			mafVMEIterator *iter = root->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated,NULL);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeArbitraryViewMesh && PipeSliceViewMesh)
					{
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,NULL);
					}
				}
			}
			iter->Delete();
			if(m_CurrentPolylineGraphEditor)
			{
				//a surface is visible only if there is a volume in the view
				if(m_CurrentVolume)
				{

					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}


			ChildViewsCameraUpdate();
			vtkDEL(tr);

		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossRotateXNormalView(mafEventBase *maf_event)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoYView);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoZView);

			PostMultiplyEventMatrixToSlicer(maf_event, Y);
			PostMultiplyEventMatrixToSlicer(maf_event, Z);

			// roll the camera based on gizmo
			mafEvent *event = mafEvent::SafeDownCast(maf_event);
			assert(event);

			mafString activeGizmoAxis = *(event->GetString());

			vtkMatrix4x4 *mat = event->GetMatrix()->GetVTKMatrix();

			vtkMAFSmartPointer<vtkTransform> tr;
			tr->SetMatrix(mat);

			m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera()->ApplyTransform(tr);
			m_ChildViewList[Z_VIEW]->GetRWI()->GetCamera()->ApplyTransform(tr);

			GetLogicManager()->CameraUpdate();

			//update the normal of the cutter plane of the surface
			mafVME *root=m_CurrentVolume->GetRoot();
			mafVMEIterator *iter = root->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated, normal);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewMesh)
					{
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,normal);
					}
				}
			}
			iter->Delete();
			if(m_CurrentPolylineGraphEditor)
			{
				//a surface is visible only if there is a volume in the view
				if(m_CurrentVolume)
				{

					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}

			//      UpdateCutPlanes();
			ChildViewsCameraUpdate();
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossTranslateXNormalView(mafEventBase *maf_event)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{   

			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoYView);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoZView);

			// post multiplying matrices coming from the gizmo to the slicers
			PostMultiplyEventMatrixToSlicers(maf_event);

			mafEvent *e = mafEvent::SafeDownCast(maf_event);

			//compute the incremental translation
			vtkTransform *tr;
			vtkNEW(tr);
			tr->PostMultiply();
			tr->SetMatrix(e->GetMatrix()->GetVTKMatrix());
			tr->Update();
			double translation[3];
			tr->GetPosition(translation);

			//increase the translation
			m_VolumeVTKDataCenterABSCoords[0]+=translation[0];
			m_VolumeVTKDataCenterABSCoords[1]+=translation[1];
			m_VolumeVTKDataCenterABSCoords[2]+=translation[2];

			//for each surface visualized change the center of the cut plane
			assert(m_CurrentVolume);
			mafVME *root=m_CurrentVolume->GetRoot();
			mafVMEIterator *iter = root->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated,NULL);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
					if(PipeArbitraryViewMesh && PipeSliceViewMesh)
					{
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,NULL);
					}
				}
			}
			iter->Delete();
			if(m_CurrentPolylineGraphEditor)
			{
				//a surface is visible only if there is a volume in the view
				if(m_CurrentVolume)
				{

					double normal[3];
					((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}

			//      UpdateCutPlanes();
			ChildViewsCameraUpdate();
			vtkDEL(tr);

		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventThis(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
		case VME_PICKED:
			{
				wxMessageBox("pick!");		
			}
			break;

		case ID_RANGE_MODIFIED:
			{
				OnLUTRangeModified();
			}
			break;
		case ID_LUT_CHOOSER:
			{
				OnLUTChooser();

			}
			break;

		case ID_RESET:
			{
				OnReset();
			}
			break;

		case ID_UPDATE_LUT:
			{
				UpdateSlicersLUT();
			}
			break;

		case  ID_SHOW_RULER:
			{

				ShowRuler(X_RULER, true);
				ShowRuler(Y_RULER, true);
				ShowRuler(Z_RULER, true);
			}
			break;

		case  ID_HIDE_RULER:
			{
				ShowRuler(X_RULER, false);
				ShowRuler(Y_RULER, false);
				ShowRuler(Z_RULER, false);
			}
			break;


		default:
			mafViewCompound::OnEvent(maf_event);
		}
	}
}
//----------------------------------------------------------------------------
mafView *mafViewArbitraryOrthoSlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;
	mafViewArbitraryOrthoSlice *v = new mafViewArbitraryOrthoSlice(m_Label);
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
mafGUI* mafViewArbitraryOrthoSlice::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = mafView::CreateGui();

	//	m_Gui->Divider(2);

	//button to reset at the sta
	m_Gui->Label("");
	m_Gui->Label(_("CTRL + MOUSE LEFT click"),true);
	m_Gui->Label("moves the cross on picked point");
	m_Gui->Divider(2);

	//m_Gui->Label("reset slices", true);
	m_Gui->Button(ID_RESET,_("reset slices"),"");


	m_Gui->Divider();

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	
	m_Gui->Update();


	// 	m_Gui->Label("");
	// 	m_Gui->Button(ID_SHOW_RULER, "Show Ruler");
	// 	m_Gui->Button(ID_HIDE_RULER, "Hide Ruler");
	// 	m_Gui->Label("");


	// vme lut update test
	// m_Gui->Button(ID_UPDATE_LUT, "update lut");

	EnableWidgets( (m_CurrentVolume != NULL) );
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::VmeRemove(mafVME *vme)
{
	if (m_CurrentVolume && vme == m_CurrentVolume) 
	{
		m_AttachCameraToSlicerZInZView->SetVme(NULL);
		m_CurrentVolume = NULL;

		m_GizmoZView->Show(false);
		cppDEL(m_GizmoZView);

		m_GizmoYView->Show(false);
		cppDEL(m_GizmoYView);

		m_GizmoXView->Show(false);
		cppDEL(m_GizmoXView);
	}

	if (m_CurrentImage && vme == m_CurrentImage){
		m_CurrentImage = NULL;
	}

	Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicers(mafEventBase *maf_event)
{  
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		// for every slicer:

		const int numSlicers = 3;
		mafVMESlicer *slicers[numSlicers] = {m_SlicerX, m_SlicerY, m_SlicerZ};

		long arg = e->GetArg();

		for (int i = 0; i < numSlicers; i++)
		{
			// handle incoming transform events
			// print matrix
			vtkTransform *tr = vtkTransform::New();


			tr->PostMultiply();
			tr->SetMatrix(slicers[i]->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
			tr->Update();

			mafMatrix absPose;
			absPose.DeepCopy(tr->GetMatrix());
			absPose.SetTimeStamp(slicers[i]->GetTimeStamp());

			if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
			{
				// move vme
				slicers[i]->SetAbsMatrix(absPose);
			} 

			// clean up
			tr->Delete();
		}

		UpdateSlicersLUT();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicer(mafEventBase *maf_event, int slicerAxis)
{  
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		// for every slicer:

		mafVMESlicer *currentSlicer = NULL;

		if (slicerAxis == X)
		{
			currentSlicer = m_SlicerX;
		}
		else if (slicerAxis == Y)
		{
			currentSlicer = m_SlicerY;
		}
		else if (slicerAxis == Z)
		{
			currentSlicer = m_SlicerZ;
		}

		assert(currentSlicer != NULL);

		// handle incoming transform event...
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(currentSlicer->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		mafMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(currentSlicer->GetTimeStamp());

		if (e->GetArg() == mafInteractorGenericMouse::MOUSE_MOVE)
		{
			// ... and update the slicer with the new abs pose
			currentSlicer->SetAbsMatrix(absPose);
		} 

		UpdateSlicersLUT();

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateSubviewsCamerasToFaceSlices()
{
	const int numCameras = 3;
	mafAttachCamera *attachCameras[numCameras] = {m_AttachCameraToSlicerXInXView, m_AttachCameraToSlicerYInYView, m_AttachCameraToSlicerZInZView};

	for (int i = 0; i < numCameras ; i++)
	{
		if (attachCameras[i] != NULL)
		{
			attachCameras[i]->UpdateCameraMatrix();
		}
	}

	for(int i=0; i<m_NumOfChildView; i++)
	{
		m_ChildViewList[i]->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::CreateGuiView()
{
	m_GuiView = new mafGUI(this);

	//m_GuiView->Label("");
	m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500,24);
	m_LutSlider->SetMinSize(wxSize(500,24));
	EnableWidgets( (m_CurrentVolume != NULL) || (m_CurrentImage!=NULL) );
	m_GuiView->Add(m_LutSlider);
	m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::EnableWidgets(bool enable)
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
void mafViewArbitraryOrthoSlice::UpdateSlicerZBehavior()
{	
	if(m_CurrentVolume->GetBehavior())
	{
		m_SlicerX->SetBehavior(m_CurrentVolume->GetBehavior());
		m_SlicerY->SetBehavior(m_CurrentVolume->GetBehavior());
		m_SlicerZ->SetBehavior(m_CurrentVolume->GetBehavior());

		// perspective view
		mafPipeSurfaceTextured *pipePerspectiveViewSlicerX=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerX);
		pipePerspectiveViewSlicerX->SetActorPicking(true);

		mafPipeSurfaceTextured *pipePerspectiveViewSlicerY=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerY);
		pipePerspectiveViewSlicerY->SetActorPicking(true);

		mafPipeSurfaceTextured *pipePerspectiveViewSlicerZ=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerZ);
		pipePerspectiveViewSlicerZ->SetActorPicking(true);

		EnableSlicersPicking(true);
	}
	else
	{
		m_SlicerZ->SetBehavior(NULL);
		m_SlicerZ->SetBehavior(NULL);
		m_SlicerZ->SetBehavior(NULL);

		// perspective view
		mafPipeSurfaceTextured *pipePerspectiveViewSlicerX=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerX);
		pipePerspectiveViewSlicerX->SetActorPicking(false);

		mafPipeSurfaceTextured *pipePerspectiveViewSlicerY=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerY);
		pipePerspectiveViewSlicerY->SetActorPicking(false);

		mafPipeSurfaceTextured *pipePerspectiveViewSlicerZ=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerZ);
		pipePerspectiveViewSlicerZ->SetActorPicking(false);

		// x view
		mafPipeSurfaceTextured *pipeXViewSlicerX=(mafPipeSurfaceTextured *)(m_ChildViewList[X_VIEW])->GetNodePipe(m_SlicerX);
		pipeXViewSlicerX->SetActorPicking(false);

		// y view
		mafPipeSurfaceTextured *pipeYViewSlicerY=(mafPipeSurfaceTextured *)(m_ChildViewList[Y_VIEW])->GetNodePipe(m_SlicerY);
		pipeYViewSlicerY->SetActorPicking(false);

		// z view
		mafPipeSurfaceTextured *pipeZViewSlicerZ=(mafPipeSurfaceTextured *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_SlicerZ);
		pipeZViewSlicerZ->SetActorPicking(false);
	}
};
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::VolumeWindowing(mafVME *volume)
{
	double sr[2];
	vtkDataSet *data = volume->GetOutput()->GetVTKData();
	data->Update();
	data->GetScalarRange(sr);

	mmaMaterial *currentSurfaceMaterial = m_SlicerZ->GetMaterial();
	m_ColorLUT = currentSurfaceMaterial->m_ColorLut;
	assert(m_ColorLUT);
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);

	m_SlicerY->GetMaterial()->m_ColorLut->SetRange((long)sr[0],(long)sr[1]);
	m_SlicerZ->GetMaterial()->m_ColorLut->SetRange((long)sr[0],(long)sr[1]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowMafVMEVolume( mafVME * vme, bool show )
{
	wxBusyInfo wait("please wait");

	mafViewVTK *xView = ((mafViewVTK *)(m_ChildViewList[X_VIEW])) ;
	assert(xView);

	double pickerTolerance = 0.03;

	// fuzzy picking
	xView->GetPicker2D()->SetTolerance(pickerTolerance);
	mafViewVTK *yView = ((mafViewVTK *)(m_ChildViewList[Y_VIEW])) ;
	assert(yView);

	// fuzzy picking
	yView->GetPicker2D()->SetTolerance(pickerTolerance);

	mafViewVTK *zView = ((mafViewVTK *)(m_ChildViewList[Z_VIEW])) ;
	assert(zView);

	// fuzzy picking
	zView->GetPicker2D()->SetTolerance(pickerTolerance);
		
	double sr[2],volumeVTKDataCenterLocalCoords[3];
	m_CurrentVolume = vme;

	EnableWidgets(true);

	vtkDataSet *volumeVTKData = vme->GetOutput()->GetVTKData();
	volumeVTKData->Update();

	volumeVTKData->GetCenter(volumeVTKDataCenterLocalCoords);

	volumeVTKData->GetScalarRange(sr);
	volumeVTKData=NULL;

	mafTransform::GetOrientation(vme->GetAbsMatrixPipe()->GetMatrix(),m_VolumeVTKDataABSOrientation);

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

	mafNEW(m_SlicerZResetMatrix);
	m_SlicerZResetMatrix->Identity();
	m_SlicerZResetMatrix->SetVTKMatrix(transformReset->GetMatrix());

	ShowSlicers(vme, show);

	vtkDEL(points);
	vtkDEL(sliceCenterLocalCoordsPolydata);
	vtkDEL(sliceCenterLocalCoordsToABSCoordsTransform);
	vtkDEL(localToABSTPDF);
	vtkDEL(transformReset);

	VolumeWindowing(vme);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowmafVMEPolylineEditor(mafVME *vme)
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{
		m_CurrentPolylineGraphEditor = (mafVMEPolylineEditor*)vme;

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(vme));
		PipeSliceViewPolylineEditor->SetModalitySlice();

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);        

	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowMafVMEMesh(mafVME *vme)
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(vme));
		mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(vme));

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,normal);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowMafVMEImage( mafVME * vme )
{
	m_CurrentImage = vme;
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::HideMafVMEVolume()
{

	EnableWidgets(false);

	m_XCameraConeVME->ReparentTo(NULL);
	mafDEL(m_XCameraConeVME);

	m_AttachCameraToSlicerXInXView->SetVme(NULL);
	m_SlicerX->SetBehavior(NULL);
	m_SlicerX->ReparentTo(NULL);
	mafDEL(m_SlicerX);

	m_YCameraConeVME->ReparentTo(NULL);
	mafDEL(m_YCameraConeVME);

	m_AttachCameraToSlicerYInYView->SetVme(NULL);
	m_SlicerY->SetBehavior(NULL);
	m_SlicerY->ReparentTo(NULL);
	mafDEL(m_SlicerY);

	m_ZCameraConeVME->ReparentTo(NULL);
	mafDEL(m_ZCameraConeVME);

	m_AttachCameraToSlicerZInZView->SetVme(NULL);
	m_SlicerZ->SetBehavior(NULL);
	m_SlicerZ->ReparentTo(NULL);
	mafDEL(m_SlicerZ);

	//remove gizmos
	m_Gui->Remove(m_GuiGizmos);
	m_Gui->Update();

	m_GizmoXView->Show(false);
	m_GizmoYView->Show(false);
	m_GizmoZView->Show(false);


	cppDEL(m_GuiGizmos);

	mafDEL(m_SlicerXResetMatrix);
	mafDEL(m_SlicerYResetMatrix);
	mafDEL(m_SlicerZResetMatrix);

	m_CurrentVolume = NULL;
	m_ColorLUT = NULL;
	m_LutWidget->SetLut(m_ColorLUT);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::HideMafVmeImage()
{
	m_CurrentImage = NULL;
	m_ColorLUT = NULL;
	m_LutWidget->SetLut(m_ColorLUT);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowVMESurfacesAndLandmarks(mafVME *vme)
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(vme));
		mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(vme));

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewSurface->SetSlice(surfaceOriginTranslated,normal);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnReset()
{
	m_GizmoZView->SetAbsPose(m_SlicerZResetMatrix);
	m_GizmoYView->SetAbsPose(m_SlicerYResetMatrix);
	m_GizmoXView->SetAbsPose(m_SlicerXResetMatrix);

	m_SlicerX->SetAbsMatrix(*m_SlicerXResetMatrix);
	m_SlicerY->SetAbsMatrix(*m_SlicerYResetMatrix);
	m_SlicerZ->SetAbsMatrix(*m_SlicerZResetMatrix);

	RestoreCameraParametersForAllSubviews();
	UpdateSlicersLUT();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnLUTRangeModified()
{
	mafVME *vme = GetSceneGraph()->GetSelectedVme();

	if( (m_CurrentVolume || m_CurrentImage) && vme)
	{
		UpdateSlicersLUT();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnLUTChooser()
{
	double *sr;

	if(m_CurrentVolume || m_CurrentImage) {
		sr = m_ColorLUT->GetRange();
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);

		m_SlicerX->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_SlicerX->GetMaterial()->m_ColorLut->Modified();

		m_SlicerY->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_SlicerY->GetMaterial()->m_ColorLut->Modified();

		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnResetSurfaceAndLandmark(mafVME *vme)
{
	mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(vme));
	mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(vme));
	if(PipeArbitraryViewSurface)
	{
		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		double surfaceOriginTranslated[3];
		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoordinatesReset[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoordinatesReset[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoordinatesReset[2] + normal[2] * 0.1;

		PipeSliceViewSurface->SetSlice(surfaceOriginTranslated, normal);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnResetMafVMEMesh(mafVME *vme)
{
	mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(vme));
	mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(vme));
	if(PipeSliceViewMesh && PipeArbitraryViewMesh)
	{
		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		double surfaceOriginTranslated[3];
		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoordinatesReset[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoordinatesReset[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoordinatesReset[2] + normal[2] * 0.1;

		PipeSliceViewMesh->SetSlice(surfaceOriginTranslated,normal);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnResetmafVMEPolylineEditor()
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafVME*)m_CurrentPolylineGraphEditor));
		PipeSliceViewPolylineEditor->SetModalitySlice();

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);            
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowSlicers( mafVME * vmeVolume, bool show )
{
	EnableWidgets( (m_CurrentVolume != NULL) );
	// register sliced volume
	m_InputVolume = mafVMEVolumeGray::SafeDownCast(vmeVolume);
	assert(m_InputVolume);

	vtkMAFSmartPointer<vtkTransform> slicerXTransform;
	slicerXTransform->SetMatrix(m_SlicerZResetMatrix->GetVTKMatrix());
	slicerXTransform->RotateY(89.999);
	slicerXTransform->Update();

	mafNEW(m_SlicerXResetMatrix);
	m_SlicerXResetMatrix->Identity();
	m_SlicerXResetMatrix->DeepCopy(slicerXTransform->GetMatrix());

	mafNEW(m_SlicerX);
	m_SlicerX->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_SlicerX->SetName("m_SlicerX");
	m_SlicerX->ReparentTo(vmeVolume);
	m_SlicerX->SetAbsMatrix(mafMatrix(slicerXTransform->GetMatrix()));
	m_SlicerX->SetSlicedVMELink(vmeVolume);
	m_SlicerX->GetMaterial()->m_ColorLut->DeepCopy(mafVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
	m_SlicerX->Update();

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_SlicerX, show);
	m_ChildViewList[X_VIEW]->VmeShow(m_SlicerX, show);

	BuildXCameraConeVME();

	vtkMAFSmartPointer<vtkTransform> slicerYTransform;
	slicerYTransform->SetMatrix(m_SlicerZResetMatrix->GetVTKMatrix());
	slicerYTransform->RotateX(90);
	slicerYTransform->Update();

	mafNEW(m_SlicerYResetMatrix);
	m_SlicerYResetMatrix->Identity();
	m_SlicerYResetMatrix->DeepCopy(slicerYTransform->GetMatrix());

	mafNEW(m_SlicerY);
	m_SlicerY->SetName("m_SlicerY");
	m_SlicerY->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_SlicerY->ReparentTo(vmeVolume);
	m_SlicerY->SetAbsMatrix(mafMatrix(slicerYTransform->GetMatrix()));
	m_SlicerY->SetSlicedVMELink(vmeVolume);
	m_SlicerY->GetMaterial()->m_ColorLut->DeepCopy(mafVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
	m_SlicerY->Update();

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_SlicerY, show);
	m_ChildViewList[Y_VIEW]->VmeShow(m_SlicerY, show);

	BuildYCameraConeVME();

	mafNEW(m_SlicerZ);
	m_SlicerZ->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_SlicerZ->SetName("m_SlicerZ");
	m_SlicerZ->ReparentTo(vmeVolume);
	m_SlicerZ->SetAbsMatrix(*m_SlicerZResetMatrix);
	m_SlicerZ->SetSlicedVMELink(vmeVolume);
	m_SlicerZ->GetMaterial()->m_ColorLut->DeepCopy(mafVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
	m_SlicerZ->Update();

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_SlicerZ, show);
	m_ChildViewList[Z_VIEW]->VmeShow(m_SlicerZ, show);

	BuildZCameraConeVME();

	mafPipeSurfaceTextured *pipePerspectiveViewZ=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerZ);
	pipePerspectiveViewZ->SetActorPicking(false);
	pipePerspectiveViewZ->SetEnableActorLOD(0);

	mafPipeSurfaceTextured *pipeZView=(mafPipeSurfaceTextured *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_SlicerZ);
	pipeZView->SetActorPicking(false);
	pipeZView->SetEnableActorLOD(0);

	UpdateSlicerZBehavior();

	const int numViews = 3;

	int sliceViewsId[numViews] =
	{
		X_VIEW,
			Y_VIEW,
			Z_VIEW
	};

	if(m_AttachCameraToSlicerXInXView == NULL &&
		m_AttachCameraToSlicerYInYView == NULL &&
		m_AttachCameraToSlicerZInZView == NULL)
	{
		m_AttachCameraToSlicerXInXView=new mafAttachCamera(m_Gui,((mafViewVTK*)m_ChildViewList[X_VIEW])->m_Rwi,this);
		m_AttachCameraToSlicerYInYView=new mafAttachCamera(m_Gui,((mafViewVTK*)m_ChildViewList[Y_VIEW])->m_Rwi,this);
		m_AttachCameraToSlicerZInZView=new mafAttachCamera(m_Gui,((mafViewVTK*)m_ChildViewList[Z_VIEW])->m_Rwi,this);
	}

	assert(m_AttachCameraToSlicerXInXView);
	assert(m_AttachCameraToSlicerYInYView);
	assert(m_AttachCameraToSlicerZInZView);

	const int numCameras = 3;

	mafAttachCamera *attachCameras[numCameras] = 
	{
		m_AttachCameraToSlicerXInXView, 
			m_AttachCameraToSlicerYInYView, 
			m_AttachCameraToSlicerZInZView
	};

	const int numSlicers = 3;
	mafVMESlicer *slicers[numSlicers] = {m_SlicerX, m_SlicerY, m_SlicerZ};

	m_AttachCameraToSlicerXInXView->SetStartingMatrix(m_SlicerX->GetOutput()->GetAbsMatrix());
	m_AttachCameraToSlicerXInXView->SetVme(m_SlicerZ);
	m_AttachCameraToSlicerXInXView->EnableAttachCamera();

	m_AttachCameraToSlicerYInYView->SetStartingMatrix(m_SlicerY->GetOutput()->GetAbsMatrix());
	m_AttachCameraToSlicerYInYView->SetVme(m_SlicerZ);
	m_AttachCameraToSlicerYInYView->EnableAttachCamera();

	m_AttachCameraToSlicerZInZView->SetStartingMatrix(m_SlicerZ->GetOutput()->GetAbsMatrix());
	m_AttachCameraToSlicerZInZView->SetVme(m_SlicerZ);
	m_AttachCameraToSlicerZInZView->EnableAttachCamera();

	ResetCameraToSlices();

	m_GizmoZView = new mafGizmoCrossRotateTranslate();
	m_GizmoZView->Create(m_SlicerZ, this, true, mafGizmoCrossRotateTranslate::Z);
	m_GizmoZView->SetName("m_GizmoZView");
	m_GizmoZView->SetInput(m_SlicerZ);
	m_GizmoZView->SetRefSys(m_SlicerZ);
	m_GizmoZView->SetAbsPose(m_SlicerZResetMatrix);

	m_GizmoZView->SetColor(mafGizmoCrossRotateTranslate::GREW, mafGizmoCrossRotateTranslate::GREEN);
	m_GizmoZView->SetColor(mafGizmoCrossRotateTranslate::GTAEW, mafGizmoCrossRotateTranslate::GREEN);
	m_GizmoZView->SetColor(mafGizmoCrossRotateTranslate::GTPEW, mafGizmoCrossRotateTranslate::GREEN);
	m_GizmoZView->SetColor(mafGizmoCrossRotateTranslate::GRNS, mafGizmoCrossRotateTranslate::RED);
	m_GizmoZView->SetColor(mafGizmoCrossRotateTranslate::GTANS, mafGizmoCrossRotateTranslate::RED);
	m_GizmoZView->SetColor(mafGizmoCrossRotateTranslate::GTPNS, mafGizmoCrossRotateTranslate::RED);

	m_GizmoZView->Show(true);

	m_GizmoYView = new mafGizmoCrossRotateTranslate();
	m_GizmoYView->Create(m_SlicerY, this, true, mafGizmoCrossRotateTranslate::Y);
	m_GizmoYView->SetName("m_GizmoYView");
	m_GizmoYView->SetInput(m_SlicerY);
	m_GizmoYView->SetRefSys(m_SlicerY);
	m_GizmoYView->SetAbsPose(m_SlicerYResetMatrix);

	m_GizmoYView->SetColor(mafGizmoCrossRotateTranslate::GREW, mafGizmoCrossRotateTranslate::BLUE);
	m_GizmoYView->SetColor(mafGizmoCrossRotateTranslate::GTAEW, mafGizmoCrossRotateTranslate::BLUE);
	m_GizmoYView->SetColor(mafGizmoCrossRotateTranslate::GTPEW, mafGizmoCrossRotateTranslate::BLUE);
	m_GizmoYView->SetColor(mafGizmoCrossRotateTranslate::GRNS, mafGizmoCrossRotateTranslate::RED);
	m_GizmoYView->SetColor(mafGizmoCrossRotateTranslate::GTANS, mafGizmoCrossRotateTranslate::RED);
	m_GizmoYView->SetColor(mafGizmoCrossRotateTranslate::GTPNS, mafGizmoCrossRotateTranslate::RED);

	m_GizmoYView->Show(true);

	m_GizmoXView = new mafGizmoCrossRotateTranslate();
	m_GizmoXView->Create(m_SlicerX, this, true, mafGizmoCrossRotateTranslate::X);
	m_GizmoXView->SetName("m_GizmoXView");
	m_GizmoXView->SetInput(m_SlicerX);
	m_GizmoXView->SetRefSys(m_SlicerX);
	m_GizmoXView->SetAbsPose(m_SlicerXResetMatrix);

	m_GizmoXView->SetColor(mafGizmoCrossRotateTranslate::GREW, mafGizmoCrossRotateTranslate::GREEN);
	m_GizmoXView->SetColor(mafGizmoCrossRotateTranslate::GTAEW, mafGizmoCrossRotateTranslate::GREEN);
	m_GizmoXView->SetColor(mafGizmoCrossRotateTranslate::GTPEW, mafGizmoCrossRotateTranslate::GREEN);
	m_GizmoXView->SetColor(mafGizmoCrossRotateTranslate::GRNS, mafGizmoCrossRotateTranslate::BLUE);
	m_GizmoXView->SetColor(mafGizmoCrossRotateTranslate::GTANS, mafGizmoCrossRotateTranslate::BLUE);
	m_GizmoXView->SetColor(mafGizmoCrossRotateTranslate::GTPNS, mafGizmoCrossRotateTranslate::BLUE);

	m_GizmoXView->Show(true);

	//Create the Gizmos' Gui
	if(!m_GuiGizmos)
		m_GuiGizmos = new mafGUI(this);

	// m_GuiGizmos->AddGui(m_GizmoZView->m_GizmoCrossTranslate->GetGui());
	// m_GuiGizmos->AddGui(m_GizmoZView->m_GizmoCrossRotate->GetGui());
	m_GuiGizmos->Update();
	if(m_Gui == NULL) CreateGui();
	m_Gui->AddGui(m_GuiGizmos);
	m_Gui->FitGui();
	m_Gui->Update();

	m_SlicerX->SetVisibleToTraverse(false);
	m_SlicerY->SetVisibleToTraverse(false);
	m_SlicerZ->SetVisibleToTraverse(false);

	UpdateSubviewsCamerasToFaceSlices();

	BuildSliceHeightFeedbackLinesVMEs();

	UpdateXnViewZPlanes();
	UpdateYnViewZPlanes();

	UpdateYnViewXPlanes();
	UpdateZnViewXPlanes();

	UpdateXnViewYPlanes();
	UpdateZnViewYPlanes();

	HideAllCutPlanes();  

	// out: slicer to use
	mafVMESlicer *targetSlicer;

	// out: slicer abs matrix for volume cut
	vtkMatrix4x4 *outputMatrix = vtkMatrix4x4::New();

	vtkDEL(outputMatrix);

	double red[3] = {1,0,0};
	CreateViewCameraNormalFeedbackActor(red, X_VIEW);

	double green[3] = {0,1,0};
	CreateViewCameraNormalFeedbackActor(green, Y_VIEW);

	double blue[3] = {0,0,1};
	CreateViewCameraNormalFeedbackActor(blue, Z_VIEW);

	// create the pick interactor for slicer X
	m_XSlicerPicker = mafInteractorCameraPicker::New();
	m_XSlicerPicker->SetListener(this);
	m_XSlicerPicker->SetName("m_XSlicerPicker");
	m_SlicerX->SetBehavior(m_XSlicerPicker);

	// create the pick interactor for slicer X
	m_YSlicerPicker = mafInteractorCameraPicker::New();
	m_YSlicerPicker->SetListener(this);
	m_YSlicerPicker->SetName("m_YSlicerPicker");
	m_SlicerY->SetBehavior(m_YSlicerPicker);

	// create the pick interactor for slicer X
	m_ZSlicerPicker = mafInteractorCameraPicker::New();
	m_ZSlicerPicker->SetListener(this);
	m_ZSlicerPicker->SetName("m_ZSlicerPicker");
	m_SlicerZ->SetBehavior(m_ZSlicerPicker);

	EnableSlicersPicking(true);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::BuildXCameraConeVME()
{
	double b[6] = {0,0,0,0,0,0};
	assert(m_CurrentVolume);
	m_CurrentVolume->GetOutput()->GetVMELocalBounds(b);

	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	p1[0] = b[0];
	p1[1] = b[2];
	p1[2] = b[4];
	p2[0] = b[1];
	p2[1] = b[3];
	p2[2] = b[5];

	double d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	double coneRadius = d / 10;

	vtkConeSource *XCameraConeSource = vtkConeSource::New();
	XCameraConeSource->SetCenter(0,0,b[1]/2 + coneRadius / 2);
	XCameraConeSource->SetResolution(20);
	XCameraConeSource->SetDirection(0,0,-1);

	XCameraConeSource->SetRadius(coneRadius);
	XCameraConeSource->SetHeight(coneRadius);

	XCameraConeSource->CappingOn();
	XCameraConeSource->Update();

	mafNEW(m_XCameraConeVME);
	// DEBUG
	m_XCameraConeVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_XCameraConeVME->SetName("Cone X Camera");
	m_XCameraConeVME->SetData(XCameraConeSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_XCameraConeVME->SetVisibleToTraverse(false);

	m_XCameraConeVME->GetMaterial()->m_Prop->SetColor(1,0,0);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetAmbient(1);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetDiffuse(0);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetSpecular(0);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetOpacity(0.2);

	assert(m_SlicerX);

	// default slicer matrix rotation component is identity when the input volume has identity pose matrix
	m_XCameraConeVME->ReparentTo(m_SlicerX);

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_XCameraConeVME, true);

	mafPipeSurface *pipeY=(mafPipeSurface *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_XCameraConeVME);
	pipeY->SetActorPicking(false);

	XCameraConeSource->Delete();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::BuildYCameraConeVME()
{
	double b[6] = {0,0,0,0,0,0};
	assert(m_CurrentVolume);
	m_CurrentVolume->GetOutput()->GetVMELocalBounds(b);

	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	p1[0] = b[0];
	p1[1] = b[2];
	p1[2] = b[4];
	p2[0] = b[1];
	p2[1] = b[3];
	p2[2] = b[5];

	double d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	double coneRadius = d / 10;

	vtkConeSource *YCameraConeSource = vtkConeSource::New();
	YCameraConeSource->SetCenter(0,0,b[3]/2 + coneRadius / 2);
	YCameraConeSource->SetResolution(20);
	YCameraConeSource->SetDirection(0,0,-1);

	YCameraConeSource->SetRadius(coneRadius);
	YCameraConeSource->SetHeight(coneRadius);

	YCameraConeSource->CappingOn();
	YCameraConeSource->Update();

	mafNEW(m_YCameraConeVME);
	// DEBUG
	m_YCameraConeVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_YCameraConeVME->SetName("m_YCameraConeVME");
	m_YCameraConeVME->SetData(YCameraConeSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_YCameraConeVME->SetVisibleToTraverse(false);

	m_YCameraConeVME->GetMaterial()->m_Prop->SetColor(0,1,0);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetAmbient(1);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetDiffuse(0);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetSpecular(0);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetOpacity(0.2);

	assert(m_SlicerY);

	/* default y slicer matrix

	1  0  0  ...
	0  0 -1  ... => RotX(90) from identity
	0  1  0  ...
	........  1

	*/

	m_YCameraConeVME->ReparentTo(m_SlicerY);

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_YCameraConeVME, true);

	mafPipeSurface *pipeX=(mafPipeSurface *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_YCameraConeVME);
	pipeX->SetActorPicking(false);

	YCameraConeSource->Delete();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::BuildZCameraConeVME()
{
	double b[6] = {0,0,0,0,0,0};
	assert(m_CurrentVolume);
	m_CurrentVolume->GetOutput()->GetVMELocalBounds(b);

	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	p1[0] = b[0];
	p1[1] = b[2];
	p1[2] = b[4];
	p2[0] = b[1];
	p2[1] = b[3];
	p2[2] = b[5];

	double d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	double coneRadius = d/10;

	vtkConeSource *ZCameraConeSource = vtkConeSource::New();
	ZCameraConeSource->SetCenter(0,0,b[5]/2 + coneRadius / 2);
	ZCameraConeSource->SetResolution(20);
	ZCameraConeSource->SetDirection(0,0,-1);

	ZCameraConeSource->SetRadius(coneRadius);
	ZCameraConeSource->SetHeight(coneRadius);

	ZCameraConeSource->CappingOn();
	ZCameraConeSource->Update();

	mafNEW(m_ZCameraConeVME);
	// DEBUG
	m_ZCameraConeVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_ZCameraConeVME->SetName("m_ZCameraConeVME");
	m_ZCameraConeVME->SetData(ZCameraConeSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_ZCameraConeVME->SetVisibleToTraverse(false);

	m_ZCameraConeVME->GetMaterial()->m_Prop->SetColor(0,0,1);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetAmbient(1);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetDiffuse(0);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetSpecular(0);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetOpacity(0.2);

	assert(m_SlicerY);
	m_ZCameraConeVME->ReparentTo(m_SlicerZ);

	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_ZCameraConeVME, true);

	mafPipeSurface *pipeX=(mafPipeSurface *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_ZCameraConeVME);
	pipeX->SetActorPicking(false);

	ZCameraConeSource->Delete();
	CameraReset();
}
//----------------------------------------------------------------------------
bool mafViewArbitraryOrthoSlice::BelongsToZNormalGizmo( mafVME * vme )
{

	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	mafGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<mafGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoZView"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	mafGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<mafGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoZView"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}
	return false;
}
//----------------------------------------------------------------------------
bool mafViewArbitraryOrthoSlice::BelongsToXNormalGizmo( mafVME * vme )
{
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	mafGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<mafGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoXView"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	mafGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<mafGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoXView"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
bool mafViewArbitraryOrthoSlice::BelongsToYNormalGizmo( mafVME * vme )
{
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	mafGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<mafGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoYView"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	mafGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<mafGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoYView"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossRTZNormalView( mafEventBase * maf_event )
{
	if (maf_event->GetSender() == m_GizmoZView->m_GizmoCrossTranslate)
	{
		OnEventGizmoCrossTranslateZNormalView(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoZView->m_GizmoCrossRotate) // from rotation gizmo
	{
		OnEventGizmoCrossRotateZNormalView(maf_event);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossRTYNormalView( mafEventBase * maf_event )
{
	if (maf_event->GetSender() == m_GizmoYView->m_GizmoCrossTranslate)
	{
		OnEventGizmoCrossTranslateYNormalView(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoYView->m_GizmoCrossRotate) // from rotation gizmo
	{
		OnEventGizmoCrossRotateYNormalView(maf_event);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoCrossRTXNormalView( mafEventBase * maf_event )
{
	if (maf_event->GetSender() == m_GizmoXView->m_GizmoCrossTranslate)
	{
		OnEventGizmoCrossTranslateXNormalView(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoXView->m_GizmoCrossRotate) // from rotation gizmo
	{
		OnEventGizmoCrossRotateXNormalView(maf_event);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PostMultiplyEventMatrixToGizmoCross( mafEventBase * inputEvent , mafGizmoCrossRotateTranslate *targetGizmo )
{
	mafEvent *e = mafEvent::SafeDownCast(inputEvent);

	vtkTransform *tr1 = vtkTransform::New();
	tr1->PostMultiply();
	tr1->SetMatrix(targetGizmo->GetAbsPose()->GetVTKMatrix());
	tr1->Concatenate(e->GetMatrix()->GetVTKMatrix());
	tr1->Update();

	mafMatrix absPose;
	absPose.DeepCopy(tr1->GetMatrix());
	absPose.SetTimeStamp(m_GizmoYView->GetAbsPose()->GetTimeStamp());

	targetGizmo->SetAbsPose(&absPose);

	vtkDEL(tr1);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ChildViewsCameraUpdate()
{
	//UpdateSlicersLUT();

	for(int i=0; i<m_NumOfChildView; i++)
	{
		m_ChildViewList[i]->CameraUpdate();
	}
}

//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::CameraUpdate()
{
	Superclass::CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ResetCameraToSlices()
{
	assert(m_ChildViewList[X_VIEW]);
	assert(m_ChildViewList[Y_VIEW]);
	assert(m_ChildViewList[Z_VIEW]);

	((mafViewVTK*)m_ChildViewList[Y_VIEW])->CameraReset(m_SlicerY);
	((mafViewVTK*)m_ChildViewList[X_VIEW])->CameraReset(m_SlicerX);
	((mafViewVTK*)m_ChildViewList[Z_VIEW])->CameraReset(m_SlicerZ);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::StoreCameraParametersForAllSubviews()
{
	((mafViewSlice*)m_ChildViewList[X_VIEW])->GetRWI()->GetCamera()->GetPosition(m_XCameraPositionForReset);
	((mafViewSlice*)m_ChildViewList[Y_VIEW])->GetRWI()->GetCamera()->GetPosition(m_YCameraPositionForReset);
	((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetPosition(m_ZCameraPositionForReset);

	((mafViewSlice*)m_ChildViewList[X_VIEW])->GetRWI()->GetCamera()->GetFocalPoint(m_XCameraFocalPointForReset);
	((mafViewSlice*)m_ChildViewList[Y_VIEW])->GetRWI()->GetCamera()->GetFocalPoint(m_YCameraFocalPointForReset);
	((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetFocalPoint(m_ZCameraFocalPointForReset);

	((mafViewSlice*)m_ChildViewList[X_VIEW])->GetRWI()->GetCamera()->GetViewUp(m_XCameraViewUpForReset);
	((mafViewSlice*)m_ChildViewList[Y_VIEW])->GetRWI()->GetCamera()->GetViewUp(m_YCameraViewUpForReset);
	((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewUp(m_ZCameraViewUpForReset);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::RestoreCameraParametersForAllSubviews()
{
	vtkCamera *xViewCamera = ((mafViewSlice*)m_ChildViewList[X_VIEW])->GetRWI()->GetCamera();
	xViewCamera->SetPosition(m_XCameraPositionForReset);
	xViewCamera->SetFocalPoint(m_XCameraFocalPointForReset);
	xViewCamera->SetViewUp(m_XCameraViewUpForReset);

	vtkCamera *yViewCamera = ((mafViewSlice*)m_ChildViewList[Y_VIEW])->GetRWI()->GetCamera();
	yViewCamera->SetPosition(m_YCameraPositionForReset);
	yViewCamera->SetFocalPoint(m_YCameraFocalPointForReset);
	yViewCamera->SetViewUp(m_YCameraViewUpForReset);

	vtkCamera *zViewCamera = ((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera();
	zViewCamera->SetPosition(m_ZCameraPositionForReset);
	zViewCamera->SetFocalPoint(m_ZCameraFocalPointForReset);
	zViewCamera->SetViewUp(m_ZCameraViewUpForReset);
}

//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::AccumulateTextures( mafVMESlicer *inSlicer, double inRXThickness , vtkImageData *outRXTexture /*= NULL */, bool showProgressBar /*= false*/ )
{	

	mafProgressBarHelper progressHelper(m_Listener);
	

	if (showProgressBar)
		progressHelper.InitProgressBar("");

	int direction = -1;

	if (inSlicer == m_SlicerX)
	{
		direction = X;
	}
	else if (inSlicer == m_SlicerY)
	{
		direction = Y;
	}
	else if (inSlicer == m_SlicerZ)
	{
		direction = Z;
	}

	assert(m_InputVolume);
	assert(m_InputVolume->IsA("mafVMEVolumeGray"));

	vtkStructuredPoints *structuredPoints = vtkStructuredPoints::SafeDownCast(m_InputVolume->GetDataPipe()->GetVTKData());

	// BEWARE: working for structured points only
	// TODO REFACTOR THIS:
	// can be easyly extended to rectilinear grid
	assert(structuredPoints);

	double spacing[3];

	structuredPoints->GetSpacing(spacing);

	double minSpacingXY = min(spacing[0], spacing[1]);
	double minSpacingYZ = min(spacing[1], spacing[2]);
	double minSpacing = min(minSpacingXY, minSpacingYZ);

	double profileDistance = minSpacing;
	int additionalProfileNumber = inRXThickness / profileDistance + 1;
	additionalProfileNumber  /= 2;

	std::ostringstream stringStream;
	stringStream << "additional profiles number: " << additionalProfileNumber << std::endl;          
	mafLogMessage(stringStream.str().c_str());

	vtkMatrix4x4 *slicerAbsMatrix = inSlicer->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();
	assert(slicerAbsMatrix);

	// copy the middle image to get original scalars
	vtkMAFSmartPointer<vtkTransform> originalABSTransform;
	originalABSTransform->SetMatrix(slicerAbsMatrix);

	mafVMEOutputSurface *outputSurface = mafVMEOutputSurface::SafeDownCast(inSlicer->GetSurfaceOutput());
	assert(outputSurface);

	vtkImageData *slicerTexture = outputSurface->GetMaterial()->GetMaterialTexture();
	assert(slicerTexture);

	inSlicer->GetSurfaceOutput()->GetVTKData()->Modified();
	inSlicer->GetSurfaceOutput()->GetVTKData()->Update();

	// sum the texture scalars in a new image: built from original default texture
	vtkMAFSmartPointer<vtkImageData> scalarsAccumulationTargetTexture;
	scalarsAccumulationTargetTexture->DeepCopy(slicerTexture);	

	// add the texture up scalars
	vtkDataArray *targetScalars = vtkUnsignedShortArray::SafeDownCast( scalarsAccumulationTargetTexture->GetPointData()->GetScalars());
	if(targetScalars == NULL) {
	    targetScalars = vtkShortArray::SafeDownCast( scalarsAccumulationTargetTexture->GetPointData()->GetScalars());
	}
	assert(targetScalars);

	assert(targetScalars->GetNumberOfComponents() == 1);

	const int numberOfTuples = targetScalars->GetNumberOfTuples();

	std::vector<double> scalarsAccumulatorVector(numberOfTuples);

	// total number of slices I'm accumulating
	int numberOfSlicesToAccumulate = additionalProfileNumber * 2 + 1;
	int slicesAlreadyProcessed = 0;


	// for each profile
	for(int profileId = -additionalProfileNumber; profileId <= additionalProfileNumber; profileId++)
	{
		if (showProgressBar)
		{
			progressHelper.UpdateProgressBar(100 * ((double )slicesAlreadyProcessed) / ((double) numberOfSlicesToAccumulate));
		}

		if (profileId == 0)
		{
			// this is the default deepcopied texture so it is taken into account already
			continue;
		}

		// build the profile probe matrix
		double currentHeight = profileId * profileDistance;
		vtkMAFSmartPointer<vtkTransform> currentSliceMatrix;
		currentSliceMatrix->PostMultiply();
		currentSliceMatrix->Concatenate(originalABSTransform);

		if (direction == X)
		{
			currentSliceMatrix->Translate(currentHeight, 0, 0);
		}
		else if (direction == Y)
		{
			currentSliceMatrix->Translate(0, currentHeight, 0);
		}
		else if (direction == Z)
		{
			currentSliceMatrix->Translate(0,0,currentHeight);
		}

		currentSliceMatrix->Update();

		mafMatrix currentProfileMafMatrix;
		currentProfileMafMatrix.SetVTKMatrix(currentSliceMatrix->GetMatrix());

		inSlicer->SetAbsMatrix(currentProfileMafMatrix);

		inSlicer->GetSurfaceOutput()->GetVTKData()->Modified();
		inSlicer->GetSurfaceOutput()->GetVTKData()->Update();

		if (false)
		{
			std::ostringstream stringStream;
			stringStream << "Slicer number: " << profileId  << " matrix" << std::endl;          
			inSlicer->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix()->PrintSelf(stringStream, NULL);
			mafLogMessage(stringStream.str().c_str());
		}
		// get the current slice profile texture

		vtkImageData *currentTexture = inSlicer->GetSurfaceOutput()->GetMaterial()->GetMaterialTexture();
		assert(currentTexture);

		vtkDataArray *currentSliceScalars = vtkUnsignedShortArray::SafeDownCast(currentTexture->GetPointData()->GetScalars());
		if(currentSliceScalars == NULL) {
	    currentSliceScalars = vtkShortArray::SafeDownCast( scalarsAccumulationTargetTexture->GetPointData()->GetScalars());
	}

		// add the scalars to the target texture
		for (int scalarId = 0; scalarId < numberOfTuples; scalarId++)
		{
			int valueToAdd = currentSliceScalars->GetTuple1(scalarId);
			scalarsAccumulatorVector[scalarId] += valueToAdd;
		}

		slicesAlreadyProcessed += 1;
		assert(slicesAlreadyProcessed <= numberOfSlicesToAccumulate);

	} // for each slice

	for (int scalarId = 0; scalarId < numberOfTuples; scalarId++)
	{
		double oldValue = scalarsAccumulatorVector[scalarId];
		int newValue = oldValue / numberOfSlicesToAccumulate;
		targetScalars->SetTuple1(scalarId, newValue);
	}


	// original texture 
	inSlicer->SetAbsMatrix(originalABSTransform->GetMatrix());

	inSlicer->GetSurfaceOutput()->GetVTKData()->Modified();
	inSlicer->GetSurfaceOutput()->GetVTKData()->Update(); 

	// test set accumulated image to slicer 
	slicerTexture->DeepCopy(scalarsAccumulationTargetTexture);

	// if the output image data is provided deepcopy the rx texture to it
	if (outRXTexture)
	{
		outRXTexture->DeepCopy(scalarsAccumulationTargetTexture);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowVTKDataAsVMESurface( vtkPolyData *vmeVTKData, mafVMESurface *vmeSurface, vtkMatrix4x4 *inputABSMatrix )
{
	assert(vmeVTKData->GetNumberOfPoints());

	// DEBUG VISUALIZATION
	vmeSurface->SetName("cutting line");
	vmeSurface->SetData(vmeVTKData, m_CurrentVolume->GetTimeStamp());
	vmeSurface->SetVisibleToTraverse(false);

	vmeSurface->GetMaterial()->m_Prop->SetAmbient(1);
	vmeSurface->GetMaterial()->m_Prop->SetDiffuse(0);
	vmeSurface->GetMaterial()->m_Prop->SetSpecular(0);
	vmeSurface->GetMaterial()->m_Prop->SetOpacity(0.4);

	vmeSurface->SetAbsMatrix(inputABSMatrix);

	vmeSurface->GetOutput()->GetVTKData()->Modified();
	vmeSurface->GetOutput()->GetVTKData()->Update();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::BuildSlicingPlane(mafVMESurface *inVME, 
													int fromDirection, int guestView, double sliceHeight, mafVMESlicer * outputSlicer,
													vtkMatrix4x4 * outputMatrix , bool showHeightText, vtkCaptionActor2D *captionActor)
{
	mafVMESlicer *plane1SourceSlicer = NULL;

	double plane2Normal[3];
	double plane2Origin[3];

	vtkMAFSmartPointer<vtkTransform> tr;

	// Z direction cut feedback
	if (fromDirection == FROM_Z && guestView == X_VIEW)
	{
		inVME->GetMaterial()->m_Prop->SetColor(0,0,1);

		plane1SourceSlicer = m_SlicerX;
		outputSlicer = m_SlicerZ;

		// testing
		vtkMatrix4x4 *mat = m_GizmoXView->GetAbsPose()->GetVTKMatrix();
		assert(mat);

		tr->SetMatrix(mat);

		// height
		tr->Translate(-sliceHeight,0,0);
		tr->Update();

		double pos[3];
		tr->GetPosition(pos);

		plane2Origin[0] = pos[0];
		plane2Origin[1] = pos[1];
		plane2Origin[2] = pos[2];

		double versor[3];
		mafMatrix::GetVersor(X, mafMatrix(mat), versor);

		plane2Normal[0] = versor[0];
		plane2Normal[1] = versor[1];
		plane2Normal[2] = versor[2];
	}
	else if (fromDirection == FROM_Z && guestView == Y_VIEW)
	{
		inVME->GetMaterial()->m_Prop->SetColor(0,0,1);

		plane1SourceSlicer = m_SlicerY;
		outputSlicer = m_SlicerZ;

		// testing
		vtkMatrix4x4 *mat = m_GizmoXView->GetAbsPose()->GetVTKMatrix();
		assert(mat);

		vtkMAFSmartPointer<vtkTransform> tr;
		tr->SetMatrix(mat);

		// negative sliceHeight means going upper :P
		tr->Translate(-sliceHeight,0,0);
		tr->Update();

		double pos[3];
		tr->GetPosition(pos);

		plane2Origin[0] = pos[0];
		plane2Origin[1] = pos[1];
		plane2Origin[2] = pos[2];

		double versor[3];
		mafMatrix::GetVersor(X, mafMatrix(mat), versor);

		plane2Normal[0] = versor[0];
		plane2Normal[1] = versor[1];
		plane2Normal[2] = versor[2];
	}

	// Y direction cut feedback
	if (fromDirection == FROM_Y && guestView == X_VIEW)
	{
		inVME->GetMaterial()->m_Prop->SetColor(0,1,0);

		plane1SourceSlicer = m_SlicerX;
		outputSlicer = m_SlicerY;

		// plane Xn plane: to be changed for pose != identity
		// cuttingplane normal from cross Zn
		// cutting plane origin from cross center

		// testing
		vtkMatrix4x4 *mat = m_GizmoXView->GetAbsPose()->GetVTKMatrix();
		assert(mat);

		vtkMAFSmartPointer<vtkTransform> tr;
		tr->SetMatrix(mat);

		// positive sliceHeight is toward triangle camera
		tr->Translate(0,-sliceHeight,0);
		tr->Update();

		double pos[3];
		tr->GetPosition(pos);


		plane2Origin[0] = pos[0];
		plane2Origin[1] = pos[1];
		plane2Origin[2] = pos[2];

		double versor[3];
		mafMatrix::GetVersor(Y, mafMatrix(mat), versor);

		plane2Normal[0] = versor[0];
		plane2Normal[1] = versor[1];
		plane2Normal[2] = versor[2];
	}
	else if (fromDirection == FROM_Y && guestView == Z_VIEW)
	{
		inVME->GetMaterial()->m_Prop->SetColor(0,1,0);

		plane1SourceSlicer = m_SlicerZ;
		outputSlicer = m_SlicerY;

		// testing
		vtkMatrix4x4 *mat = m_GizmoXView->GetAbsPose()->GetVTKMatrix();
		assert(mat);

		vtkMAFSmartPointer<vtkTransform> tr;
		tr->SetMatrix(mat);

		// positive sliceHeight is toward triangle camera
		tr->Translate(0,-sliceHeight,0);
		tr->Update();

		double pos[3];
		tr->GetPosition(pos);

		plane2Origin[0] = pos[0];
		plane2Origin[1] = pos[1];
		plane2Origin[2] = pos[2];

		double versor[3];
		mafMatrix::GetVersor(Y, mafMatrix(mat), versor);

		plane2Normal[0] = versor[0];
		plane2Normal[1] = versor[1];
		plane2Normal[2] = versor[2];
	}

	// X direction cut feedback
	if (fromDirection == FROM_X && guestView == Y_VIEW)
	{
		inVME->GetMaterial()->m_Prop->SetColor(1,0,0);

		plane1SourceSlicer = m_SlicerY;
		outputSlicer = m_SlicerX;

		// plane Xn plane: to be changed for pose != identity
		// cuttingplane normal from cross Zn
		// cutting plane origin from cross center

		// testing
		vtkMatrix4x4 *mat = m_GizmoXView->GetAbsPose()->GetVTKMatrix();
		assert(mat);

		vtkMAFSmartPointer<vtkTransform> tr;
		tr->SetMatrix(mat);

		// positive sliceHeight is toward triangle camera
		tr->Translate(0,0,sliceHeight);
		tr->Update();

		double pos[3];
		tr->GetPosition(pos);

		plane2Origin[0] = pos[0];
		plane2Origin[1] = pos[1];
		plane2Origin[2] = pos[2];

		double versor[3];
		mafMatrix::GetVersor(Z, mafMatrix(mat), versor);

		plane2Normal[0] = versor[0];
		plane2Normal[1] = versor[1];
		plane2Normal[2] = versor[2];
	}
	else if (fromDirection == FROM_X && guestView == Z_VIEW)
	{
		inVME->GetMaterial()->m_Prop->SetColor(1,0,0);

		plane1SourceSlicer = m_SlicerZ;
		outputSlicer = m_SlicerX;

		// testing
		vtkMatrix4x4 *mat = m_GizmoXView->GetAbsPose()->GetVTKMatrix();
		assert(mat);

		vtkMAFSmartPointer<vtkTransform> tr;
		tr->SetMatrix(mat);

		// positive sliceHeight is toward triangle camera
		tr->Translate(0,0,sliceHeight);
		tr->Update();

		double pos[3];
		tr->GetPosition(pos);

		plane2Origin[0] = pos[0];
		plane2Origin[1] = pos[1];
		plane2Origin[2] = pos[2];

		double versor[3];
		mafMatrix::GetVersor(Z, mafMatrix(mat), versor);

		plane2Normal[0] = versor[0];
		plane2Normal[1] = versor[1];
		plane2Normal[2] = versor[2];
	}

	if (outputMatrix)
	{
		outputMatrix->DeepCopy(tr->GetMatrix());
	}

	vtkLinearTransform *inputABSTransform = NULL;
	inputABSTransform = plane1SourceSlicer->GetAbsMatrixPipe()->GetVTKTransform();

	assert(m_ViewXnSliceYBoundsVMEVector[BOUND_0]);

	// build implicit plane for cutting
	vtkCutter *boundsCutter = vtkCutter::New();

	vtkPlane *plane2 = vtkPlane::New();
	plane2->SetOrigin(plane2Origin);
	plane2->SetNormal(plane2Normal);

	vtkPolyData *m_Plane1PD = vtkPolyData::SafeDownCast(plane1SourceSlicer->GetSurfaceOutput()->GetVTKData());
	assert(m_Plane1PD);
	assert(m_Plane1PD->GetNumberOfCells() == 1);

	vtkTransformPolyDataFilter *plane1TPDF = vtkTransformPolyDataFilter::New();
	plane1TPDF->SetInput(m_Plane1PD);
	plane1TPDF->SetTransform(inputABSTransform);
	plane1TPDF->Update();

	boundsCutter->SetInput(plane1TPDF->GetOutput());
	boundsCutter->SetCutFunction(plane2);
	boundsCutter->Update();

	int numPoints = boundsCutter->GetOutput()->GetNumberOfPoints();

	if (numPoints >= 2 )
	{
		//assert(numPoints == 2);
		assert(true);

		vtkPoints *points = boundsCutter->GetOutput()->GetPoints();

		double p1[3];
		points->GetPoint(0, p1);

		double p2[3];
		points->GetPoint(1, p2);

		vtkLineSource *lineSource =vtkLineSource::New();
		lineSource->SetPoint1(p1);
		lineSource->SetPoint2(p2);
		lineSource->Update();

		vtkMAFTubeFilter *tubeFilter = vtkMAFTubeFilter::New();
		tubeFilter->SetInput(lineSource->GetOutput());
		tubeFilter->SetRadius(0.3); // to be adapted to input vme
		tubeFilter->SetNumberOfSides(10);
		tubeFilter->Update();

		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

		ShowVTKDataAsVMESurface(tubeFilter->GetOutput(), inVME , matrix);

		if (showHeightText == true)
		{
			ShowCaptionActor(captionActor, guestView, captionActor->GetCaption() , (p1[0] + p2[0])/2, 
				(p1[1] + p2[1])/2 , (p1[2] + p2[2])/2);
		}

		tubeFilter->Delete();
		matrix->Delete();
		lineSource->Delete();
	}

	plane1TPDF->Delete();
	boundsCutter->Delete();
	plane2->Delete();

}
//----------------------------------------------------------------------------

void mafViewArbitraryOrthoSlice::AddVMEToMSFTree(mafVMESurface *vme)
{
	assert(vme != NULL);
	vme->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	GetLogicManager()->VmeAdd(vme);
	assert(vme);
}

////////////// Z cut planes
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateYnViewZPlanes()
{

	assert(m_FeedbackLineHeight > 0);

	assert(m_ViewYnSliceZBoundsVMEVector[BOUND_0]);
	BuildSlicingPlane(m_ViewYnSliceZBoundsVMEVector[BOUND_0], FROM_Z, Y_VIEW,  m_FeedbackLineHeight[BLUE]);

	// build ruler

	// for every axial section

	assert(m_ViewYnSliceZBoundsVMEVector[BOUND_1]);
	BuildSlicingPlane(m_ViewYnSliceZBoundsVMEVector[BOUND_1], FROM_Z, Y_VIEW,  -m_FeedbackLineHeight[BLUE]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateXnViewZPlanes()
{
	assert(m_ViewXnSliceZBoundsVMEVector[BOUND_0]);

	BuildSlicingPlane(m_ViewXnSliceZBoundsVMEVector[BOUND_0], FROM_Z, X_VIEW,  m_FeedbackLineHeight[BLUE]);

	assert(m_ViewXnSliceZBoundsVMEVector[BOUND_1]);

	BuildSlicingPlane(m_ViewXnSliceZBoundsVMEVector[BOUND_1], FROM_Z, X_VIEW,  -m_FeedbackLineHeight[BLUE]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateZCutPlanes()
{
	// build Yn view images export gizmos
	if (m_ViewYnSliceZBoundsVMEVector[BOUND_0] && m_ViewYnSliceZBoundsVMEVector[BOUND_1])
	{
		UpdateYnViewZPlanes();
		UpdateXnViewZPlanes();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowZCutPlanes( bool show )
{
	ShowVMESurfacesVector(m_ViewXnSliceZBoundsVMEVector, X_VIEW, show);
	ShowVMESurfacesVector(m_ViewYnSliceZBoundsVMEVector, Y_VIEW, show);
	ShowVMESurfacesVector(m_ViewXnSliceZBoundsVMEVector, PERSPECTIVE_VIEW, show);
	ShowVMESurfacesVector(m_ViewYnSliceZBoundsVMEVector, PERSPECTIVE_VIEW, show);
}

////////////// X cut planes
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateYnViewXPlanes()
{
	BuildSlicingPlane(m_ViewYnSliceXBoundsVMEVector[BOUND_0], FROM_X, Y_VIEW,  m_FeedbackLineHeight[RED]);

	assert(m_FeedbackLineHeight > 0);

	BuildSlicingPlane(m_ViewYnSliceXBoundsVMEVector[BOUND_1], FROM_X, Y_VIEW,  -m_FeedbackLineHeight[RED]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateZnViewXPlanes()
{
	BuildSlicingPlane(m_ViewZnSliceXBoundsVMEVector[BOUND_0], FROM_X, Z_VIEW,  m_FeedbackLineHeight[RED]);

	assert(m_FeedbackLineHeight > 0);

	BuildSlicingPlane(m_ViewZnSliceXBoundsVMEVector[BOUND_1], FROM_X, Z_VIEW,  -m_FeedbackLineHeight[RED]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowXCutPlanes( bool show )
{
	ShowVMESurfacesVector(m_ViewYnSliceXBoundsVMEVector, Y_VIEW, show);
	ShowVMESurfacesVector(m_ViewZnSliceXBoundsVMEVector, Z_VIEW, show);

	ShowVMESurfacesVector(m_ViewYnSliceXBoundsVMEVector, PERSPECTIVE_VIEW, show);
	ShowVMESurfacesVector(m_ViewZnSliceXBoundsVMEVector, PERSPECTIVE_VIEW, show);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateXnViewYPlanes()
{
	BuildSlicingPlane(m_ViewXnSliceYBoundsVMEVector[BOUND_0], FROM_Y, X_VIEW,  m_FeedbackLineHeight[GREEN]);

	BuildSlicingPlane(m_ViewXnSliceYBoundsVMEVector[BOUND_1], FROM_Y, X_VIEW,  -m_FeedbackLineHeight[GREEN]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateZnViewYPlanes()
{
	BuildSlicingPlane(m_ViewZnSliceYBoundsVMEVector[BOUND_0], FROM_Y, Z_VIEW,  m_FeedbackLineHeight[GREEN]);

	BuildSlicingPlane(m_ViewZnSliceYBoundsVMEVector[BOUND_1], FROM_Y, Z_VIEW,  -m_FeedbackLineHeight[GREEN]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowYCutPlanes( bool show )
{
	ShowVMESurfacesVector(m_ViewXnSliceYBoundsVMEVector, X_VIEW, show);
	ShowVMESurfacesVector(m_ViewZnSliceYBoundsVMEVector, Z_VIEW, show);
	ShowVMESurfacesVector(m_ViewXnSliceYBoundsVMEVector, PERSPECTIVE_VIEW, show);
	ShowVMESurfacesVector(m_ViewZnSliceYBoundsVMEVector, PERSPECTIVE_VIEW, show);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::BuildSliceHeightFeedbackLinesVMEs()
{
	// view Xn

	/*

	P      |   Zn
	|   GC_Zn
	--------------------
	Xn     |   Yn
	|	GC_Xn  |   GC_Yn     
	---                      

	Y
	V
	|      <X
	----------------------
	Z    |   Z 
	V        V
	|	Y>              <X   
	---					  

	*/

	m_ViewXnSliceYBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewXnSliceYBoundsVMEVector[BOUND_0]);
	AddVMEToMSFTree(m_ViewXnSliceYBoundsVMEVector[BOUND_0]);

	m_ViewXnSliceYBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewXnSliceYBoundsVMEVector[BOUND_1]);
	AddVMEToMSFTree(m_ViewXnSliceYBoundsVMEVector[BOUND_1]);

	m_ViewXnSliceZBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewXnSliceZBoundsVMEVector[BOUND_0]);
	AddVMEToMSFTree(m_ViewXnSliceZBoundsVMEVector[BOUND_0]);

	m_ViewXnSliceZBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewXnSliceZBoundsVMEVector[BOUND_1]);
	AddVMEToMSFTree(m_ViewXnSliceZBoundsVMEVector[BOUND_1]);
	//-----------

	// view Yn

	/*

	P      |   Zn
	|   GC_Zn
	--------------------
	Xn     |   Yn
	GC_Xn  |   GC_Yn     |
	---

	Y
	V
	|      <X
	----------------------
	Z    |   Z 
	V        V
	Y>              <X   | 
	---

	*/

	m_ViewYnSliceZBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewYnSliceZBoundsVMEVector[BOUND_0]);
	AddVMEToMSFTree(m_ViewYnSliceZBoundsVMEVector[BOUND_0]);

	m_ViewYnSliceZBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewYnSliceZBoundsVMEVector[BOUND_1]);
	AddVMEToMSFTree(m_ViewYnSliceZBoundsVMEVector[BOUND_1]);

	m_ViewYnSliceXBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewYnSliceXBoundsVMEVector[BOUND_0]);
	AddVMEToMSFTree(m_ViewYnSliceXBoundsVMEVector[BOUND_0]);

	m_ViewYnSliceXBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewYnSliceXBoundsVMEVector[BOUND_1]);
	AddVMEToMSFTree(m_ViewYnSliceXBoundsVMEVector[BOUND_1]);

	m_ViewZnSliceXBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewZnSliceXBoundsVMEVector[BOUND_0]);
	AddVMEToMSFTree(m_ViewZnSliceXBoundsVMEVector[BOUND_0]);

	m_ViewZnSliceXBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewZnSliceXBoundsVMEVector[BOUND_1]);
	AddVMEToMSFTree(m_ViewZnSliceXBoundsVMEVector[BOUND_1]);

	m_ViewZnSliceYBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewZnSliceYBoundsVMEVector[BOUND_0]);
	AddVMEToMSFTree(m_ViewZnSliceYBoundsVMEVector[BOUND_0]);

	m_ViewZnSliceYBoundsVMEVector.push_back(NULL);
	mafNEW(m_ViewZnSliceYBoundsVMEVector[BOUND_1]);
	AddVMEToMSFTree(m_ViewZnSliceYBoundsVMEVector[BOUND_1]);
}

//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::HideAllCutPlanes()
{
	ShowXCutPlanes(false);
	ShowYCutPlanes(false);
	ShowZCutPlanes(false);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateSlicers(int axis)
{
	const int numSlicers = 3;
	mafVMESlicer *slicers[numSlicers] = {m_SlicerX, m_SlicerY, m_SlicerZ};

	slicers[axis]->SetAbsMatrix(slicers[axis]->GetAbsMatrixPipe()->GetMatrix());
	slicers[axis]->GetSurfaceOutput()->GetVTKData()->Modified();
	slicers[axis]->GetSurfaceOutput()->GetVTKData()->Update();
}


//----------------------------------------------------------------------------
mafPipeSurface * mafViewArbitraryOrthoSlice::GetPipe(int inView, mafVMESurface *inSurface)
{
	mafPipeSurface *pipe = (mafPipeSurface *)((m_ChildViewList[inView])->GetNodePipe(inSurface));
	return pipe;
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::CreateViewCameraNormalFeedbackActor(double col[3], int view)
{
	double m_BorderColor[3];
	//  bool m_Border = false;

	m_BorderColor[0] = col[0];
	m_BorderColor[1] = col[1];
	m_BorderColor[2] = col[2];

	//if(m_Border) BorderDelete();
	int size[2];
	this->GetWindow()->GetSize(&size[0],&size[1]);
	vtkSphereSource *ss = vtkSphereSource::New();

	ss->SetRadius(size[0] / 20.0);
	ss->SetCenter(0,0,0);
	ss->SetThetaResolution(1);
	ss->Update();

	vtkCoordinate *coord = vtkCoordinate::New();
	coord->SetCoordinateSystemToDisplay();
	coord->SetValue(size[0]-1, size[1]-1, 0);

	vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
	pdmd->SetInput(ss->GetOutput());
	pdmd->SetTransformCoordinate(coord);

	vtkProperty2D *pd = vtkProperty2D::New();
	pd->SetDisplayLocationToForeground();
	pd->SetLineWidth(4);
	pd->SetColor(col[0],col[1],col[2]);
	pd->SetOpacity(0.2);

	vtkActor2D *m_Border = vtkActor2D::New();
	m_Border->SetMapper(pdmd);
	m_Border->SetProperty(pd);
	m_Border->SetPosition(1,1);

	((mafViewVTK*)(m_ChildViewList[view]))->m_Rwi->m_RenFront->AddActor2D(m_Border);

	vtkDEL(ss);
	vtkDEL(coord);
	vtkDEL(pdmd);
	vtkDEL(pd);
	vtkDEL(m_Border);
}

//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::EnableSlicersPicking(bool enable)
{
	// x view
	mafPipeSurfaceTextured *pipeXViewSlicerX=(mafPipeSurfaceTextured *)(m_ChildViewList[X_VIEW])->GetNodePipe(m_SlicerX);
	pipeXViewSlicerX->SetActorPicking(enable);
	pipeXViewSlicerX->ShowAxisOff();
	pipeXViewSlicerX->SelectionActorOff();

	// y view
	mafPipeSurfaceTextured *pipeYViewSlicerY=(mafPipeSurfaceTextured *)(m_ChildViewList[Y_VIEW])->GetNodePipe(m_SlicerY);
	pipeYViewSlicerY->SetActorPicking(enable);
	pipeYViewSlicerY->ShowAxisOff();
	pipeYViewSlicerY->SelectionActorOff();

	// z view
	mafPipeSurfaceTextured *pipeZViewSlicerZ=(mafPipeSurfaceTextured *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_SlicerZ);
	pipeZViewSlicerZ->SetActorPicking(enable);
	pipeZViewSlicerZ->ShowAxisOff();
	pipeZViewSlicerZ->SelectionActorOff();

	// perspective X slicer
	mafPipeSurfaceTextured *pipePerspectiveViewSlicerX=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerX);
	pipePerspectiveViewSlicerX->SetActorPicking(false);
	pipePerspectiveViewSlicerX->ShowAxisOff();
	pipePerspectiveViewSlicerX->SelectionActorOff();

	// perspective Y slicer
	mafPipeSurfaceTextured *pipePerspectiveViewSlicerY=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerY);
	pipePerspectiveViewSlicerY->SetActorPicking(false);
	pipePerspectiveViewSlicerY->ShowAxisOff();
	pipePerspectiveViewSlicerY->SelectionActorOff();

	// perspective Z slicer
	mafPipeSurfaceTextured *pipePerspectiveViewSlicerZ=(mafPipeSurfaceTextured *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_SlicerZ);
	pipePerspectiveViewSlicerZ->SetActorPicking(false);
	pipePerspectiveViewSlicerZ->ShowAxisOff();
	pipePerspectiveViewSlicerZ->SelectionActorOff();

}

//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateSlicersLUT()
{
	double low, hi;
	m_LutSlider->GetSubRange(&low,&hi);
	m_ColorLUT->SetTableRange(low,hi);

	mafVMEOutputSurface *surfaceOutputSlicerX = mafVMEOutputSurface::SafeDownCast(m_SlicerX->GetOutput());
	assert(surfaceOutputSlicerX);
	surfaceOutputSlicerX->Update();
	surfaceOutputSlicerX->GetMaterial()->m_ColorLut->SetTableRange(low,hi);

	mafVMEOutputSurface *surfaceOutputSlicerY = mafVMEOutputSurface::SafeDownCast(m_SlicerY->GetOutput());
	assert(surfaceOutputSlicerY);
	surfaceOutputSlicerY->Update();
	surfaceOutputSlicerY->GetMaterial()->m_ColorLut->SetTableRange(low,hi);

	mafVMEOutputSurface *surfaceOutputSlicerZ = mafVMEOutputSurface::SafeDownCast(m_SlicerZ->GetOutput());
	assert(surfaceOutputSlicerZ);
	surfaceOutputSlicerZ->Update();
	surfaceOutputSlicerZ->GetMaterial()->m_ColorLut->SetTableRange(low,hi);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateWindowing(bool enable,mafVME *vme)
{
	if(vme->GetOutput() && vme->GetOutput()->IsA("mafVMEOutputVolume") && enable)
	{
		VolumeWindowing(vme);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::GenerateMoveEvent( mafInteractorCameraPicker * picker, double * pickedPointCoordinates )
{
	mafGizmoCrossRotateTranslate *gizmo = NULL;

	if (picker == m_XSlicerPicker)
	{
		gizmo = m_GizmoXView;
	}
	else if (picker == m_YSlicerPicker)
	{
		gizmo = m_GizmoYView;
	}
	else if (picker == m_ZSlicerPicker)
	{
		gizmo = m_GizmoZView;
	}

	vtkMatrix4x4 *gizmoStartMatrix = gizmo->GetAbsPose()->GetVTKMatrix();

	double startPosition[3]; 
	double endPosition[3];
	double delta[3];

	mafMatrix mat;
	mat.DeepCopy(gizmoStartMatrix);     
	mafTransform::GetPosition(gizmoStartMatrix, startPosition);

	mafMatrix ngm;
	ngm.DeepCopy(gizmoStartMatrix);

	endPosition[0] = pickedPointCoordinates[0];
	endPosition[1] = pickedPointCoordinates[1];
	endPosition[2] = pickedPointCoordinates[2];

	delta[0] = endPosition[0] - startPosition[0];
	delta[1] = endPosition[1] - startPosition[1];
	delta[2] = endPosition[2] - startPosition[2];

	mafTransform::SetPosition(ngm, endPosition);

	mafMatrix matrixToSend;
	mafTransform::SetPosition(matrixToSend, delta);

	mafEvent fakeEvent;
	fakeEvent.SetId(ID_TRANSFORM);
	fakeEvent.SetMatrix(&matrixToSend);
	fakeEvent.SetArg(mafInteractorGenericMouse::MOUSE_MOVE);

	if (picker == m_XSlicerPicker)
	{
		OnEventGizmoCrossTranslateXNormalView(&fakeEvent);
	}
	else if (picker == m_YSlicerPicker)
	{
		OnEventGizmoCrossTranslateYNormalView(&fakeEvent);
	}
	else if (picker == m_ZSlicerPicker)
	{
		OnEventGizmoCrossTranslateZNormalView(&fakeEvent);
	}

	gizmo->SetAbsPose(&ngm);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowVMESurfacesVector( vector<mafVMESurface *> &inVector, int view, bool show )
{
	int size = inVector.size();

	for (int i = 0; i < size; i++)
	{
		m_ChildViewList[view]->VmeShow(inVector[i], show);

		mafPipeSurface *pipe = NULL;

		pipe = GetPipe(view, inVector[i]);
		if (pipe) pipe->SetActorPicking(false);

	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowRulerVMEVector(vector<mafVMESurface *> &rulerVector, 
													 vector<vtkCaptionActor2D *> &captionActorVector , int fromDirection , int guestView)
{
	rulerVector.clear();

	int choosedExportAxis = fromDirection;

	double step = 2 * m_FeedbackLineHeight[choosedExportAxis] / (m_NumberOfAxialSections[choosedExportAxis] - 1);

	int color = fromDirection;

	double relativeHeight = 0;

	for (int idAxialSection = 0 ; idAxialSection < m_NumberOfAxialSections[color] ; idAxialSection++)
	{
		// build the slicing plane
		rulerVector.push_back(NULL);
		mafNEW(rulerVector[idAxialSection]);

		AddVMEToMSFTree(rulerVector[idAxialSection]);

		double height = -m_FeedbackLineHeight[color] + step*idAxialSection;

		captionActorVector.push_back(NULL);
		vtkNEW(captionActorVector[idAxialSection]);

		wxString sliceRelativeHight;

		relativeHeight = step * idAxialSection;

		if (color == GREEN)
		{
			relativeHeight = 2 * m_FeedbackLineHeight[GREEN] - relativeHeight;
			sliceRelativeHight << relativeHeight;
		}
		else
		{
			sliceRelativeHight << relativeHeight;
		}
		
		captionActorVector[idAxialSection]->SetCaption(sliceRelativeHight.c_str());
		BuildSlicingPlane(rulerVector[idAxialSection],fromDirection, guestView, height , NULL, NULL,
			true, captionActorVector[idAxialSection]);	    
	}

	ShowVMESurfacesVector(rulerVector, guestView, true);

}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowRuler( int ruler , bool show)
{
	if (ruler == X_RULER)
	{
		if (show == true)
		{
			ShowRulerVMEVector(m_ViewYnSliceXRulerVMEVector, m_ViewYnSliceXRulerTextActorsVector,
				FROM_X, Y_VIEW);
			
			ShowRulerVMEVector(m_ViewZnSliceXRulerVMEVector, m_ViewZnSliceXRulerTextActorsVector,
				FROM_X, Z_VIEW);
			
			ShowVMESurfacesVector(m_ViewYnSliceXRulerVMEVector, PERSPECTIVE_VIEW, true);
			ShowVMESurfacesVector(m_ViewZnSliceXRulerVMEVector, PERSPECTIVE_VIEW, true);

		}
		else
		{
			HideRulerVMEVector(m_ViewYnSliceXRulerVMEVector,  m_ViewYnSliceXRulerTextActorsVector,
				Y_VIEW);
			HideRulerVMEVector(m_ViewZnSliceXRulerVMEVector,  m_ViewZnSliceXRulerTextActorsVector,
				Z_VIEW);

			ShowVMESurfacesVector(m_ViewYnSliceXRulerVMEVector, PERSPECTIVE_VIEW, false);
			ShowVMESurfacesVector(m_ViewZnSliceXRulerVMEVector, PERSPECTIVE_VIEW, false);

		}

	}
	else if (ruler == Y_RULER)
	{
		if (show == true)
		{
			ShowRulerVMEVector(m_ViewXnSliceYRulerVMEVector,  m_ViewXnSliceYRulerTextActorsVector,
				FROM_Y, X_VIEW);
			ShowRulerVMEVector(m_ViewZnSliceYRulerVMEVector, m_ViewZnSliceYRulerTextActorsVector ,
				FROM_Y, Z_VIEW);	

			ShowVMESurfacesVector(m_ViewXnSliceYRulerVMEVector, PERSPECTIVE_VIEW, true);
			ShowVMESurfacesVector(m_ViewZnSliceYRulerVMEVector, PERSPECTIVE_VIEW, true);
		}
		else
		{
			HideRulerVMEVector(m_ViewXnSliceYRulerVMEVector, m_ViewXnSliceYRulerTextActorsVector,
				X_VIEW);
			HideRulerVMEVector(m_ViewZnSliceYRulerVMEVector, m_ViewZnSliceYRulerTextActorsVector,
				Z_VIEW);	

			ShowVMESurfacesVector(m_ViewXnSliceYRulerVMEVector, PERSPECTIVE_VIEW, false);
			ShowVMESurfacesVector(m_ViewZnSliceYRulerVMEVector, PERSPECTIVE_VIEW, false);

		}

	}
	else if (ruler == Z_RULER)
	{
		if (show == true)
		{
			ShowRulerVMEVector(m_ViewXnSliceZRulerVMEVector, m_ViewXnSliceZRulerTextActorsVector,
				FROM_Z, X_VIEW);
			ShowRulerVMEVector(m_ViewYnSliceZRulerVMEVector, m_ViewYnSliceZRulerTextActorsVector,
				FROM_Z, Y_VIEW);

			ShowVMESurfacesVector(m_ViewXnSliceYRulerVMEVector, PERSPECTIVE_VIEW, true);
			ShowVMESurfacesVector(m_ViewYnSliceZRulerVMEVector, PERSPECTIVE_VIEW, true);

		}
		else
		{
			HideRulerVMEVector(m_ViewXnSliceZRulerVMEVector, m_ViewXnSliceZRulerTextActorsVector, 
				X_VIEW);
			HideRulerVMEVector(m_ViewYnSliceZRulerVMEVector, m_ViewYnSliceZRulerTextActorsVector, 
				Y_VIEW);

			ShowVMESurfacesVector(m_ViewXnSliceYRulerVMEVector, PERSPECTIVE_VIEW, false);
			ShowVMESurfacesVector(m_ViewYnSliceZRulerVMEVector, PERSPECTIVE_VIEW, false);

		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::HideRulerVMEVector(vector<mafVMESurface *> &rulerVector, 
													 vector<vtkCaptionActor2D *> &captionActorVector , int guestView)
{
	ShowVMESurfacesVector(rulerVector, guestView, false);
	int size  = rulerVector.size();

	for (int i = 0; i < size; i++)
	{
		rulerVector[i]->ReparentTo(NULL);
		rulerVector[i] = NULL;

		vtkRenderer *currentRenderer = ((mafViewVTK*)(m_ChildViewList[guestView]))->m_Rwi->m_RenFront;
		currentRenderer->RemoveActor2D(captionActorVector[i]);
		captionActorVector[i]->Delete();

	}

	captionActorVector.clear();
	rulerVector.clear();

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowCaptionActor(vtkCaptionActor2D *actor, 
												   int guestView, wxString text, double x, double y ,double z)
{
	assert(actor != NULL);

	actor->SetPosition(0,0);
	actor->ThreeDimensionalLeaderOff();

	actor->SetHeight(0.01);
	actor->SetWidth(0.01);
	actor->BorderOff();

	actor->SetCaption(text.c_str());
	actor->SetVisibility(true);
	actor->SetAttachmentPoint(x,y,z);

	vtkRenderer *currentRenderer = ((mafViewVTK*)(m_ChildViewList[guestView]))->m_Rwi->m_RenFront;

	currentRenderer->AddActor2D(actor);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ShowCutPlanes( int axis , bool show)
{
	if (axis == RED)
	{
		ShowXCutPlanes(show);
	}
	else if (axis == GREEN)
	{
		ShowYCutPlanes(show);
	}
	else if (axis == BLUE)
	{
		ShowZCutPlanes(show);
	}
}
