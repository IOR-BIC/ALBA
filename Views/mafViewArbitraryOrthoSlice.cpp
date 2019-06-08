/*=========================================================================

 Program: MAF2
 Module: mafViewArbitraryOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi
 
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

#include "mafGizmoCrossRotateTranslate.h"
#include "mafGUI.h"
#include "mafDecl.h"
#include "mafViewArbitraryOrthoSlice.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeSlice.h"
#include "mafVMEVolumeGray.h"

#include "mafEvent.h"
#include "mafAttachCamera.h"
#include "mafInteractorGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEIterator.h"
#include "mafVMEOutputSurface.h"
#include "mafAttribute.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"

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
#include "mafTransformFrame.h"
#include "mafVMEGizmo.h"
#include "mafGizmoInterface.h"
#include "mafDataPipe.h"
#include "vtkStructuredPoints.h"
#include "vtkDataSetWriter.h"
#include "vtkUnsignedShortArray.h"
#include "mafRWIBase.h"
#include "wx\busyinfo.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkSphereSource.h"
#include "vtkMAFSmartPointer.h"

mafCxxTypeMacro(mafViewArbitraryOrthoSlice);

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

//----------------------------------------------------------------------------
mafViewArbitraryOrthoSlice::mafViewArbitraryOrthoSlice(wxString label) : mafViewCompoundWindowing(label, 2, 2)
{
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
}
//----------------------------------------------------------------------------
mafViewArbitraryOrthoSlice::~mafViewArbitraryOrthoSlice()
{
	for(int i=X;i<=Z;i++)
	{
// 		cppDEL(m_GizmoRT[i]);
// 		mafDEL(m_Slicer[i]);
		mafDEL(m_SlicerResetMatrix[i]);
// 		mafDEL(m_CameraConeVME[i]);
		delete(m_CameraToSlicer[i]);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PackageView()
{
	m_View3d = new mafViewVTK("",CAMERA_PERSPECTIVE,true,false,0,false,mafAxes::HEAD);
	m_View3d->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
	m_View3d->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);
	PlugChildView(m_View3d);

	CreateAndPlugSliceView(2);
	CreateAndPlugSliceView(0);
	CreateAndPlugSliceView(1);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::CreateAndPlugSliceView(int v)
{
	m_ViewSlice[v] = new mafViewVTK("", CAMERA_OS_X + v, true, false, 0, false, mafAxes::HEAD);
	m_ViewSlice[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);

	m_ViewSlice[v]->PlugVisualPipe("mafVMEImage", "mafPipeBox", NON_VISIBLE);
	m_ViewSlice[v]->PlugVisualPipe("mafVMESegmentationVolume", "mafPipeVolumeOrthoSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMEPolyline", "mafPipePolylineSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMEPolylineSpline", "mafPipePolylineSlice");
	m_ViewSlice[v]->PlugVisualPipe("mafVMEMeter", "mafPipePolyline");
	m_ViewSlice[v]->PlugVisualPipe("medVMEMuscleWrapper", "mafPipeSurfaceSlice");

	PlugChildView(m_ViewSlice[v]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::VmeShow(mafVME *vme, bool show)
{
	if (vme->IsA("mafVMEGizmo"))
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
			mafPipeSlice *pipeSlice = mafPipeSlice::SafeDownCast(m_ChildViewList[view]->GetNodePipe(vme));
			if (pipeSlice)
			{
				double surfaceOriginTranslated[3];
				double normal[3];
				((mafViewSlice*)m_ChildViewList[view])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
				surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
				surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
				surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

				pipeSlice->SetSlice(surfaceOriginTranslated, normal);
			}
		}
	}

	vme->Update();
	if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
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
		for (int i = X; i <= Z; i++)
		{
			if (maf_event->GetSender() == m_GizmoRT[i]->m_GizmoCrossTranslate)
			{
				OnEventGizmoTranslate(maf_event, i);
				return;
			}
			else if (maf_event->GetSender() == m_GizmoRT[i]->m_GizmoCrossRotate) // from rotation gizmo
			{
				OnEventGizmoRotate(maf_event, i);
				return;
			}
		}
			mafEventMacro(*maf_event); 
	}
	else
	{
		Superclass::OnEvent(maf_event);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoTranslate(mafEventBase *maf_event, int side)
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			vtkCamera *zViewCamera = ((mafViewSlice*)m_ChildViewList[Z_VIEW])->GetRWI()->GetCamera();
			int orthoPlanes[2];
			GetOrthoPlanes(side, orthoPlanes);

			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoRT[orthoPlanes[0]]);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoRT[orthoPlanes[1]]);

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
			vtkDEL(tr);

			SetSlices();
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::GetOrthoPlanes(int side, int * orthoPlanes)
{
	if (side == X)
	{
		orthoPlanes[0] = Y;
		orthoPlanes[1] = Z;
	}
	else if (side == Y)
	{
		orthoPlanes[0] = Z;
		orthoPlanes[1] = X;
	}
	else
	{
		orthoPlanes[0] = Y;
		orthoPlanes[1] = X;
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventGizmoRotate(mafEventBase *maf_event, int side)
{
	switch (maf_event->GetId())
	{
		case ID_TRANSFORM:
		{
			// roll the camera based on gizmo
			mafEvent *event = mafEvent::SafeDownCast(maf_event);
			vtkMatrix4x4 *mat = event->GetMatrix()->GetVTKMatrix();
			vtkMAFSmartPointer<vtkTransform> tr;
			tr->SetMatrix(mat);

			int orthoPlanes[2];
			GetOrthoPlanes(side, orthoPlanes);

			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoRT[orthoPlanes[0]]);
			PostMultiplyEventMatrixToGizmoCross(maf_event, m_GizmoRT[orthoPlanes[1]]);
			PostMultiplyEventMatrixToSlicer(maf_event, orthoPlanes[0]);
			PostMultiplyEventMatrixToSlicer(maf_event, orthoPlanes[1]);

			m_ChildViewList[AsixToView(orthoPlanes[0])]->GetRWI()->GetCamera()->ApplyTransform(tr);
			m_ChildViewList[AsixToView(orthoPlanes[1])]->GetRWI()->GetCamera()->ApplyTransform(tr);

			SetSlices();
		}
		break;

		default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::SetSlices()
{
	//update the normal of the cutter plane of the surface
	mafVME *root = m_CurrentVolume->GetRoot();
	for (int view = Z_VIEW; view <= Y_VIEW; view++)
	{
		double surfaceOriginTranslated[3];
		double normal[3];
		((mafViewSlice*)m_ChildViewList[view])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
		surfaceOriginTranslated[0] = m_VolumeVTKDataCenterABSCoords[0] + normal[0] * 0.1;
		surfaceOriginTranslated[1] = m_VolumeVTKDataCenterABSCoords[1] + normal[1] * 0.1;
		surfaceOriginTranslated[2] = m_VolumeVTKDataCenterABSCoords[2] + normal[2] * 0.1;

		mafVMEIterator *iter = root->NewIterator();
		for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
		{
			mafPipeSlice *pipeSlice = mafPipeSlice::SafeDownCast(m_ChildViewList[view]->GetNodePipe(node));
			if (pipeSlice)
				pipeSlice->SetSlice(surfaceOriginTranslated, normal);
		}
		iter->Delete();
	}
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnEventThis(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
		case ID_UPDATE_LUT:
			UpdateSlicersLUT();
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

	m_Gui->Label("");
	m_Gui->Button(ID_RESET,_("Reset slices"),"");
	m_Gui->Divider();

	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);
	m_Gui->Update();
		
	EnableWidgets( (m_CurrentVolume != NULL) );
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::VmeRemove(mafVME *vme)
{
	if (m_CurrentVolume && vme == m_CurrentVolume) 
	{
		HideVolume();
		m_CurrentVolume = NULL;
	}

	Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicers(mafEventBase *maf_event)
{  
	for (int i = 0; i < 3; i++)
		PostMultiplyEventMatrixToSlicer(maf_event, i);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::PostMultiplyEventMatrixToSlicer(mafEventBase *maf_event, int slicerAxis)
{  
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		// handle incoming transform event...
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_Slicer[slicerAxis]->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		mafMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(m_Slicer[slicerAxis]->GetTimeStamp());

		if (e->GetArg() == mafInteractorGenericMouse::MOUSE_MOVE)
		{
			// ... and update the slicer with the new abs pose
			m_Slicer[slicerAxis]->SetAbsMatrix(absPose);
		} 

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateSubviewsCamerasToFaceSlices()
{
	for (int i = X; i <= Z ; i++)
	{
		if (m_CameraToSlicer[i] != NULL)
			m_CameraToSlicer[i]->UpdateCameraMatrix();
	}

	CameraUpdate();
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
	EnableWidgets(m_CurrentVolume != NULL);
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
void mafViewArbitraryOrthoSlice::VolumeWindowing(mafVME *volume)
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
void mafViewArbitraryOrthoSlice::ShowVolume( mafVME * vme, bool show )
{
	if (m_CurrentVolume == vme) return;

	m_CurrentVolume = vme;

	wxBusyInfo wait("please wait");

	for (int i = X; i <= Z; i++)
	{
		mafViewVTK *xView = ((mafViewVTK *)(m_ChildViewList[AsixToView(i)]));
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


	vtkMAFSmartPointer<vtkTransform> slicerTransform;
	for (int i = X; i <= Z; i++)
	{
		slicerTransform->SetMatrix(transformReset->GetMatrix());
		if(i==X)
			slicerTransform->RotateY(89.999);
		else if(i==Y)
			slicerTransform->RotateX(90);
		slicerTransform->Update();

		mafNEW(m_SlicerResetMatrix[i]);
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
void mafViewArbitraryOrthoSlice::HideVolume()
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
		mafDEL(m_CameraConeVME[i]);
		mafDEL(m_Slicer[i]);
		mafDEL(m_SlicerResetMatrix[i]);
	}

	m_CurrentVolume = NULL;
	m_ColorLUT = NULL;
	m_LutWidget->SetLut(m_ColorLUT);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnReset()
{
	for (int i = X; i <= Z; i++)
	{
		m_GizmoRT[i]->SetAbsPose(m_SlicerResetMatrix[i]);
		m_Slicer[i]->SetAbsMatrix(*m_SlicerResetMatrix[i]);
	}

	RestoreCameraParametersForAllSubviews();
	UpdateSlicersLUT();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnLUTRangeModified()
{
	mafVME *vme = GetSceneGraph()->GetSelectedVme();

	if( m_CurrentVolume)
		UpdateSlicersLUT();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::OnLUTChooser()
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
void mafViewArbitraryOrthoSlice::ShowSlicers(mafVME * vmeVolume, bool show)
{
	char slicerNames[3][10] = { "m_SlicerX","m_SlicerY","m_SlicerZ" };
	char gizmoNames[3][13] = { "m_GizmoXView","m_GizmoYView","m_GizmoZView" };
	enum mafGizmoCrossRotateTranslate::COLOR gizmoColors[3][2] = { { mafGizmoCrossRotateTranslate::GREEN, mafGizmoCrossRotateTranslate::BLUE },
																																 { mafGizmoCrossRotateTranslate::BLUE,  mafGizmoCrossRotateTranslate::RED },
																																 { mafGizmoCrossRotateTranslate::GREEN, mafGizmoCrossRotateTranslate::RED } };

	EnableWidgets((m_CurrentVolume != NULL));
	// register sliced volume
	m_InputVolume = mafVMEVolumeGray::SafeDownCast(vmeVolume);
	assert(m_InputVolume);

	for (int i = X; i <= Z; i++)
	{
		if(m_Slicer[i]==NULL)
			mafNEW(m_Slicer[i]);
		m_Slicer[i]->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
		m_Slicer[i]->SetName(slicerNames[i]);
		m_Slicer[i]->ReparentTo(vmeVolume);
		m_Slicer[i]->SetAbsMatrix(*m_SlicerResetMatrix[i]);
		m_Slicer[i]->SetSlicedVMELink(vmeVolume);
		m_Slicer[i]->SetUpdateVTKPropertiesFromMaterial(false);
		m_Slicer[i]->GetMaterial()->m_ColorLut->DeepCopy(mafVMEVolumeGray::SafeDownCast(m_CurrentVolume)->GetMaterial()->m_ColorLut);
		m_Slicer[i]->Update();
		m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(m_Slicer[i], show);
		m_ChildViewList[AsixToView(i)]->VmeShow(m_Slicer[i], show);
		BuildCameraConeVME(i);
		m_CameraToSlicer[i] = new mafAttachCamera(m_Gui, ((mafViewVTK*)m_ChildViewList[AsixToView(i)])->m_Rwi, this);
		m_CameraToSlicer[i]->SetStartingMatrix(m_Slicer[i]->GetOutput()->GetAbsMatrix());
		m_CameraToSlicer[i]->SetVme(m_Slicer[Z]);
		m_CameraToSlicer[i]->EnableAttachCamera();

	}

	ResetCameraToSlices();
		
	for (int i = X; i <= Z; i++)
	{
		if(m_GizmoRT[i]==NULL)
		{
			m_GizmoRT[i] = new mafGizmoCrossRotateTranslate();
			m_GizmoRT[i]->Create(m_Slicer[i], this, true, i);
		}

		m_GizmoRT[i]->SetName(gizmoNames[i]);
		m_GizmoRT[i]->SetInput(m_Slicer[i]);
		m_GizmoRT[i]->SetRefSys(m_Slicer[i]);
		m_GizmoRT[i]->SetAbsPose(m_SlicerResetMatrix[i]);
		m_GizmoRT[i]->SetColor(mafGizmoCrossRotateTranslate::GREW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(mafGizmoCrossRotateTranslate::GTAEW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(mafGizmoCrossRotateTranslate::GTPEW, gizmoColors[i][0]);
		m_GizmoRT[i]->SetColor(mafGizmoCrossRotateTranslate::GRNS, gizmoColors[i][1]);
		m_GizmoRT[i]->SetColor(mafGizmoCrossRotateTranslate::GTANS, gizmoColors[i][1]);
		m_GizmoRT[i]->SetColor(mafGizmoCrossRotateTranslate::GTPNS, gizmoColors[i][1]);
		m_GizmoRT[i]->Show(true);
		m_Slicer[i]->SetVisibleToTraverse(false);
	}

	UpdateSubviewsCamerasToFaceSlices();
	CreateViewCameraNormalFeedbackActors();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::BuildCameraConeVME(int side)
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
		mafNEW(m_CameraConeVME[side]);

	// DEBUG
	m_CameraConeVME[side]->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
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

	mafPipeSurface *pipeY = (mafPipeSurface *)(m_ChildViewList[PERSPECTIVE_VIEW])->GetNodePipe(m_CameraConeVME[side]);
	pipeY->SetActorPicking(false);

	cameraConeSource->Delete();
}
//----------------------------------------------------------------------------
bool mafViewArbitraryOrthoSlice::BelongsToNormalGizmo( mafVME * vme, int side)
{
	char nameVect[][13] = { "m_GizmoXView", "m_GizmoYView", "m_GizmoZView" };
	char *name=nameVect[side];

	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(vme);

	mafObserver *mediator = NULL;
	mediator = gizmo->GetMediator();
	
	mafGizmoInterface *gizmoMediator = NULL;
	gizmoMediator = dynamic_cast<mafGizmoInterface *>(mediator);
	if (gizmoMediator && gizmoMediator->GetName().Equals(name))
		return true;
	else
		return false;
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
	absPose.SetTimeStamp(m_GizmoRT[1]->GetAbsPose()->GetTimeStamp());

	targetGizmo->SetAbsPose(&absPose);

	vtkDEL(tr1);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::ResetCameraToSlices()
{
	for(int i=0;i<=Z;i++)
		((mafViewVTK*)m_ChildViewList[AsixToView(i)])->CameraReset(m_Slicer[i]);
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::StoreCameraParametersForAllSubviews()
{
	for (int i = 0; i <= Z; i++)
	{
		((mafViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetPosition(m_CameraPositionForReset[i]);
		((mafViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetFocalPoint(m_CameraFocalPointForReset[i]);
		((mafViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera()->GetViewUp(m_CameraViewUpForReset[i]);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::RestoreCameraParametersForAllSubviews()
{
	for (int i = 0; i <= Z; i++)
	{
		vtkCamera *xViewCamera = ((mafViewSlice*)m_ChildViewList[AsixToView(i)])->GetRWI()->GetCamera();
		xViewCamera->SetPosition(m_CameraPositionForReset[i]);
		xViewCamera->SetFocalPoint(m_CameraFocalPointForReset[i]);
		xViewCamera->SetViewUp(m_CameraViewUpForReset[i]);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateSlicers(int axis)
{
	m_Slicer[axis]->SetAbsMatrix(m_Slicer[axis]->GetAbsMatrixPipe()->GetMatrix());
	m_Slicer[axis]->GetSurfaceOutput()->GetVTKData()->Modified();
	m_Slicer[axis]->GetSurfaceOutput()->GetVTKData()->Update();
}
//----------------------------------------------------------------------------
mafPipeSurface * mafViewArbitraryOrthoSlice::GetPipe(int inView, mafVMESurface *inSurface)
{
	return (mafPipeSurface *)((m_ChildViewList[inView])->GetNodePipe(inSurface));
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::CreateViewCameraNormalFeedbackActors()
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

		((mafViewVTK*)(m_ChildViewList[AsixToView(i)]))->m_Rwi->m_RenFront->AddActor2D(border);

		vtkDEL(ss);
		vtkDEL(coord);
		vtkDEL(pdmd);
		vtkDEL(pd);
		vtkDEL(border);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateSlicersLUT()
{
	double low, hi;
	m_LutSlider->GetSubRange(&low,&hi);
	m_ColorLUT->SetTableRange(low,hi);

	for (int i = X; i <= Z; i++)
	{
		mafVMEOutputSurface *surfaceOutputSlicer = mafVMEOutputSurface::SafeDownCast(m_Slicer[i]->GetOutput());
		surfaceOutputSlicer->Update();
		surfaceOutputSlicer->GetMaterial()->m_ColorLut->SetTableRange(low, hi);
	}

	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewArbitraryOrthoSlice::UpdateWindowing(bool enable,mafVME *vme)
{
	if(vme->GetOutput() && vme->GetOutput()->IsA("mafVMEOutputVolume") && enable)
		VolumeWindowing(vme);
}
