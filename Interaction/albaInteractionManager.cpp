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

#include "albaEventBase.h"

#include "albaInteractionFactory.h"

#include "albaDeviceManager.h"
#include "albaDeviceSet.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaToolFlyTo.h"
  
#include "albaVME.h"
#include "albaVMEGizmo.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <set>
#include <assert.h>



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
  m_CurrentDevice     = NULL;
  
  albaNEW(m_DeviceManager);
  m_DeviceManager->SetListener(this);
  m_DeviceManager->SetName("DeviceManager");
  
  CreateGUI();

  m_DeviceManager->Initialize();
  AddDeviceToTree(m_DeviceManager->GetDeviceSet()); // add dev_mgr node as root
  
  // create positional event router
  albaNEW(m_PositionalEventRouter);
  m_PositionalEventRouter->SetName("PositionalEventRouter");
  m_PositionalEventRouter->SetListener(this);
  m_PositionalEventRouter->Initialize();

  // create a Mouse device
  // the device is plugged here instead of App::Init() since mouse is plugged 
  // by default in all applications!
  albaPlugDevice<albaDeviceButtonsPadMouse>("Mouse");
  albaDeviceButtonsPadMouse *mouse_device = (albaDeviceButtonsPadMouse *)m_DeviceManager->AddDevice("albaDeviceButtonsPadMouse",true); // add as persistent device
  assert(mouse_device);
  mouse_device->AddObserver(m_PositionalEventRouter, MCH_INPUT); // the mouse events go to the PER
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
  ID_OK
};
//------------------------------------------------------------------------------
void albaInteractionManager::OnEvent(albaEventBase *event)
{
  assert(event);
 
  albaID id=event->GetId();

  albaEvent *e=albaEvent::SafeDownCast(event);
 
  if (e)
  {
    switch(e->GetId())
    {
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
          }
          return;
         break;
        case ID_DEVICE_TREE:
          if (e->GetSender() == m_DeviceTree)
          { 
            // a device has been selected on the devices tree
            if (albaDevice *device = GetDeviceManager()->GetDevice(m_DeviceTree->GetSelectedNode()))
            {
              // remove the GUI of the previous selected device (if any)
              if(m_CurrentDevice)
              {
                albaGUI *previous_gui = m_CurrentDevice->GetGui();
                m_Gui->Remove(previous_gui);
              }
              // Set the current device
              m_CurrentDevice = device;
              m_CurrentDevice->UpdateGui();

              // Insert the current device GUI
              albaGUI *gui = m_CurrentDevice->GetGui();
              assert(gui);
              
              m_Gui->AddGui(gui,0,0);
              m_Gui->FitGui(); // update the size of m_Gui
              m_Gui->GetParent()->FitInside(); // update the (albaGUIHolder) ScrollBar settings
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
  m_Gui->Label("installed devices");
  m_Gui->Add(m_DeviceTree,0,0);
  m_Gui->Label("selected device options");
  m_Gui->Divider();
} 

//----------------------------------------------------------------------------
albaGUI* albaInteractionManager::GetGui()
{
  return m_Gui;
}
