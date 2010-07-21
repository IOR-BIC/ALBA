/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewArbitraryOrthoSlice.cpp,v $
Language:  C++
Date:      $Date: 2010-07-21 09:48:59 $
Version:   $Revision: 1.1.2.5 $
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
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
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

	m_GizmoTranslate  = NULL;
	m_GizmoRotate     = NULL;
	
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
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE);
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
//	m_ViewArbitrary->PlugVisualPipe("mafVMELabeledVolume", "mafPipeBox", MUTEX);

	m_ViewSliceX = new mafViewVTK("",CAMERA_OS_X);
// 	m_ViewSliceX->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceX->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceX->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);
 	m_ViewSliceX->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo");
  m_ViewSliceX->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
// 	m_ViewSliceX->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceX->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	m_ViewSliceY = new mafViewVTK("",CAMERA_OS_Y);
// 	m_ViewSliceY->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceY->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceY->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);
	m_ViewSliceY->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo");
	m_ViewSliceY->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
// 	m_ViewSliceY->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceY->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice", NON_VISIBLE);

	m_ViewSliceZ = new mafViewVTK("",CAMERA_OS_Z);
// 	m_ViewSliceZ->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceZ->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice", NON_VISIBLE);
// 	m_ViewSliceZ->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice", NON_VISIBLE);
	m_ViewSliceZ->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo");
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
	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);

	m_ChildViewList[X_VIEW]->VmeShow(node, show);
	m_ChildViewList[Y_VIEW]->VmeShow(node, show);
	m_ChildViewList[Z_VIEW]->VmeShow(node, show);

	mafVME *vme=mafVME::SafeDownCast(node);
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
	else if (maf_event->GetSender() == m_GizmoTranslate) // from translation gizmo
	{
		OnEventGizmoTranslate(maf_event);
	}
	else if (maf_event->GetSender() == m_GizmoRotate) // from rotation gizmo
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

			// post multiplying matrices coming from the gizmo to the vme
			PostMultiplyEventMatrix(maf_event);

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
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);

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
        m_GizmoRotate->Show(m_ShowGizmo==1 && m_ComboChooseActiveGizmo == GIZMO_ROTATE? true : false);
		m_GizmoTranslate->Show(m_ShowGizmo==1 && m_ComboChooseActiveGizmo == GIZMO_TRANSLATE? true : false);
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

    m_Gui->Label("show gizmo", true);
    m_Gui->Bool(ID_SHOW_GIZMO, "",&m_ShowGizmo);

	m_Gui->Divider(2);

	//combo box to choose the type of gizmo
	m_Gui->Label("choose gizmo", true);
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
		m_GizmoTranslate->Show(false);
		cppDEL(m_GizmoTranslate);
		m_GizmoRotate->Show(false);
		cppDEL(m_GizmoRotate);
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

   
	}
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

	m_AttachCameraToSlicerXInXView->SetVme(NULL);
	m_SlicerX->SetBehavior(NULL);
	m_SlicerX->ReparentTo(NULL);

	m_AttachCameraToSlicerYInYView->SetVme(NULL);
	m_SlicerY->SetBehavior(NULL);
	m_SlicerY->ReparentTo(NULL);

	m_AttachCameraToSlicerZInZView->SetVme(NULL);
	m_SlicerZ->SetBehavior(NULL);
	m_SlicerZ->ReparentTo(NULL);

	//remove gizmos
	m_Gui->Remove(m_GuiGizmos);
	m_Gui->Update();
	m_GizmoTranslate->Show(false);
	m_GizmoRotate->Show(false);

	mafDEL(m_SlicerX);
	mafDEL(m_SlicerY);
	mafDEL(m_SlicerZ);

	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
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
		m_GizmoRotate->SetAbsPose(m_SlicerZResetMatrix);
		m_GizmoTranslate->SetAbsPose(m_SlicerZResetMatrix);
    
    m_SlicerX->SetAbsMatrix(*m_SlicerXResetMatrix);
    m_SlicerY->SetAbsMatrix(*m_SlicerYResetMatrix);
		m_SlicerZ->SetAbsMatrix(*m_SlicerZResetMatrix);

		//update because I need to refresh the normal of the camera
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

				m_GizmoTranslate->Show(true);
				m_GizmoTranslate->SetAbsPose(m_SlicerZ->GetOutput()->GetAbsMatrix(),0);
				m_GizmoRotate->Show(false);
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

				m_GizmoTranslate->Show(false);
				m_GizmoRotate->Show(true);
				m_GizmoRotate->SetAbsPose(m_SlicerZ->GetOutput()->GetAbsMatrix(),0);
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

	// TODO: should create three different translation gizmos, one for each plane.
	m_GizmoTranslate = new mafGizmoTranslate(m_SlicerZ, this);
	m_GizmoTranslate->SetInput(m_SlicerZ);
	m_GizmoTranslate->SetRefSys(m_SlicerZ);
	m_GizmoTranslate->SetAbsPose(m_SlicerZResetMatrix);
	m_GizmoTranslate->SetStep(X_AXIS,1.0);
	m_GizmoTranslate->SetStep(Y_AXIS,1.0);
	m_GizmoTranslate->SetStep(Z_AXIS,1.0);
	m_GizmoTranslate->Show(false);

	// 1 rotation gizmo for the triad
	m_GizmoRotate = new mafGizmoRotate(m_SlicerZ, this);
	m_GizmoRotate->SetInput(m_SlicerZ);
	m_GizmoRotate->SetRefSys(m_SlicerZ);
	m_GizmoRotate->SetAbsPose(m_SlicerZResetMatrix);
	m_GizmoRotate->Show(true);

	m_ComboChooseActiveGizmo = GIZMO_ROTATE;

	//Create the Gizmos' Gui
	if(!m_GuiGizmos)
		m_GuiGizmos = new mafGUI(this);
	
	m_GuiGizmos->AddGui(m_GizmoTranslate->GetGui());
	m_GuiGizmos->AddGui(m_GizmoRotate->GetGui());
	m_GuiGizmos->Update();
	m_Gui->AddGui(m_GuiGizmos);
	m_Gui->FitGui();
	m_Gui->Update();

	m_SlicerX->SetVisibleToTraverse(false);
	m_SlicerY->SetVisibleToTraverse(false);
	m_SlicerZ->SetVisibleToTraverse(false);

	CameraUpdate();
}
