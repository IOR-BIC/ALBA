/*=========================================================================

 Program: MAF2
 Module: mafViewImage
 Authors: Daniele Giunchi
 
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

#include "mafGUI.h"
#include "mafViewImage.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"

#include "vtkDataSet.h"
#include "vtkMAFRayCast3DPicker.h"
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
mafViewImage::mafViewImage(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid, stereo)
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
mafView *mafViewImage::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewImage *v = new mafViewImage(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewImage::Create()
//----------------------------------------------------------------------------
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  RWI_LAYERS num_layers = ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
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
int mafViewImage::GetNodeStatus(mafVME *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    n = m_Sg->Vme2Node(vme);
    if (vme->IsMAFType(mafVMEImage) || vme->IsA("mafVMEAdvancedProber") || vme->IsMAFType(mafVMESurface))
    {
      if (n != NULL)
      {
      	n->SetMutex(true);
      }
    }
    else
    {
      if (n != NULL)
      {
      	n->SetPipeCreatable(false);
      }
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mafGUI *mafViewImage::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

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
  //mafEventMacro(*maf_event);
	Superclass::OnEvent(maf_event);
}
//----------------------------------------------------------------------------
void mafViewImage::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  if (vme->IsMAFType(mafVMEImage) || vme->IsA("mafVMEAdvancedProber") || vme->IsMAFType(mafVMESurface))
  {
    if (show)
    {
      m_AttachCamera->SetVme(vme);
      Superclass::VmeShow(vme, show);
    }
    else
    {
      m_AttachCamera->SetVme(NULL);
      Superclass::VmeShow(vme, show);
    }
    CameraUpdate();
    m_Rwi->CameraReset(vme);
    m_Rwi->CameraUpdate();
  }
  else
	{
    m_AttachCamera->SetVme(NULL);
	}
  
}
//----------------------------------------------------------------------------
void mafViewImage::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
	mafViewVTK::VmeDeletePipe(vme);
	if (m_AttachCamera)
	{
		m_AttachCamera->SetVme(NULL);
	}
}