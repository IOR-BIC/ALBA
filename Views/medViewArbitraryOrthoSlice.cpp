/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewArbitraryOrthoSlice.cpp,v $
Language:  C++
Date:      $Date: 2010-12-15 10:32:08 $
Version:   $Revision: 1.1.2.29 $
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
#include <algorithm>

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
					//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}


			UpdateXView2DActors();
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
	//m_Gui->Bool(ID_SHOW_GIZMO, "",&m_ShowGizmo);

	// m_Gui->Divider(2);

	//combo box to choose the type of gizmo
	//m_Gui->Label("choose gizmo", true);
	// 	wxString Text[2]={_("translation gizmo"),_("rotation gizmo")};
	// 	m_Gui->Combo(ID_COMBO_CHOOSE_ACTIVE_GIZMO,"",&m_ComboChooseActiveGizmo,2,Text);


	m_Gui->Divider();


	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	m_Gui->Divider();

	m_Gui->Update();

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
		m_Gui->Enable(ID_COMBO_CHOOSE_ACTIVE_GIZMO, enable);
		m_Gui->Enable(ID_LUT_CHOOSER, enable);
		m_Gui->Enable(ID_SHOW_GIZMO, enable);
		m_Gui->FitGui();
		m_Gui->Update();
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
		//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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

	mafMatrix mat;

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

	vtkConeSource *coneSource = vtkConeSource::New();
	coneSource->SetCenter(0,0,d/2);
	coneSource->SetResolution(20);
	coneSource->SetDirection(0,0,-1);

	coneSource->SetRadius(d / 8);
	coneSource->SetHeight(d / 8);

	coneSource->CappingOn();
	coneSource->Update();

	mafNEW(m_XCameraConeVME);
	// DEBUG
	m_XCameraConeVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_XCameraConeVME->SetName("Cone X Camera");
	m_XCameraConeVME->SetData(coneSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_XCameraConeVME->SetVisibleToTraverse(false);

	m_XCameraConeVME->GetMaterial()->m_Prop->SetColor(1,0,0);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetAmbient(1);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetDiffuse(0);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetSpecular(0);
	m_XCameraConeVME->GetMaterial()->m_Prop->SetOpacity(0.3);

	assert(m_SlicerX);
	m_XCameraConeVME->ReparentTo(m_SlicerX);

	// DEBUG
	//m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_XCameraConeVME, true);
	m_ChildViewList[Y_VIEW]->VmeShow(m_XCameraConeVME, true);
	m_ChildViewList[Z_VIEW]->VmeShow(m_XCameraConeVME, true);
	//m_ChildViewList[Z_VIEW]->VmeShow(m_ConeVME, true);

	//mafPipeSurface *pipe=(mafPipeSurface *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_XCameraConeVME);
	//pipe->SetActorPicking(false);

	mafPipeSurface *pipeY=(mafPipeSurface *)(m_ChildViewList[Y_VIEW])->GetNodePipe(m_XCameraConeVME);
	pipeY->SetActorPicking(false);

	mafPipeSurface *pipeZ=(mafPipeSurface *)(m_ChildViewList[Z_VIEW])->GetNodePipe(m_XCameraConeVME);
	pipeZ->SetActorPicking(false);

	coneSource->Delete();
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

	vtkConeSource *coneSource = vtkConeSource::New();
	coneSource->SetCenter(0,0,d/2);
	coneSource->SetResolution(20);
	coneSource->SetDirection(0,0,-1);

	coneSource->SetRadius(d / 8);
	coneSource->SetHeight(d / 8);

	coneSource->CappingOn();
	coneSource->Update();

	mafNEW(m_YCameraConeVME);
	// DEBUG
	m_YCameraConeVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_YCameraConeVME->SetName("m_YCameraConeVME");
	m_YCameraConeVME->SetData(coneSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_YCameraConeVME->SetVisibleToTraverse(false);

	m_YCameraConeVME->GetMaterial()->m_Prop->SetColor(0,1,0);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetAmbient(1);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetDiffuse(0);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetSpecular(0);
	m_YCameraConeVME->GetMaterial()->m_Prop->SetOpacity(0.3);

	assert(m_SlicerY);
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

	coneSource->Delete();
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

	vtkConeSource *coneSource = vtkConeSource::New();
	coneSource->SetCenter(0,0,d/2);
	coneSource->SetResolution(20);
	coneSource->SetDirection(0,0,-1);

	coneSource->SetRadius(d / 8);
	coneSource->SetHeight(d / 8);

	coneSource->CappingOn();
	coneSource->Update();

	mafNEW(m_ZCameraConeVME);
	// DEBUG
	m_ZCameraConeVME->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_ZCameraConeVME->SetName("m_ZCameraConeVME");
	m_ZCameraConeVME->SetData(coneSource->GetOutput(), m_CurrentVolume->GetTimeStamp());
	m_ZCameraConeVME->SetVisibleToTraverse(false);

	m_ZCameraConeVME->GetMaterial()->m_Prop->SetColor(0,0,1);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetAmbient(1);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetDiffuse(0);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetSpecular(0);
	m_ZCameraConeVME->GetMaterial()->m_Prop->SetOpacity(0.3);

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

	coneSource->Delete();
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
	for(int i=0; i<m_NumOfChildView; i++)
	{
		m_ChildViewList[i]->CameraUpdate();
	}
}

void medViewArbitraryOrthoSlice::MyMethod( double viewUp[3], double viewPlaneNormal[3], wxString &leftLetter, wxString &rightLetter)
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

	MyMethod(viewUp, viewPlaneNormal, leftLetter, rightLetter);

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

	MyMethod(viewUp, viewPlaneNormal, leftLetter, rightLetter);

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

	MyMethod(viewUp, viewPlaneNormal, leftLetter, rightLetter);

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

