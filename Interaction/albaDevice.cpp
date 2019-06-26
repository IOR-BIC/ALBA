/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDevice
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// to be includes first: includes wxWindows too...
#include "albaDefines.h"

// base includes
#include "albaDevice.h"
#include "mmuIdFactory.h"

// GUI
#include "albaGUI.h"

// serialization
#include "albaStorageElement.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaDevice::DEVICE_NAME_CHANGED)
ALBA_ID_IMP(albaDevice::DEVICE_STARTED)
ALBA_ID_IMP(albaDevice::DEVICE_STOPPED)

albaCxxTypeMacro(albaDevice)

//------------------------------------------------------------------------------
albaDevice::albaDevice()
//------------------------------------------------------------------------------
{
  m_Gui       = NULL;
  m_ID        = 0;
  m_Start     = false;
  m_AutoStart = false; // auto is enabled when device is started the first time
  m_Locked    = false;
  m_PersistentFalg = false;
}

//------------------------------------------------------------------------------
albaDevice::~albaDevice()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaDevice::SetName(const char *name)
//------------------------------------------------------------------------------
{
  Superclass::SetName(name);
  InvokeEvent(this,DEVICE_NAME_CHANGED); // send event to device manager (up)
}

//------------------------------------------------------------------------------
int albaDevice::InternalInitialize()
//------------------------------------------------------------------------------
{
  int ret=Superclass::InternalInitialize();
  m_AutoStart = 1; // enable auto starting of device

  // update the GUI if present
  return ret;
}

//------------------------------------------------------------------------------
int albaDevice::Start()
//------------------------------------------------------------------------------
{
  if (Initialize())
    return ALBA_ERROR;

  // send an event to advise interactors this device has been started
  InvokeEvent(this,DEVICE_STARTED,MCH_INPUT);

  return ALBA_OK;
}

//------------------------------------------------------------------------------
void albaDevice::Stop()
//------------------------------------------------------------------------------
{
  if (!m_Initialized)
    return;

  Shutdown();

  // send an event to advise interactors this device has been stopped
  InvokeEvent(this,DEVICE_STOPPED,MCH_INPUT);
}

//----------------------------------------------------------------------------
albaGUI *albaDevice::GetGui()
//----------------------------------------------------------------------------
{
  if (!m_Gui)
    CreateGui();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaDevice::CreateGui()
//----------------------------------------------------------------------------
{
  /*  //SIL. 07-jun-2006 : 
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->String(ID_NAME,"name",&m_Name);
  m_Gui->Divider();
  m_Gui->Button(ID_ACTIVATE,"activate device");
  m_Gui->Button(ID_SHUTDOWN,"shutdown device");
  m_Gui->Bool(ID_AUTO_START,"auto start",&m_AutoStart,0,"automatically start device on application startup");
  m_Gui->Enable(ID_ACTIVATE,!IsInitialized());
  m_Gui->Enable(ID_SHUTDOWN,IsInitialized()!=0);
  */

  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->String(ID_NAME,"name",&m_Name);
  m_Gui->Divider();
  m_Gui->Bool(ID_ACTIVATE,"start",&m_Start,0,"activate/deactivate this device");
  m_Gui->Bool(ID_AUTO_START,"auto start",&m_AutoStart,0,"automatically activate device on application startup");
  //m_Gui->Enable(ID_ACTIVATE,!IsInitialized());
  //m_Gui->Enable(ID_SHUTDOWN,IsInitialized()!=0);

	m_Gui->Divider();
}

//----------------------------------------------------------------------------
void albaDevice::UpdateGui()
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    //m_Gui->Enable(ID_ACTIVATE,!IsInitialized());
    //m_Gui->Enable(ID_SHUTDOWN,IsInitialized()!=0);
    m_Start = IsInitialized();
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void albaDevice::OnEvent(albaEventBase *e)
//----------------------------------------------------------------------------
{
  albaEvent *ev = albaEvent::SafeDownCast(e);
  if (ev&& ev->GetSender()==m_Gui)
  {
    switch(ev->GetId()) 
    {
    case ID_NAME:
      SetName(m_Name); // force sending an event
      break;
    case ID_ACTIVATE:
      if(m_Start) // user request to Start
      {
          if (Initialize())
            albaErrorMessage("Cannot Initialize Device","I/O Error");
      }
      else // user request to Stop
      {
        Shutdown();
      }
      UpdateGui();
      break;
    /*  //SIL. 07-jun-2006 : 
    case ID_SHUTDOWN:
      Shutdown();
      UpdateGui();
      break; 
    */  
    }
    
    return;
  }
  else
  {
    // pass event to superclass to be processed
    Superclass::OnEvent(e);
  }
  
}
//------------------------------------------------------------------------------
int albaDevice::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  if (node->StoreText("Name",m_Name)==ALBA_OK && \
      node->StoreInteger("ID",(m_ID-MIN_DEVICE_ID))==ALBA_OK && \
      node->StoreInteger("AutoStart",m_AutoStart)==ALBA_OK)  
  return ALBA_OK;

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaDevice::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{  
  // Device Name
  if (node->RestoreText("Name",m_Name)==ALBA_OK)
  {
    int dev_id;
    node->RestoreInteger("ID",dev_id);
    SetID(dev_id+MIN_DEVICE_ID);
    int flag;
    // AutoStart flag (optional)
    if (node->RestoreInteger("AutoStart",flag)==ALBA_OK)
    {
      SetAutoStart(flag!=0);      
    }

    // the ID???
    return ALBA_OK;
  }

  return ALBA_ERROR;
}
