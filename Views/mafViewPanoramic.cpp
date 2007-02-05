/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewPanoramic.cpp,v $
Language:  C++
Date:      $Date: 2007-02-05 20:48:09 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
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

#include "mafViewPanoramic.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEAdvancedProber.h"
#include "mafVMESurface.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mafGizmoSlice.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"

const int CT_GIZMO_NUMBER  = 1;


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewPanoramic);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewPanoramic::mafViewPanoramic(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid, show_ruler, stereo)
//----------------------------------------------------------------------------
{
	m_GizmoColor[0][0] = 1; m_GizmoColor[0][1] = 0; m_GizmoColor[0][2] = 0;
	//m_GizmoColor[1][0] = 0; m_GizmoColor[1][1] = 1; m_GizmoColor[1][2] = 0;

	for(int j=0; j<CT_GIZMO_NUMBER; j++) 
	{
		m_GizmoSlice[j] = NULL;
		m_Pos[j]=0;
	}
}
//----------------------------------------------------------------------------
mafViewPanoramic::~mafViewPanoramic()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafView *mafViewPanoramic::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	mafViewPanoramic *v = new mafViewPanoramic(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	v->m_PipeMap = m_PipeMap;
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
void mafViewPanoramic::Create()
//----------------------------------------------------------------------------
{
	RWI_LAYERS num_layers = ONE_LAYER;

	m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
	m_Rwi->SetListener(this);
	m_Rwi->CameraSet(m_CameraPosition);
	m_Win = m_Rwi->m_RwiBase;

	m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
	m_Sg->SetListener(this);
	m_Rwi->m_Sg = m_Sg;

	vtkNEW(m_Picker3D);
	vtkNEW(m_Picker2D);
	m_Picker2D->InitializePickList();

}
//-------------------------------------------------------------------------
int mafViewPanoramic::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
	mafSceneNode *n = NULL;
	if (m_Sg != NULL)
	{
		if (vme->IsMAFType(mafVMEImage) || vme->IsA("mafVMEAdvancedProber") || vme->IsMAFType(mafVMESurface))
		{
			n = m_Sg->Vme2Node(vme);
			n->m_Mutex = true;
		}
		else
		{
			n = m_Sg->Vme2Node(vme);
			n->m_PipeCreatable = false;
		}
	}

	return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mmgGui *mafViewPanoramic::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);
	m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
	m_Gui->AddGui(m_AttachCamera->GetGui());
	m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewPanoramic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
void mafViewPanoramic::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	if (node->IsMAFType(mafVMEImage) || node->IsMAFType(mafVMESurface))
	{
		if (show)
		{
			m_AttachCamera->SetVme(node);
			Superclass::VmeShow(node, show);
		}
		else
		{
			m_AttachCamera->SetVme(NULL);
			Superclass::VmeShow(node, show);
		}
		CameraUpdate();
		m_Rwi->CameraReset(node);
		m_Rwi->CameraUpdate();
	}
	else if (node->IsA("mafVMEAdvancedProber"))
	{
		m_CurrentPanoramic = mafVMEAdvancedProber::SafeDownCast(node);
		GizmoCreate();
	}
	else
	{
		m_AttachCamera->SetVme(NULL);
	}

}
//----------------------------------------------------------------------------
void mafViewPanoramic::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
	mafViewVTK::VmeDeletePipe(vme);
	if (m_AttachCamera)
	{
		m_AttachCamera->SetVme(NULL);
	}
}
//----------------------------------------------------------------------------
void mafViewPanoramic::GizmoCreate()
//----------------------------------------------------------------------------
{
	for(int i=0; i<CT_GIZMO_NUMBER; i++) 
	{
		double slice[3];
		/*mafPipeVolumeSlice *p = NULL;
		p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafViewCompound *)m_ChildViewList[CT_COMPOUND_VIEW])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
		p->GetSliceOrigin(slice);*/
		double b[6];
		m_CurrentPanoramic->GetOutput()->GetVTKData()->GetBounds(b);
		m_GizmoSlice[i] = new mafGizmoSlice(m_CurrentPanoramic, this);
		m_GizmoSlice[i]->CreateGizmoSliceInLocalPositionOnAxis(i,mafGizmoSlice::GIZMO_SLICE_Z,b[2]);
		m_GizmoSlice[i]->SetColor(m_GizmoColor[i]);

		this->VmeShow(m_GizmoSlice[i]->GetOutput(), true);
	}
}