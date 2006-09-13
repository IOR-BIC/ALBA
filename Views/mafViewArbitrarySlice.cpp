/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewArbitrarySlice.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-13 13:47:53 $
  Version:   $Revision: 1.1 $
  Authors:   Matteo Giacomoni
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

#include "mafViewArbitrarySlice.h"
#include "mafViewArbitrary.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafSceneGraph.h"
#include "mafEvent.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttachCamera.h"
#include "mmiGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafNodeIterator.h"
#include "mmgLutPreset.h"
#include "mafVMEOutputSurface.h"

#include "vtkTransform.h"
#include "vtkLookupTable.h"
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

enum
{
  ARBITRARY_VIEW = 0,
	SLICE_VIEW,
};
enum
{
	GIZMO_TRANSLATE = 0,
	GIZMO_ROTATE,
};
enum
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
};

//----------------------------------------------------------------------------
mafViewArbitrarySlice::mafViewArbitrarySlice(wxString label, bool show_ruler)
: mafViewCompound(label, 1, 2)
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = NULL;
	m_ViewSlice = NULL;
	m_GizmoTranslate = NULL;
	m_GizmoRotate = NULL;
	m_MatrixReset = NULL;
	m_CurrentVolume = NULL;
	m_Slicer = NULL;
	m_GuiGizmos = NULL;

	m_SliceCenterSurface[0] = 0.0;
	m_SliceCenterSurface[1] = 0.0;
	m_SliceCenterSurface[2] = 0.0;

	m_SliceCenterSurfaceReset[0] = 0.0;
	m_SliceCenterSurfaceReset[1] = 0.0;
	m_SliceCenterSurfaceReset[2] = 0.0;

	m_TypeGizmo = GIZMO_TRANSLATE;

}
//----------------------------------------------------------------------------
mafViewArbitrarySlice::~mafViewArbitrarySlice()
//----------------------------------------------------------------------------
{
	m_MatrixReset = NULL;
	m_CurrentVolume = NULL;

	if(m_GizmoTranslate)
	{
		m_GizmoTranslate->Show(false);
		cppDEL(m_GizmoTranslate);
	}
	if(m_GizmoRotate)
	{
		m_GizmoRotate->Show(false);
		cppDEL(m_GizmoRotate);
	}
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::PackageView()
//----------------------------------------------------------------------------
{
	m_ViewArbitrary = new mafViewVTK("",CAMERA_PERSPECTIVE);
	m_ViewArbitrary->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewArbitrary->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", MUTEX);
	m_ViewSlice = new mafViewVTK("",CAMERA_CT);
	m_ViewSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	m_ViewSlice->PlugVisualPipe("mafVMEGizmo", "mafPipeGizmo", NON_VISIBLE);
	m_ViewSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeBox", NON_VISIBLE);
	
	PlugChildView(m_ViewArbitrary);
	PlugChildView(m_ViewSlice);

}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	mafVME *Vme=mafVME::SafeDownCast(node);
	Vme->Update();
	if (show)
	{
		if(Vme->IsA("mafVMEVolumeGray"))
		{
			double sr[2],SliceCenterVolumeReset[3];
			mafVMEVolumeGray *Volume=mafVMEVolumeGray::SafeDownCast(Vme);
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

			//((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->ApplyTransform(transform);
		
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

			m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
			m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);

			vtkNEW(m_vtkLUT);
      m_vtkLUT->SetRange(sr);
      m_vtkLUT->Build();
      lutPreset(4,m_vtkLUT);

			m_CurrentVolume->GetMaterial()->m_ColorLut=m_vtkLUT;
			m_CurrentVolume->Update();

			mafNEW(m_Slicer);
			m_Slicer->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_Slicer->ReparentTo(mafVME::SafeDownCast(node));
			m_Slicer->SetPose(m_SliceCenterSurfaceReset,m_SliceAngleReset,0);
			m_Slicer->SetName("Slicer");
			//m_Slicer->GetMaterial()->m_ColorLut=m_CurrentVolume->GetMaterial()->m_ColorLut;
			((mafVMEOutputSurface*)m_Slicer->GetOutput())->GetTexture()->UpdateData();
			m_Slicer->GetMaterial()->UpdateProp();
			m_Slicer->SetAbsMatrix(*m_MatrixReset);
			m_Slicer->Update();
			double sr2[2];
			m_Slicer->GetMaterial()->GetMaterialTexture()->GetScalarRange(sr2);
			m_ChildViewList[ARBITRARY_VIEW]->VmeShow(m_Slicer, show);
			m_ChildViewList[SLICE_VIEW]->VmeShow(m_Slicer, show);
			((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->GetAttachCamera()->SetVme(m_Slicer);
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
				m_GuiGizmos = new mmgGui(this);
			m_GuiGizmos->AddGui(m_GizmoTranslate->GetGui());
			m_GuiGizmos->AddGui(m_GizmoRotate->GetGui());
			m_GuiGizmos->Update();
			m_Gui->AddGui(m_GuiGizmos);
			m_Gui->FitGui();
			m_Gui->Update();

			vtkDEL(pts);
			vtkDEL(pd);
			vtkDEL(transform);
			vtkDEL(filter);
			vtkDEL(TransformReset);
		}
		else if(Vme->IsA("mafVMEGizmo"))
		{
			m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
		}
		else if(Vme->IsA("mafVMESurface"))
		{
			//a surface is visible only if there is a volume in the view
			if(m_CurrentVolume)
			{
				m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
				m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);

				double normal[3];
				((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

				mafPipeSurfaceSlice *PipeArbitraryViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
				PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
				PipeArbitraryViewSurface->SetNormal(normal);
				mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
				PipeSliceViewSurface->SetSlice(m_SliceCenterSurface);
				PipeSliceViewSurface->SetNormal(normal);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
		}
		else//another type of vme
		{
			m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
			m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);
		}
	}
	else//if show=false
	{
		m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
		m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);

		if(Vme->IsA("mafVMEVolumeGray"))
		{
			m_Slicer->ReparentTo(NULL);
			((mafViewVTK*)m_ChildViewList[SLICE_VIEW])->GetAttachCamera()->SetVme(NULL);

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

			//find if some surfaces are show , if yes show off
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *Inode = iter->GetFirstNode(); Inode; Inode = iter->GetNextNode())
			{
				if(Inode->IsA("mafVMESurface"))
				{
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(Inode));
					mafPipeSurfaceSlice *PipeArbitraryViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(Inode));
					if(PipeSliceViewSurface && PipeArbitraryViewSurface)
					{
						mafEventMacro(mafEvent(this, VME_SHOW, Inode, false));
					}
				}
			}
			iter->Delete();
			m_CurrentVolume = NULL;
		}
	}
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (maf_event->GetSender() == this->m_Gui) // from this view gui
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
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
			m_GizmoRotate->SetAbsPose(matrix);

			//for each surface visualized change the center of the cut plane
			mafNode *root=m_CurrentVolume->GetRoot();
			mafNodeIterator *iter = root->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface"))
				{
					mafPipeSurfaceSlice *PipeArbitraryViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if(PipeArbitraryViewSurface && PipeSliceViewSurface)
					{
						PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewSurface->SetSlice(m_SliceCenterSurface);
					}
				}
			}
			iter->Delete();

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
				if(node->IsA("mafVMESurface"))
				{
					mafPipeSurfaceSlice *PipeArbitraryViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if(PipeArbitraryViewSurface && PipeArbitraryViewSurface)
					{
						double normal[3];
						((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
						PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewSurface->SetNormal(normal);
					}
				}
			}
			iter->Delete();
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
					m_GizmoRotate->Show(false);
				}
				else if(m_TypeGizmo == GIZMO_ROTATE)
				{
					m_GizmoTranslate->Show(false);
					m_GizmoRotate->Show(true);
				}
			}
			break;
		case ID_RESET:
			{
			m_SliceCenterSurface[0]=m_SliceCenterSurfaceReset[0];
			m_SliceCenterSurface[1]=m_SliceCenterSurfaceReset[1];
			m_SliceCenterSurface[2]=m_SliceCenterSurfaceReset[2];
			
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
				if(node->IsA("mafVMESurface"))
				{
					mafPipeSurfaceSlice *PipeArbitraryViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[ARBITRARY_VIEW])->GetNodePipe(node));
					mafPipeSurfaceSlice *PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(((mafViewSlice *)m_ChildViewList[SLICE_VIEW])->GetNodePipe(node));
					if(PipeArbitraryViewSurface && PipeArbitraryViewSurface)
					{
						double normal[3];
						((mafViewSlice*)m_ChildViewList[SLICE_VIEW])->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
						PipeArbitraryViewSurface->SetNormal(normal);
						PipeSliceViewSurface->SetNormal(normal);
						PipeArbitraryViewSurface->SetSlice(m_SliceCenterSurface);
						PipeSliceViewSurface->SetSlice(m_SliceCenterSurface);
					}
				}
			}
			iter->Delete();
			break;
			}
			default:
        mafViewCompound::OnEvent(maf_event);
		}
	}
}
//----------------------------------------------------------------------------
mafView *mafViewArbitrarySlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
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
mmgGui* mafViewArbitrarySlice::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);

	m_ChildViewList[SLICE_VIEW]->GetGui();

	//combo box to choose the type of gizmo
	wxString Text[2]={"Gizmo Translation","Gizmo Rotation"};
	m_Gui->Combo(ID_COMBO_GIZMOS,"Gizmo",&m_TypeGizmo,2,Text);
	//button to reset at the star position
	m_Gui->Button(ID_RESET,"Reset","");

	m_Gui->Divider(2);
	m_Gui->Update();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewArbitrarySlice::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && node == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    m_GizmoTranslate->Show(false);
		cppDEL(m_GizmoTranslate);
		m_GizmoRotate->Show(false);
		cppDEL(m_GizmoRotate);
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

    if (arg == mmiGenericMouse::MOUSE_MOVE)
    {
      // move vme
      ((mafVME *)m_Slicer)->SetAbsMatrix(absPose);
    } 

    // clean up
    tr->Delete();
  }
}