/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractionManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-08-24 16:15:15 $
  Version:   $Revision: 1.20 $
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

#include <wx/tipdlg.h>
#include <wx/choicdlg.h>

#include "mafInteractionManager.h"

#include "mafDecl.h"

// GUIs
#include "mmgGui.h"
#include "mmgDialog.h"
#include "mmgButton.h"
#include "mmgGuiHolder.h"
#include "mmgTree.h"
#include "mmgCheckListBox.h"

#include "mafEvent.h"

#include "mafSceneGraph.h"
#include "mafViewVTK.h"
#include "mafRWIBase.h"

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
#include "mafXMLStorage.h"
#include "mafStorageElement.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"

#include <set>
#include <assert.h>

#define MIS_VERSION "2.0"

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

  m_Devices           = NULL;
  m_DeviceTree        = NULL;
  //m_Dialog            = NULL;
  m_SettingsPanel     = NULL;
  m_BindingsPanel     = NULL;
  m_CurrentDevice     = NULL;
  m_CurrentRenderer   = NULL;
  m_ActionsList       = NULL;
  m_Bindings          = NULL;
  m_SettingFileName = mafGetApplicationDirectory().c_str();
  m_SettingFileName.Append("Config/Presets");
  if(!::wxDirExists(m_SettingFileName)) m_SettingFileName = mafGetApplicationDirectory().c_str();
  
  mafNEW(m_DeviceManager);
  m_DeviceManager->SetListener(this);
  m_DeviceManager->SetName("DeviceManager");
  
  CreateGUI();

  m_DeviceManager->Initialize();
  AddDeviceToTree(m_DeviceManager->GetDeviceSet()); // add dev_mgr node as root
  
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
  mafDEL(m_DeviceManager);
  mafDEL(m_StaticEventRouter);
  mafDEL(m_PositionalEventRouter);
  //vtkDEL(m_CurrentRenderer);
  cppDEL(m_Frame);
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

  m_PositionalEventRouter = per;

  // bind new PER
  m_PositionalEventRouter->SetName("PositionalEventRouter");
  pointing_action->BindInteractor(m_PositionalEventRouter);
  m_PositionalEventRouter->SetListener(this);
}

//----------------------------------------------------------------------------
void mafInteractionManager::PushPER(mmiPER *per)
//----------------------------------------------------------------------------
{
  mmiPER *old_per=GetPER();
  m_PERList.push_back(old_per);
  SetPER(per);
}
//----------------------------------------------------------------------------
bool mafInteractionManager::PopPER()
//----------------------------------------------------------------------------
{
  // retrieve and delete last item
  mafAutoPointer<mmiPER> old_per=*(m_PERList.rbegin());
  m_PERList.pop_back(); 

  assert(old_per); // should always be != NULL

  if (old_per) // if not NULL
  {
    SetPER(old_per);
    return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------
void mafInteractionManager::SetCurrentRenderer(vtkRenderer *ren)
//----------------------------------------------------------------------------
{
  if (ren)
    ren->UnRegister(NULL);

  m_CurrentRenderer = ren;

  if (ren)
    ren->Register(NULL);
}

//----------------------------------------------------------------------------
void mafInteractionManager::CameraFlyToMode()   
//----------------------------------------------------------------------------
{
  if(m_PositionalEventRouter)
    m_PositionalEventRouter->FlyToMode();
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
const mafInteractionManager::mmuAvatarsMap &mafInteractionManager::GetAvatars()
//------------------------------------------------------------------------------
{
  return m_Avatars;
}

//------------------------------------------------------------------------------
void mafInteractionManager::GetAvatars(mmuAvatarsVector &avatars)
//------------------------------------------------------------------------------
{
  avatars.clear();
  avatars.resize(m_Avatars.size());
  int i=0;
  for (mmuAvatarsMap::iterator it=m_Avatars.begin();it!=m_Avatars.end();it++,i++)
  {
    avatars[i]=it->second.GetPointer();
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::AddAvatar(mafAvatar *avatar)
//------------------------------------------------------------------------------
{
  m_Avatars[avatar->GetName()] = avatar;
  
  // attach to the avatar both as a listener and an event source
  avatar->SetListener(this);

  // Link the avatar to the current renderer (if present)
  avatar->SetRenderer(m_CurrentRenderer);
  avatar->SetView(m_SelectedView);
}

//------------------------------------------------------------------------------
int mafInteractionManager::RemoveAvatar(mafAvatar *avatar)
//------------------------------------------------------------------------------
{
  assert(avatar);

  mmuAvatarsMap::iterator it=m_Avatars.find(avatar->GetName());
  if (it==m_Avatars.end())
    return false;
  
  // detach the avatar
  avatar->SetListener(NULL);
  
  // unlink the avatar from the renderer
  avatar->SetRenderer(NULL);
    
  m_Avatars.erase(it);

  return true;
}
//------------------------------------------------------------------------------
mafAvatar *mafInteractionManager::GetAvatar(const char *name)
//------------------------------------------------------------------------------
{
  mmuAvatarsMap::iterator it=m_Avatars.find(name);  
  return (it!=m_Avatars.end())?it->second:NULL;
}

//------------------------------------------------------------------------------
void mafInteractionManager::ViewSelected(mafView *view)
//------------------------------------------------------------------------------
{
  mafEvent e(this,VIEW_SELECT,view);
  OnViewSelected(&e);
}

//------------------------------------------------------------------------------
void mafInteractionManager::PreResetCamera(vtkRenderer *ren)
//------------------------------------------------------------------------------
{ 
  // propagate event to all avatars...
  for (mmuAvatarsMap::iterator it=m_Avatars.begin();it!=m_Avatars.end();it++)
  {
    it->second->OnEvent(&mafEventBase(this,CAMERA_PRE_RESET,ren));
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::PostResetCamera(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  // propagate event to all avatars...
  for (mmuAvatarsMap::iterator it=m_Avatars.begin();it!=m_Avatars.end();it++)
  {
    it->second->OnEvent(&mafEventBase(this,CAMERA_POST_RESET,ren));
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::VmeSelected(mafNode *node)
//------------------------------------------------------------------------------
{
  mafVME *vme=mafVME::SafeDownCast(node);
  if (vme)
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
      // avoid too much renderings which would trash the CPU
      if((vtkTimerLog::GetCurrentTime()-m_LastRenderTime)>m_IntraFrameTime)
      {
        // render requested view
        view->CameraUpdate();
        m_LastRenderTime=vtkTimerLog::GetCurrentTime(); // store time at end of rendering
      }
      
    }
    else
    {
      // ask logic->view_mgr to perform a global update
      mafEventMacro(mafEvent(this,CAMERA_SYNCHRONOUS_UPDATE));
      m_LastRenderTime=vtkTimerLog::GetCurrentTime(); // store time at end of rendering
    }
  }
}

//------------------------------------------------------------------------------
void mafInteractionManager::OnViewSelected(mafEvent *event)
//------------------------------------------------------------------------------
{  
  mafViewVTK *view = mafViewVTK::SafeDownCast(event->GetView()); 
  m_SelectedView = view;

  if (view)
  {
    vtkRenderer *ren = view->GetFrontRenderer();
    SetCurrentRenderer(ren);
    event->SetData(ren); // add the renderer to the event to be sent to avatars
  }
  else
  {
    SetCurrentRenderer(NULL);

  }

  // propagate event to all avatars...
  for (mmuAvatarsMap::iterator it=m_Avatars.begin();it!=m_Avatars.end();it++)
  {
    //it->second->OnEvent(&mafEventBase(this,VIEW_SELECT));    
    it->second->OnEvent(event);
  }

  // propagate VIEW_SELECTED event to the mouse device too...
  GetMouseDevice()->OnEvent(event);
  
  // TODO: force a render to the view here....

  /*
  // TODO: remove call to Render() and substitute with sending a CameraUpdate event.
  // Force a rendering of the old render window 
  // to update the display after removing the avatars
  if (old_rw&&!old_rw->GetNeverRendered())
  {
    old_rw->Render();
    old_rw->UnRegister(NULL);
  }
  
  // retrieve new render window
  vtkRenderWindow *rw=(m_CurrentRenderer)?m_CurrentRenderer->GetRenderWindow():NULL;
  
  // TODO: remove call to Render() and substitute with sending a CameraUpdate event.
  // Force a rendering of the new render window
  // to update the display after adding the avatars
  if (rw&&!rw->GetNeverRendered())
    rw->Render();

  */
}

//------------------------------------------------------------------------------
void mafInteractionManager::OnDeviceAdded(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafDevice *device=(mafDevice *)event->GetData(); 
  mafDeviceSet *parent=(mafDeviceSet *)event->GetSender();
  assert(device);
  // forward device to the GUI
  AddDeviceToTree(device,parent);
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnDeviceRemoving(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafDevice *device=(mafDevice *)event->GetData(); 
  assert(device);
  m_StaticEventRouter->UnBindDeviceFromAllActions(device);
  // forward event to the GUI
  RemoveDeviceFromTree(device);
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnDeviceNameChanged(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafDevice *device=(mafDevice *)event->GetSender(); 
  assert(device);
  // forward device to the GUI
  UpdateDevice(device);
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
    mafAction *action=mafAction::SafeDownCast((mafObject *)e->GetSender());
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
    if((vtkTimerLog::GetCurrentTime()-m_LastRenderTime)>m_IntraFrameTime)
    {
      // ask logic->view_mgr to perform a global update
      mafEventMacro(mafEvent(this,CAMERA_SYNCHRONOUS_UPDATE));
      m_LastRenderTime=vtkTimerLog::GetCurrentTime(); // store time at end of rendering
    }
    
  }
  else
  {
    // avoid too much renderings which would trash the CPU
    if((vtkTimerLog::GetCurrentTime()-m_LastRenderTime)>m_IntraFrameTime)
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
      m_LastRenderTime=vtkTimerLog::GetCurrentTime(); // store time at end of rendering
    }
  }
}
//------------------------------------------------------------------------------
void mafInteractionManager::OnCameraUpdate(mafEventBase *event)
//------------------------------------------------------------------------------
{
  if (mafEvent *e=mafEvent::SafeDownCast(event))
  {
    CameraUpdate(e->GetView());
  }
  else
  {
    CameraUpdate(NULL);
  }

}
//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
enum IMANAGER_WIDGET_ID
{
	ID_DEVICE_TREE = MINID,
	ID_ADD_DEVICE,
	ID_REMOVE_DEVICE,
  ID_DEVICE_SETTINGS,
  ID_DEVICE_BINDINGS,
  ID_BINDING_LIST,
  ID_STORE,
  ID_LOAD,
  ID_OK
};
//------------------------------------------------------------------------------
void mafInteractionManager::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
 
  mafID id=event->GetId();

  mafString action_name;

  mafEvent *e=mafEvent::SafeDownCast(event);
 
  if (e)
  {
    if (id==mafAction::DEVICE_BIND)
    {
      mafEvent *e=mafEvent::SafeDownCast(event);
      OnBindDeviceToAction(e);
    }
    else
    {
      switch(e->GetId())
      {
        case ID_OK:
		    case wxOK:
          //m_Dialog->EndModal(wxOK);
          m_Frame->Show(false);
          return; 
		    break;
        case ID_STORE:
        { 
          //SIL. 4-7-2005: begin
          std::string result = 
          mafGetSaveFile(m_SettingFileName, "Interaction Settings (*.xml)| *.xml", "Save Interaction Settings");
          if( result == "") return;
          m_SettingFileName = result.c_str();
          //SIL. 4-7-2005: end

          Store(m_SettingFileName);
        
          return;
        }
        break;
        case ID_LOAD:
        {
          //SIL. 4-7-2005: begin
          std::string result = 
          mafGetOpenFile(m_SettingFileName, "Interaction Settings (*.xml)| *.xml", "Load Interaction Settings");
          if( result == "") return;
          m_SettingFileName = result.c_str();
          //SIL. 4-7-2005: end

          Restore(m_SettingFileName);
          //camera reset here?
        
          return;
        }
        break;
        case ID_BINDING_LIST:
        {
          assert(m_CurrentDevice);
          action_name=m_ActionsList->GetItemLabel(m_ActionsList->GetSelection());
          //action_name=m_ActionsList->GetItemLabel(e->GetArg());
          //int id=m_ActionsList->FindItemIndex(e->GetArg());
      
          if (m_ActionsList->IsItemChecked(m_ActionsList->GetSelection()))
          {
            GetSER()->BindDeviceToAction(m_CurrentDevice,action_name);
          }
          else
          {
            GetSER()->UnBindDeviceFromAction(m_CurrentDevice,action_name);
          }
          return;
        }
        break;
        case ID_ADD_DEVICE:
        {
          wxString device_name,device_type;
          int sel=DeviceChooser(device_name,device_type);
      
          if (sel>=0)
          {
            // add the new device to the devices manager's pool
            mafDevice *device = GetDeviceManager()->AddDevice(device_type);
            // the device is added to the list by an 
            // event returned by DeviceManager which is
            // served by InteractionManager by calling
            // AddDevice of this class.
          }
          return;
        }
        break;
        case ID_REMOVE_DEVICE:
          if (m_CurrentDevice)
          {
            int success = GetDeviceManager()->RemoveDevice(m_CurrentDevice);
            if (!success)
            {
              wxMessageBox("Cannot remove device: I/O manager error");
            }
            else
            {
              m_StaticEventRouter->UnBindDeviceFromAllActions(m_CurrentDevice);
            }
          }
          return;
         break;
        case VME_SELECT:
          if (e->GetSender() == m_DeviceTree)
          { 
            // Event rised by the CheckTree used to show the devices tree
            if (mafDevice *device = GetDeviceManager()->GetDevice(e->GetArg()))
            {
              m_CurrentDevice = device;
              // Show device settings panel
              assert(m_CurrentDevice->GetGui());
              mmgGui *gui = m_CurrentDevice->GetGui();
              assert(gui);
              m_SettingsPanel->Put(gui);
              // force update
              m_CurrentDevice->UpdateGui();
  
              // Update Bindings panel
              UpdateBindings();
            }
            else
            {
              assert(false);
            }
            return;
          }          
        break;
		    default:
          if (e->GetSender() == m_Devices || e->GetSender() == m_DeviceTree)
          {
			      e->Log();
            return;
          }
		    break; 
      }
    }
    return;
  }
  
  if (id==VME_SELECT)
  {
    // event rised by PER to advise of VME selection
    mafEventMacro(mafEvent(event->GetSender(),VME_SELECT,(mafVME *)event->GetData()));
    //mafEventMacro(*event);
  }
  else if (id==VIEW_SELECT)
  {
    mafEvent *e=mafEvent::SafeDownCast(event);
    assert(event);
    OnViewSelected(e);
  }  
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
    OnCameraUpdate(e);
  }
  /*else if (id==ShowContextualMenuEvent)
  {
    mafVME *vme = ((mafEventBase *)event)->GetVme();
    bool vme_context_menu = (vme != NULL) && !vme->IsA("mafVMEGizmo");
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
  else
  {
    InvokeEvent(event);
  }
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

  writer.SetURL(filename);
  writer.SetDocument(this);

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

  reader.SetURL(filename);
  reader.SetDocument(this);

  return reader.Restore();
}

//------------------------------------------------------------------------------
int mafInteractionManager::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  // store device settings
  if (node->StoreObject("DeviceManager",m_DeviceManager)==NULL)
    return MAF_ERROR;
  
  // store bindings
  if (node->StoreObject("DeviceBindings",m_StaticEventRouter)==NULL)
    return MAF_ERROR;
  
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafInteractionManager::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if (node->RestoreObject("DeviceManager",m_DeviceManager)!=MAF_OK)
    return MAF_ERROR;
  
  if (node->RestoreObject("DeviceBindings",m_StaticEventRouter)!=MAF_OK)
    return MAF_ERROR;
  
  return MAF_OK;
}

//----------------------------------------------------------------------------
bool mafInteractionManager::ShowModal()
//----------------------------------------------------------------------------
{
   //modified by Marco. 13-7-2005  return m_Dialog->ShowModal() != 0;
  m_Frame->Show(true);
  return true;
}
//----------------------------------------------------------------------------
int mafInteractionManager::DeviceChooser(wxString &dev_name,wxString &dev_type)
//----------------------------------------------------------------------------
{
  mafInteractionFactory *iFactory=mafInteractionFactory::GetInstance();
  
  assert(iFactory);
  if (iFactory->GetNumberOfDevices()>0)
  {
    wxString *devices = new wxString[iFactory->GetNumberOfDevices()];


    for (int id=0;id<iFactory->GetNumberOfDevices();id++)
    {
      devices[id]=iFactory->GetDeviceDescription(iFactory->GetDeviceName(id));
    }

    //wxSingleChoiceDialog chooser(m_Dialog,"select a device","Device Chooser",iFactory->GetNumberOfDevices(),devices);
    wxSingleChoiceDialog chooser(m_Frame,"select a device","Device Chooser",iFactory->GetNumberOfDevices(),devices);

    int index=-1;
    if (chooser.ShowModal()==wxID_OK)
    {
      index = chooser.GetSelection();
    
      if (index>=0)
      {
        dev_type = iFactory->GetDeviceName(index);
        dev_name = iFactory->GetDeviceDescription(dev_type);
      }
    }
  
    delete [] devices;
    return index;
  }
  else
  {
    return -1;
  }
}

//----------------------------------------------------------------------------
void mafInteractionManager::UpdateBindings()
//----------------------------------------------------------------------------
{
  if (m_CurrentDevice)
  {
    /* Fill in actions' list */
    m_ActionsList->Clear();
    
    const mmiSER::mmuActionsMap *actions=GetSER()->GetActions();

    int i=0;
    for (mmiSER::mmuActionsMap::const_iterator it=actions->begin();it!=actions->end();it++)
    {
      mafAction *action = it->second;
      bool found=false;

      // search through the list
      for (mafAction::mmuDeviceList::const_iterator it_list=action->GetDevices()->begin();it_list!=action->GetDevices()->end();it_list++)
      {
        if (it_list->GetPointer() == m_CurrentDevice)
          found = true;
      }
      m_ActionsList->AddItem(i++,action->GetName(),found);
    }

    m_Bindings->Update();
  }
}

//----------------------------------------------------------------------------
void mafInteractionManager::AddDeviceToTree(mafDevice *device,mafDeviceSet *parent)
//----------------------------------------------------------------------------
{
  assert(device);
  mafID parent_id=(parent?parent->GetID():0);
  m_DeviceTree->AddNode(device->GetID(),parent_id,device->GetName());
  //m_DeviceTree->Update();
}

//----------------------------------------------------------------------------
void mafInteractionManager::RemoveDeviceFromTree(mafDevice *device)
//----------------------------------------------------------------------------
{
  assert(device);
  m_DeviceTree->DeleteNode(device->GetID());
}
/*
//----------------------------------------------------------------------------
void mafInteractionManager::UpdateDeviceList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_DeviceList->Number();i++)
  {
    mafDevice *device=(mafDevice *)m_DeviceList->GetClientData(i);
    assert(device);
    m_DeviceList->SetString(i,device->GetName());
  }
  
  m_Devices->Update();
}
*/
//----------------------------------------------------------------------------
void mafInteractionManager::UpdateDevice(mafDevice *device)
//----------------------------------------------------------------------------
{
  assert(device);
  m_DeviceTree->SetNodeLabel(device->GetID(),device->GetName());
}
//----------------------------------------------------------------------------
void mafInteractionManager::CreateGUI() 
//----------------------------------------------------------------------------
{
  /*****
  
  //SIL. 4-7-2005: 
   // Changed Layout, Behaviour on Resize, Buttons Implementation

  m_Dialog = new mmgDialog("I/O Devices Settings",mafRESIZABLE | mafCLOSEWINDOW);

  m_DeviceTree = new mmgTree(m_Dialog,ID_DEVICE_TREE);
  m_DeviceTree->SetTitle("connected devices");
  m_DeviceTree->SetSize(230,300);
  m_DeviceTree->SetListener(this);

  // Holder of Device's settings GUI ==========
  m_SettingsPanel = new mmgGuiHolder(m_Dialog,ID_DEVICE_SETTINGS);
  m_SettingsPanel->Show(true);
  m_SettingsPanel->SetSize(215,300); // h era 230
  m_SettingsPanel->SetTitle("device settings");

  // Device's binding GUI =====================
  m_BindingsPanel = new mmgNamedPanel(m_Dialog,ID_DEVICE_BINDINGS);
  m_BindingsPanel->SetTitle("device bindings");
  m_BindingsPanel->SetSize(215,100); // h era 230

  m_Bindings = new mmgGui(this);
  m_Bindings->Label("available actions");
  m_Bindings->Label("available actions");
  m_ActionsList = m_Bindings->CheckList(ID_BINDING_LIST,"",60,"actions the devices is assigned to");
  m_Bindings->Show(true);
  m_Bindings->FitGui();
  m_BindingsPanel->Add(m_Bindings);

  // Buttons =====================
  mmgButton *add = new mmgButton(m_Dialog,ID_ADD_DEVICE,"add device",wxPoint(0,0)); 
  add->SetListener(this);
  mmgButton *remove = new mmgButton(m_Dialog,ID_REMOVE_DEVICE,"remove device",wxPoint(0,0));
  remove->SetListener(this);
  mmgButton *load = new mmgButton(m_Dialog,ID_LOAD,"load",wxPoint(0,0));
  load->SetListener(this);
  mmgButton *save = new mmgButton(m_Dialog,ID_STORE,"save as",wxPoint(0,0));
  save->SetListener(this);
  mmgButton *close = new mmgButton(m_Dialog,ID_OK,"close",wxPoint(0,0));
  close->SetListener(this);

  // sizers ====================================
  wxBoxSizer *v_sizer = new wxBoxSizer(wxVERTICAL);
  v_sizer->Add(m_SettingsPanel,1,wxEXPAND|wxALL, 1 );//////////////////////////////
  v_sizer->Add(m_BindingsPanel,0,wxALL, 1 );

  wxBoxSizer *h_sizer = new wxBoxSizer(wxHORIZONTAL);
  h_sizer->Add(m_DeviceTree,1,wxEXPAND|wxALL, 1 );
  h_sizer->Add(v_sizer,0,wxEXPAND);

  wxBoxSizer *h2_sizer = new wxBoxSizer(wxHORIZONTAL);
  h2_sizer->Add(add);
  h2_sizer->Add(remove);
  h2_sizer->Add(load);
  h2_sizer->Add(save);
  h2_sizer->Add(close);

  m_Dialog->Add(h_sizer,1,wxEXPAND);
  m_Dialog->Add(h2_sizer,0,wxCENTRE|wxALL,1);

  *****/

  m_Frame = new wxFrame(NULL,-1,"I/O Devices Settings",wxPoint(-1,-1), wxSize(430,500));

  mmgNamedPanel *FOO = new mmgNamedPanel(m_Frame,-1,false,true);
  FOO->Show();
  
  m_DeviceTree = new mmgTree(FOO,ID_DEVICE_TREE);
  m_DeviceTree->SetTitle("connected devices");
  m_DeviceTree->SetSize(230,300);
  m_DeviceTree->SetListener(this);

  // Holder of Device's settings GUI ==========
  m_SettingsPanel = new mmgGuiHolder(FOO ,ID_DEVICE_SETTINGS);
  m_SettingsPanel->Show(true);
  m_SettingsPanel->SetSize(215,300); // h era 230
  m_SettingsPanel->SetTitle("device settings");

  // Device's binding GUI =====================
  m_BindingsPanel = new mmgNamedPanel(FOO,ID_DEVICE_BINDINGS);
  m_BindingsPanel->SetTitle("device bindings");
  m_BindingsPanel->SetSize(215,100); // h era 230

  m_Bindings = new mmgGui(this);
  m_Bindings->Label("available actions");
  m_Bindings->Label("available actions");
  m_ActionsList = m_Bindings->CheckList(ID_BINDING_LIST,"",60,"actions the devices is assigned to");
  m_Bindings->Show(true);
  m_Bindings->FitGui();
  m_BindingsPanel->Add(m_Bindings);

  // Buttons =====================
  mmgButton *add = new mmgButton(FOO,ID_ADD_DEVICE,"add device",wxPoint(0,0)); 
  add->SetListener(this);
  mmgButton *remove = new mmgButton(FOO,ID_REMOVE_DEVICE,"remove device",wxPoint(0,0));
  remove->SetListener(this);
  mmgButton *load = new mmgButton(FOO,ID_LOAD,"load",wxPoint(0,0));
  load->SetListener(this);
  mmgButton *save = new mmgButton(FOO,ID_STORE,"save as",wxPoint(0,0));
  save->SetListener(this);
  mmgButton *close = new mmgButton(FOO,ID_OK,"close",wxPoint(0,0));
  close->SetListener(this);

  // sizers ====================================
  wxBoxSizer *v_sizer = new wxBoxSizer(wxVERTICAL);
  v_sizer->Add(m_SettingsPanel,1,wxEXPAND|wxALL, 1 );//////////////////////////////
  v_sizer->Add(m_BindingsPanel,0,wxALL, 1 );

  wxBoxSizer *h_sizer = new wxBoxSizer(wxHORIZONTAL);
  h_sizer->Add(m_DeviceTree,1,wxEXPAND|wxALL, 1 );
  h_sizer->Add(v_sizer,0,wxEXPAND);

  wxBoxSizer *h2_sizer = new wxBoxSizer(wxHORIZONTAL);
  h2_sizer->Add(add);
  h2_sizer->Add(remove);
  h2_sizer->Add(load);
  h2_sizer->Add(save);
  h2_sizer->Add(close);

  FOO->Add(h_sizer,1,wxEXPAND);
  FOO->Add(h2_sizer,0,wxCENTRE|wxALL,1);
}
