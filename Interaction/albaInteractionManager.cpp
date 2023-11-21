/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionManager
 Authors: Marco Petrone
 
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

#include <wx/tipdlg.h>
#include <wx/choicdlg.h>

#include "albaInteractionManager.h"

#include "albaDecl.h"

// GUIs
#include "albaGUI.h"
#include "albaGUIDialog.h"
#include "albaGUIButton.h"
#include "albaGUIHolder.h"
#include "albaGUITree.h"
#include "albaGUICheckListBox.h"

#include "albaEvent.h"

#include "albaSceneGraph.h"
#include "albaView.h"
#include "albaViewVTK.h"
#include "albaRWIBase.h"

#include "albaInteractorPER.h"
#include "albaInteractorSER.h"

#include "albaEventBase.h"

#include "albaInteractionFactory.h"

#include "albaDeviceManager.h"
#include "albaAvatar3D.h"
#include "albaAction.h"
#include "albaDeviceSet.h"
#include "albaDeviceButtonsPadMouse.h"
  
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaXMLStorage.h"
#include "albaStorageElement.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"

#include <set>
#include <assert.h>

#define MIS_VERSION "2.0"

#define DEFAULT_INTRA_FRAME_TIME 0.02 

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractionManager);

//------------------------------------------------------------------------------
albaInteractionManager::albaInteractionManager()
{
  m_Gui = NULL;  //SIL. 07-jun-2006 : 
  m_Frame = NULL;  //SIL. 07-jun-2006 : 
  m_SelectedView      = NULL;
  m_LockRenderingFlag = false;
  m_LockRenderingFlag = 0;
  m_IntraFrameTime    = DEFAULT_INTRA_FRAME_TIME;

  m_Devices           = NULL;
  m_DeviceTree        = NULL;
  //m_Dialog            = NULL;
  m_SettingsPanel     = NULL;
  //m_BindingsPanel     = NULL;
  m_CurrentDevice     = NULL;
  m_CurrentRenderer   = NULL;
  m_ActionsList       = NULL;
  m_Bindings          = NULL;
  m_SettingFileName = albaGetLastUserFolder();
  
  albaNEW(m_DeviceManager);
  m_DeviceManager->SetListener(this);
  m_DeviceManager->SetName("DeviceManager");
  
  CreateGUI();

  m_DeviceManager->Initialize();
  AddDeviceToTree(m_DeviceManager->GetDeviceSet()); // add dev_mgr node as root
  
  // create the static event router and connect it
  albaNEW(m_StaticEventRouter);
  m_StaticEventRouter->SetName("StaticEventRouter");
  m_StaticEventRouter->SetListener(this);
  
  // define the action for pointing and manipulating with negative priority to allow
  // static priority defined by operations to override it.
  albaAction *pointing_action = m_StaticEventRouter->AddAction("PointAndManipulate",-10);

  // create positional event router
  albaNEW(m_PositionalEventRouter);
  m_PositionalEventRouter->SetName("PositionalEventRouter");
  pointing_action->BindInteractor(m_PositionalEventRouter);
  m_PositionalEventRouter->SetListener(this);

  // create a Mouse device
  // the device is plugged here instead of App::Init() since mouse is plugged 
  // by default in all applications!
  albaPlugDevice<albaDeviceButtonsPadMouse>("Mouse");
  albaDeviceButtonsPadMouse *mouse_device = (albaDeviceButtonsPadMouse *)m_DeviceManager->AddDevice("albaDeviceButtonsPadMouse",true); // add as persistent device
  assert(mouse_device);
  pointing_action->BindDevice(mouse_device); // bind mouse to point&manipulate action
}

//------------------------------------------------------------------------------
albaInteractionManager::~albaInteractionManager()
{
  while(!m_PERList.empty())
  {
    albaInteractorPER *old_per=*(m_PERList.rbegin());
    m_PERList.pop_back();
    assert(old_per);
    SetPER(old_per);
  }

  m_DeviceManager->Shutdown();
  cppDEL(m_Frame);
  albaDEL(m_DeviceManager);
  albaDEL(m_StaticEventRouter);
  albaDEL(m_PositionalEventRouter);
}

//------------------------------------------------------------------------------
void albaInteractionManager::SetPER(albaInteractorPER *per)
{
  albaAction *pointing_action = m_StaticEventRouter->GetAction("PointAndManipulate");
  assert(pointing_action);

  // unbind old PER
  if (m_PositionalEventRouter)
  {
    pointing_action->UnBindInteractor(m_PositionalEventRouter);
    m_PositionalEventRouter->SetListener(NULL);
  }

	//move camera observers from current per to new per
	std::vector <albaObserver *> obVect;
	m_PositionalEventRouter->GetCameraMouseInteractor()->GetObservers(MCH_UP, obVect);
	m_PositionalEventRouter->GetCameraMouseInteractor()->RemoveAllObservers();

  m_PositionalEventRouter = per;

	for (int i = 0; i < obVect.size(); i++)
		m_PositionalEventRouter->GetCameraMouseInteractor()->AddObserver(obVect[i]);

  // bind new PER
  m_PositionalEventRouter->SetName("PositionalEventRouter");
  pointing_action->BindInteractor(m_PositionalEventRouter);
  m_PositionalEventRouter->SetListener(this);
}

//----------------------------------------------------------------------------
void albaInteractionManager::PushPER(albaInteractorPER *per)
{
  albaInteractorPER *old_per=GetPER();
  m_PERList.push_back(old_per);
  SetPER(per);
}
//----------------------------------------------------------------------------
bool albaInteractionManager::PopPER()
{
  // retrieve and delete last item
  albaInteractorPER *old_per=*(m_PERList.rbegin());
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
void albaInteractionManager::SetCurrentRenderer(vtkRenderer *ren)
{
  if (ren)
    ren->UnRegister(NULL);

  m_CurrentRenderer = ren;

  if (ren)
    ren->Register(NULL);
}

//----------------------------------------------------------------------------
void albaInteractionManager::CameraFlyToMode()   
{
  if(m_PositionalEventRouter)
    m_PositionalEventRouter->FlyToMode();
}

//------------------------------------------------------------------------------
void albaInteractionManager::EnableSelect(bool enable)
{
  m_PositionalEventRouter->EnableSelect(enable);
}

//------------------------------------------------------------------------------
albaDeviceButtonsPadMouse *albaInteractionManager::GetMouseDevice()
{
  return albaDeviceButtonsPadMouse::SafeDownCast(m_DeviceManager->GetDevice("Mouse"));
}

//------------------------------------------------------------------------------
int albaInteractionManager::BindAction(const char *action,albaInteractor *agent)
{
  return m_StaticEventRouter->BindAction(action,agent);
}

//------------------------------------------------------------------------------
int albaInteractionManager::UnBindAction(const char *action,albaInteractor *agent)
{
  return m_StaticEventRouter->UnBindAction(action,agent);
}

//------------------------------------------------------------------------------
albaAction *albaInteractionManager::AddAction(const char *name, float priority)
{
  return m_StaticEventRouter->AddAction(name,priority);
}

//------------------------------------------------------------------------------
albaAction *albaInteractionManager::GetAction(const char *name)
{
  return m_StaticEventRouter->GetAction(name);
}

//------------------------------------------------------------------------------
const albaInteractionManager::mmuAvatarsMap &albaInteractionManager::GetAvatars()
{
  return m_Avatars;
}

//------------------------------------------------------------------------------
void albaInteractionManager::GetAvatars(mmuAvatarsVector &avatars)
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
void albaInteractionManager::AddAvatar(albaAvatar *avatar)
{
  m_Avatars[avatar->GetName()] = avatar;
  
  // attach to the avatar both as a listener and an event source
  avatar->SetListener(this);

  // Link the avatar to the current renderer (if present)
  avatar->SetRendererAndView(m_CurrentRenderer,m_SelectedView);
}

//------------------------------------------------------------------------------
int albaInteractionManager::RemoveAvatar(albaAvatar *avatar)
{
  assert(avatar);

  mmuAvatarsMap::iterator it=m_Avatars.find(avatar->GetName());
  if (it==m_Avatars.end())
    return false;
  
  // detach the avatar
  avatar->SetListener(NULL);
  
  // unlink the avatar from the renderer
  avatar->SetRendererAndView(NULL,NULL);
    
  m_Avatars.erase(it);

  return true;
}
//------------------------------------------------------------------------------
albaAvatar *albaInteractionManager::GetAvatar(const char *name)
{
  mmuAvatarsMap::iterator it=m_Avatars.find(name);  
  return (it!=m_Avatars.end())?it->second:NULL;
}

//------------------------------------------------------------------------------
void albaInteractionManager::ViewSelected(albaView *view)
{
  albaEvent e(this,VIEW_SELECT,view);
  OnViewSelected(&e);
}

//------------------------------------------------------------------------------
void albaInteractionManager::PreResetCamera(vtkRenderer *ren)
{ 
  // propagate event to all avatars...
  for (mmuAvatarsMap::iterator it=m_Avatars.begin();it!=m_Avatars.end();it++)
  {
    it->second->OnEvent(&albaEventBase(this,CAMERA_PRE_RESET,ren));
  }
}

//------------------------------------------------------------------------------
void albaInteractionManager::PostResetCamera(vtkRenderer *ren)
{
  // propagate event to all avatars...
  for (mmuAvatarsMap::iterator it=m_Avatars.begin();it!=m_Avatars.end();it++)
  {
    it->second->OnEvent(&albaEventBase(this,CAMERA_POST_RESET,ren));
  }
}

//------------------------------------------------------------------------------
void albaInteractionManager::VmeSelected(albaVME *vme)
{
  if (vme)
    m_PositionalEventRouter->OnVmeSelected(vme);
}

//------------------------------------------------------------------------------
void albaInteractionManager::CameraUpdate(albaView *view)
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
      albaEventMacro(albaEvent(this,CAMERA_SYNCHRONOUS_UPDATE));
      m_LastRenderTime=vtkTimerLog::GetCurrentTime(); // store time at end of rendering
    }
  }
}

//------------------------------------------------------------------------------
void albaInteractionManager::OnViewSelected(albaEvent *event)
{  
  albaView *v = event->GetView();
	albaViewVTK *view = albaViewVTK::SafeDownCast(v); 
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
    //it->second->OnEvent(&albaEventBase(this,VIEW_SELECT));    
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
void albaInteractionManager::OnDeviceAdded(albaEventBase *event)
{
  albaDevice *device=(albaDevice *)event->GetData(); 
  albaDeviceSet *parent=(albaDeviceSet *)event->GetSender();
  assert(device);
  // forward device to the GUI
  AddDeviceToTree(device,parent);
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnDeviceRemoving(albaEventBase *event)
{
  albaDevice *device=(albaDevice *)event->GetData(); 
  assert(device);
  m_StaticEventRouter->UnBindDeviceFromAllActions(device);
  // forward event to the GUI
  RemoveDeviceFromTree(device);
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnDeviceNameChanged(albaEventBase *event)
{
  albaDevice *device=(albaDevice *)event->GetSender(); 
  assert(device);
  // forward device to the GUI
  UpdateDevice(device);
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnBindDeviceToAction(albaEvent *e)
{
  // this event is rosed by albaAction to ask for 
  // binding of a device to an action. Device is specified by its ID.
  albaDevice *device=m_DeviceManager->GetDevice(e->GetArg());
  if (device)
  {
    albaAction *action=albaAction::SafeDownCast((albaObject *)e->GetSender());
    m_StaticEventRouter->BindDeviceToAction(device,action);
  }
  else
  {
    albaErrorMacro("Bind Restoring Error: unknown device ID!");
  }
}

//------------------------------------------------------------------------------
void albaInteractionManager::OnAddAvatar(albaEventBase *event)
{
  AddAvatar((albaAvatar *)event->GetData());
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnRemoveAvatar(albaEventBase *event)
{
  RemoveAvatar((albaAvatar *)event->GetData());
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnStartDispatching()
{
  m_LockRenderingFlag = true;
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnEndDispatching()
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
      albaEventMacro(albaEvent(this,CAMERA_SYNCHRONOUS_UPDATE));
      m_LastRenderTime=vtkTimerLog::GetCurrentTime(); // store time at end of rendering
    }
    
  }
  else
  {
    // avoid too much renderings which would trash the CPU
    if((vtkTimerLog::GetCurrentTime()-m_LastRenderTime)>m_IntraFrameTime)
    {
      // traverse the list and perform all requested updates
      for (std::set<albaView *>::iterator it=m_CameraUpdateRequests.begin(); \
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
void albaInteractionManager::OnCameraUpdate(albaEventBase *event)
{
  if (albaEvent *e=albaEvent::SafeDownCast(event))
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
void albaInteractionManager::OnEvent(albaEventBase *event)
{
  assert(event);
 
  albaID id=event->GetId();

  albaString action_name;

  albaEvent *e=albaEvent::SafeDownCast(event);
 
  if (e)
  {
    if (id==albaAction::DEVICE_BIND)
    {
      albaEvent *e=albaEvent::SafeDownCast(event);
      OnBindDeviceToAction(e);
    }
    else
    {
      switch(e->GetId())
      {
        /*
        case ID_OK:
		    case wxOK:
          //m_Dialog->EndModal(wxOK);
          if(m_Frame) m_Frame->Show(false);
          return; 
		    break;
		    */
        case ID_STORE:
        { 
          //SIL. 4-7-2005: begin
          std::string result = 
          albaGetSaveFile(m_SettingFileName, "Interaction Settings (*.xml)| *.xml", "Save Interaction Settings");
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
          albaGetOpenFile(m_SettingFileName, "Interaction Settings (*.xml)| *.xml", "Load Interaction Settings");
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
            albaDevice *device = GetDeviceManager()->AddDevice(device_type);
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
            if (albaDevice *device = GetDeviceManager()->GetDevice(e->GetArg()))
            {
              // remove the GUI and Bindings-GUI for the previous seleced device (if any)
              if(m_CurrentDevice)
              {
                albaGUI *previous_gui = m_CurrentDevice->GetGui();
                m_Gui->Remove(previous_gui);
                m_Gui->Remove(m_Bindings);
              }
              // Set the current device
              m_CurrentDevice = device;
              m_CurrentDevice->UpdateGui();

              // Insert the current device GUI and append the m_Bindings Gui
              albaGUI *gui = m_CurrentDevice->GetGui();
              assert(gui);
              
              m_Gui->AddGui(gui,0,0);
              m_Gui->AddGui(m_Bindings,0,0);
              m_Gui->FitGui(); // update the size of m_Gui
              m_Gui->GetParent()->FitInside(); // update the (albaGUIHolder) ScrollBar settings
              
              //Update Bindings panel
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
          else
          {
            // Paolo 9/11/2005: events to be forwarded to logic
            albaEventMacro(*e);
          }
      break; 
      }
    }
    return;
  }
  
  if (id == VME_SELECT || id == VME_DCLICKED)
  {
    // event raised by PER to advise of VME selection or double click on a VME
    albaEventMacro(albaEvent(event->GetSender(), id, (albaVME *)event->GetData()));
  }
  else if (id == VIEW_SELECT)
  {
    albaEvent *e = albaEvent::SafeDownCast(event);
    assert(event);
    OnViewSelected(e);
  }  
  else if (id == albaDeviceManager::DISPATCH_START)
  {
    OnStartDispatching();    
  }
  else if (id == albaDeviceManager::DISPATCH_END)
  {
    OnEndDispatching();
  }
  else if (id == CAMERA_UPDATE)
  {
    OnCameraUpdate(e);
  }
  else if (id == SHOW_VIEW_CONTEXTUAL_MENU)
  {
    albaVME *vme = (albaVME *)event->GetData();
    bool vme_context_menu = (vme != NULL) && !vme->IsA("albaVMEGizmo");
    albaEventMacro(albaEvent(event->GetSender(), SHOW_VIEW_CONTEXTUAL_MENU, vme_context_menu));
  }
  else if (id == albaDevice::DEVICE_NAME_CHANGED) 
  {
    OnDeviceNameChanged(event);
  }
  else if (id == albaDeviceSet::DEVICE_ADDED) 
  {
    OnDeviceAdded(event);
  }
  else if (id == albaDeviceSet::DEVICE_REMOVING) 
  {
    OnDeviceRemoving(event);
  }
  else if (id == AVATAR_ADDED) 
  {
    OnAddAvatar(event);
  }
  else if (id == AVATAR_REMOVED) 
  {
    OnRemoveAvatar(event); 
  }
  else
  {
    InvokeEvent(event);
  }
}

//------------------------------------------------------------------------------
int albaInteractionManager::Store(const char *filename)
{
  assert(filename);
  albaXMLStorage writer;
  
  // Multimod Interaction Settings file format
  writer.SetFileType("MIS");
  writer.SetVersion(MIS_VERSION);

  writer.SetURL(filename);
  writer.SetDocument(this);

  return writer.Store();
}

//------------------------------------------------------------------------------
int albaInteractionManager::Restore(const char *filename)
{
  assert(filename);
  albaXMLStorage reader;

  // Multimod Interaction Settings file format
  reader.SetFileType("MIS");
  reader.SetVersion(MIS_VERSION);

  reader.SetURL(filename);
  reader.SetDocument(this);

  return reader.Restore();
}

//------------------------------------------------------------------------------
int albaInteractionManager::InternalStore(albaStorageElement *node)
{
  // store device settings
  if (node->StoreObject("DeviceManager",m_DeviceManager)==NULL)
    return ALBA_ERROR;
  
  // store bindings
  if (node->StoreObject("DeviceBindings",m_StaticEventRouter)==NULL)
    return ALBA_ERROR;
  
  return ALBA_OK;
}

//------------------------------------------------------------------------------
int albaInteractionManager::InternalRestore(albaStorageElement *node)
{
  if (node->RestoreObject("DeviceManager",m_DeviceManager)!=ALBA_OK)
    return ALBA_ERROR;
  
  if (node->RestoreObject("DeviceBindings",m_StaticEventRouter)!=ALBA_OK)
    return ALBA_ERROR;
  
  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaInteractionManager::DeviceChooser(wxString &dev_name,wxString &dev_type)
{
  albaInteractionFactory *iFactory=albaInteractionFactory::GetInstance();
  
  assert(iFactory);
  if (iFactory->GetNumberOfDevices()>0)
  {
    wxString *devices = new wxString[iFactory->GetNumberOfDevices()];


    for (int id=0;id<iFactory->GetNumberOfDevices();id++)
    {
      devices[id]=iFactory->GetDeviceTypeName(iFactory->GetDeviceName(id));
    }

    //wxSingleChoiceDialog chooser(m_Dialog,"select a device","Device Chooser",iFactory->GetNumberOfDevices(),devices);
    wxSingleChoiceDialog chooser(albaGetFrame(),"select a device","Device Chooser",iFactory->GetNumberOfDevices(),devices);

    int index=-1;
    if (chooser.ShowModal()==wxID_OK)
    {
      index = chooser.GetSelection();
    
      if (index>=0)
      {
        dev_type = iFactory->GetDeviceName(index);
        dev_name = iFactory->GetDeviceTypeName(dev_type);
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
void albaInteractionManager::UpdateBindings()
{
  if (m_CurrentDevice)
  {
    /* Fill in actions' list */
    m_ActionsList->Clear();
    
    const albaInteractorSER::mmuActionsMap *actions=GetSER()->GetActions();

    int i=0;
    for (albaInteractorSER::mmuActionsMap::const_iterator it=actions->begin();it!=actions->end();it++)
    {
      albaAction *action = it->second;
      bool found=false;

      // search through the list
      for (albaAction::mmuDeviceList::const_iterator it_list=action->GetDevices()->begin();it_list!=action->GetDevices()->end();it_list++)
      {
        if (it_list->GetPointer() == m_CurrentDevice)
          found = true;
      }
      m_ActionsList->AddItem(i++,action->GetName(),found);
    }
  }
}

//----------------------------------------------------------------------------
void albaInteractionManager::AddDeviceToTree(albaDevice *device,albaDeviceSet *parent)
{
  assert(device);
  albaID parent_id=(parent?parent->GetID():0);
  m_DeviceTree->AddNode(device->GetID(),parent_id,device->GetName());
  //m_DeviceTree->Update();
}

//----------------------------------------------------------------------------
void albaInteractionManager::RemoveDeviceFromTree(albaDevice *device)
{
  assert(device);
  m_DeviceTree->DeleteNode(device->GetID());
}
//----------------------------------------------------------------------------
void albaInteractionManager::UpdateDevice(albaDevice *device)
{
  assert(device);
  m_DeviceTree->SetNodeLabel(device->GetID(),device->GetName());
}
//----------------------------------------------------------------------------
void albaInteractionManager::CreateGUI() 
{
  m_Gui = new albaGUI(this);

  m_DeviceTree = new albaGUITree(m_Gui,ID_DEVICE_TREE,false,true);
  m_DeviceTree->SetMinSize(wxSize(250,100));
  m_DeviceTree->SetListener(this);

  m_Gui->Button(ID_ADD_DEVICE,"Add Device");
  m_Gui->Button(ID_REMOVE_DEVICE,"Remove Device");
  m_Gui->Button(ID_LOAD,"Load");
  m_Gui->Button(ID_STORE,"Save as");
  m_Gui->Label("installed devices");
  m_Gui->Add(m_DeviceTree,0,0);
  m_Gui->Label("selected device options");
  m_Gui->Divider();

  m_Bindings = new albaGUI(this); // bindings will be added to m_Gui later (at the device selection)
  m_Bindings->Divider(1);
  m_Bindings->Label("actions associated to this device",false);
  m_ActionsList = m_Bindings->CheckList(ID_BINDING_LIST,"",60);
  m_ActionsList->SetSize(250,100);
  m_ActionsList->SetMinSize(wxSize(250,100));
} 

//----------------------------------------------------------------------------
albaGUI* albaInteractionManager::GetGui()
{
  return m_Gui;
}
