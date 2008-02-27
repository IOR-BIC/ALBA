/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewImage.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-27 13:17:43 $
  Version:   $Revision: 1.5 $
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

#include "mafViewImage.h"
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


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewImage::mafViewImage(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid, show_ruler, stereo)
//----------------------------------------------------------------------------
{
  m_CurrentImage = NULL;
}
//----------------------------------------------------------------------------
mafViewImage::~mafViewImage()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafView *mafViewImage::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewImage *v = new mafViewImage(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewImage::Create()
//----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->InitializePickList();

}
//-------------------------------------------------------------------------
int mafViewImage::GetNodeStatus(mafNode *vme)
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
mmgGui *mafViewImage::CreateGui()
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
void mafViewImage::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  /*if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      default:
        mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }*/
  mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
void mafViewImage::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  if (node->IsMAFType(mafVMEImage) || node->IsA("mafVMEAdvancedProber") || node->IsMAFType(mafVMESurface))
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
  else
	{
    m_AttachCamera->SetVme(NULL);
	}
  
}
//----------------------------------------------------------------------------
void mafViewImage::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
	mafViewVTK::VmeDeletePipe(vme);
	if (m_AttachCamera)
	{
		m_AttachCamera->SetVme(NULL);
	}
}