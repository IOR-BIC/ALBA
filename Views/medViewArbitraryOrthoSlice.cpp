/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewArbitraryOrthoSlice.cpp,v $
Language:  C++
Date:      $Date: 2011-01-31 18:08:05 $
Version:   $Revision: 1.1.2.30 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGizmoCrossRotateTranslate.h"
#include "medDecl.h"
#include "medViewArbitraryOrthoSlice.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeImage3D.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipeSurface.h"
#include "mafPipeSurfaceTextured.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "medGizmoCrossTranslate.h"
#include "medGizmoCrossRotate.h"
#include "mafSceneGraph.h"
#include "mafEvent.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttachCamera.h"
#include "mafInteractorGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafNodeIterator.h"
#include "mafGUILutPreset.h"
#include "mafVMEOutputSurface.h"
#include "mafAttribute.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafPipeMesh.h"
#include "mafPipeMeshSlice.h"
#include "medPipePolylineGraphEditor.h"

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
#include "medOpMatrixVectorMath.h"
#include "mafTransformFrame.h"
#include "mafVMEGizmo.h"
#include "mafGizmoInterface.h"
#include "mafDataPipe.h"
#include <algorithm>
#include "vtkStructuredPoints.h"
#include "vtkDataSetWriter.h"
#include "vtkUnsignedShortArray.h"
#include "vtkLineSource.h"
#include "vtkMatrix4x4.h"
#include "vtkBMPWriter.h"
#include "mafRWIBase.h"
#include "vtkPNGWriter.h"

mafCxxTypeMacro(medViewArbitraryOrthoSlice);


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

medViewArbitraryOrthoSlice::medViewArbitraryOrthoSlice(wxString label, bool show_ruler)
: medViewCompoundWindowing(label, 2, 2)

{
	m_EnableThickness = 0;
	m_PathFromDialog = "";
	m_EnableExportImages = 0;
	m_ComboChooseAxisDirection = 0;
	m_NumberOfAxialSections = 3;
	m_FeedbackLineHeight = 5;

	m_ExportPlanesHeight = 20;
	m_Thickness = 2;

	m_ViewXnSliceYpVME = NULL;
	m_ViewXnSliceYmVME = NULL;

	m_ViewXnSliceZpVME = NULL;
	m_ViewXnSliceZmVME = NULL;

	m_ViewYnSliceZpVME = NULL;
	m_ViewYnSliceZmVME = NULL;

	m_ViewZnSliceXpVME = NULL;
	m_ViewZnSliceXmVME = NULL;

	m_ViewYnSliceXpVME = NULL;
	m_ViewYnSliceXmVME = NULL;

	m_ViewZnSliceYpVME = NULL;
	m_ViewZnSliceYmVME = NULL;

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

	m_TextActorLeftXView = vtkActor2D::New();
	m_TextMapperLeftXView = vtkTextMapper::New();
	m_TextActorLeftXView->SetMapper(m_TextMapperLeftXView);

	m_TextActorRightXView = vtkActor2D::New();
	m_TextMapperRightXView = vtkTextMapper::New();
	m_TextActorRightXView->SetMapper(m_TextMapperRightXView);

	m_TextActorLeftYView = vtkActor2D::New();
	m_TextMapperLeftYView = vtkTextMapper::New();
	m_TextActorLeftYView->SetMapper(m_TextMapperLeftYView);

	m_TextActorRightYView = vtkActor2D::New();
	m_TextMapperRightYView = vtkTextMapper::New();
	m_TextActorRightYView->SetMapper(m_TextMapperRightYView);

	m_TextActorLeftZView = vtkActor2D::New();
	m_TextMapperLeftZView = vtkTextMapper::New();
	m_TextActorLeftZView->SetMapper(m_TextMapperLeftZView);

	m_TextActorRightZView = vtkActor2D::New();
	m_TextMapperRightZView = vtkTextMapper::New();
	m_TextActorRightZView->SetMapper(m_TextMapperRightZView);
}

medViewArbitraryOrthoSlice::~medViewArbitraryOrthoSlice()
{
	m_SlicerZResetMatrix   = NULL;
	m_CurrentVolume = NULL;
	m_CurrentImage  = NULL;
	m_ColorLUT      = NULL;

	cppDEL(m_GizmoZView);

	vtkDEL(m_TextActorLeftXView);
	vtkDEL(m_TextMapperLeftXView);

	vtkDEL(m_TextActorRightXView);
	vtkDEL(m_TextMapperRightXView);

	vtkDEL(m_TextActorLeftYView);
	vtkDEL(m_TextMapperLeftYView);

	vtkDEL(m_TextActorRightYView);
	vtkDEL(m_TextMapperRightYView);

	vtkDEL(m_TextActorLeftZView);
	vtkDEL(m_TextMapperLeftZView);

	vtkDEL(m_TextActorRightZView);
	vtkDEL(m_TextMapperRightZView);

}

void medViewArbitraryOrthoSlice::PackageView()
{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE,true,false,false,0,false,mafAxes::HEAD);
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
	//	m_ViewArbitrary->PlugVisualPipe("mafVMELabeledVolume", "mafPipeBox", MUTEX);

	m_ViewArbitrary->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);

	m_ViewSliceX = new mafViewVTK("",CAMERA_OS_X,true,false,false,0,false,mafAxes::HEAD);


	// 	m_ViewSliceX->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);

	m_ViewSliceX->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	m_ViewSliceY = new mafViewVTK("",CAMERA_OS_Y,true,false,false,0,false,mafAxes::HEAD);


	// 	m_ViewSliceY->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);
	//m_ViewSliceY->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo");
	m_ViewSliceY->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	m_ViewSliceZ = new mafViewVTK("",CAMERA_OS_Z,true,false,false,0,false,mafAxes::HEAD);


	// 	m_ViewSliceZ->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceZ->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceZ->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);
	//m_ViewSliceZ->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo");
	m_ViewSliceZ->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	// 	m_ViewSliceZ->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceZ->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSliceZ);
	PlugChildView(m_ViewSliceX);
	PlugChildView(m_ViewSliceY);

}

void medViewArbitraryOrthoSlice::VmeShow(mafNode *node, bool show)

{
	mafVME *vme=mafVME::SafeDownCast(node);

	if (vme->IsA("mafVMEGizmo"))
	{
		if (BelongsToZNormalGizmo(vme))
		{
			m_ChildViewList[Z_VIEW]->VmeShow(node, show);
			m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);
		}
		else if (BelongsToXNormalGizmo(vme))
		{
			m_ChildViewList[X_VIEW]->VmeShow(node, show);
			m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);
		}
		else if (BelongsToYNormalGizmo(vme))
		{
			m_ChildViewList[Y_VIEW]->VmeShow(node, show);
			m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);
		}
		else
		{
			return;
		}
	}

	else
	{
		m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);
		m_ChildViewList[Z_VIEW]->VmeShow(node, show);
		m_ChildViewList[X_VIEW]->VmeShow(node, show);
		m_ChildViewList[Y_VIEW]->VmeShow(node, show);
	}

	vme->Update();
	if (show)
	{
		if(((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			ShowMafVMEVolume(vme, show);
			StoreCameraParametersForAllSubviews();
		}

		else if(vme->IsA("mafVMESurface") || vme->IsA("mafVMESurfaceParametric") || vme->IsA("mafVMELandmark") || vme->IsA("mafVMELandmarkCloud"))
		{
			// ShowVMESurfacesAndLandmarks(node);

		}
		else if(vme->IsA("medVMEPolylineEditor"))
		{
			ShowMedVMEPolylineEditor(node);
		}
		else if(vme->IsA("mafVMEMesh"))
		{
			ShowMafVMEMesh(node);

		}
		else if(vme->IsA("mafVMEImage"))
		{	
			ShowMafVMEImage(vme);
		}
	}
	else//if show==false
	{
		if(vme->IsA("medVMEPolylineGraphEditor"))
		{
			m_CurrentPolylineGraphEditor = NULL;
		}

		if(((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			HideMafVMEVolume();
		}
		else if(vme->IsA("mafVMEImage")) {
			HideMafVmeImage();
		}
	}

	if(GetSceneGraph()->GetSelectedVme()==node) {
		UpdateWindowing( show && this->ActivateWindowing(node), node);
	}

}

void medViewArbitraryOrthoSlice::OnEvent(mafEventBase *maf_event)

{
	if (maf_event->GetSender() == this->m_Gui || maf_event->GetSender() == this->m_LutSlider) // from this view gui
	{
		OnEventThis(maf_event); 
	}
	else if (maf_event->GetId() == ID_TRANSFORM)
	{	
		if (maf_event->GetSender() == m_GizmoZView->m_GizmoCrossRotate || maf_event->GetSender() == m_GizmoZView->m_GizmoCrossTranslate)
		{	
			OnEventGizmoCrossZNormal(maf_event);
		}		
		else if (maf_event->GetSender() == m_GizmoYView->m_GizmoCrossRotate || maf_event->GetSender() == m_GizmoYView->m_GizmoCrossTranslate)
		{
			OnEventGizmoCrossYNormal(maf_event);
		}
		else if (maf_event->GetSender() == m_GizmoXView->m_GizmoCrossRotate || maf_event->GetSender() == m_GizmoXView->m_GizmoCrossTranslate)
		{
			OnEventGizmoCrossXNormal(maf_event);
		}
		else
		{
			// if no one can handle this event send it to the operation listener
			mafEventMacro(*maf_event); 
		}	
	}
	else
	{
		mafEventMacro(*maf_event);
	}
}

void medViewArbitraryOrthoSlice::OnEventGizmoCrossTranslateZNormal(mafEventBase *maf_event)
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
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
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
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
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
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
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

					medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
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

void medViewArbitraryOrthoSlice::OnEventGizmoCrossRotateZNormal(mafEventBase *maf_event)
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


			// 			}
			// 			else if (activeGizmoAxis.Equals("Y"))
			// 			{
			// 				UpdateCameraYViewOnEventGizmoCrossRotateZNormal(event);
			// 			}
			// 			else if (activeGizmoAxis.Equals("X"))
			// 			{
			// 				UpdateCameraXViewOnEventGizmoCrossRotateZNormal(event);
			// 			}


			mafEventMacro(mafEvent(this,CAMERA_UPDATE));

			//update the normal of the cutter plane of the surface
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
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

						//PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewSurface->SetNormal(normal);

						//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
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

						//PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewMesh->SetNormal(normal);

						//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
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

					medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}

			UpdateXView2DActors();
			UpdateYView2DActors();

			UpdateCutPlanes();

			ChildViewsCameraUpdate();

		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}

void medViewArbitraryOrthoSlice::OnEventGizmoCrossRotateYNormal(mafEventBase *maf_event)
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
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
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

						//PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewSurface->SetNormal(normal);

						//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
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

						//PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewMesh->SetNormal(normal);

						//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
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

					medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
					//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}


			UpdateXView2DActors();
			UpdateZView2DActors();

			ChildViewsCameraUpdate();

			mafEventMacro(mafEvent(this,CAMERA_UPDATE));
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}


void medViewArbitraryOrthoSlice::OnEventGizmoCrossTranslateYNormal(mafEventBase *maf_event)
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
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
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
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
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
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
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

					medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
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

void medViewArbitraryOrthoSlice::OnEventGizmoCrossRotateXNormal(mafEventBase *maf_event)
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

			mafEventMacro(mafEvent(this,CAMERA_UPDATE));

			//update the normal of the cutter plane of the surface
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
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
						//PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewSurface->SetNormal(normal);

						//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
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

						//PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewMesh->SetNormal(normal);

						//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
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

					medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
				}
			}

			UpdateYView2DActors();
			UpdateZView2DActors();

			ChildViewsCameraUpdate();
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}


void medViewArbitraryOrthoSlice::OnEventGizmoCrossTranslateXNormal(mafEventBase *maf_event)
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
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
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
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
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
						PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
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

					medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
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

void medViewArbitraryOrthoSlice::OnEventThis(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
		case ID_RANGE_MODIFIED:
			{
				OnRangeModified();
			}
			break;
		case ID_LUT_CHOOSER:
			{
				OnLUTChooser();

			}
			break;

		case ID_CHOOSE_DIR:
			{
				OnID_CHOOSE_DIR();
			}
			break;

		case  ID_COMBO_CHOOSE_CUTTING_AXIS:
			{
				OnEventID_COMBO_CHOOSE_CUTTING_AXIS();
			}
			break;

		case  ID_ENABLE_EXPORT_IMAGES:
			{
				// disable thickness
				if (m_EnableThickness == 1)
				{
					m_EnableThickness = 0;
					m_Gui->Update();
					OnEventID_ENABLE_THICKNESS();
				}

				OnEventID_ENABLE_EXPORT_IMAGES();
			}
			break;

		case ID_EXPORT:
			{
				OnEventID_EXPORT();
			}
			break;

			// thickness

		case ID_ENABLE_THICKNESS:
			{
				// disable thickness
				if (m_EnableExportImages == 1)
				{
					m_EnableExportImages = 0;
					m_Gui->Update();
					OnEventID_ENABLE_EXPORT_IMAGES();
				}

				OnEventID_ENABLE_THICKNESS();
			}
			break;

		case ID_THICKNESS:
			{
				OnEventID_THICKNESS();
			}
			break;
		case ID_COMBO_CHOOSE_THICKNESS_AXIS:
			{
				OnEventID_COMBO_THICKNESS_AXIS();
			}
			break;

		case ID_EXPORT_PLANES_HEIGHT:
			{
				OnEventID_EXPORT_PLANES_HEIGHT();
			}

		case ID_RESET:
			{
				OnReset();
			}
			break;

		default:
			mafViewCompound::OnEvent(maf_event);
		}
	}
}

mafView *medViewArbitraryOrthoSlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;
	medViewArbitraryOrthoSlice *v = new medViewArbitraryOrthoSlice(m_Label);
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

mafGUI* medViewArbitraryOrthoSlice::CreateGui()

{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);

	//	m_Gui->Divider(2);

	//button to reset at the start position
	m_Gui->Label("");
	//m_Gui->Label("reset slices", true);
	m_Gui->Button(ID_RESET,_("reset slices"),"");

	//m_Gui->Divider(2);

	//m_Gui->Label("show gizmo", true);

	m_Gui->Divider();


	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	m_Gui->Divider();

	m_Gui->Update();

	m_Gui->Divider(2);
	m_Gui->Label("Thickness", true);
	m_Gui->Bool(ID_ENABLE_THICKNESS, "",&m_EnableThickness);

	m_ComboThicknessAxis = 0;
	wxString Text[3]={_("X AXIS"),_("Y AXIS"),_("Z AXIS")};
	m_Gui->Combo(ID_COMBO_CHOOSE_THICKNESS_AXIS,"",&m_ComboThicknessAxis,3,Text);
	m_Gui->Label("thickness:");
	m_Gui->Double(ID_THICKNESS,_(""),&m_Thickness,0, 10);

	m_Gui->Label("");

	m_Gui->Divider(2);
	m_Gui->Label("Export Images", true);
	m_Gui->Bool(ID_ENABLE_EXPORT_IMAGES, "",&m_EnableExportImages);
	//  m_Gui->Label("");

	m_ComboChooseAxisDirection = 0;
	wxString Text2[3]={_("X AXIS"),_("Y AXIS"),_("Z AXIS")};
	m_Gui->Combo(ID_COMBO_CHOOSE_CUTTING_AXIS,"",&m_ComboChooseAxisDirection,3,Text2);
	
	m_Gui->Label("Export planes height:");
	m_Gui->Integer(ID_EXPORT_PLANES_HEIGHT,_(""),&m_ExportPlanesHeight,0, 10);
	m_Gui->Label("Axial sections:");
	m_Gui->Integer(ID_NUMBER_OF_AXIAL_SECTIONS,_(""),&m_NumberOfAxialSections,0, 10);
	
	m_Gui->Button(ID_CHOOSE_DIR,"Choose export dir");
	m_Gui->Button(ID_EXPORT,"Write images");

	EnableWidgets( (m_CurrentVolume != NULL) );
	return m_Gui;
}

void medViewArbitraryOrthoSlice::VmeRemove(mafNode *node)

{
	if (m_CurrentVolume && node == m_CurrentVolume) 
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

	if (m_CurrentImage && node == m_CurrentImage){
		m_CurrentImage = NULL;
	}

	Superclass::VmeRemove(node);
}

void medViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicers(mafEventBase *maf_event)
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

			// 			std::ostringstream stringStream;
			// 			stringStream << "Concatenating to slicer: " << std::endl;          
			// 			tr->GetMatrix()->PrintSelf(stringStream, NULL);
			// 			mafLogMessage(stringStream.str().c_str());

			tr->PostMultiply();
			tr->SetMatrix(slicers[i]->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
			tr->Update();

			// _DEBUG_
			if (i == 0) // Xn slicer
			{
				// 					std::ostringstream stringStream;
				// 					stringStream << "Concatenating to slicer: " << std::endl;          
				// 					tr->GetMatrix()->PrintSelf(stringStream, NULL);
				// 					mafLogMessage(stringStream.str().c_str());
			}

			mafMatrix absPose;
			absPose.DeepCopy(tr->GetMatrix());
			absPose.SetTimeStamp(slicers[i]->GetTimeStamp());

			if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
			{
				// move vme
				slicers[i]->SetAbsMatrix(absPose);
				UpdateThicknessStuff();

			} 

			

			// clean up
			tr->Delete();
		}
	}
}

void medViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicer(mafEventBase *maf_event, int slicerAxis)
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

		// handle incoming transform events
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
			// move vme
			currentSlicer->SetAbsMatrix(absPose);
		} 

		// clean up
		tr->Delete();
	}
}


void medViewArbitraryOrthoSlice::UpdateSubviewsCamerasToFaceSlices()

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

void medViewArbitraryOrthoSlice::CreateGuiView()

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

void medViewArbitraryOrthoSlice::EnableWidgets(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_RESET, enable);
		m_Gui->Enable(ID_COMBO_CHOOSE_CUTTING_AXIS, enable);
		m_Gui->Enable(ID_LUT_CHOOSER, enable);
		m_Gui->Enable(ID_SHOW_GIZMO, enable);
		m_Gui->FitGui();
		m_Gui->Update();

		if (m_EnableExportImages)
		{
			EnableExportImages(true);
		}
		else
		{
			EnableExportImages(false);
		}

		if (m_EnableThickness)
		{
			EnableThicknessGUI(true);
		}
		else
		{
			EnableThicknessGUI(false);
		}
	}
	m_LutSlider->Enable(enable);

}

void medViewArbitraryOrthoSlice::UpdateSlicerZBehavior()

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

		// x view
		mafPipeSurfaceTextured *pipeXViewSlicerX=(mafPipeSurfaceTextured *)(m_ChildViewList[X_VIEW])->GetNodePipe(m_SlicerX);
		pipeXViewSlicerX->SetActorPicking(true);

		// y view
		mafPipeSurfaceTextured *pipeYViewSlicerY=(mafPipeSurfaceTextured *)(m_ChildViewList[Y_VIEW])->GetNodePipe(m_SlicerY);
		pipeYViewSlicerY->SetActorPicking(true);

		// z view
		mafPipeSurfaceTextured *pipeZViewSlicerZ=(mafPipeSurfaceTextured *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_SlicerZ);
		pipeZViewSlicerZ->SetActorPicking(true);
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


void medViewArbitraryOrthoSlice::VolumeWindowing(mafVME *volume)

{
	double sr[2];
	vtkDataSet *data = ((mafVME *)volume)->GetOutput()->GetVTKData();
	data->Update();
	data->GetScalarRange(sr);

	mmaMaterial *currentSurfaceMaterial = m_SlicerZ->GetMaterial();
	m_ColorLUT = m_SlicerZ->GetMaterial()->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);
}

void medViewArbitraryOrthoSlice::ShowMafVMEVolume( mafVME * vme, bool show )
{
	Update2DActors();

	mafViewVTK *xView = ((mafViewVTK *)(m_ChildViewList[X_VIEW])) ;
	assert(xView);

	double pickerTolerance = 0.03;

	// fuzzy picking
	xView->GetPicker2D()->SetTolerance(pickerTolerance);

	xView->GetSceneGraph()->m_AlwaysVisibleRenderer->AddActor2D(m_TextActorLeftXView);
	xView->GetSceneGraph()->m_AlwaysVisibleRenderer->AddActor2D(m_TextActorRightXView);

	mafViewVTK *yView = ((mafViewVTK *)(m_ChildViewList[Y_VIEW])) ;
	assert(yView);

	// fuzzy picking
	yView->GetPicker2D()->SetTolerance(pickerTolerance);

	yView->GetSceneGraph()->m_AlwaysVisibleRenderer->AddActor2D(m_TextActorLeftYView);
	yView->GetSceneGraph()->m_AlwaysVisibleRenderer->AddActor2D(m_TextActorRightYView);

	mafViewVTK *zView = ((mafViewVTK *)(m_ChildViewList[Z_VIEW])) ;
	assert(zView);

	// fuzzy picking
	zView->GetPicker2D()->SetTolerance(pickerTolerance);

	zView->GetSceneGraph()->m_AlwaysVisibleRenderer->AddActor2D(m_TextActorLeftZView);
	zView->GetSceneGraph()->m_AlwaysVisibleRenderer->AddActor2D(m_TextActorRightZView);

	double sr[2],volumeVTKDataCenterLocalCoords[3];
	mafVME *vmeVolume=mafVME::SafeDownCast(vme);
	m_CurrentVolume = vmeVolume;

	EnableWidgets(true);

	vtkDataSet *volumeVTKData = vmeVolume->GetOutput()->GetVTKData();
	volumeVTKData->Update();

	volumeVTKData->GetCenter(volumeVTKDataCenterLocalCoords);

	volumeVTKData->GetScalarRange(sr);
	volumeVTKData=NULL;

	mafTransform::GetOrientation(vmeVolume->GetAbsMatrixPipe()->GetMatrix(),m_VolumeVTKDataABSOrientation);

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
	sliceCenterLocalCoordsToABSCoordsTransform->SetMatrix(vmeVolume->GetOutput()->GetMatrix()->GetVTKMatrix());
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

	ShowSlicers(vmeVolume, show);

	vtkDEL(points);
	vtkDEL(sliceCenterLocalCoordsPolydata);
	vtkDEL(sliceCenterLocalCoordsToABSCoordsTransform);
	vtkDEL(localToABSTPDF);
	vtkDEL(transformReset);

	VolumeWindowing(vmeVolume);
}

void medViewArbitraryOrthoSlice::ShowMedVMEPolylineEditor( mafNode * node )
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{
		m_CurrentPolylineGraphEditor = (medVMEPolylineEditor*)node;

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
		PipeSliceViewPolylineEditor->SetModalitySlice();

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);        

	}
}

void medViewArbitraryOrthoSlice::ShowMafVMEMesh( mafNode * node )
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
		//PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
		//PipeArbitraryViewSurface->SetNormal(normal);
		mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
		PipeSliceViewMesh->SetNormal(normal);
		//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}

void medViewArbitraryOrthoSlice::ShowMafVMEImage( mafVME * vme )
{
	mafVME *Image  = mafVME::SafeDownCast(vme);
	m_CurrentImage = Image;
}

void medViewArbitraryOrthoSlice::HideMafVMEVolume()
{

	mafViewVTK *xView = ((mafViewVTK *)(m_ChildViewList[X_VIEW])) ;
	assert(xView);

	xView->GetSceneGraph()->m_AlwaysVisibleRenderer->RemoveActor2D(m_TextActorLeftXView);
	xView->GetSceneGraph()->m_AlwaysVisibleRenderer->RemoveActor2D(m_TextActorRightXView);

	mafViewVTK *yView = ((mafViewVTK *)(m_ChildViewList[Y_VIEW])) ;
	assert(yView);

	yView->GetSceneGraph()->m_AlwaysVisibleRenderer->RemoveActor2D(m_TextActorLeftYView);
	yView->GetSceneGraph()->m_AlwaysVisibleRenderer->RemoveActor2D(m_TextActorRightYView);

	mafViewVTK *zView = ((mafViewVTK *)(m_ChildViewList[Z_VIEW])) ;
	assert(zView);

	zView->GetSceneGraph()->m_AlwaysVisibleRenderer->RemoveActor2D(m_TextActorLeftZView);
	zView->GetSceneGraph()->m_AlwaysVisibleRenderer->RemoveActor2D(m_TextActorRightZView);

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

void medViewArbitraryOrthoSlice::HideMafVmeImage()
{
	m_CurrentImage = NULL;
	m_ColorLUT = NULL;
	m_LutWidget->SetLut(m_ColorLUT);
}

void medViewArbitraryOrthoSlice::ShowVMESurfacesAndLandmarks( mafNode * node )
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
		mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
		PipeSliceViewSurface->SetNormal(normal);
	}
}

void medViewArbitraryOrthoSlice::OnReset()
{
	{
		m_GizmoZView->SetAbsPose(m_SlicerZResetMatrix);
		m_GizmoYView->SetAbsPose(m_SlicerYResetMatrix);
		m_GizmoXView->SetAbsPose(m_SlicerXResetMatrix);

		m_SlicerX->SetAbsMatrix(*m_SlicerXResetMatrix);
		m_SlicerY->SetAbsMatrix(*m_SlicerYResetMatrix);
		m_SlicerZ->SetAbsMatrix(*m_SlicerZResetMatrix);

		//update because I need to refresh the normal of the camera
		//mafEventMacro(mafEvent(this,CAMERA_UPDATE));

		RestoreCameraParametersForAllSubviews();

		UpdateXView2DActors();
		UpdateYView2DActors();
		UpdateZView2DActors();

		UpdateCutPlanes();

		mafEventMacro(mafEvent(this,CAMERA_UPDATE));

		//update the normal of the cutter plane of the surface
		mafNode *root=m_CurrentVolume->GetRoot();
		mafNodeIterator *iter = root->NewIterator();
		for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		{
			if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
			{
				//				OnResetSurfaceAndLandmark(node);

			}
			if(node->IsA("mafVMEMesh"))
			{
				//				OnResetMafVMEMesh(node);
			}
		}
		iter->Delete();
		if(m_CurrentPolylineGraphEditor)
		{
			//			OnResetMedVMEPolylineEditor();

		}
	}
}

void medViewArbitraryOrthoSlice::OnRangeModified()
{
	mafVME *node = mafVME::SafeDownCast(GetSceneGraph()->GetSelectedVme());

	if( (m_CurrentVolume || m_CurrentImage) && node)
	{
		double low, hi;
		m_LutSlider->GetSubRange(&low,&hi);
		m_ColorLUT->SetTableRange(low,hi);

		m_SlicerX->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_SlicerX->GetMaterial()->m_ColorLut->Modified();

		m_SlicerY->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_SlicerY->GetMaterial()->m_ColorLut->Modified();

		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}

void medViewArbitraryOrthoSlice::OnLUTChooser()
{
	double *sr;

	if(m_CurrentVolume || m_CurrentImage) {
		sr = m_ColorLUT->GetRange();
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);

		m_SlicerX->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_SlicerX->GetMaterial()->m_ColorLut->Modified();

		m_SlicerY->GetMaterial()->m_ColorLut->DeepCopy(m_ColorLUT);
		m_SlicerY->GetMaterial()->m_ColorLut->Modified();

		mafEventMacro(mafEvent(this,CAMERA_UPDATE));

	}
}


void medViewArbitraryOrthoSlice::OnResetSurfaceAndLandmark( mafNode * node )
{
	mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
	mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
	if(PipeArbitraryViewSurface)
	{
		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
		PipeSliceViewSurface->SetNormal(normal);

		double surfaceOriginTranslated[3];
		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoordinatesReset[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoordinatesReset[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoordinatesReset[2] + normal[2] * 0.1;

		PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
	}
}

void medViewArbitraryOrthoSlice::OnResetMafVMEMesh( mafNode * node )
{
	mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(node));
	mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe(node));
	if(PipeSliceViewMesh && PipeArbitraryViewMesh)
	{
		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
		//PipeArbitraryViewSurface->SetNormal(normal);
		PipeSliceViewMesh->SetNormal(normal);

		double surfaceOriginTranslated[3];
		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoordinatesReset[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoordinatesReset[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoordinatesReset[2] + normal[2] * 0.1;

		//PipeArbitraryViewSurface->SetSlice(surfaceOriginTranslated);
		PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
	}
}

void medViewArbitraryOrthoSlice::OnResetMedVMEPolylineEditor()
{
	//a surface is visible only if there is a volume in the view
	if(m_CurrentVolume)
	{

		double normal[3];
		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

		medPipePolylineGraphEditor *PipeSliceViewPolylineEditor = medPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[Z_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
		PipeSliceViewPolylineEditor->SetModalitySlice();

		double surfaceOriginTranslated[3];

		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);            
	}
}

void medViewArbitraryOrthoSlice::ShowSlicers( mafVME * vmeVolume, bool show )
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

	m_GizmoZView = new medGizmoCrossRotateTranslate();
	m_GizmoZView->Create(m_SlicerZ, this, true, medGizmoCrossRotateTranslate::Z);
	m_GizmoZView->SetName("m_GizmoZView");
	m_GizmoZView->SetInput(m_SlicerZ);
	m_GizmoZView->SetRefSys(m_SlicerZ);
	m_GizmoZView->SetAbsPose(m_SlicerZResetMatrix);
	m_GizmoZView->Show(true);

	m_GizmoYView = new medGizmoCrossRotateTranslate();
	m_GizmoYView->Create(m_SlicerY, this, true, medGizmoCrossRotateTranslate::Y);
	m_GizmoYView->SetName("m_GizmoYView");
	m_GizmoYView->SetInput(m_SlicerY);
	m_GizmoYView->SetRefSys(m_SlicerY);
	m_GizmoYView->SetAbsPose(m_SlicerYResetMatrix);
	m_GizmoYView->Show(true);

	m_GizmoXView = new medGizmoCrossRotateTranslate();
	m_GizmoXView->Create(m_SlicerX, this, true, medGizmoCrossRotateTranslate::X);
	m_GizmoXView->SetName("m_GizmoXView");
	m_GizmoXView->SetInput(m_SlicerX);
	m_GizmoXView->SetRefSys(m_SlicerX);
	m_GizmoXView->SetAbsPose(m_SlicerXResetMatrix);
	m_GizmoXView->Show(true);

	//Create the Gizmos' Gui
	if(!m_GuiGizmos)
		m_GuiGizmos = new mafGUI(this);

	// m_GuiGizmos->AddGui(m_GizmoZView->m_GizmoCrossTranslate->GetGui());
	// m_GuiGizmos->AddGui(m_GizmoZView->m_GizmoCrossRotate->GetGui());
	m_GuiGizmos->Update();
	m_Gui->AddGui(m_GuiGizmos);
	m_Gui->FitGui();
	m_Gui->Update();

	m_SlicerX->SetVisibleToTraverse(false);
	m_SlicerY->SetVisibleToTraverse(false);
	m_SlicerZ->SetVisibleToTraverse(false);

	UpdateSubviewsCamerasToFaceSlices();

	BuildSliceHeightFeedbackLinesVMEs();

	//   mafNEW(m_ViewZnSliceYpVME);
	//   AddVMEToMSFTree(m_ViewYnSliceXpVME);
	// 
	//   mafNEW(m_ViewYnSliceXmVME);
	//   AddVMEToMSFTree(m_ViewYnSliceXmVME);

	UpdateXnViewZPlanes();
	UpdateYnViewZPlanes();

	UpdateYnViewXPlanes();
	UpdateZnViewXPlanes();

	UpdateXnViewYPlanes();
	UpdateZnViewYPlanes();

	HideCutPlanes();  

	// out: slicer to use
	mafVMESlicer *targetSlicer;

	// out: slicer abs matrix for volume cut
	vtkMatrix4x4 *outputMatrix = vtkMatrix4x4::New();

	//   // build Xn view images export gizmos
	//   BuildSlicingPlane(m_ViewXnSliceYpVME, 
	//     FROM_Y, X_VIEW,  20, targetSlicer, outputMatrix);
	// 
	//   BuildSlicingPlane(m_ViewXnSliceYmVME, 
	//     FROM_Y, X_VIEW,  -20, targetSlicer, outputMatrix);

	//   BuildSlicingPlane(m_ViewXnSliceZpVME, 
	//     FROM_Z, X_VIEW,  m_ZCutHeight, targetSlicer, outputMatrix);
	// 
	//   BuildSlicingPlane(m_ViewXnSliceZmVME, 
	//     FROM_Z, X_VIEW,  -m_ZCutHeight, targetSlicer, outputMatrix);
	// 
	// 
	//   // build Yn view images export gizmos
	//   BuildSlicingPlane(m_ViewYnSliceZpVME, 
	//     FROM_Z, Y_VIEW,  20, targetSlicer, outputMatrix);
	// 
	//   BuildSlicingPlane(m_ViewYnSliceZmVME, 
	//     FROM_Z, Y_VIEW,  -20, targetSlicer, outputMatrix);
	// // 
	//   BuildSlicingPlane(m_ViewYnSliceXpVME, 
	//     FROM_X, Y_VIEW,  20, targetSlicer, outputMatrix);
	// 
	//   BuildSlicingPlane(m_ViewYnSliceXmVME, 
	//     FROM_X, Y_VIEW,  -20, targetSlicer, outputMatrix);

	vtkDEL(outputMatrix);
}

void medViewArbitraryOrthoSlice::BuildXCameraConeVME()
{
	double b[6] = {0,0,0,0,0,0};
	assert(m_CurrentVolume);
	m_CurrentVolume->GetOutput()->GetBounds(b);

	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	p1[0] = b[0];
	p1[1] = b[2];
	p1[2] = b[4];
	p2[0] = b[1];
	p2[1] = b[3];
	p2[2] = b[5];

	double d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	vtkConeSource *XCameraConeSource = vtkConeSource::New();
	XCameraConeSource->SetCenter(0,0,d/2);
	XCameraConeSource->SetResolution(20);
	XCameraConeSource->SetDirection(0,0,-1);

	XCameraConeSource->SetRadius(d / 8);
	XCameraConeSource->SetHeight(d / 8);

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


	// DEBUG
	//m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_XCameraConeVME, true);
	m_ChildViewList[Y_VIEW]->VmeShow(m_XCameraConeVME, true);
	m_ChildViewList[Z_VIEW]->VmeShow(m_XCameraConeVME, true);
	//m_ChildViewList[Z_VIEW]->VmeShow(m_ConeVME, true);

	mafPipeSurface *pipeY=(mafPipeSurface *)(m_ChildViewList[Y_VIEW])->GetNodePipe(m_XCameraConeVME);
	pipeY->SetActorPicking(false);

	mafPipeSurface *pipeZ=(mafPipeSurface *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_XCameraConeVME);
	pipeZ->SetActorPicking(false);

	XCameraConeSource->Delete();
}

void medViewArbitraryOrthoSlice::BuildYCameraConeVME()
{
	double b[6] = {0,0,0,0,0,0};
	assert(m_CurrentVolume);
	m_CurrentVolume->GetOutput()->GetBounds(b);

	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	p1[0] = b[0];
	p1[1] = b[2];
	p1[2] = b[4];
	p2[0] = b[1];
	p2[1] = b[3];
	p2[2] = b[5];

	double d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	vtkConeSource *YCameraConeSource = vtkConeSource::New();
	YCameraConeSource->SetCenter(0,0,d/2);
	YCameraConeSource->SetResolution(20);
	YCameraConeSource->SetDirection(0,0,-1);

	YCameraConeSource->SetRadius(d / 8);
	YCameraConeSource->SetHeight(d / 8);

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

	// DEBUG
	//m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_YCameraConeVME, true);
	m_ChildViewList[X_VIEW]->VmeShow(m_YCameraConeVME, true);
	//m_ChildViewList[Y_VIEW]->VmeShow(m_YCameraConeVME, true);
	m_ChildViewList[Z_VIEW]->VmeShow(m_YCameraConeVME, true);

	mafPipeSurface *pipeX=(mafPipeSurface *)(m_ChildViewList[X_VIEW])->GetNodePipe(m_YCameraConeVME);
	pipeX->SetActorPicking(false);

	mafPipeSurface *pipeZ=(mafPipeSurface *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_YCameraConeVME);
	pipeZ->SetActorPicking(false);

	YCameraConeSource->Delete();
}

void medViewArbitraryOrthoSlice::BuildZCameraConeVME()
{
	double b[6] = {0,0,0,0,0,0};
	assert(m_CurrentVolume);
	m_CurrentVolume->GetOutput()->GetBounds(b);

	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	p1[0] = b[0];
	p1[1] = b[2];
	p1[2] = b[4];
	p2[0] = b[1];
	p2[1] = b[3];
	p2[2] = b[5];

	double d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	vtkConeSource *ZCameraConeSource = vtkConeSource::New();
	ZCameraConeSource->SetCenter(0,0,d/2);
	ZCameraConeSource->SetResolution(20);
	ZCameraConeSource->SetDirection(0,0,-1);

	ZCameraConeSource->SetRadius(d / 8);
	ZCameraConeSource->SetHeight(d / 8);

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

	// DEBUG
	//m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_ZCameraConeVME, true);
	m_ChildViewList[X_VIEW]->VmeShow(m_ZCameraConeVME, true);
	//m_ChildViewList[Y_VIEW]->VmeShow(m_ZCameraConeVME, true);
	m_ChildViewList[Y_VIEW]->VmeShow(m_ZCameraConeVME, true);

	mafPipeSurface *pipeX=(mafPipeSurface *)(m_ChildViewList[X_VIEW])->GetNodePipe(m_ZCameraConeVME);
	pipeX->SetActorPicking(false);

	mafPipeSurface *pipeY=(mafPipeSurface *)(m_ChildViewList[Y_VIEW])->GetNodePipe(m_ZCameraConeVME);
	pipeY->SetActorPicking(false);

	ZCameraConeSource->Delete();
	CameraReset();
}

bool medViewArbitraryOrthoSlice::BelongsToZNormalGizmo( mafVME * vme )
{

	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	medGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<medGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoZView"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	medGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<medGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoZView"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;



}

bool medViewArbitraryOrthoSlice::BelongsToXNormalGizmo( mafVME * vme )
{
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	medGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<medGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoXView"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	medGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<medGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoXView"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;
}

bool medViewArbitraryOrthoSlice::BelongsToYNormalGizmo( mafVME * vme )
{
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	medGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<medGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoYView"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	medGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<medGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoYView"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;
}


void medViewArbitraryOrthoSlice::OnEventGizmoCrossZNormal( mafEventBase * maf_event )
{
	if (maf_event->GetSender() == m_GizmoZView->m_GizmoCrossTranslate)
	{
		OnEventGizmoCrossTranslateZNormal(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoZView->m_GizmoCrossRotate) // from rotation gizmo
	{
		OnEventGizmoCrossRotateZNormal(maf_event);
	}
}

void medViewArbitraryOrthoSlice::OnEventGizmoCrossYNormal( mafEventBase * maf_event )
{
	if (maf_event->GetSender() == m_GizmoYView->m_GizmoCrossTranslate)
	{
		OnEventGizmoCrossTranslateYNormal(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoYView->m_GizmoCrossRotate) // from rotation gizmo
	{
		OnEventGizmoCrossRotateYNormal(maf_event);
	}
}

void medViewArbitraryOrthoSlice::OnEventGizmoCrossXNormal( mafEventBase * maf_event )
{
	if (maf_event->GetSender() == m_GizmoXView->m_GizmoCrossTranslate)
	{
		OnEventGizmoCrossTranslateXNormal(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoXView->m_GizmoCrossRotate) // from rotation gizmo
	{
		OnEventGizmoCrossRotateXNormal(maf_event);
	}
}


void medViewArbitraryOrthoSlice::UpdateCameraZViewOnEventGizmoCrossRotateZNormal( mafEvent * event )
{
	mafMatrix *gizmoABSPose = m_GizmoZView->m_GizmoCrossRotate->GetAbsPose();

	double gizmoABSCenter[3];
	mafTransform::GetPosition(*gizmoABSPose , gizmoABSCenter);

	double oldZViewCameraFocalPoint[3];

	vtkCamera *zViewCamera = NULL;

	zViewCamera = m_ChildViewList[Z_VIEW]->GetRWI()->GetCamera();
	assert(zViewCamera);


	zViewCamera->GetFocalPoint(oldZViewCameraFocalPoint);

	double oldZViewCameraPosition[3];
	zViewCamera->GetPosition(oldZViewCameraPosition);

	double newZViewCameraPosition[3];
	double newZViewCameraFocalPoint[3];

	double GOminusFPVector[3];
	GOminusFPVector[0] = gizmoABSCenter[0] - oldZViewCameraFocalPoint[0];
	GOminusFPVector[1] = gizmoABSCenter[1] - oldZViewCameraFocalPoint[1];
	GOminusFPVector[2] = gizmoABSCenter[2] - oldZViewCameraFocalPoint[2];

	mafTransform::AddVectors(oldZViewCameraPosition, GOminusFPVector, newZViewCameraPosition);

	zViewCamera->SetFocalPoint(gizmoABSCenter);
	zViewCamera->SetPosition(newZViewCameraPosition);

	double zAngle = event->GetDouble();
	m_ChildViewList[Z_VIEW]->GetRWI()->GetCamera()->Roll(-zAngle);

}

void medViewArbitraryOrthoSlice::UpdateCameraYViewOnEventGizmoCrossRotateZNormal( mafEvent * event )
{
	mafMatrix *gizmoABSPose = m_GizmoYView->m_GizmoCrossRotate->GetAbsPose();

	double gizmoABSCenter[3];
	mafTransform::GetPosition(*gizmoABSPose , gizmoABSCenter);

	double oldYViewCameraFocalPoint[3];

	vtkCamera *yViewCamera = NULL;

	yViewCamera = m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera();
	assert(yViewCamera);

	yViewCamera->GetFocalPoint(oldYViewCameraFocalPoint);

	double oldYViewCameraPosition[3];
	yViewCamera->GetPosition(oldYViewCameraPosition);

	double newYViewCameraPosition[3];
	double newYViewCameraFocalPoint[3];

	double GOminusFPVector[3];
	GOminusFPVector[0] = gizmoABSCenter[0] - oldYViewCameraFocalPoint[0];
	GOminusFPVector[1] = gizmoABSCenter[1] - oldYViewCameraFocalPoint[1];
	GOminusFPVector[2] = gizmoABSCenter[2] - oldYViewCameraFocalPoint[2];

	mafTransform::AddVectors(oldYViewCameraPosition, GOminusFPVector, newYViewCameraPosition);

	yViewCamera->SetFocalPoint(gizmoABSCenter);
	yViewCamera->SetPosition(newYViewCameraPosition);

	double yAngle = event->GetDouble();
	m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera()->Roll(yAngle);
}

void medViewArbitraryOrthoSlice::UpdateCameraXViewOnEventGizmoCrossRotateZNormal( mafEvent * event )
{
	mafMatrix *gizmoABSPose = m_GizmoZView->m_GizmoCrossRotate->GetAbsPose();

	double gizmoABSCenter[3];
	mafTransform::GetPosition(*gizmoABSPose , gizmoABSCenter);

	double oldXViewCameraFocalPoint[3];

	vtkCamera *xViewCamera = NULL;

	xViewCamera = m_ChildViewList[X_VIEW]->GetRWI()->GetCamera();
	assert(xViewCamera);

	xViewCamera->GetFocalPoint(oldXViewCameraFocalPoint);

	double oldXViewCameraPosition[3];
	xViewCamera->GetPosition(oldXViewCameraPosition);


	double newXViewCameraPosition[3];
	double newXViewCameraFocalPoint[3];

	double GOminusFPVector[3];
	GOminusFPVector[0] = gizmoABSCenter[0] - oldXViewCameraFocalPoint[0];
	GOminusFPVector[1] = gizmoABSCenter[1] - oldXViewCameraFocalPoint[1];
	GOminusFPVector[2] = gizmoABSCenter[2] - oldXViewCameraFocalPoint[2];

	mafTransform::AddVectors(oldXViewCameraPosition, GOminusFPVector, newXViewCameraPosition);

	xViewCamera->SetFocalPoint(gizmoABSCenter);
	xViewCamera->SetPosition(newXViewCameraPosition);

	double xAngle = event->GetDouble();
	m_ChildViewList[X_VIEW]->GetRWI()->GetCamera()->Roll(xAngle);
}

void medViewArbitraryOrthoSlice::PostMultiplyEventMatrixToGizmoCross( mafEventBase * inputEvent , medGizmoCrossRotateTranslate *targetGizmo )
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

void medViewArbitraryOrthoSlice::ChildViewsCameraUpdate()
{
	UpdateCutPlanes();

	for(int i=0; i<m_NumOfChildView; i++)
	{
		m_ChildViewList[i]->CameraUpdate();
	}
}

void medViewArbitraryOrthoSlice::GetLeftRightLettersFromCamera( double viewUp[3], double viewPlaneNormal[3], wxString &leftLetter, wxString &rightLetter)
{
	std::ostringstream stringStream;

	double cross[3] = {0,0,0};
	vtkMath::Cross(viewUp, viewPlaneNormal, cross);
	stringStream << "cross: " << cross[0] << " " << cross[1] << " " << cross[2] << std::endl; 

	double xn[3] = {1,0,0};
	double yn[3] = {0,1,0};
	double zn[3] = {0,0,1};

	double xDot = vtkMath::Dot(cross, xn);
	double yDot = vtkMath::Dot(cross, yn);
	double zDot = vtkMath::Dot(cross, zn);

	double absXDot = abs(xDot);
	double absYDot = abs(yDot);
	double absZDot = abs(zDot);

	vector<double> v;
	v.push_back(absXDot);
	v.push_back(absYDot);
	v.push_back(absZDot);

	double max = *(max_element(v.begin(), v.end()));
	stringStream << "max value: " << max << std::endl;

	int maxIndex = -1;

	string leftRight;

	if (max == absXDot)
	{
		maxIndex = 0;
		if (xDot >= 0)
		{
			leftRight = "R--L";
			leftLetter = "R";
			rightLetter = "L";
		}
		else
		{
			leftRight = "L--R";
			leftLetter = "L";
			rightLetter = "R";
		}
	}
	else if (max == absYDot)
	{
		maxIndex = 1;
		if (yDot >= 0)
		{
			leftRight = "A--P";
			leftLetter = "A";
			rightLetter = "P";
		}
		else
		{
			leftRight = "P--A";
			leftLetter = "P";
			rightLetter = "A";
		}

	}
	else if (max == absZDot)
	{
		maxIndex = 2;
		if (zDot >= 0)
		{
			leftRight = "I--S";
			leftLetter = "I";
			rightLetter = "S";
		}
		else
		{
			leftRight = "S--I";
			leftLetter = "S";
			rightLetter = "I";
		}
	}

	stringStream << "max index: " << maxIndex << std::endl;
	stringStream << "labels: " << leftRight.c_str() << std::endl;


	mafLogMessage(stringStream.str().c_str());

}


void medViewArbitraryOrthoSlice::OnLayout()
{
	mafViewCompound::OnLayout();

	wxSize size = ((wxWindowBase*)((mafViewVTK *)(m_ChildViewList[Z_VIEW]))->GetRWI())->GetSize();

	OnLayoutInternal(size);

}

void medViewArbitraryOrthoSlice::OnLayoutInternal( wxSize &windowSize )
{
	int		defaultTextWidth		= 127;
	int		defaultTextShiftX		= 20;
	int		defaultTextFontSize	= 4;
	double	numCharsInWindowLength				= 1.0;

	int textFontSize				= 4;
	int textShiftXLeft					= 10;

	double textHeigth = 0.70 * windowSize.GetHeight();


	numCharsInWindowLength =  (double)(windowSize.GetWidth()) / (double)(defaultTextWidth);
	textFontSize = (int)(defaultTextFontSize * numCharsInWindowLength +0.5);

	if (textFontSize < defaultTextFontSize )
		textFontSize = defaultTextFontSize;

	double textShiftXRight = windowSize.GetWidth() - textFontSize -textShiftXLeft;

	m_TextMapperLeftXView->GetTextProperty()->SetFontSize(textFontSize);
	m_TextActorLeftXView->SetPosition(textShiftXLeft, textHeigth);

	m_TextMapperRightXView->GetTextProperty()->SetFontSize(textFontSize);
	m_TextActorRightXView->SetPosition(textShiftXRight, textHeigth);

	m_TextMapperLeftYView->GetTextProperty()->SetFontSize(textFontSize);
	m_TextActorLeftYView->SetPosition(textShiftXLeft, textHeigth);

	m_TextMapperRightYView->GetTextProperty()->SetFontSize(textFontSize);
	m_TextActorRightYView->SetPosition(textShiftXRight, textHeigth);

	m_TextMapperLeftZView->GetTextProperty()->SetFontSize(textFontSize);
	m_TextActorLeftZView->SetPosition(textShiftXLeft, textHeigth);

	m_TextMapperRightZView->GetTextProperty()->SetFontSize(textFontSize);
	m_TextActorRightZView->SetPosition(textShiftXRight, textHeigth);

}

void medViewArbitraryOrthoSlice::UpdateZView2DActors()
{
	vtkCamera *zViewCamera = NULL;
	zViewCamera = m_ChildViewList[Z_VIEW]->GetRWI()->GetCamera();
	assert(zViewCamera);

	double viewUp[3] = {0,0,0};
	zViewCamera->GetViewUp(viewUp);

	std::ostringstream stringStream;
	stringStream << "viewUp Z: " << viewUp[0] << " " << viewUp[1] << " " << viewUp[2] << std::endl;          

	double viewPlaneNormal[3] = {0,0,0};
	zViewCamera->GetViewPlaneNormal(viewPlaneNormal);

	stringStream << "viewPlaneNormal Z: " << viewPlaneNormal[0] << " " << viewPlaneNormal[1] << " " << viewPlaneNormal[2] << std::endl; 

	wxString leftLetter = "UNDEFINED";
	wxString rightLetter = "UNDEFINED";

	GetLeftRightLettersFromCamera(viewUp, viewPlaneNormal, leftLetter, rightLetter);

	stringStream << leftLetter.c_str() << " " << rightLetter.c_str() << std::endl;

	mafLogMessage(stringStream.str().c_str());

	m_TextMapperLeftZView->SetInput(leftLetter.c_str());
	m_TextMapperRightZView->SetInput(rightLetter.c_str());
}

void medViewArbitraryOrthoSlice::UpdateXView2DActors()
{
	vtkCamera *xViewCamera = NULL;
	xViewCamera = m_ChildViewList[X_VIEW]->GetRWI()->GetCamera();
	assert(xViewCamera);

	double viewUp[3] = {0,0,0};
	xViewCamera->GetViewUp(viewUp);

	std::ostringstream stringStream;
	stringStream << "viewUp X: " << viewUp[0] << " " << viewUp[1] << " " << viewUp[2] << std::endl;          

	double viewPlaneNormal[3] = {0,0,0};
	xViewCamera->GetViewPlaneNormal(viewPlaneNormal);

	stringStream << "viewPlaneNormal X: " << viewPlaneNormal[0] << " " << viewPlaneNormal[1] << " " << viewPlaneNormal[2] << std::endl; 

	wxString leftLetter = "UNDEFINED";
	wxString rightLetter = "UNDEFINED";

	GetLeftRightLettersFromCamera(viewUp, viewPlaneNormal, leftLetter, rightLetter);

	stringStream << leftLetter.c_str() << " " << rightLetter.c_str() << std::endl;

	m_TextMapperLeftXView->SetInput(leftLetter.c_str());
	m_TextMapperRightXView->SetInput(rightLetter.c_str());
}

void medViewArbitraryOrthoSlice::UpdateYView2DActors()
{
	vtkCamera *yViewCamera = NULL;
	yViewCamera = m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera();
	assert(yViewCamera);

	double viewUp[3] = {0,0,0};
	yViewCamera->GetViewUp(viewUp);

	std::ostringstream stringStream;
	stringStream << "viewUp Y: " << viewUp[0] << " " << viewUp[1] << " " << viewUp[2] << std::endl;          

	double viewPlaneNormal[3] = {0,0,0};
	yViewCamera->GetViewPlaneNormal(viewPlaneNormal);

	stringStream << "viewPlaneNormal Y: " << viewPlaneNormal[0] << " " << viewPlaneNormal[1] << " " << viewPlaneNormal[2] << std::endl; 

	wxString leftLetter = "UNDEFINED";
	wxString rightLetter = "UNDEFINED";

	GetLeftRightLettersFromCamera(viewUp, viewPlaneNormal, leftLetter, rightLetter);

	stringStream << leftLetter.c_str() << " " << rightLetter.c_str() << std::endl;

	mafLogMessage(stringStream.str().c_str());

	m_TextMapperLeftYView->SetInput(leftLetter.c_str());
	m_TextMapperRightYView->SetInput(rightLetter.c_str());

}

void medViewArbitraryOrthoSlice::Update2DActors()
{
	UpdateXView2DActors();
	UpdateYView2DActors();
	UpdateZView2DActors();
}

void medViewArbitraryOrthoSlice::CameraUpdate()
{
	Superclass::CameraUpdate();
}

void medViewArbitraryOrthoSlice::ResetCameraToSlices()
{
	assert(m_ChildViewList[X_VIEW]);
	assert(m_ChildViewList[Y_VIEW]);
	assert(m_ChildViewList[Z_VIEW]);

	((mafViewVTK*)m_ChildViewList[Y_VIEW])->CameraReset(m_SlicerY);
	((mafViewVTK*)m_ChildViewList[X_VIEW])->CameraReset(m_SlicerX);
	((mafViewVTK*)m_ChildViewList[Z_VIEW])->CameraReset(m_SlicerZ);
}

void medViewArbitraryOrthoSlice::StoreCameraParametersForAllSubviews()
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

void medViewArbitraryOrthoSlice::RestoreCameraParametersForAllSubviews()
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

void medViewArbitraryOrthoSlice::Accumulate3TexturePlayGround()
{
	vtkMatrix4x4 *slicerZAbsMatrix = m_SlicerZ->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();
	assert(slicerZAbsMatrix);

	// 	if (true)
	// 	{
	// 		std::ostringstream stringStream;
	// 		stringStream << "SlicerZ abs matrix: " << std::endl;          
	// 		slicerZAbsMatrix->PrintSelf(stringStream, NULL);
	// 		mafLogMessage(stringStream.str().c_str());
	// 	}

	mafVMEOutputSurface *outputSurface = mafVMEOutputSurface::SafeDownCast(m_SlicerZ->GetSurfaceOutput());
	assert(outputSurface);

	vtkImageData *texture = outputSurface->GetMaterial()->GetMaterialTexture();
	assert(texture);

	// store the original texture on disk
	mafString root = "c:/Devel/TestData/exportTextures/";
	mafString textureOriginalFileName = root;
	textureOriginalFileName.Append("textureOriginal.vtk");

	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();

	vtkMAFSmartPointer<vtkDataSetWriter> dsw;
	dsw->SetInput(texture);
	dsw->SetFileName(textureOriginalFileName.GetCStr());
	// dsw->Write();

	// sum the three texture sclars in a new image
	vtkMAFSmartPointer<vtkImageData> scalarsAccumulationTargetTexture;
	scalarsAccumulationTargetTexture->DeepCopy(texture);	

	// copy the middle image to get original scalars
	vtkMAFSmartPointer<vtkTransform> originalABSTransform;
	originalABSTransform->SetMatrix(slicerZAbsMatrix);

	/// texture up
	double zPlusHeight = 10;
	vtkMAFSmartPointer<vtkTransform> textureUpTr;
	textureUpTr->PostMultiply();
	textureUpTr->Concatenate(originalABSTransform);
	textureUpTr->Translate(0,0,zPlusHeight);

	mafMatrix textureUpMafMatrix;
	textureUpMafMatrix.SetVTKMatrix(textureUpTr->GetMatrix());
	m_SlicerZ->SetAbsMatrix(textureUpMafMatrix);

	mafString textureUpFileName = root;
	textureUpFileName.Append("textureUp.vtk");

	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();

	dsw->SetInput(texture);
	dsw->SetFileName(textureUpFileName.GetCStr());
	// dsw->Write();

	// add the texture up scalars
	vtkUnsignedShortArray *targetScalars = vtkUnsignedShortArray::SafeDownCast( scalarsAccumulationTargetTexture->GetPointData()->GetScalars());
	assert(targetScalars);

	assert(targetScalars->GetNumberOfComponents() == 1);

	vtkUnsignedShortArray *upScalars = vtkUnsignedShortArray::SafeDownCast(texture->GetPointData()->GetScalars());

	for (int scalarId = 0; scalarId < targetScalars->GetNumberOfTuples(); scalarId++)
	{
		unsigned short oldValue = targetScalars->GetValue(scalarId);
		unsigned short valueToAdd = upScalars->GetValue(scalarId);
		unsigned short newValue = (oldValue + valueToAdd) / 2;
		targetScalars->SetValue(scalarId, newValue);
	}

	// write middle + upper accumulation on disk
	mafString textureAccumulatorFileName = root;
	textureAccumulatorFileName.Append("textureAccumulator.vtk");

	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();

	dsw->SetInput(scalarsAccumulationTargetTexture);
	dsw->SetFileName(textureAccumulatorFileName.GetCStr());
	// dsw->Write();

	// texture down
	vtkMAFSmartPointer<vtkTransform> textureDownTransform;
	textureDownTransform->PostMultiply();
	textureDownTransform->Concatenate(originalABSTransform);
	textureDownTransform->Translate(0,0,-5);

	mafMatrix textureDownMafMatrix;
	textureDownMafMatrix.SetVTKMatrix(textureDownTransform->GetMatrix());
	m_SlicerZ->SetAbsMatrix(textureDownMafMatrix);

	mafString textureDownFileName = root;
	textureDownFileName.Append("textureDown.vtk");

	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();

	dsw->SetInput(texture);
	dsw->SetFileName(textureDownFileName.GetCStr());
	// dsw->Write();

	// add the texture down scalars
	vtkUnsignedShortArray *downScalars = vtkUnsignedShortArray::SafeDownCast(texture->GetPointData()->GetScalars());

	for (int scalarId = 0; scalarId < targetScalars->GetNumberOfTuples(); scalarId++)
	{
		unsigned short oldValue = targetScalars->GetValue(scalarId);
		unsigned short valueToAdd = downScalars->GetValue(scalarId);
		unsigned short newValue = (oldValue + valueToAdd) / 2;
		targetScalars->SetValue(scalarId, newValue);
	}

	// original texture 
	m_SlicerZ->SetAbsMatrix(originalABSTransform->GetMatrix());

	mafString inTheMiddleFileName = root;
	inTheMiddleFileName.Append("inTheMiddle.vtk");

	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
	m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();

	dsw->SetInput(texture);
	dsw->SetFileName(inTheMiddleFileName.GetCStr());
	// dsw->Write();

	// test set accumulated image to slicer 
	texture->DeepCopy(scalarsAccumulationTargetTexture);

}

void medViewArbitraryOrthoSlice::AccumulateNTextureFromThickness(mafVMESlicer *inputSlicer)
{	
	// prevent wasted cpu time :P
	if (m_EnableThickness == 0)
	{
		return;
	}

	int direction = -1;

	if (inputSlicer == m_SlicerX)
	{
		direction = X;
	}
	else if (inputSlicer == m_SlicerY)
	{
		direction = Y;
	}
	else if (inputSlicer == m_SlicerZ)
	{
		direction = Z;
	}

	assert(m_InputVolume);
	assert(m_InputVolume->IsA("mafVMEVolumeGray"));

	vtkStructuredPoints *structuredPoints = vtkStructuredPoints::SafeDownCast(m_InputVolume->GetDataPipe()->GetVTKData());

	// BEWARE: working for structured points only
	assert(structuredPoints);

	double spacing[3];

	structuredPoints->GetSpacing(spacing);

	double minSpacingXY = min(spacing[0], spacing[1]);
	double minSpacingYZ = min(spacing[1], spacing[2]);
	double minSpacing = min(minSpacingXY, minSpacingYZ);

	double profileDistance = minSpacing;
	int additionalProfileNumber = m_Thickness / profileDistance + 1;
	additionalProfileNumber  /= 2;

	std::ostringstream stringStream;
	stringStream << "additional profiles number: " << additionalProfileNumber << std::endl;          
	mafLogMessage(stringStream.str().c_str());

	vtkMatrix4x4 *slicerAbsMatrix = inputSlicer->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();
	assert(slicerAbsMatrix);

	// copy the middle image to get original scalars
	vtkMAFSmartPointer<vtkTransform> originalABSTransform;
	originalABSTransform->SetMatrix(slicerAbsMatrix);

	mafVMEOutputSurface *outputSurface = mafVMEOutputSurface::SafeDownCast(inputSlicer->GetSurfaceOutput());
	assert(outputSurface);

	vtkImageData *texture = outputSurface->GetMaterial()->GetMaterialTexture();
	assert(texture);

	inputSlicer->GetSurfaceOutput()->GetVTKData()->Modified();
	inputSlicer->GetSurfaceOutput()->GetVTKData()->Update();

	// sum the texture scalars in a new image: built from original default texture
	vtkMAFSmartPointer<vtkImageData> scalarsAccumulationTargetTexture;
	scalarsAccumulationTargetTexture->DeepCopy(texture);	

	// add the texture up scalars
	vtkUnsignedShortArray *targetScalars = vtkUnsignedShortArray::SafeDownCast( scalarsAccumulationTargetTexture->GetPointData()->GetScalars());
	assert(targetScalars);

	assert(targetScalars->GetNumberOfComponents() == 1);

	// debug with three profiles: -20 , 0 , 20
	// additionalProfileNumber = 1;
	// profileDistance = 20;

	// for each profile
	for(int profileId = -additionalProfileNumber; profileId <= additionalProfileNumber; profileId++)
	{
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

		inputSlicer->SetAbsMatrix(currentProfileMafMatrix);

		inputSlicer->GetSurfaceOutput()->GetVTKData()->Modified();
		inputSlicer->GetSurfaceOutput()->GetVTKData()->Update();

		if (false)
		{
			std::ostringstream stringStream;
			stringStream << "Slicer number: " << profileId  << " matrix" << std::endl;          
			inputSlicer->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix()->PrintSelf(stringStream, NULL);
			mafLogMessage(stringStream.str().c_str());
		}
		// get the current slice profile texture


		vtkImageData *currentTexture = inputSlicer->GetSurfaceOutput()->GetMaterial()->GetMaterialTexture();
		assert(currentTexture);

		vtkUnsignedShortArray *currentSliceScalars = vtkUnsignedShortArray::SafeDownCast(currentTexture->GetPointData()->GetScalars());

		// add the scalars to the target texture
		for (int scalarId = 0; scalarId < targetScalars->GetNumberOfTuples(); scalarId++)
		{
			unsigned short oldValue = targetScalars->GetValue(scalarId);
			unsigned short valueToAdd = currentSliceScalars->GetValue(scalarId);
			unsigned short newValue = (oldValue + valueToAdd) / 2;
			targetScalars->SetValue(scalarId, newValue);
		}
	}

	// original texture 
	inputSlicer->SetAbsMatrix(originalABSTransform->GetMatrix());

	inputSlicer->GetSurfaceOutput()->GetVTKData()->Modified();
	inputSlicer->GetSurfaceOutput()->GetVTKData()->Update(); 

	// test set accumulated image to slicer 
	texture->DeepCopy(scalarsAccumulationTargetTexture);

}

void medViewArbitraryOrthoSlice::ShowPlaneFeedbackLine( int fromDirection , 
													   vtkMatrix4x4 *outputMatrix)
{	

}

void medViewArbitraryOrthoSlice::ShowVTKDataAsVMESurface( vtkPolyData *vmeVTKData, mafVMESurface *vmeSurface, vtkMatrix4x4 *inputABSMatrix )
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

void medViewArbitraryOrthoSlice::BuildSlicingPlane(mafVMESurface *inVME, 
												   int fromDirection, int guestView, double sliceHeight, mafVMESlicer * outputSlicer,
												   vtkMatrix4x4 * outputMatrix )
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

	assert(m_ViewXnSliceYpVME);

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

		vtkTubeFilter *tubeFilter = vtkTubeFilter::New();
		tubeFilter->SetInput(lineSource->GetOutput());
		tubeFilter->SetRadius(0.3); // to be adapted to input vme
		tubeFilter->SetNumberOfSides(10);
		tubeFilter->Update();

		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

		ShowVTKDataAsVMESurface(tubeFilter->GetOutput(), inVME , matrix);

		tubeFilter->Delete();
		matrix->Delete();
		lineSource->Delete();
	}

	plane1TPDF->Delete();
	boundsCutter->Delete();
	plane2->Delete();
}

void medViewArbitraryOrthoSlice::AddVMEToMSFTree(mafVMESurface *vme)
{
	assert(vme != NULL);
	vme->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	mafEventMacro(mafEvent(this, VME_ADD, vme));
	assert(vme);
}

////////////// Z cut planes

void medViewArbitraryOrthoSlice::UpdateYnViewZPlanes()
{
	BuildSlicingPlane(m_ViewYnSliceZpVME, 
		FROM_Z, Y_VIEW,  m_FeedbackLineHeight);

	BuildSlicingPlane(m_ViewYnSliceZmVME, 
		FROM_Z, Y_VIEW,  -m_FeedbackLineHeight);
}

void medViewArbitraryOrthoSlice::UpdateXnViewZPlanes()
{
	BuildSlicingPlane(m_ViewXnSliceZpVME, 
		FROM_Z, X_VIEW,  m_FeedbackLineHeight);

	BuildSlicingPlane(m_ViewXnSliceZmVME, 
		FROM_Z, X_VIEW,  -m_FeedbackLineHeight);
}

void medViewArbitraryOrthoSlice::UpdateZCutPlanes()
{
	// build Yn view images export gizmos
	if (m_ViewYnSliceZpVME && m_ViewYnSliceZmVME)
	{
		UpdateYnViewZPlanes();
		UpdateXnViewZPlanes();
	}
}

void medViewArbitraryOrthoSlice::ShowZCutPlanes( bool show )
{
	m_ChildViewList[X_VIEW]->VmeShow(m_ViewXnSliceZpVME, show);
	m_ChildViewList[X_VIEW]->VmeShow(m_ViewXnSliceZmVME, show);
	m_ChildViewList[Y_VIEW]->VmeShow(m_ViewYnSliceZpVME, show);
	m_ChildViewList[Y_VIEW]->VmeShow(m_ViewYnSliceZmVME, show);

}

////////////// X cut planes

void medViewArbitraryOrthoSlice::UpdateYnViewXPlanes()
{
	BuildSlicingPlane(m_ViewYnSliceXpVME, 
		FROM_X, Y_VIEW,  m_FeedbackLineHeight);

	BuildSlicingPlane(m_ViewYnSliceXmVME, 
		FROM_X, Y_VIEW,  -m_FeedbackLineHeight);
}

void medViewArbitraryOrthoSlice::UpdateZnViewXPlanes()
{
	BuildSlicingPlane(m_ViewZnSliceXpVME, 
		FROM_X, Z_VIEW,  m_FeedbackLineHeight);

	BuildSlicingPlane(m_ViewZnSliceXmVME, 
		FROM_X, Z_VIEW,  -m_FeedbackLineHeight);
}

void medViewArbitraryOrthoSlice::UpdateXCutPlanes()
{
	// build Yn view images export gizmos
	if (m_ViewYnSliceXpVME && m_ViewZnSliceXmVME)
	{
		UpdateYnViewXPlanes();
		UpdateZnViewXPlanes();
	}
}

void medViewArbitraryOrthoSlice::ShowXCutPlanes( bool show )
{
	m_ChildViewList[Y_VIEW]->VmeShow(m_ViewYnSliceXpVME, show);
	m_ChildViewList[Y_VIEW]->VmeShow(m_ViewYnSliceXmVME, show);
	m_ChildViewList[Z_VIEW]->VmeShow(m_ViewZnSliceXpVME, show);
	m_ChildViewList[Z_VIEW]->VmeShow(m_ViewZnSliceXmVME, show);
}

////////////// Y cut planes

void medViewArbitraryOrthoSlice::UpdateXnViewYPlanes()
{
	BuildSlicingPlane(m_ViewXnSliceYpVME, 
		FROM_Y, X_VIEW,  m_FeedbackLineHeight);

	BuildSlicingPlane(m_ViewXnSliceYmVME, 
		FROM_Y, X_VIEW,  -m_FeedbackLineHeight);
}

void medViewArbitraryOrthoSlice::UpdateZnViewYPlanes()
{
	BuildSlicingPlane(m_ViewZnSliceYpVME, 
		FROM_Y, Z_VIEW,  m_FeedbackLineHeight);

	BuildSlicingPlane(m_ViewZnSliceYmVME, 
		FROM_Y, Z_VIEW,  -m_FeedbackLineHeight);
}

void medViewArbitraryOrthoSlice::UpdateYCutPlanes()
{
	// build Yn view images export gizmos
	if (m_ViewZnSliceYpVME && m_ViewZnSliceYmVME)
	{
		UpdateZnViewYPlanes();
		UpdateXnViewYPlanes();
	}
}

void medViewArbitraryOrthoSlice::ShowYCutPlanes( bool show )
{
	m_ChildViewList[X_VIEW]->VmeShow(m_ViewXnSliceYpVME, show);
	m_ChildViewList[X_VIEW]->VmeShow(m_ViewXnSliceYmVME, show);
	m_ChildViewList[Z_VIEW]->VmeShow(m_ViewZnSliceYpVME, show);
	m_ChildViewList[Z_VIEW]->VmeShow(m_ViewZnSliceYmVME, show);
}

void medViewArbitraryOrthoSlice::BuildSliceHeightFeedbackLinesVMEs()
{
	// view Xn
	mafNEW(m_ViewXnSliceYpVME);
	AddVMEToMSFTree(m_ViewXnSliceYpVME);

	mafNEW(m_ViewXnSliceYmVME);
	AddVMEToMSFTree(m_ViewXnSliceYmVME);

	mafNEW(m_ViewXnSliceZpVME);
	AddVMEToMSFTree(m_ViewXnSliceZpVME);

	mafNEW(m_ViewXnSliceZmVME);
	AddVMEToMSFTree(m_ViewXnSliceZmVME);

	// view Yn
	mafNEW(m_ViewYnSliceZpVME);
	AddVMEToMSFTree(m_ViewYnSliceZpVME);

	mafNEW(m_ViewYnSliceZmVME);
	AddVMEToMSFTree(m_ViewYnSliceZmVME);

	mafNEW(m_ViewYnSliceXpVME);
	AddVMEToMSFTree(m_ViewYnSliceXpVME);

	mafNEW(m_ViewYnSliceXmVME);
	AddVMEToMSFTree(m_ViewYnSliceXmVME);

	// view Zn
	mafNEW(m_ViewZnSliceXpVME);
	AddVMEToMSFTree(m_ViewZnSliceXpVME);

	mafNEW(m_ViewZnSliceXmVME);
	AddVMEToMSFTree(m_ViewZnSliceXmVME);

	mafNEW(m_ViewZnSliceYpVME);
	AddVMEToMSFTree(m_ViewZnSliceYpVME);

	mafNEW(m_ViewZnSliceYmVME);
	AddVMEToMSFTree(m_ViewZnSliceYmVME);
}

void medViewArbitraryOrthoSlice::UpdateCutPlanes()
{
	if (!m_EnableThickness && !m_EnableExportImages)
	{
		return;
	}

	UpdateXCutPlanes();
	UpdateYCutPlanes();
	UpdateZCutPlanes();

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));  
}

void medViewArbitraryOrthoSlice::OnEventID_COMBO_CHOOSE_CUTTING_AXIS()
{
	UpdateCutPlanes();

	if (m_EnableExportImages)
	{
		HideCutPlanes();  
		if (m_ComboChooseAxisDirection == X)
		{
			ShowXCutPlanes(true);
		}
		else if (m_ComboChooseAxisDirection == Y)
		{
			ShowYCutPlanes(true);
		}
		else if (m_ComboChooseAxisDirection == Z)
		{
			ShowZCutPlanes(true);
		}  

		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}

void medViewArbitraryOrthoSlice::HideCutPlanes()
{
	ShowXCutPlanes(false);
	ShowYCutPlanes(false);
	ShowZCutPlanes(false);

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));

}

void medViewArbitraryOrthoSlice::OnID_CHOOSE_DIR()
{
	wxDirDialog dialog(NULL);
	dialog.SetReturnCode(wxID_OK);
	int ret_code = dialog.ShowModal();
	m_PathFromDialog = dialog.GetPath();

}

void medViewArbitraryOrthoSlice::EnableExportImages( bool enable )
{
	m_Gui->Enable(ID_COMBO_CHOOSE_CUTTING_AXIS, enable);
	m_Gui->Enable(ID_CHOOSE_DIR, enable);
	m_Gui->Enable(ID_EXPORT_PLANES_HEIGHT, enable);
	m_Gui->Enable(ID_EXPORT, enable);
	m_Gui->Enable(ID_NUMBER_OF_AXIAL_SECTIONS, enable);	
}

void medViewArbitraryOrthoSlice::EnableThicknessGUI( bool enable )
{
	m_Gui->Enable(ID_COMBO_CHOOSE_THICKNESS_AXIS, enable);
	m_Gui->Enable(ID_THICKNESS, enable);
}

void medViewArbitraryOrthoSlice::OnEventID_ENABLE_EXPORT_IMAGES()
{
	EnableExportImages(m_EnableExportImages);
	OnEventID_COMBO_CHOOSE_CUTTING_AXIS();

	if (m_EnableExportImages == 0)  // disable export images
	{
		HideCutPlanes();
	}
	else // enable
	{
		m_FeedbackLineHeight = m_ExportPlanesHeight;
		UpdateCutPlanes();
	}

}

void medViewArbitraryOrthoSlice::OnEventID_EXPORT()
{
	if (m_ComboChooseAxisDirection == X)
	{
		// export X slices BMP
		double thickness = m_FeedbackLineHeight * 2;
		double step = thickness / (m_NumberOfAxialSections - 1);

		vtkMAFSmartPointer<vtkMatrix4x4> originalMatrix;

		vtkMatrix4x4 *slicerMatrix = m_SlicerX->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();
		originalMatrix->DeepCopy(slicerMatrix);

		double height = -m_FeedbackLineHeight;

		for (int i = 0; i < m_NumberOfAxialSections; i++)
		{          
			vtkMAFSmartPointer<vtkTransform> tr;
			tr->PostMultiply();
			tr->SetMatrix(originalMatrix);
			tr->Translate(height,0,0);
			tr->Update();

			m_SlicerX->SetAbsMatrix(tr->GetMatrix());

			m_SlicerX->GetSurfaceOutput()->GetVTKData()->Modified();
			m_SlicerX->GetSurfaceOutput()->GetVTKData()->Update();

			mafVMEOutputSurface *outputSurface = mafVMEOutputSurface::SafeDownCast(m_SlicerX->GetSurfaceOutput());
			assert(outputSurface);

			vtkImageData *texture = outputSurface->GetMaterial()->GetMaterialTexture();
			assert(texture);


			vtkMAFSmartPointer<vtkPNGWriter> bmpWriter;
			bmpWriter->SetInput(texture);

			wxString fileName = m_PathFromDialog.c_str();
			fileName.append("/SlicerX_");
			fileName << i;
			fileName << ".png";
			//fileName.append("vtk");

			bmpWriter->SetFileName(fileName);
			bmpWriter->Write();

			height = height + step;
		}

		m_SlicerX->SetAbsMatrix(mafMatrix(originalMatrix));

		m_SlicerX->GetSurfaceOutput()->GetVTKData()->Modified();
		m_SlicerX->GetSurfaceOutput()->GetVTKData()->Update();

	}
	else if (m_ComboChooseAxisDirection == Y)
	{
		// export Y slices BMP
		double thickness = m_FeedbackLineHeight * 2;
		double step = thickness / (m_NumberOfAxialSections - 1);

		vtkMAFSmartPointer<vtkMatrix4x4> originalMatrix;

		vtkMatrix4x4 *slicerMatrix = m_SlicerY->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();
		originalMatrix->DeepCopy(slicerMatrix);

		double height = -m_FeedbackLineHeight;

		for (int i = 0; i < m_NumberOfAxialSections; i++)
		{          
			vtkMAFSmartPointer<vtkTransform> tr;
			tr->PostMultiply();
			tr->SetMatrix(originalMatrix);
			tr->Translate(0,height,0);
			tr->Update();

			m_SlicerY->SetAbsMatrix(tr->GetMatrix());

			m_SlicerY->GetSurfaceOutput()->GetVTKData()->Modified();
			m_SlicerY->GetSurfaceOutput()->GetVTKData()->Update();

			mafVMEOutputSurface *outputSurface = mafVMEOutputSurface::SafeDownCast(m_SlicerY->GetSurfaceOutput());
			assert(outputSurface);

			vtkImageData *texture = outputSurface->GetMaterial()->GetMaterialTexture();
			assert(texture);


			vtkMAFSmartPointer<vtkPNGWriter> bmpWriter;
			bmpWriter->SetInput(texture);

			wxString fileName = m_PathFromDialog.c_str();
			fileName.append("/SlicerY_");
			fileName << i;
			fileName << ".png";
			//fileName.append("vtk");

			bmpWriter->SetFileName(fileName);
			bmpWriter->Write();

			height = height + step;
		}

		m_SlicerY->SetAbsMatrix(mafMatrix(originalMatrix));

		m_SlicerY->GetSurfaceOutput()->GetVTKData()->Modified();
		m_SlicerY->GetSurfaceOutput()->GetVTKData()->Update();

	}
	else if (m_ComboChooseAxisDirection == Z)
	{
		// export Z slices BMP
		double thickness = m_FeedbackLineHeight * 2;
		double step = thickness / (m_NumberOfAxialSections - 1);

		vtkMAFSmartPointer<vtkMatrix4x4> originalMatrix;

		vtkMatrix4x4 *slicerMatrix = m_SlicerZ->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();
		originalMatrix->DeepCopy(slicerMatrix);

		double height = -m_FeedbackLineHeight;

		for (int i = 0; i < m_NumberOfAxialSections; i++)
		{          
			vtkMAFSmartPointer<vtkTransform> tr;
			tr->PostMultiply();
			tr->SetMatrix(originalMatrix);
			tr->Translate(0,0,height);
			tr->Update();

			m_SlicerZ->SetAbsMatrix(tr->GetMatrix());

			m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
			m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();

			mafVMEOutputSurface *outputSurface = mafVMEOutputSurface::SafeDownCast(m_SlicerZ->GetSurfaceOutput());
			assert(outputSurface);

			vtkImageData *texture = outputSurface->GetMaterial()->GetMaterialTexture();
			assert(texture);

			vtkMAFSmartPointer<vtkPNGWriter> bmpWriter;
			bmpWriter->SetInput(texture);

			wxString fileName = m_PathFromDialog.c_str();
			fileName.append("/SlicerZ_");
			fileName << i;
			fileName << ".png";

			bmpWriter->SetFileName(fileName);
			bmpWriter->Write();

			height = height + step;
		}

		m_SlicerZ->SetAbsMatrix(mafMatrix(originalMatrix));

		m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Modified();
		m_SlicerZ->GetSurfaceOutput()->GetVTKData()->Update();
	}
}

void medViewArbitraryOrthoSlice::OnEventID_ENABLE_THICKNESS()
{
	EnableThickness(m_EnableThickness);
}

void medViewArbitraryOrthoSlice::OnEventID_COMBO_THICKNESS_AXIS()
{
	UpdateCutPlanes();
	UpdateThicknessStuff();
	if (m_EnableThickness)
	{
		HideCutPlanes();  
		if (m_ComboThicknessAxis == X)
		{
			ShowXCutPlanes(true);
		}
		else if (m_ComboThicknessAxis == Y)
		{
			ShowYCutPlanes(true);
		}
		else if (m_ComboThicknessAxis == Z)
		{
			ShowZCutPlanes(true);
		}  

		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}

}

void medViewArbitraryOrthoSlice::UpdateThicknessStuff()
{
	if (m_EnableThickness) // slicer X
	{
		UpdateSlicers();

		if (m_ComboThicknessAxis == X)
		{
			AccumulateNTextureFromThickness(m_SlicerX);
		}
		else if (m_ComboThicknessAxis == Y)
		{
			AccumulateNTextureFromThickness(m_SlicerY);
		}
		else if (m_ComboThicknessAxis == Z)
		{
			AccumulateNTextureFromThickness(m_SlicerZ);
		}
	}
	else
	{
		UpdateSlicers();
	}
}


void medViewArbitraryOrthoSlice::UpdateSlicers()
{
	const int numSlicers = 3;
	mafVMESlicer *slicers[numSlicers] = {m_SlicerX, m_SlicerY, m_SlicerZ};

	for (int i = 0; i < numSlicers; i++)
	{
		slicers[i]->SetAbsMatrix(slicers[i]->GetAbsMatrixPipe()->GetMatrix());
		slicers[i]->GetSurfaceOutput()->GetVTKData()->Modified();
		slicers[i]->GetSurfaceOutput()->GetVTKData()->Update();
	}
}

void medViewArbitraryOrthoSlice::OnEventID_THICKNESS()
{
	m_FeedbackLineHeight = m_Thickness;
	UpdateCutPlanes();
	UpdateThicknessStuff();
	ChildViewsCameraUpdate();
}

void medViewArbitraryOrthoSlice::EnableThickness(bool enable)
{
	EnableThicknessGUI(enable);

	if (enable)
	{
		m_FeedbackLineHeight = m_Thickness;
		OnEventID_COMBO_THICKNESS_AXIS();
	}
	else
	{
		HideCutPlanes();
		UpdateThicknessStuff();
		ChildViewsCameraUpdate();
	}
}

void medViewArbitraryOrthoSlice::OnEventID_EXPORT_PLANES_HEIGHT()
{
	m_FeedbackLineHeight = m_ExportPlanesHeight;
	UpdateCutPlanes();
	ChildViewsCameraUpdate();
}
