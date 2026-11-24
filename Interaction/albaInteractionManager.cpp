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

#include "albaPositionalEventRouter.h"
#include "albaStaticEventRouter.h"

#include "albaEventBase.h"

#include "albaInteractionFactory.h"

#include "albaDeviceManager.h"
#include "albaAction.h"
#include "albaDeviceSet.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaToolFlyTo.h"
  
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaXMLStorage.h"
#include "albaStorageElement.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <set>
#include <assert.h>

#define MIS_VERSION "2.0"


//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractionManager);

//------------------------------------------------------------------------------
albaInteractionManager::albaInteractionManager()
{
  m_FlyToTool = NULL;
  m_Gui = NULL;  //SIL. 07-jun-2006 : 
  m_Frame = NULL;  //SIL. 07-jun-2006 : 
  m_SelectedView      = NULL;

  m_Devices           = NULL;
  m_DeviceTree        = NULL;
  //m_Dialog            = NULL;
  m_SettingsPanel     = NULL;
  //m_BindingsPanel     = NULL;
  m_CurrentDevice     = NULL;
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
  if (m_FlyToTool)
    m_PositionalEventRouter->PopTool(m_FlyToTool);
  albaDEL(m_FlyToTool);
  m_DeviceManager->Shutdown();
  cppDEL(m_Frame);
  albaDEL(m_DeviceManager);
  albaDEL(m_StaticEventRouter);
  albaDEL(m_PositionalEventRouter);
}

//----------------------------------------------------------------------------
void albaInteractionManager::PushTool(albaInteractionTool *tool)
{
  m_PositionalEventRouter->PushTool(tool);
}
//----------------------------------------------------------------------------
void albaInteractionManager::PopTool(albaInteractionTool *tool)
{
  m_PositionalEventRouter->PopTool(tool);
}
//----------------------------------------------------------------------------
bool albaInteractionManager::OnKey(int keyCode, bool pressed)
{
  return m_PositionalEventRouter->OnKey(keyCode, pressed);
}
//----------------------------------------------------------------------------
void albaInteractionManager::SetEventTracing(bool enable)
{
  m_PositionalEventRouter->SetEventTracing(enable);
}

//----------------------------------------------------------------------------
void albaInteractionManager::CameraFlyToMode()
{
  if (m_FlyToTool == NULL)
    albaNEW(m_FlyToTool);
  PushTool(m_FlyToTool);
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
void albaInteractionManager::ViewSelected(albaView *view)
{
  m_SelectedView = albaViewVTK::SafeDownCast(view);
  GetMouseDevice()->SetView(view);
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
void albaInteractionManager::OnStartDispatching()
{
	if (GetViewManager())
		GetViewManager()->EnableCameraUpdateSkip();
}
//------------------------------------------------------------------------------
void albaInteractionManager::OnEndDispatching()
{
	if (GetViewManager())
		GetViewManager()->DisableCameraUpdateSkip();
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
        case ID_DEVICE_TREE:
          if (e->GetSender() == m_DeviceTree)
          { 
            // a device has been selected on the devices tree
            if (albaDevice *device = GetDeviceManager()->GetDevice(m_DeviceTree->GetSelectedNode()))
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
  
  if (id == albaDeviceManager::DISPATCH_START)
  {
    OnStartDispatching();    
  }
  else if (id == albaDeviceManager::DISPATCH_END)
  {
    OnEndDispatching();
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
    
    const albaStaticEventRouter::mmuActionsMap *actions=GetSER()->GetActions();

    int i=0;
    for (albaStaticEventRouter::mmuActionsMap::const_iterator it=actions->begin();it!=actions->end();it++)
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
  m_DeviceTree->AddNode(device->GetID(),parent_id,device->GetName(),true);
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
