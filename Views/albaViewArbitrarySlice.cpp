/*=========================================================================

Program: ALBA
Module: albaViewArbitrarySlice
Authors: Eleonora Mambrini , Stefano Perticoni

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
	//----------------------------------------------------------------------------
{
	m_ViewArbitrary   = NULL;
	m_ViewSlice       = NULL;
	m_GizmoTranslate  = NULL;
	m_GizmoRotate     = NULL;
	m_MatrixReset     = NULL;
	m_CurrentVolume   = NULL;
	m_Slicer          = NULL;
	m_GuiGizmos       = NULL;
	m_AttachCamera    = NULL;

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
albaViewArbitrarySlice::~albaViewArbitrarySlice()
	//----------------------------------------------------------------------------
{
	m_MatrixReset   = NULL;
	m_CurrentVolume = NULL;
	m_ColorLUT      = NULL;

}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::PackageView()
	//----------------------------------------------------------------------------
{
	m_ViewArbitrary = new albaViewVTK("",CAMERA_PERSPECTIVE);
	m_ViewArbitrary->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", MUTEX);
	m_ViewArbitrary->PlugVisualPipe("albaVMELabeledVolume", "albaPipeBox", MUTEX);

	m_ViewSlice = new albaViewVTK("",CAMERA_OS_Z);
	m_ViewSlice->PlugVisualPipe("albaVMESurface", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMESurfaceParametric", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMEMesh", "albaPipeMeshSlice");
	m_ViewSlice->PlugVisualPipe("albaVMEGizmo", "albaPipeGizmo", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("albaVMELandmark", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMELandmarkCloud", "albaPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("albaVMERefSys", "albaPipeSurfaceSlice");

	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSlice);

}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VmeShow(albaVME *vme, bool show)
	//----------------------------------------------------------------------------
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(vme, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(vme, show);
	vme->Update();
	if (show)
	{
		if(vme->GetOutput()->IsA("albaVMEOutputVolume") && vme != m_CurrentVolume)
		{
			double sr[2],SliceCenterVolumeReset[3];
			albaVME *Volume=vme;
			m_CurrentVolume = Volume;

			// get the VTK volume
			vtkDataSet *data = vme->GetOutput()->GetVTKData();
			data->Update();
			//Get center of Volume to can the reset
			data->GetCenter(SliceCenterVolumeReset);
			//Get scalar range of the volume
			data->GetScalarRange(sr);
			data=NULL;

			albaTransform::GetOrientation(vme->GetAbsMatrixPipe()->GetMatrix(),m_SliceAngleReset);

			//Compute the center of Volume in absolute coordinate, to center the surface and gizmo
			vtkPoints *pts;
			vtkNEW(pts);
			pts->InsertNextPoint(SliceCenterVolumeReset);
			vtkPolyData *pd=vtkPolyData::New();
			pd->SetPoints(pts);
			vtkTransform *transform;
			vtkNEW(transform);
			transform->Identity();
			transform->SetMatrix(vme->GetOutput()->GetMatrix()->GetVTKMatrix());
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
			albaNEW(m_MatrixReset);
			m_MatrixReset->Identity();
			m_MatrixReset->SetVTKMatrix(TransformReset->GetMatrix());

			//Create VME slicer
			albaNEW(m_Slicer);
			m_Slicer->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_Slicer->ReparentTo(vme);
			m_Slicer->SetPose(m_SliceCenterSurfaceReset,m_SliceAngleReset,0);
			m_Slicer->SetAbsMatrix(*m_MatrixReset);
			m_Slicer->SetSlicedVMELink(vme);
			m_Slicer->GetMaterial()->m_ColorLut->DeepCopy(albaVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
			m_Slicer->Update();

			//Show Slicer
			m_ChildViewList[ARBITRARY_VIEW]->VmeShow(m_Slicer, show);
			m_ChildViewList[SLICE_VIEW]->VmeShow(m_Slicer, show);

			albaPipeSurfaceTextured *pArb=(albaPipeSurfaceTextured *)(m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_Slicer);
			pArb->SetActorPicking(false);
			pArb->SetEnableActorLOD(0);
			albaPipeSurfaceTextured *pSli=(albaPipeSurfaceTextured *)(m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_Slicer);
			pSli->SetActorPicking(false);
			pSli->SetEnableActorLOD(0);

			UpdateSlicerBehavior();

			//Set camera of slice view in way that it will follow the volume
			if(!m_AttachCamera)
				m_AttachCamera=new albaAttachCamera(m_Gui,((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->m_Rwi,this);
			m_AttachCamera->SetStartingMatrix(m_Slicer->GetOutput()->GetAbsMatrix());
			m_AttachCamera->SetVme(m_Slicer);
			m_AttachCamera->EnableAttachCamera();
			((albaViewVTK*)m_ChildViewList[SLICE_VIEW])->CameraReset(m_Slicer);

			CreateGizmos();
			
			m_Gui->FitGui();
			m_Gui->Update();

			m_Slicer->SetVisibleToTraverse(false);

			vtkDEL(pts);
			vtkDEL(pd);
			vtkDEL(transform);
			vtkDEL(filter);
			vtkDEL(TransformReset);
		}
		
		albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(vme));
		if (pipeSlice)
		{
			double surfaceOriginTranslated[3];
			double normal[3];
			((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
			surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
			surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
			surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

			pipeSlice->SetSlice(surfaceOriginTranslated, normal);
		}
		albaPipeMeshSlice *pipeSliceViewMesh = albaPipeMeshSlice::SafeDownCast(pipeSlice);
		if (pipeSliceViewMesh)
			pipeSliceViewMesh->SetFlipNormalOff();
	}
	else//if show=false
	{
		if(vme->GetOutput()->IsA("albaVMEOutputVolume"))
		{
			m_AttachCamera->SetVme(NULL);
			m_Slicer->SetBehavior(NULL);
			m_Slicer->ReparentTo(NULL);
			
			albaDEL(m_Slicer);

			DestroyGizmos();

			albaDEL(m_MatrixReset);

			m_CurrentVolume = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
	}

	if (ActivateWindowing(vme))
		UpdateWindowing(show, vme);
	
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEvent(albaEventBase *alba_event)
	//----------------------------------------------------------------------------
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
		// if no one can handle this event send it to the operation listener
		Superclass::OnEvent(alba_event);
	}	
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::OnEventGizmoTranslate(albaEventBase *alba_event)
	//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    

			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);

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
			albaMatrix *matrix;
			albaNEW(matrix);
			matrix->Identity();
			matrix->SetVTKMatrix(TransformReset->GetMatrix());

			double surfaceOriginTranslated[3];
			double normal[3];
			((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
			surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
			surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
			surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

			//for each surface visualized change the center of the cut plane
			albaVME *root=m_CurrentVolume->GetRoot();
			albaVMEIterator *iter = root->NewIterator();
			for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(m_ChildViewList[SLICE_VIEW]->GetNodePipe(node));
				if (pipeSlice)
					pipeSlice->SetSlice(surfaceOriginTranslated, NULL);
			}
			iter->Delete();
			CameraUpdate();
			vtkDEL(tr);
			vtkDEL(TransformReset);
			vtkDEL(matrix);
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
	//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);

			//update the normal of the cutter plane of the surface
			albaVME *root=m_CurrentVolume->GetRoot();
			albaVMEIterator *iter = root->NewIterator();
			double surfaceOriginTranslated[3];
			double normal[3];
			((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
			surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
			surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
			surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;

			for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
				if(pipeSlice)
					pipeSlice->SetSlice(surfaceOriginTranslated, normal);
			}
			iter->Delete();
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
	//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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

		case ID_RANGE_MODIFIED:
			{
				albaVME *vme = GetSceneGraph()->GetSelectedVme();

				if( m_CurrentVolume && vme)
				{
					double low, hi;
					m_LutSlider->GetSubRange(&low,&hi);
					m_ColorLUT->SetTableRange(low,hi);
					GetLogicManager()->CameraUpdate();
				}
			}
			break;
		case ID_LUT_CHOOSER:
			{
				double *sr;

				if(m_CurrentVolume)
				{
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
				GetLogicManager()->CameraUpdate();
				//update the normal of the cutter plane of the surface
				albaVME *root=m_CurrentVolume->GetRoot();
				albaVMEIterator *iter = root->NewIterator();
				double surfaceOriginTranslated[3];
				double normal[3];
				((albaViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
				surfaceOriginTranslated[0] = m_SliceCenterSurface[0] + normal[0] * 0.1;
				surfaceOriginTranslated[1] = m_SliceCenterSurface[1] + normal[1] * 0.1;
				surfaceOriginTranslated[2] = m_SliceCenterSurface[2] + normal[2] * 0.1;
				for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
				{
					albaPipeSlice *pipeSlice = albaPipeSlice::SafeDownCast(((albaViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if (pipeSlice)
						pipeSlice->SetSlice(surfaceOriginTranslated, normal);
				}
				iter->Delete();
				break;
			}
		case ID_TRILINEAR_INTERPOLATION_ON:
			{
				if (m_Slicer)
				{
					m_Slicer->SetTrilinearInterpolation(m_TrilinearInterpolationOn == TRUE);
					GetLogicManager()->CameraUpdate();
				}
			}
			break;
		default:
			albaViewCompound::OnEvent(alba_event);
		}
	}
}
//----------------------------------------------------------------------------
albaView *albaViewArbitrarySlice::Copy(albaObserver *Listener, bool lightCopyEnabled)
	//----------------------------------------------------------------------------
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
	//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

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

	EnableWidgets(m_CurrentVolume);
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VmeRemove(albaVME *vme)
	//----------------------------------------------------------------------------
{
	if (m_CurrentVolume && vme == m_CurrentVolume) 
	{
		m_AttachCamera->SetVme(NULL);

		DestroyGizmos();

		m_CurrentVolume->RemoveObserver(this);
		m_CurrentVolume = NULL;
	}

	Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::PostMultiplyEventMatrix(albaEventBase *alba_event)
	//----------------------------------------------------------------------------
{  
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_Slicer->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		albaMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(m_Slicer->GetTimeStamp());

		if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
		{
			// move vme
			m_Slicer->SetAbsMatrix(absPose);
		} 

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::CameraUpdate()
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
void albaViewArbitrarySlice::CreateGuiView()
	//----------------------------------------------------------------------------
{
	m_GuiView = new albaGUI(this);

	m_LutSlider = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500,24);
	m_LutSlider->SetMinSize(wxSize(500,24));
	EnableWidgets( m_CurrentVolume );
	m_GuiView->Add(m_LutSlider);
	m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::EnableWidgets(bool enable)
	//----------------------------------------------------------------------------
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_RESET, enable);
		m_Gui->Enable(ID_COMBO_GIZMOS, enable);
		m_Gui->Enable(ID_LUT_CHOOSER, enable);
		m_LutSlider->Enable(enable);
		m_Gui->FitGui();
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaViewArbitrarySlice::UpdateSlicerBehavior()
	//----------------------------------------------------------------------------
{
	if(m_CurrentVolume->GetBehavior())
	{
		m_Slicer->SetBehavior(m_CurrentVolume->GetBehavior());
		albaPipeSurfaceTextured *pArb=(albaPipeSurfaceTextured *)(m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_Slicer);
		pArb->SetActorPicking(true);
		albaPipeSurfaceTextured *pSli=(albaPipeSurfaceTextured *)(m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_Slicer);
		pSli->SetActorPicking(true);
	}
	else
	{
		m_Slicer->SetBehavior(NULL);
		albaPipeSurfaceTextured *pArb=(albaPipeSurfaceTextured *)(m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(m_Slicer);
		pArb->SetActorPicking(false);
		albaPipeSurfaceTextured *pSli=(albaPipeSurfaceTextured *)(m_ChildViewList[SLICE_VIEW])->GetNodePipe(m_Slicer);
		pSli->SetActorPicking(false);
	}
};

//----------------------------------------------------------------------------
char ** albaViewArbitrarySlice::GetIcon()
{
#include "pic/VIEW_ARBITRARY.xpm"
	return VIEW_ARBITRARY_xpm;
}

//----------------------------------------------------------------------------
void albaViewArbitrarySlice::VolumeWindowing(albaVME *volume)
	//----------------------------------------------------------------------------
{
	double sr[2];
	vtkDataSet *data = volume->GetOutput()->GetVTKData();
	data->Update();
	data->GetScalarRange(sr);

	mmaMaterial *currentSurfaceMaterial = m_Slicer->GetMaterial();
	m_ColorLUT = m_Slicer->GetMaterial()->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
	m_LutSlider->SetSubRange((long)currentSurfaceMaterial->m_TableRange[0],(long)currentSurfaceMaterial->m_TableRange[1]);
}

//-------------------------------------------------------------------------
void albaViewArbitrarySlice::CreateGizmos()
{
	if (m_CurrentVolume == NULL)
	{
		albaLogMessage("Generate - Current volume = NULL");
		return;
	}

	albaLogMessage("Generate");

	m_Slicer->SetUpdateVTKPropertiesFromMaterial(false);
	// Create the Gizmos
	m_GizmoTranslate = new albaGizmoTranslate(m_Slicer, this);
	m_GizmoTranslate->SetInput(m_Slicer);
	m_GizmoTranslate->SetRefSys(m_Slicer);
	m_GizmoTranslate->SetAbsPose(m_MatrixReset);
	m_GizmoTranslate->SetStep(X_AXIS, 1.0);
	m_GizmoTranslate->SetStep(Y_AXIS, 1.0);
	m_GizmoTranslate->SetStep(Z_AXIS, 1.0);
	m_GizmoTranslate->Show(true);

	m_GizmoRotate = new albaGizmoRotate(m_Slicer, this);
	m_GizmoRotate->SetInput(m_Slicer);
	m_GizmoRotate->SetRefSys(m_Slicer);
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
}

//-------------------------------------------------------------------------
void albaViewArbitrarySlice::DestroyGizmos()
{
	if (m_CurrentVolume == NULL)
	{
		albaLogMessage("Destroy - Current volume = NULL");
		return;
	}
	albaLogMessage("Destroy");

	// Remove Gizmos
	m_Gui->Remove(m_GuiGizmos);
	m_Gui->Update();

	m_GizmoTranslate->Show(false);
	cppDEL(m_GizmoTranslate);

	m_GizmoRotate->Show(false);
	cppDEL(m_GizmoRotate);

	cppDEL(m_GuiGizmos);

	m_Gui->Update();
}