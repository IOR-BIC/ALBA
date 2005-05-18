/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractionManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 17:29:04 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
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

#include "mafInteractionManager.h"

#include "mafEvent.h"

#include "mafSceneGraph.h"
#include "mafView.h"

#include "mmiPER.h"
#include "mmiSER.h"

#include "mafEventBase.h"

#include "mafInteractionFactory.h"

#include "mafDeviceManager.h"
#include "mafAvatar3D.h"
#include "mafAction.h"
#include "mafDeviceSet.h"
#include "mmdMouse.h"

#include "mafVME.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"

#include "mafXMLStorage.h"

#include <set>
#include <assert.h>

#define MIF_VERSION "2.0"

#define DEFAULT_INTRA_FRAME_TIME 0.02 

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractionManager);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractionManager::mafInteractionManager()
//------------------------------------------------------------------------------
{
  m_SelectedView      = NULL;
  m_LockRenderingFlag = false;
  m_LockRenderingFlag = 0;
  m_IntraFrameTime    = DEFAULT_INTRA_FRAME_TIME;
  
  m_DeviceManager = new mafDeviceManager;
  m_DeviceManager->SetListener(this);
  m_DeviceManager->SetName("DeviceManager");
  
  //m_SettingsDialog = new mmgInteractionSettings; // the "Interaction Settings" dialog
  //m_SettingsDialog->SetInteractionManager(this);
  
  m_DeviceManager->Initialize();
  //SettingsDialog->AddDeviceToTree(m_DeviceManager->GetDeviceSet()); // add dev_mgr node as root
  
  // create the static event router and connect it
  mafNEW(m_StaticEventRouter);
  m_StaticEventRouter->SetName("StaticEventRouter");
  m_StaticEventRouter->SetListener(this);
  //m_StaticEventRouter->PlugEventSource(this,CameraUpdateChannel); // propagate camera events to actions

  // define the action for pointing and manipulating with negative priority to allow
  // static priority defined by operations to override it.
  mafAction *pointing_action = m_StaticEventRouter->AddAction("PointAndManipulate",-10);

  // create positional event router
  mafNEW(m_PositionalEventRouter);
  m_PositionalEventRouter->SetName("PositionalEventRouter");
  pointing_action->BindInteractor(m_PositionalEventRouter);
  m_PositionalEventRouter->SetListener(this);

  
  // create a Mouse device
  // the device is plugged here instead of App::Init() since mouse is plugged 
  // by default in all applications!
  mafPlugDevice<mmdMouse>("Mouse");
  mmdMouse *mouse_device = (mmdMouse *)m_DeviceManager->AddDevice("mmdMouse",true); // add as persistent device
  assert(mouse_device);
  //mafAction *mouse_action = m_StaticEventRouter->AddAction("Mouse"); // action for RWIs output
  //mouse_action->BindDevice(mouse_device); // bind mouse to mouse action
  pointing_action->BindDevice(mouse_device); // bind mouse to point&manipulate action
}

//------------------------------------------------------------------------------
mafInteractionManager::~mafInteractionManager()
//------------------------------------------------------------------------------
{
  m_DeviceManager->Shutdown();
  cppDEL(m_DeviceManager);
  mafDEL(m_StaticEventRouter);
  mafDEL(m_PositionalEventRouter);
  vtkDEL(m_CurrentRenderer);
}

//------------------------------------------------------------------------------
void mafInteractionManager::SetPER(mmiPER *per)
//------------------------------------------------------------------------------
{
  mafAction *pointing_action = m_StaticEventRouter->GetAction("PointAndManipulate");
  assert(pointing_action);

  // unbind old PER
  if (m_PositionalEventRouter)
  {
    pointing_action->UnBindInteractor(m_PositionalEventRouter);
    m_PositionalEventRouter->SetListener(NULL);
  }

  vtkSetObjectBodyMacro(m_PositionalEventRouter,mmiPER,per);

  // bind new PER
  m_PositionalEventRouter->SetName("m_PositionalEventRouter");
  pointing_action->BindInteractor(m_PositionalEventRouter);
  m_PositionalEventRouter->SetListener(this);
}

//----------------------------------------------------------------------------
void mafInteractionManager::PushPER(mmiPER *per)
//----------------------------------------------------------------------------
{
  mmiPER *old_per=GetPER();
  m_PERList.AppendItem(old_per);
  SetPER(per);
}
//----------------------------------------------------------------------------
bool mafInteractionManager::PopPER()
//----------------------------------------------------------------------------
{
  mmiPER *old_per=m_PERList.GetItem(m_PERList.GetNumberOfItems()-1);
  if (old_per)
  {
    SetPER(old_per);
    m_PERList.RemoveItem(old_per);
    return true;
  }
  
  return false;
}
//----------------------------------------------------------------------------
void mafInteractionManager::CameraFlyToMode()   
//----------------------------------------------------------------------------
{
  if(m_PositionalEventRouter) m_PositionalEventRouter->FlyToMode();
}

//------------------------------------------------------------------------------
void mafInteractionManager::EnableSelect(bool enable)
//------------------------------------------------------------------------------
{
  m_PositionalEventRouter->EnableSelect(enable);
}

//------------------------------------------------------------------------------
mmdMouse *mafInteractionManager::GetMouseDevice()
//------------------------------------------------------------------------------
{
  return mmdMouse::SafeDownCast(m_DeviceManager->GetDevice("Mouse"));
}
//------------------------------------------------------------------------------
mafAction *mafInteractionManager::GetMouseAction()
//------------------------------------------------------------------------------
{
  return m_StaticEventRouter->GetAction("Mouse");
}

//------------------------------------------------------------------------------
int mafInteractionManager::BindAction(const char *action,mafInteractor *agent)
//------------------------------------------------------------------------------
{
  return m_StaticEventRouter->BindAction(action,agent);
}

//------------------------------------------------------------------------------
int mafInteractionManager::UnBindAction(const char *action,mafInteractor *agent)
//------------------------------------------------------------------------------
{
  return m_StaticEventRouter->UnBindAction(action,agent);
}

//------------------------------------------------------------------------------
mafAction *mafInteractionManager::AddAction(const char *name, float priority)
//------------------------------------------------------------------------------
{
  return m_StaticEventRouter->AddAction(name,priority);
}

//------------------------------------------------------------------------------
mafAction *mafInteractionManager::GetAction(const char *name)
//------------------------------------------------------------------------------
{
  return m_StaticEventRouter->GetAction(name);
}

//------------------------------------------------------------------------------
vtkTemplatedMap<wxString,mafAvatar> &mafInteractionManager::GetAvatars()
//------------------------------------------------------------------------------
{
  return m_Avatars;
}

//------------------------------------------------------------------------------
void mafInteractionManager::AddAvatar(mafAvatar *avatar)
//------------------------------------------------------------------------------
{
  this->m_Avatars.SetItem(avatar->GetName(),avatar);
  
  // attach to the avatar both as a listener and an event source
  avatar->SetListener(this);
  //avatar->PlugEventSource(this,CameraUpdateChannel); // to be removed

  // Link the avatar to the current renderer (if present)
  avatar->SetRenderer(m_CurrentRenderer);
  avatar->SetView(m_SelectedView);
}

//------------------------------------------------------------------------------
int mafInteractionManager::RemoveAvatar(mafAvatar *avatar)
//------------------------------------------------------------------------------
{
  if (!m_Avatars.IsItemPresent(avatar))
    return false;
  
  // detach the avatar
  avatar->SetListener(NULL);
  avatar->UnPlugEventSource(this);
  
  // unlink the avatar from the renderer
  avatar->SetRenderer(NULL);
    
  return m_Avatars.RemoveItem(avatar);
}
//------------------------------------------------------------------------------
mafAvatar *mafInteractionManager::GetAvatar(const char *name)
//------------------------------------------------------------------------------
{
  return m_Avatars.GetItem(name);
}

//------------------------------------------------------------------------------
void mafInteractionManager::ViewSelected(mafView *view)
//------------------------------------------------------------------------------
{
  mafEventBase e(this,VIEW_SELECT,view);
  OnViewSelected(e);
}

//------------------------------------------------------------------------------
void mafInteractionManager::PreResetCamera(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  //InvokeEvent(mflSmartEvent(CAMERA_PRE_RESET,this,ren),CameraUpdateChannel);
  
  // propagate event to all avatars... 
}

//------------------------------------------------------------------------------
void mafInteractionManager::PostResetCamera(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  //InvokeEvent(mflSmartEvent(PostResetCameraEvent,this,ren),CameraUpdateChannel);

  // propagate event to all avatars...
}


//------------------------------------------------------------------------------
void mafInteractionManager::VmeSelected(mflVME *vme)
//------------------------------------------------------------------------------
{
  m_PositionalEventRouter->OnVmeSelected(vme);
}

//------------------------------------------------------------------------------
void mafInteractionManager::CameraUpdate(mafView *view)
//------------------------------------------------------------------------------
{
  if (m_LockRenderingFlag)
  {
    m_CameraUpdateRequests.insert(view);
  }
  else
  {
    if (view)
    {
      // avoid too much renders which would trash the CPU
      if((vtkTimerLog::GetTimeStamp()-m_LastRenderTime)>m_IntraFrameTime)
      {
        // render requested view
        view->CameraUpdate();
        m_LastRenderTime=vtkTimerLog::GetTimeStamp(); // store time at end of rendering
      }
      
    }
    else
    {
      // ask logic->view_mgr to perform a global update
      mafEventMacro(mafEvent(this,CAMERA_SYNCHRONOUS_UPDATE));
      m_LastRenderTime=vtkTimerLog::GetTimeStamp(); // store time at end of rendering
    }
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::OnViewSelected(mafEventBase *event)
//------------------------------------------------------------------------------
{
  vtkRenderer *ren;
  mafView *view = event->GetView();

  if (view)
  {
    // Here we could manage different kind of views...
    mafSceneGraph *sg=view->GetSceneGraph();
    assert(sg);
    
    ren=sg->m_ren1;
    
    // currently all views should have the renderer
    //assert(ren);
  }
  else
  {
    ren=NULL;
  }
  
  m_SelectedView = view;
  
  // Store old render window
  vtkRenderWindow *old_rw=(m_CurrentRenderer)?m_CurrentRenderer->GetRenderWindow():NULL;
  if (old_rw)
    old_rw->Register(this);
    
  SetCurrentRenderer(ren);
  
  event->SetData(m_CurrentRenderer); 

  // propagate event to all Avatars...
  //InvokeEvent(event,CameraUpdateChannel);

  // propagate to the mouse device too...
  GetMouseDevice()->ProcessEvent(event);
  
  // TODO: remove call to Render() and substitute with sending a CameraUpdate event.
  // Force a rendering of the old render window 
  // to update the display after removing the avatars
  if (old_rw&&!old_rw->GetNeverRendered())
  {
    old_rw->Render();
    old_rw->UnRegister(this);
  }
  
  // retrieve new render window
  vtkRenderWindow *rw=(m_CurrentRenderer)?m_CurrentRenderer->GetRenderWindow():NULL;
  
  // TODO: remove call to Render() and substitute with sending a CameraUpdate event.
  // Force a rendering of the new render window
  // to update the display after adding the avatars
  if (rw&&!rw->GetNeverRendered())
    rw->Render();
}

//------------------------------------------------------------------------------
void mafInteractionManager::OnDeviceAdded(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafDevice *device=(mafDevice *)event->GetData(); 
  mafDeviceSet *parent=(mafDeviceSet *)event->GetSender();
  assert(device);
  // forward device to the GUI
  //m_SettingsDialog->AddDeviceToTree(device,parent);
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnDeviceRemoving(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafDevice *device=(mafDevice *)event->GetData(); 
  assert(device);
  m_StaticEventRouter->UnBindDeviceFromAllActions(device);
  // forward event to the GUI
  //SettingsDialog->RemoveDeviceFromTree(device);
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnDeviceNameChanged(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafDevice *device=(mafDevice *)event->GetSender(); 
  assert(device);
  // forward device to the GUI
  //SettingsDialog->UpdateDevice(device);
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnBindDeviceToAction(mafEvent *e)
//------------------------------------------------------------------------------
{
  // this event is rosed by mafAction to ask for 
  // binding of a device to an action. Device is specified by its ID.
  mafDevice *device=m_DeviceManager->GetDevice(e->GetArg());
  if (device)
  {
    mafAction *action=mafAction::SafeDownCast((vtkObject *)e->GetSender());
    m_StaticEventRouter->BindDeviceToAction(device,action);

  }
  else
  {
    mafErrorMacro("Bind Restoring Error: unknown device ID!");
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::OnAddAvatar(mafEventBase *event)
//------------------------------------------------------------------------------
{
  AddAvatar((mafAvatar *)event->GetData());
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnRemoveAvatar(mafEventBase *event)
//------------------------------------------------------------------------------
{
  RemoveAvatar((mafAvatar *)event->GetData());
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnStartDispatching()
//------------------------------------------------------------------------------
{
  m_LockRenderingFlag = true;
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnEndDispatching()
//------------------------------------------------------------------------------
{
  m_LockRenderingFlag = false;

  if (m_CameraUpdateRequests.empty())
    return;

  // Dispatch pending rendering requests.
  if (*(m_CameraUpdateRequests.begin())==NULL)
  {
    // perform a global update and free the queue
    m_CameraUpdateRequests.clear();
    
    // avoid too much renderings which would trash the CPU
    if((vtkTimerLog::GetTimeStamp()-m_LastRenderTime)>m_IntraFrameTime)
    {
      // ask logic->view_mgr to perform a global update
      mafEventMacro(mafEvent(this,CAMERA_SYNCHRONOUS_UPDATE));
      m_LastRenderTime=vtkTimerLog::GetTimeStamp(); // store time at end of rendering
    }
    
  }
  else
  {
    // avoid too much renderings which would trash the CPU
    if((vtkTimerLog::GetTimeStamp()-m_LastRenderTime)>m_IntraFrameTime)
    {
      // traverse the list and perform all requested updates
      for (std::set<mafView *>::iterator it=m_CameraUpdateRequests.begin(); \
      it!=m_CameraUpdateRequests.end();it++)
      {
        assert(*it!=NULL); // for DEBUGGING 
        (*it)->CameraUpdate();          
      }
      // clear the queue
      m_CameraUpdateRequests.clear();
      m_LastRenderTime=vtkTimerLog::GetTimeStamp(); // store time at end of rendering
    }
  }
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnCameraUpdate(mafEventBase *e)
//------------------------------------------------------------------------------
{
  CameraUpdate(e->GetView());
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnEvent(mafEventBase *event,unsigned long channel)
//------------------------------------------------------------------------------
{
  assert(event);
 
  mafID id=event->GetID();
    
  //if (channel==CameraUpdateChannel)
  //{
  if (id==VIEW_SELECT)
  {
    mafEventBase *e=(mafEventBase *)event;
    OnViewSelected(e);
  }
  //else
  //{
  //  InvokeEvent(event,channel);
  //}
  //}
  else if (id==mafDeviceManager::DISPATCH_START)
  {
    OnStartDispatching();    
  }
  else if (id==mafDeviceManager::DISPATCH_END)
  {
    OnEndDispatching();
  }
  else if (id==CAMERA_UPDATE)
  {
    mafEventBase *e=(mafEventBase *)event;
    OnCameraUpdate(e);
  }
  else if (id==VME_SELECTED)
  {
    mafEventMacro(mafEvent(event->GetSender(),VME_SELECT,(mflVME *)event->GetData()));
  }
  /*
  else if (id==WrapMafEvent)
  {
      assert(event->GetPointer());
	  mafEvent *blob = (mafEvent *)event->GetPointer(); 
	  mafEventMacro(*blob);
  }
  */
  /*else if (id==ShowContextualMenuEvent)
  {
    mflVME *vme = ((mafEventBase *)event)->GetVme();
    bool vme_context_menu = (vme != NULL) && !vme->IsA("mflVMEGizmo");
    mafEventMacro(mafEvent(event->GetSender(),SHOW_CONTEXTUAL_MENU, vme_context_menu));
  }*/
  else if (id==mafDevice::DEVICE_NAME_CHANGED) 
  {
    OnDeviceNameChanged(event);
  }
  else if (id==mafDeviceSet::DEVICE_ADDED) 
  {
    OnDeviceAdded(event);
  }
  else if (id==mafDeviceSet::DEVICE_REMOVING) 
  {
    OnDeviceRemoving(event);
  }
  else if (id==AVATAR_ADDED) 
  {
    OnAddAvatar(event);
  }
  else if (id==AVATAR_REMOVED) 
  {
    OnRemoveAvatar(event); 
  }
  else if (id==mafAction::DEVICE_BIND_TO_ACTION)
  {
    mafEvent *e=mafEventBase::SafeDownCast(event);
    OnBindDeviceToAction(e);
  }
  else
  {
    InvokeEvent(event);
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::ShowSettingsPanel()
//------------------------------------------------------------------------------
{
  //SettingsDialog->ShowModal(); 
}

//------------------------------------------------------------------------------
int mafInteractionManager::Store(const char *filename)
//------------------------------------------------------------------------------
{
  assert(filename);
  mafXMLStorage writer;
  
  // Multimod Interaction Settings file format
  writer.SetFileType("MIS");
  writer.SetVersion(MIS_VERSION);

  writer.SetFileName(filename);
  writer->SetDocument(this);

  return writer.Store();
}

//------------------------------------------------------------------------------
int mafInteractionManager::Restore(const char *filename)
//------------------------------------------------------------------------------
{
  assert(filename);
  mafXMLStorage reader;

  // Multimod Interaction Settings file format
  reader.SetFileType("MIS");
  reader.SetVersion(MIS_VERSION);

  reader.SetFileName(filename);
  reader->SetDocument(this);

  return reader->Restore();
}

//------------------------------------------------------------------------------
int mafInteractionManager::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  // store device settings
  if (node->StoreObject("DeviceManager",m_DeviceManager))
    return MAF_ERROR;
  
  // store bindings
  if (node->StoreObject("DeviceBindings",m_StaticEventRouter));
    return MAF_ERROR;
  
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafInteractionManager::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if (node->RestoreObject("DeviceManager",m_DeviceManager))
    return MAF_ERROR;
  
  if (node->RestoreObject("DeviceBindings",m_StaticEventRouter))
    return MAF_ERROR;
  
  return MAF_OK;
}