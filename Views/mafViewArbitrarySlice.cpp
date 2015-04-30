/*=========================================================================

Program: MAF2
Module: mafViewArbitrarySlice
Authors: Eleonora Mambrini , Stefano Perticoni

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "mafViewArbitrarySlice.h"
#include "mafGUI.h"
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

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewArbitrarySlice);
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
mafViewArbitrarySlice::mafViewArbitrarySlice(wxString label, bool show_ruler)
	: mafViewCompoundWindowing(label, 1, 2)
	//----------------------------------------------------------------------------
{
	m_ViewArbitrary   = NULL;
	m_ViewSlice       = NULL;
	m_GizmoTranslate  = NULL;
	m_GizmoRotate     = NULL;
	m_MatrixReset     = NULL;
	m_CurrentVolume   = NULL;
	m_CurrentImage    = NULL;
	m_Slicer          = NULL;
	m_GuiGizmos       = NULL;
	m_AttachCamera    = NULL;
	m_CurrentPolylineGraphEditor = NULL;

	m_SliceCenterSurface[0] = 0.0;
	m_SliceCenterSurface[1] = 0.0;
	m_SliceCenterSurface[2] = 0.0;

	m_SliceCenterSurfaceReset[0] = 0.0;
	m_SliceCenterSurfaceReset[1] = 0.0;
	m_SliceCenterSurfaceReset[2] = 0.0;

	m_TypeGizmo = GIZMO_TRANSLATE;

	m_TrilinearInterpolationOn = TRUE;
}
//----------------------------------------------------------------------------
mafViewArbitrarySlice::~mafViewArbitrarySlice()
	//----------------------------------------------------------------------------
{
	m_MatrixReset   = NULL;
	m_CurrentVolume = NULL;
	m_CurrentImage  = NULL;
	m_ColorLUT      = NULL;

}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::PackageView()
	//----------------------------------------------------------------------------
{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE);
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
	m_ViewArbitrary->PlugVisualPipe("mafVMELabeledVolume", "mafPipeBox", MUTEX);

	m_ViewSlice = new mafViewVTK("",CAMERA_OS_Z);
	m_ViewSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice");
	m_ViewSlice->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice");

	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSlice);

}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::VmeShow(mafNode *node, bool show)
	//----------------------------------------------------------------------------
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);
	mafVME *Vme=mafVME::SafeDownCast(node);
	Vme->Update();
	if (show)
	{
		if(((mafVME *)Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			double sr[2],SliceCenterVolumeReset[3];
			mafVME *Volume=mafVME::SafeDownCast(Vme);
			m_CurrentVolume = Volume;

			// get the VTK volume
			vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
			data->Update();
			//Get center of Volume to can the reset
			data->GetCenter(SliceCenterVolumeReset);
			//Get scalar range of the volume
			data->GetScalarRange(sr);
			data=NULL;

			mafTransform::GetOrientation(Vme->GetAbsMatrixPipe()->GetMatrix(),m_SliceAngleReset);

			//Compute the center of Volume in absolute coordinate, to center the surface and gizmo
			vtkPoints *pts;
			vtkNEW(pts);
			pts->InsertNextPoint(SliceCenterVolumeReset);
			vtkPolyData *pd=vtkPolyData::New();
			pd->SetPoints(pts);
			vtkTransform *transform;
			vtkNEW(transform);
			transform->Identity();
			transform->SetMatrix(Vme->GetOutput()->GetMatrix()->GetVTKMatrix());
			transform->Update();
			vtkTransformPolyDataFilter *filter;
			vtkNEW(filter);
			filter->SetInput(pd);
			filter->SetTransform(transform);
			filter->Update();
			filter->GetOutput()->GetCenter(m_SliceCenterSurface);
			filter->GetOutput()->GetCenter(m_SliceCenterSurfaceReset);

			//Create a matrix to permit the reset of the gizmos
			vtkTransform *TransformReset;
			vtkNEW(TransformReset);
			TransformReset->Identity();
			TransformReset->Translate(m_SliceCenterSurfaceReset);
			TransformReset->RotateX(m_SliceAngleReset[0]);
			TransformReset->RotateY(m_SliceAngleReset[1]);
			TransformReset->RotateZ(m_SliceAngleReset[2]);
			TransformReset->Update();
			mafNEW(m_MatrixReset);
			m_MatrixReset->Identity();
			m_MatrixReset->SetVTKMatrix(TransformReset->GetMatrix());

			//Create VME slicer
			mafNEW(m_Slicer);
			m_Slicer->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_Slicer->ReparentTo(mafVME::SafeDownCast(node));
			m_Slicer->SetPose(m_SliceCenterSurfaceReset,m_SliceAngleReset,0);
			m_Slicer->SetAbsMatrix(*m_MatrixReset);
			m_Slicer->SetSlicedVMELink(mafVME::SafeDownCast(node));
			m_Slicer->GetMaterial()->m_ColorLut->DeepCopy(mafVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
			m_Slicer->Update();

			//Show Slicer
			m_ChildViewList[ARBITRARY_VIEW]->VmeShow(m_Slicer, show);
			m_ChildViewList[SLICE_VIEW]->VmeShow(m_Slicer, show);

			mafPipeSurfaceTextured *pArb=(mafPipeSurfaceTextured *)(m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_Slicer);
			pArb->SetActorPicking(false);
			pArb->SetEnableActorLOD(0);
			mafPipeSurfaceTextured *pSli=(mafPipeSurfaceTextured *)(m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_Slicer);
			pSli->SetActorPicking(false);
			pSli->SetEnableActorLOD(0);

			UpdateSlicerBehavior();

			//Set camera of slice viw in way that it will follow the volume
			if(!m_AttachCamera)
				m_AttachCamera=new mafAttachCamera(m_Gui,((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->m_Rwi,this);
			m_AttachCamera->SetStartingMatrix(m_Slicer->GetOutput()->GetAbsMatrix());
			m_AttachCamera->SetVme(m_Slicer);
			m_AttachCamera->EnableAttachCamera();
			((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_Slicer);

			// create the gizmos
			m_GizmoTranslate = new mafGizmoTranslate(m_Slicer, this);
			m_GizmoTranslate->SetInput(m_Slicer);
			m_GizmoTranslate->SetRefSys(m_Slicer);
			m_GizmoTranslate->SetAbsPose(m_MatrixReset);
			m_GizmoTranslate->SetStep(X_AXIS,1.0);
			m_GizmoTranslate->SetStep(Y_AXIS,1.0);
			m_GizmoTranslate->SetStep(Z_AXIS,1.0);
			m_GizmoTranslate->Show(true);

			m_GizmoRotate = new mafGizmoRotate(m_Slicer, this);
			m_GizmoRotate->SetInput(m_Slicer);
			m_GizmoRotate->SetRefSys(m_Slicer);
			m_GizmoRotate->SetAbsPose(m_MatrixReset);
			m_GizmoRotate->Show(false);

			m_TypeGizmo = GIZMO_TRANSLATE;

			//Create the Gizmos' Gui
			if(!m_GuiGizmos)
				m_GuiGizmos = new mafGUI(this);
			m_GuiGizmos->AddGui(m_GizmoTranslate->GetGui());
			m_GuiGizmos->AddGui(m_GizmoRotate->GetGui());
			m_GuiGizmos->Update();
			m_Gui->AddGui(m_GuiGizmos);
			m_Gui->FitGui();
			m_Gui->Update();

			m_Slicer->SetVisibleToTraverse(false);

			vtkDEL(pts);
			vtkDEL(pd);
			vtkDEL(transform);
			vtkDEL(filter);
			vtkDEL(TransformReset);
		}
		else if(Vme->IsA("mafVMESurface") || Vme->IsA("mafVMESurfaceParametric") || Vme->IsA("mafVMELandmark") || Vme->IsA("mafVMELandmarkCloud"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{

				double normal[3];
				((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

				mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
				mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));

				double surfaceOriginTranslated[3];

				surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
				surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
				surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

				PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
				PipeSliceViewSurface->SetNormal(normal);
			}
		}
		else if(Vme->IsA("mafVMEPolylineEditor"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{
				m_CurrentPolylineGraphEditor = (mafVMEPolylineEditor*)node;

				double normal[3];
				((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

				mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
				PipeSliceViewPolylineEditor->SetModalitySlice();

				double surfaceOriginTranslated[3];

				surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
				surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
				surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

				PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);        
			}
		}
		else if(Vme->IsA("mafVMEMesh"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{

				double normal[3];
				((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

				mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
				mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));

				double surfaceOriginTranslated[3];

				surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
				surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
				surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

				PipeSliceViewMesh->SetSlice(surfaceOriginTranslated);
				PipeSliceViewMesh->SetNormal(normal);
				PipeSliceViewMesh->SetFlipNormalOff();
			}
		}

		else if(Vme->IsA("mafVMEImage"))
		{	
			mafVME *Image  = mafVME::SafeDownCast(Vme);
			m_CurrentImage = Image;

		}

	}
	else//if show=false
	{
		if(Vme->IsA("mafVMEPolylineGraphEditor"))
		{
			m_CurrentPolylineGraphEditor = NULL;
		}

		if(((mafVME *)Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			m_AttachCamera->SetVme(NULL);
			m_Slicer->SetBehavior(NULL);
			m_Slicer->ReparentTo(NULL);

			//remove gizmos
			m_Gui->Remove(m_GuiGizmos);
			m_Gui->Update();
			m_GizmoTranslate->Show(false);
			m_GizmoRotate->Show(false);

			mafDEL(m_Slicer);
			cppDEL(m_GizmoTranslate);
			cppDEL(m_GizmoRotate);
			cppDEL(m_GuiGizmos);
			mafDEL(m_MatrixReset);

			m_CurrentVolume = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
		else if(Vme->IsA("mafVMEImage")) {
			m_CurrentImage = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}

	}

	if(GetSceneGraph()->GetSelectedVme()==node) 
	{
		UpdateWindowing( show && this->ActivateWindowing(node), node);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::OnEvent(mafEventBase *maf_event)
	//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::OnEventGizmoTranslate(mafEventBase *maf_event)
	//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    

			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
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
			mafMatrix *matrix;
			mafNEW(matrix);
			matrix->Identity();
			matrix->SetVTKMatrix(TransformReset->GetMatrix());

			//for each surface visualized change the center of the cut plane
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{
						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
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
					((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

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
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::OnEventGizmoRotate(mafEventBase *maf_event)
	//----------------------------------------------------------------------------
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
					((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

					mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if(PipeSliceViewSurface)
					{

						PipeSliceViewSurface->SetNormal(normal);

						PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
					}
				}
				if(node->IsA("mafVMEMesh"))
				{
					double surfaceOriginTranslated[3];
					double normal[3];
					((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
					surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

					mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if(PipeSliceViewMesh)
					{

						PipeSliceViewMesh->SetNormal(normal);

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
					((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

					mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
					PipeSliceViewPolylineEditor->SetModalitySlice();

					double surfaceOriginTranslated[3];

					surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

					PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);          
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
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::OnEventThis(mafEventBase *maf_event)
	//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
		case ID_COMBO_GIZMOS:
			if(m_CurrentVolume)
			{
				if(m_TypeGizmo == GIZMO_TRANSLATE)
				{
					m_GizmoTranslate->Show(true);
					m_GizmoTranslate->SetAbsPose(m_Slicer->GetOutput()->GetAbsMatrix(),0);
					m_GizmoRotate->Show(false);
				}
				else if(m_TypeGizmo == GIZMO_ROTATE)
				{
					m_GizmoTranslate->Show(false);
					m_GizmoRotate->Show(true);
					m_GizmoRotate->SetAbsPose(m_Slicer->GetOutput()->GetAbsMatrix(),0);
				}
			}
			CameraUpdate();
			break;

		case ID_HELP:
			{
				mafEvent helpEvent;
				helpEvent.SetSender(this);
				mafString viewLabel = this->m_Label;
				helpEvent.SetString(&viewLabel);
				helpEvent.SetId(OPEN_HELP_PAGE);
				mafEventMacro(helpEvent);
			}
			break;

		case ID_RANGE_MODIFIED:
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
			break;
		case ID_LUT_CHOOSER:
			{
				double *sr;

				if(m_CurrentVolume || m_CurrentImage) {
					sr = m_ColorLUT->GetRange();
					m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
				}
			}
			break;
		case ID_RESET:
			{
				m_GizmoRotate->SetAbsPose(m_MatrixReset);
				m_GizmoTranslate->SetAbsPose(m_MatrixReset);
				m_Slicer->SetAbsMatrix(*m_MatrixReset);
				//update because I need to refresh the normal of the camera
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				//update the normal of the cutter plane of the surface
				mafNode *root=m_CurrentVolume->GetRoot();
				mafNodeIterator *iter = root->NewIterator();
				for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
				{
					if(node->IsA("mafVMESurface") || node->IsA("mafVMESurfaceParametric") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud"))
					{
						mafPipeSurface *PipeArbitraryViewSurface = mafPipeSurface::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
						mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
						if(PipeArbitraryViewSurface)
						{
							double normal[3];
							((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
							PipeSliceViewSurface->SetNormal(normal);

							double surfaceOriginTranslated[3];
							surfaceOriginTranslated[0] = m_SliceCenterSurfaceReset[0] + normal[0] * 0.1;
							surfaceOriginTranslated[1] = m_SliceCenterSurfaceReset[1] + normal[1] * 0.1;
							surfaceOriginTranslated[2] = m_SliceCenterSurfaceReset[2] + normal[2] * 0.1;

							PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
						}
					}
					if(node->IsA("mafVMEMesh"))
					{
						mafPipeMesh *PipeArbitraryViewMesh = mafPipeMesh::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
						mafPipeMeshSlice *PipeSliceViewMesh = mafPipeMeshSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
						if(PipeSliceViewMesh && PipeArbitraryViewMesh)
						{
							double normal[3];
							((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
							PipeSliceViewMesh->SetNormal(normal);

							double surfaceOriginTranslated[3];
							surfaceOriginTranslated[0] = m_SliceCenterSurfaceReset[0] + normal[0] * 0.1;
							surfaceOriginTranslated[1] = m_SliceCenterSurfaceReset[1] + normal[1] * 0.1;
							surfaceOriginTranslated[2] = m_SliceCenterSurfaceReset[2] + normal[2] * 0.1;

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
						((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

						mafPipePolylineGraphEditor *PipeSliceViewPolylineEditor = mafPipePolylineGraphEditor::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe((mafNode*)m_CurrentPolylineGraphEditor));
						PipeSliceViewPolylineEditor->SetModalitySlice();

						double surfaceOriginTranslated[3];

						surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
						surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
						surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

						PipeSliceViewPolylineEditor->SetSlice(surfaceOriginTranslated, normal);            
					}
				}
				break;
			}
		case ID_TRILINEAR_INTERPOLATION_ON:
			{
				m_Slicer->SetTrilinearInterpolation(m_TrilinearInterpolationOn == TRUE);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));

			}
			break;
		default:
			mafViewCompound::OnEvent(maf_event);
		}
	}
}
//----------------------------------------------------------------------------
mafView *mafViewArbitrarySlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
	//----------------------------------------------------------------------------
{
	m_LightCopyEnabled = lightCopyEnabled;
	mafViewArbitrarySlice *v = new mafViewArbitrarySlice(m_Label);
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
mafGUI* mafViewArbitrarySlice::CreateGui()
	//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);

	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	mafEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		m_Gui->Button(ID_HELP, "Help","");	
	}

	//combo box to choose the type of gizmo
	m_Gui->Label("");
	m_Gui->Label("Choose Gizmo");
	wxString Text[2]={_("Gizmo Translation"),_("Gizmo Rotation")};
	m_Gui->Combo(ID_COMBO_GIZMOS,"",&m_TypeGizmo,2,Text);

	m_Gui->Label("");
	//button to reset at the start position
	m_Gui->Button(ID_RESET,_("Reset"),"");
	m_Gui->Divider(2);

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

	m_Gui->Bool(ID_TRILINEAR_INTERPOLATION_ON,"Interpolation",&m_TrilinearInterpolationOn,1);

	m_Gui->Divider();
	m_Gui->Update();

	EnableWidgets( (m_CurrentVolume != NULL) || (m_CurrentImage != NULL) );
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::VmeRemove(mafNode *node)
	//----------------------------------------------------------------------------
{
	if (m_CurrentVolume && node == m_CurrentVolume) 
	{
		m_AttachCamera->SetVme(NULL);
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
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::PostMultiplyEventMatrix(mafEventBase *maf_event)
	//----------------------------------------------------------------------------
{  
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(((mafVME *)m_Slicer)->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		mafMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(m_Slicer->GetTimeStamp());

		if (arg == mafInteractorGenericMouse::MOUSE_MOVE)
		{
			// move vme
			((mafVME *)m_Slicer)->SetAbsMatrix(absPose);
		} 

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::CameraUpdate()
	//----------------------------------------------------------------------------
{
	if (m_AttachCamera != NULL)
	{
		//Camera follows the slicer
		m_AttachCamera->UpdateCameraMatrix();
	}
	for(int i=0; i<m_NumOfChildView; i++)
	{
		
		m_ChildViewList[i]->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::CreateGuiView()
	//----------------------------------------------------------------------------
{
	m_GuiView = new mafGUI(this);

	m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500,24);
	m_LutSlider->SetMinSize(wxSize(500,24));
	EnableWidgets( (m_CurrentVolume != NULL) || (m_CurrentImage!=NULL) );
	m_GuiView->Add(m_LutSlider);
	m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::EnableWidgets(bool enable)
	//----------------------------------------------------------------------------
{
	if (m_Gui)
	{
		bool tmp_enable = (m_CurrentVolume || m_CurrentImage);
		m_Gui->Enable(ID_RESET, tmp_enable);
		m_Gui->Enable(ID_COMBO_GIZMOS, tmp_enable);
		m_Gui->Enable(ID_LUT_CHOOSER, tmp_enable);
		m_Gui->FitGui();
		m_Gui->Update();
	}
	m_LutSlider->Enable(enable);

}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::UpdateSlicerBehavior()
	//----------------------------------------------------------------------------
{
	if(m_CurrentVolume->GetBehavior())
	{
		m_Slicer->SetBehavior(m_CurrentVolume->GetBehavior());
		mafPipeSurfaceTextured *pArb=(mafPipeSurfaceTextured *)(m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_Slicer);
		pArb->SetActorPicking(true);
		mafPipeSurfaceTextured *pSli=(mafPipeSurfaceTextured *)(m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_Slicer);
		pSli->SetActorPicking(true);
	}
	else
	{
		m_Slicer->SetBehavior(NULL);
		mafPipeSurfaceTextured *pArb=(mafPipeSurfaceTextured *)(m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_Slicer);
		pArb->SetActorPicking(false);
		mafPipeSurfaceTextured *pSli=(mafPipeSurfaceTextured *)(m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_Slicer);
		pSli->SetActorPicking(false);
	}
};

//----------------------------------------------------------------------------
void mafViewArbitrarySlice::VolumeWindowing(mafVME *volume)
	//----------------------------------------------------------------------------
{

	double sr[2];
	vtkDataSet *data = ((mafVME *)volume)->GetOutput()->GetVTKData();
	data->Update();
	data->GetScalarRange(sr);

	mmaMaterial *currentSurfaceMaterial = m_Slicer->GetMaterial();
	m_ColorLUT = m_Slicer->GetMaterial()->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);

}