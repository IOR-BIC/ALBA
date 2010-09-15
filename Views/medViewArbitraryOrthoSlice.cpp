/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewArbitraryOrthoSlice.cpp,v $
Language:  C++
Date:      $Date: 2010-09-15 16:11:53 $
Version:   $Revision: 1.1.2.11 $
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
#include "medOpMatrixVectorMath.h"
#include "mafTransformFrame.h"
#include "mafVMEGizmo.h"

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

medViewArbitraryOrthoSlice::medViewArbitraryOrthoSlice(wxString label, bool show_ruler)
: medViewCompoundWindowing(label, 2, 2)

{
	m_ViewArbitrary   = NULL;

	m_ViewSliceX       = NULL;
	m_ViewSliceY       = NULL;
	m_ViewSliceZ       = NULL;

	m_GizmoCrossTranslateYNormal  = NULL;
	m_GizmoCrossRotateYNormal     = NULL;

	m_GizmoCrossTranslateYNormal  = NULL;
	m_GizmoCrossRotateYNormal     = NULL;

	m_GizmoCrossTranslateZNormal  = NULL;
	m_GizmoCrossRotateZNormal     = NULL;

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

	m_ComboChooseActiveGizmo = GIZMO_ROTATE;

	m_XCameraConeVME = NULL;
	m_YCameraConeVME = NULL;
	m_ZCameraConeVME = NULL;

	m_XCameraRollForReset = 0;
	m_YCameraRollForReset = 0;
	m_ZCameraRollForReset = 0;
}

medViewArbitraryOrthoSlice::~medViewArbitraryOrthoSlice()

{
	m_SlicerZResetMatrix   = NULL;
	m_CurrentVolume = NULL;
	m_CurrentImage  = NULL;
	m_ColorLUT      = NULL;

}

void medViewArbitraryOrthoSlice::PackageView()

{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE,true,false,false,0,false,mafAxes::CUBE);
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
	//	m_ViewArbitrary->PlugVisualPipe("mafVMELabeledVolume", "mafPipeBox", MUTEX);

	m_ViewArbitrary->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);

	m_ViewSliceX = new mafViewVTK("",CAMERA_OS_X,true,false,false,0,false,mafAxes::CUBE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);

	m_ViewSliceX->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceX->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	m_ViewSliceY = new mafViewVTK("",CAMERA_OS_Y,true,false,false,0,false,mafAxes::CUBE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);
	//m_ViewSliceY->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo");
	m_ViewSliceY->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
	// 	m_ViewSliceY->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	m_ViewSliceZ = new mafViewVTK("",CAMERA_OS_Z,true,false,false,0,false,mafAxes::CUBE);
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
		if (IsZNormalGizmoComponent(vme))
		{
			m_ChildViewList[Z_VIEW]->VmeShow(node, show);
		}
		else if (IsXNormalGizmoComponent(vme))
		{
			m_ChildViewList[X_VIEW]->VmeShow(node, show);
		}
		else if (IsYNormalGizmoComponent(vme))
		{
			m_ChildViewList[Y_VIEW]->VmeShow(node, show);
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
	else if (maf_event->GetSender() == m_GizmoCrossTranslateZNormal) // from translation gizmo
	{
		OnEventGizmoTranslate(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoCrossRotateZNormal) // from rotation gizmo
	{
		OnEventGizmoRotate(maf_event);
	}
	else
	{
		// if no one can handle this event send it to the operation listener
		mafEventMacro(*maf_event); 
	}	
}

void medViewArbitraryOrthoSlice::OnEventGizmoTranslate(mafEventBase *maf_event)

{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    

			m_GizmoCrossRotateZNormal->SetAbsPose(m_GizmoCrossTranslateZNormal->GetAbsPose());
			
			
			mafEvent *e = mafEvent::SafeDownCast(maf_event);
			
			// post multiplying matrices coming from the gizmo to the vme
			PostMultiplyEventMatrix(maf_event);

			
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

			//change the position of rotation gizmos
			vtkTransform *TransformReset;
			vtkNEW(TransformReset);
			TransformReset->Identity();
			TransformReset->Translate(m_VolumeVTKDataCenterABSCoords);
			TransformReset->RotateX(m_VolumeVTKDataABSOrientation[0]);
			TransformReset->RotateY(m_VolumeVTKDataABSOrientation[1]);
			TransformReset->RotateZ(m_VolumeVTKDataABSOrientation[2]);
			TransformReset->Update();
			mafMatrix *matrix;
			mafNEW(matrix);
			matrix->Identity();
			matrix->SetVTKMatrix(TransformReset->GetMatrix());

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
			
			CameraUpdate();
			vtkDEL(tr);
			vtkDEL(TransformReset);
			vtkDEL(matrix);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}

void medViewArbitraryOrthoSlice::OnEventGizmoRotate(mafEventBase *maf_event)

{
	switch(maf_event->GetId())
	{
		case ID_TRANSFORM:
		{
			m_GizmoCrossTranslateZNormal->SetAbsPose(m_GizmoCrossRotateZNormal->GetAbsPose(),0);

			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);

			// roll the camera based on gizmo

			mafEvent *event = mafEvent::SafeDownCast(maf_event);
			assert(event);

      mafString activeGizmoAxis = *(event->GetString());

      vtkMatrix4x4 *mat = event->GetMatrix()->GetVTKMatrix();

      vtkMAFSmartPointer<vtkTransform> tr;
      tr->SetMatrix(mat);

      mafMatrix *gizmoABSPose = m_GizmoCrossRotateZNormal->GetAbsPose();

      double gizmoABSCenter[3];
      mafTransform::GetPosition(*gizmoABSPose , gizmoABSCenter);

      if (activeGizmoAxis.Equals("Z"))
      {		    
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
      else if (activeGizmoAxis.Equals("Y"))
      {		    
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
        m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera()->Roll(-yAngle);
      }
      else if (activeGizmoAxis.Equals("X"))
      {		    
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

			// Y View camera update

// 			double oldYViewCameraFocalPoint[3];
// 
// 			vtkCamera *yViewCamera = NULL;
// 
// 			yViewCamera = m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera();
// 			assert(yViewCamera);
// 
// 			yViewCamera->GetFocalPoint(oldYViewCameraFocalPoint);
// 
// 			double oldYViewCameraPosition[3];
// 			yViewCamera->GetPosition(oldYViewCameraPosition);
// 
// 			double newYViewCameraPosition[3];
// 			double newYViewCameraFocalPoint[3];
// 
// 			GOminusFPVector[0] = gizmoABSCenter[0] - oldYViewCameraFocalPoint[0];
// 			GOminusFPVector[1] = gizmoABSCenter[1] - oldYViewCameraFocalPoint[1];
// 			GOminusFPVector[2] = gizmoABSCenter[2] - oldYViewCameraFocalPoint[2];
// 
// 			mafTransform::AddVectors(oldYViewCameraPosition, GOminusFPVector, newYViewCameraPosition);
// 
// 			yViewCamera->SetFocalPoint(gizmoABSCenter);
// 			yViewCamera->SetPosition(newYViewCameraPosition);
// 
// //			m_ChildViewList[Y_VIEW]->GetRWI()->GetCamera()->Roll(-orientationFromEvent[1]);

			/////


			
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

			CameraUpdate();
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
		case ID_COMBO_CHOOSE_ACTIVE_GIZMO:
			OnChooseActiveGizmo();

			break;
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

		case ID_SHOW_GIZMO:
			{
				m_GizmoCrossRotateZNormal->Show(m_ShowGizmo==1 && m_ComboChooseActiveGizmo == GIZMO_ROTATE? true : false);
				m_GizmoCrossTranslateZNormal->Show(m_ShowGizmo==1 && m_ComboChooseActiveGizmo == GIZMO_TRANSLATE? true : false);
				m_Gui->Enable(ID_COMBO_CHOOSE_ACTIVE_GIZMO, m_ShowGizmo == 1 ? true : false);		
				CameraUpdate();
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

mafView *medViewArbitraryOrthoSlice::Copy(mafObserver *Listener)

{
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

	m_Gui->Divider(2);

	//button to reset at the start position
	m_Gui->Label("reset slices", true);
	m_Gui->Button(ID_RESET,_("reset"),"");

	m_Gui->Divider(2);

	//m_Gui->Label("show gizmo", true);
	//m_Gui->Bool(ID_SHOW_GIZMO, "",&m_ShowGizmo);

	m_Gui->Divider(2);

	//combo box to choose the type of gizmo
	//m_Gui->Label("choose gizmo", true);
	wxString Text[2]={_("translation gizmo"),_("rotation gizmo")};
	m_Gui->Combo(ID_COMBO_CHOOSE_ACTIVE_GIZMO,"",&m_ComboChooseActiveGizmo,2,Text);


	m_Gui->Divider(2);


	// m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

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
		m_GizmoCrossTranslateZNormal->Show(false);
		cppDEL(m_GizmoCrossTranslateZNormal);
		m_GizmoCrossRotateZNormal->Show(false);
		cppDEL(m_GizmoCrossRotateZNormal);
	}

	if (m_CurrentImage && node == m_CurrentImage){
		m_CurrentImage = NULL;
	}

	Superclass::VmeRemove(node);
}

void medViewArbitraryOrthoSlice::PostMultiplyEventMatrix(mafEventBase *maf_event)

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

void medViewArbitraryOrthoSlice::CameraUpdate()

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
	// m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	//m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);
}

void medViewArbitraryOrthoSlice::ShowMafVMEVolume( mafVME * vme, bool show )
{
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

	m_XCameraRollForReset = ((mafViewSlice*)m_ChildViewList[X_VIEW])->GetRWI()->GetCamera()->GetRoll();
	m_YCameraRollForReset = ((mafViewSlice*)m_ChildViewList[Y_VIEW])->GetRWI()->GetCamera()->GetRoll();
	m_ZCameraRollForReset = ((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->GetRoll();
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
	m_GizmoCrossTranslateZNormal->Show(false);
	m_GizmoCrossRotateZNormal->Show(false);


	cppDEL(m_GizmoCrossTranslateZNormal);
	cppDEL(m_GizmoCrossRotateZNormal);
	cppDEL(m_GuiGizmos);

	mafDEL(m_SlicerXResetMatrix);
	mafDEL(m_SlicerYResetMatrix);
	mafDEL(m_SlicerZResetMatrix);

	m_CurrentVolume = NULL;
	m_ColorLUT = NULL;
	// m_LutWidget->SetLut(m_ColorLUT);
}

void medViewArbitraryOrthoSlice::HideMafVmeImage()
{
	m_CurrentImage = NULL;
	m_ColorLUT = NULL;
	// m_LutWidget->SetLut(m_ColorLUT);
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
		m_GizmoCrossRotateZNormal->SetAbsPose(m_SlicerZResetMatrix);
		m_GizmoCrossTranslateZNormal->SetAbsPose(m_SlicerZResetMatrix);

		m_SlicerX->SetAbsMatrix(*m_SlicerXResetMatrix);
		m_SlicerY->SetAbsMatrix(*m_SlicerYResetMatrix);
		m_SlicerZ->SetAbsMatrix(*m_SlicerZResetMatrix);
		
		//update because I need to refresh the normal of the camera
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));


		((mafViewSlice*)m_ChildViewList[X_VIEW])->GetRWI()->GetCamera()->SetRoll(m_XCameraRollForReset);

		((mafViewSlice*)m_ChildViewList[Y_VIEW])->GetRWI()->GetCamera()->SetRoll(m_YCameraRollForReset);

		((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera()->SetRoll(m_ZCameraRollForReset);

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
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}

void medViewArbitraryOrthoSlice::OnLUTChooser()
{
	double *sr;

	if(m_CurrentVolume || m_CurrentImage) {
		sr = m_ColorLUT->GetRange();
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	}
}

void medViewArbitraryOrthoSlice::OnChooseActiveGizmo()
{
	{
		if(m_CurrentVolume)
		{
			if(m_ComboChooseActiveGizmo == GIZMO_TRANSLATE)
			{
				// DEBUG && WORKAROUND:
				// this workaround code (querying for vtk matrix) is needed to update the slicer output matrix
				vtkMatrix4x4 *mat = m_SlicerZ->GetOutput()->GetAbsMatrix()->GetVTKMatrix();
				std::ostringstream stringStream;
				stringStream << std::endl;          
				mat->PrintSelf(stringStream, NULL);
				mafLogMessage(stringStream.str().c_str());

				m_GizmoCrossTranslateZNormal->Show(true);
				m_GizmoCrossTranslateZNormal->SetAbsPose(m_SlicerZ->GetOutput()->GetAbsMatrix(),0);
				m_GizmoCrossRotateZNormal->Show(true);
				this->CameraUpdate();
			}
			else if(m_ComboChooseActiveGizmo == GIZMO_ROTATE)
			{
				// DEBUG && WORKAROUND:
				// this workaround code (querying for vtk matrix) is needed to update the slicer output matrix
				vtkMatrix4x4 *mat = m_SlicerZ->GetOutput()->GetAbsMatrix()->GetVTKMatrix();
				std::ostringstream stringStream;
				stringStream << std::endl;          
				mat->PrintSelf(stringStream,NULL);          
				mafLogMessage(stringStream.str().c_str());

				m_GizmoCrossTranslateZNormal->Show(false);
				m_GizmoCrossRotateZNormal->Show(true);
				m_GizmoCrossRotateZNormal->SetAbsPose(m_SlicerZ->GetOutput()->GetAbsMatrix(),0);
				this->CameraUpdate();
			}
		}
		CameraUpdate();
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
	((mafViewVTK*)m_ChildViewList[X_VIEW])->CameraReset(m_SlicerX);

	m_AttachCameraToSlicerYInYView->SetStartingMatrix(m_SlicerY->GetOutput()->GetAbsMatrix());
	m_AttachCameraToSlicerYInYView->SetVme(m_SlicerZ);
	m_AttachCameraToSlicerYInYView->EnableAttachCamera();
	((mafViewVTK*)m_ChildViewList[Y_VIEW])->CameraReset(m_SlicerY);

	m_AttachCameraToSlicerZInZView->SetStartingMatrix(m_SlicerZ->GetOutput()->GetAbsMatrix());
	m_AttachCameraToSlicerZInZView->SetVme(m_SlicerZ);
	m_AttachCameraToSlicerZInZView->EnableAttachCamera();
	((mafViewVTK*)m_ChildViewList[Z_VIEW])->CameraReset(m_SlicerZ);

	m_GizmoCrossTranslateZNormal = new medGizmoCrossTranslate(m_SlicerZ, this, true, medGizmoCrossTranslate::Z);
	m_GizmoCrossTranslateZNormal->SetName("m_GizmoCrossTranslateZNormal");
	m_GizmoCrossTranslateZNormal->SetInput(m_SlicerZ);
	m_GizmoCrossTranslateZNormal->SetRefSys(m_SlicerZ);
	m_GizmoCrossTranslateZNormal->SetAbsPose(m_SlicerZResetMatrix);
	m_GizmoCrossTranslateZNormal->SetStep(1.0);
	m_GizmoCrossTranslateZNormal->SetStep(1.0);
	m_GizmoCrossTranslateZNormal->SetStep(1.0);
	m_GizmoCrossTranslateZNormal->Show(true);

	m_GizmoCrossRotateZNormal = new medGizmoCrossRotate(m_SlicerZ, this, true, medGizmoCrossTranslate::Z);
	m_GizmoCrossRotateZNormal->SetName("m_GizmoCrossRotateZNormal");
	m_GizmoCrossRotateZNormal->SetInput(m_SlicerZ);
	m_GizmoCrossRotateZNormal->SetRefSys(m_SlicerZ);
	m_GizmoCrossRotateZNormal->SetAbsPose(m_SlicerZResetMatrix);
	m_GizmoCrossRotateZNormal->Show(true);


	m_GizmoCrossTranslateYNormal = new medGizmoCrossTranslate(m_SlicerY, this, true, medGizmoCrossTranslate::Y);
	m_GizmoCrossTranslateYNormal->SetName("m_GizmoCrossTranslateYNormal");
	m_GizmoCrossTranslateYNormal->SetInput(m_SlicerY);
	m_GizmoCrossTranslateYNormal->SetRefSys(m_SlicerY);
	m_GizmoCrossTranslateYNormal->SetAbsPose(m_SlicerYResetMatrix);
	m_GizmoCrossTranslateYNormal->SetStep(1.0);
	m_GizmoCrossTranslateYNormal->SetStep(1.0);
	m_GizmoCrossTranslateYNormal->SetStep(1.0);
	m_GizmoCrossTranslateYNormal->Show(true);

	m_GizmoCrossRotateYNormal = new medGizmoCrossRotate(m_SlicerY, this, true, medGizmoCrossTranslate::Y);
	m_GizmoCrossRotateYNormal->SetName("m_GizmoCrossRotateYNormal");
	m_GizmoCrossRotateYNormal->SetInput(m_SlicerY);
	m_GizmoCrossRotateYNormal->SetRefSys(m_SlicerY);
	m_GizmoCrossRotateYNormal->SetAbsPose(m_SlicerYResetMatrix);
	m_GizmoCrossRotateYNormal->Show(true);

	m_GizmoCrossTranslateXNormal = new medGizmoCrossTranslate(m_SlicerX, this, true, medGizmoCrossTranslate::X);
	m_GizmoCrossTranslateXNormal->SetName("m_GizmoCrossTranslateXNormal");
	m_GizmoCrossTranslateXNormal->SetInput(m_SlicerX);
	m_GizmoCrossTranslateXNormal->SetRefSys(m_SlicerX);
	m_GizmoCrossTranslateXNormal->SetAbsPose(m_SlicerXResetMatrix);
	m_GizmoCrossTranslateXNormal->SetStep(1.0);
	m_GizmoCrossTranslateXNormal->SetStep(1.0);
	m_GizmoCrossTranslateXNormal->SetStep(1.0);
	m_GizmoCrossTranslateXNormal->Show(true);

	m_GizmoCrossRotateXNormal = new medGizmoCrossRotate(m_SlicerX, this, true, medGizmoCrossTranslate::X);
	m_GizmoCrossRotateXNormal->SetName("m_GizmoCrossRotateXNormal");
	m_GizmoCrossRotateXNormal->SetInput(m_SlicerX);
	m_GizmoCrossRotateXNormal->SetRefSys(m_SlicerX);
	m_GizmoCrossRotateXNormal->SetAbsPose(m_SlicerXResetMatrix);
	m_GizmoCrossRotateXNormal->Show(true);

	m_ComboChooseActiveGizmo = GIZMO_ROTATE;

	//Create the Gizmos' Gui
	if(!m_GuiGizmos)
		m_GuiGizmos = new mafGUI(this);

	m_GuiGizmos->AddGui(m_GizmoCrossTranslateZNormal->GetGui());
	m_GuiGizmos->AddGui(m_GizmoCrossRotateZNormal->GetGui());
	m_GuiGizmos->Update();
	m_Gui->AddGui(m_GuiGizmos);
	m_Gui->FitGui();
	m_Gui->Update();

	m_SlicerX->SetVisibleToTraverse(false);
	m_SlicerY->SetVisibleToTraverse(false);
	m_SlicerZ->SetVisibleToTraverse(false);

	CameraUpdate();
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

	coneSource->SetRadius(d / 3);
	coneSource->SetHeight(d / 3);

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

	coneSource->SetRadius(d / 3);
	coneSource->SetHeight(d / 3);

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

	coneSource->SetRadius(d / 3);
	coneSource->SetHeight(d / 3);

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

bool medViewArbitraryOrthoSlice::IsZNormalGizmoComponent( mafVME * vme )
{
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	medGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<medGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoCrossTranslateZNormal"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	medGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<medGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoCrossRotateZNormal"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;
	


}


bool medViewArbitraryOrthoSlice::IsXNormalGizmoComponent( mafVME * vme )
{
	return false;
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	medGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<medGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoCrossTranslateXNormal"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	medGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<medGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoCrossRotateXNormal"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;



}

bool medViewArbitraryOrthoSlice::IsYNormalGizmoComponent( mafVME * vme )
{
	return false;
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();

	medGizmoCrossTranslate *translate = NULL;
	translate = dynamic_cast<medGizmoCrossTranslate *>(mediator);

	std::ostringstream stringStream;
	stringStream << "Gizmo name: " << vme->GetName() << std::endl;        

	if (translate && translate->GetName().Equals("m_GizmoCrossTranslateYNormal"))
	{
		stringStream << "Gizmo master: " << translate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	medGizmoCrossRotate *rotate = NULL;
	rotate = dynamic_cast<medGizmoCrossRotate *>(mediator);

	if (rotate && rotate->GetName().Equals("m_GizmoCrossRotateYNormal"))
	{
		stringStream << "Gizmo master: " << rotate->GetName() << std::endl;       
		mafLogMessage(stringStream.str().c_str());
		return true;
	}

	return false;



}
