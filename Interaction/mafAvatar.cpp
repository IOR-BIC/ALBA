/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-02 15:18:15 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone & Michele Diegoli
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// base includes
#include "mafAvatar.h"

// interactors & devices
#include "mmdTracker.h"

// gui
#include "mmgGui.h"

// events
#include "mafEventInteraction.h"
#include "mafEvent.h"
#include "mmuIdFactory.h"

// Input Transformations
#include "mafMatrix.h"
#include "mafTransform.h"
//#include "mafCameraTransform.h"
#include "mafOBB.h"

// Serialization
#include "mafStorageElement.h"

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
  MAF_ID_IMP(AVATAR_SCREEN_PLANE_SETTINGS);
  MAF_ID_IMP(AVATAR_WORKING_BOX_SETTINGS);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAvatar)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAvatar::mafAvatar()
//------------------------------------------------------------------------------
{
  m_View                  = NULL;
  m_Cursor3DHideCounter   = 0;
  m_Cursor2DHideCounter   = 0;
  m_Mode                  = MODE_3D;
  m_Gui                   = NULL;
}

//------------------------------------------------------------------------------
mafAvatar::~mafAvatar()
//------------------------------------------------------------------------------
{
  Shutdown();
  SetActor3D(NULL);
  cppDEL(m_Gui);
}

//----------------------------------------------------------------------------
mmgGui *mafAvatar::GetGui()
//----------------------------------------------------------------------------
{
  if (!m_Gui)
    CreateGui();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafAvatar::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(NULL);

  if (m_Gui==NULL)
  {
    m_Gui=new mmgGui(this);
    assert(m_Gui);

    m_Gui->Divider(2);
    m_Gui->Label("Avatar Settings", true);
    m_Gui->Divider();
    m_Gui->String(ID_NAME,"name",&m_Name);
    m_Gui->Divider();
  }
  
}

//----------------------------------------------------------------------------
void mafAvatar::UpdateGui()
//----------------------------------------------------------------------------
{
  if (m_Gui)
    m_Gui->Update();
}

//------------------------------------------------------------------------------
mmdTracker *mafAvatar::GetTracker()
//------------------------------------------------------------------------------
{
  return mmdTracker::SafeDownCast(GetDevice());
}
//------------------------------------------------------------------------------
void mafAvatar::SetTracker(mmdTracker *tracker)
//------------------------------------------------------------------------------
{
  // Note tracker is always set before initializing...
  SetDevice(tracker);
}

//------------------------------------------------------------------------------
void mafAvatar::SetActor3D(vtkProp3D *avatar)
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
void mafAvatar::SetActor2D(vtkActor2D *avatar)
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
void mafAvatar::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  // If already initialized we have to shutdown and then restart
  int old_flag=m_Initialized; 
  if (old_flag)
  {
    Shutdown();
  }
  
  m_Renderer=ren;
  //CanonicalToWorldTransform->SetRenderer(ren);
  
  if (old_flag)
  {
    Initialize();
  }
}

//------------------------------------------------------------------------------
void mafAvatar::SetView(mafView *view)
//------------------------------------------------------------------------------
{
  m_View=view;
}

//------------------------------------------------------------------------------
int mafAvatar::InternalInitialize()
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

  return MAF_OK;
}
//------------------------------------------------------------------------------
void mafAvatar::InternalShutdown()
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
void mafAvatar::Hide()
//------------------------------------------------------------------------------
{
  HideCursor3D();

  HideCursor2D();
}

//------------------------------------------------------------------------------
void mafAvatar::Show()
//------------------------------------------------------------------------------
{
  ShowCursor3D();
  ShowCursor2D();
}

//------------------------------------------------------------------------------
void mafAvatar::HideCursor3D()
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
void mafAvatar::ShowCursor3D()
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
void mafAvatar::HideCursor2D()
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
void mafAvatar::ShowCursor2D()
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
int mafAvatar::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  return node->StoreText("Name",m_Name);
}

//------------------------------------------------------------------------------
int mafAvatar::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  return node->RestoreText("Name",m_Name);
}

//------------------------------------------------------------------------------
void mafAvatar::OnPreResetCamera(mafEventBase *event)
//------------------------------------------------------------------------------
{
  if (m_Renderer&&(vtkRenderer *)event->GetData()==m_Renderer.GetPointer())
    Hide(); // Hide to avoid this avatar influence the Frustom size
}
//------------------------------------------------------------------------------
void mafAvatar::OnPostResetCamera(mafEventBase *event)
//------------------------------------------------------------------------------
{
  if (m_Renderer&&(vtkRenderer *)event->GetData()==m_Renderer.GetPointer())
    Show(); // Display again the avatar
}
//------------------------------------------------------------------------------
void mafAvatar::OnViewSelected(mafEvent *e)
//------------------------------------------------------------------------------
{
  SetRenderer((vtkRenderer *)e->GetData());
  m_View=e->GetView();
}
//------------------------------------------------------------------------------
void mafAvatar::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetId();
  
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
    mafEvent *e=mafEvent::SafeDownCast(event);
    OnViewSelected(e);
  }
}
