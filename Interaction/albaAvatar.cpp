/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar
 Authors: Marco Petrone & Michele Diegoli
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// base includes
#include "albaAvatar.h"

// interactors & devices
#include "albaDeviceButtonsPadTracker.h"

// gui
#include "albaGUI.h"

// events
#include "albaEventInteraction.h"
#include "albaEvent.h"
#include "mmuIdFactory.h"

// Input Transformations
#include "albaMatrix.h"
#include "albaTransform.h"
//#include "albaCameraTransform.h"
#include "albaOBB.h"

// Serialization
#include "albaStorageElement.h"

// Visualization
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkProp3D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkProperty.h"

// system includes
#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
  ALBA_ID_IMP(AVATAR_SCREEN_PLANE_SETTINGS);
  ALBA_ID_IMP(AVATAR_WORKING_BOX_SETTINGS);

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaAvatar)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAvatar::albaAvatar()
//------------------------------------------------------------------------------
{
  m_View                  = NULL;
  m_Cursor3DHideCounter   = 0;
  m_Cursor2DHideCounter   = 0;
  m_Mode                  = MODE_3D;
  m_Gui                   = NULL;
}

//------------------------------------------------------------------------------
albaAvatar::~albaAvatar()
//------------------------------------------------------------------------------
{
  Shutdown();
  SetActor3D(NULL);
  cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
albaGUI *albaAvatar::GetGui()
//----------------------------------------------------------------------------
{
  if (!m_Gui)
    CreateGui();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaAvatar::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  if (m_Gui==NULL)
  {
    m_Gui=new albaGUI(this);
    assert(m_Gui);

    m_Gui->Divider(2);
    m_Gui->Label("Avatar Settings", true);
    m_Gui->Divider();
    m_Gui->String(ID_NAME,"name",&m_Name);
    m_Gui->Divider();
  }
  
}

//----------------------------------------------------------------------------
void albaAvatar::UpdateGui()
//----------------------------------------------------------------------------
{
  if (m_Gui)
    m_Gui->Update();
}

//------------------------------------------------------------------------------
albaDeviceButtonsPadTracker *albaAvatar::GetTracker()
//------------------------------------------------------------------------------
{
  return albaDeviceButtonsPadTracker::SafeDownCast(GetDevice());
}
//------------------------------------------------------------------------------
void albaAvatar::SetTracker(albaDeviceButtonsPadTracker *tracker)
//------------------------------------------------------------------------------
{
  // Note tracker is always set before initializing...
  SetDevice(tracker);
}

//------------------------------------------------------------------------------
void albaAvatar::SetActor3D(vtkProp3D *avatar)
//------------------------------------------------------------------------------
{
  // If already initialized we have to shutdown and then restart
  int old_flag=m_Initialized;
  if (old_flag)
  {
    Shutdown();
  }

  m_Actor3D=avatar;

  if (old_flag)
  {
    Initialize();
  }
}

//------------------------------------------------------------------------------
void albaAvatar::SetActor2D(vtkActor2D *avatar)
//------------------------------------------------------------------------------
{
  // If already initialized we have to shutdown and then restart
  int old_flag=m_Initialized;
  if (old_flag)
  {
    Shutdown();
  }

  m_Actor2D=avatar;

  if (old_flag)
  {
    Initialize();
  }
}

//------------------------------------------------------------------------------
void albaAvatar::SetRendererAndView(vtkRenderer *ren, albaView *view)
{
  // If already initialized we have to shutdown and then restart
  int old_flag=m_Initialized; 
  if (old_flag)
  {
    Shutdown();
  }
  
	Superclass::SetRendererAndView(ren, view);
  
  if (old_flag)
  {
    Initialize();
  }
}

//------------------------------------------------------------------------------
int albaAvatar::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (m_Renderer)
  { 
    if (m_Actor3D)
    {
      m_Renderer->AddActor(m_Actor3D);
    }
    if(m_Actor2D) 
    {
      m_Renderer->AddActor2D(m_Actor2D);
    }
  }

  return ALBA_OK;
}
//------------------------------------------------------------------------------
void albaAvatar::InternalShutdown()
//------------------------------------------------------------------------------
{
  if (m_Renderer)
  { 
    if (m_Actor3D)
    {
      m_Renderer->RemoveActor(m_Actor3D);
    }
    if(m_Actor2D) 
    {
      m_Renderer->RemoveActor2D(m_Actor2D);
    }
  }
  
  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
void albaAvatar::Hide()
//------------------------------------------------------------------------------
{
  HideCursor3D();

  HideCursor2D();
}

//------------------------------------------------------------------------------
void albaAvatar::Show()
//------------------------------------------------------------------------------
{
  ShowCursor3D();
  ShowCursor2D();
}

//------------------------------------------------------------------------------
void albaAvatar::HideCursor3D()
//------------------------------------------------------------------------------
{
  if (m_Actor3D)
  {
    if (m_Cursor3DHideCounter==0)
      m_Actor3D->SetVisibility(0);

    m_Cursor3DHideCounter++;
  }
}

//------------------------------------------------------------------------------
void albaAvatar::ShowCursor3D()
//------------------------------------------------------------------------------
{
  if (m_Cursor3DHideCounter>0)
    m_Cursor3DHideCounter--;

  if (m_Actor3D&& m_Cursor3DHideCounter==0)
  {
    this->m_Actor3D->SetVisibility(1);
  }
}

//------------------------------------------------------------------------------
void albaAvatar::HideCursor2D()
//------------------------------------------------------------------------------
{
  if (m_Actor2D)
  {
    if (m_Cursor2DHideCounter==0)
      m_Actor2D->SetVisibility(0);

    m_Cursor2DHideCounter++;
  }
}

//------------------------------------------------------------------------------
void albaAvatar::ShowCursor2D()
//------------------------------------------------------------------------------
{
  if (m_Cursor2DHideCounter>0)
    m_Cursor2DHideCounter--;

  if (m_Actor2D&& m_Cursor2DHideCounter==0)
  {
    this->m_Actor2D->SetVisibility(1);
  }
}

//------------------------------------------------------------------------------
int albaAvatar::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  return node->StoreText("Name",m_Name);
}

//------------------------------------------------------------------------------
int albaAvatar::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  return node->RestoreText("Name",m_Name);
}

//------------------------------------------------------------------------------
void albaAvatar::OnPreResetCamera(albaEventBase *event)
//------------------------------------------------------------------------------
{
  if (m_Renderer&&(vtkRenderer *)event->GetData()==m_Renderer.GetPointer())
    Hide(); // Hide to avoid this avatar influence the Frustom size
}
//------------------------------------------------------------------------------
void albaAvatar::OnPostResetCamera(albaEventBase *event)
//------------------------------------------------------------------------------
{
  if (m_Renderer&&(vtkRenderer *)event->GetData()==m_Renderer.GetPointer())
    Show(); // Display again the avatar
}
//------------------------------------------------------------------------------
void albaAvatar::OnViewSelected(albaEvent *e)
//------------------------------------------------------------------------------
{
  SetRendererAndView((vtkRenderer *)e->GetData(), e->GetView());
}
//------------------------------------------------------------------------------
void albaAvatar::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  albaID id=event->GetId();
  
  if (id==CAMERA_PRE_RESET)
  {
    OnPreResetCamera(event);
  }
  else if (id==CAMERA_POST_RESET)
  {
    OnPostResetCamera(event);
  }
  else if (id==VIEW_SELECT)
  {
    albaEvent *e=albaEvent::SafeDownCast(event);
    OnViewSelected(e);
  }
}
