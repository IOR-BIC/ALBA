/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewImage
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


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

#include "albaGUI.h"
#include "albaViewImage.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVME.h"
#include "albaVMEVolume.h"
#include "albaVMESlicer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMESurface.h"
#include "albaPipeFactory.h"
#include "albaPipe.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaAttachCamera.h"

#include "vtkDataSet.h"
#include "vtkALBARayCast3DPicker.h"
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
albaCxxTypeMacro(albaViewImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewImage::albaViewImage(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo)
:albaViewVTK(label,camera_position,show_axes,show_grid, stereo)
//----------------------------------------------------------------------------
{
  m_CurrentImage = NULL;
	m_CanSpin = false;
}
//----------------------------------------------------------------------------
albaViewImage::~albaViewImage()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
albaView *albaViewImage::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewImage *v = new albaViewImage(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
	v->m_CanSpin = m_CanSpin;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewImage::Create()
//----------------------------------------------------------------------------
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  RWI_LAYERS num_layers = ONE_LAYER;
  
  m_Rwi = new albaRWI(albaGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new albaSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack, m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->InitializePickList();

	PlugVisualPipe("albaVMEImage", "albaPipeImage3D", MUTEX);

}

//-------------------------------------------------------------------------
albaGUI *albaViewImage::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

  m_AttachCamera = new albaAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
	m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewImage::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  /*if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      default:
        albaEventMacro(*alba_event);
      break;
    }
  }
  else
  {
    albaEventMacro(*alba_event);
  }*/
  //albaEventMacro(*alba_event);
	Superclass::OnEvent(alba_event);
}
//----------------------------------------------------------------------------
void albaViewImage::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
  if (vme->IsALBAType(albaVMEImage))
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
		Superclass::VmeShow(vme, show);
	}
}
//----------------------------------------------------------------------------
void albaViewImage::VmeDeletePipe(albaVME *vme)
//----------------------------------------------------------------------------
{
	albaViewVTK::VmeDeletePipe(vme);
	if (m_AttachCamera)
	{
		m_AttachCamera->SetVme(NULL);
	}
}