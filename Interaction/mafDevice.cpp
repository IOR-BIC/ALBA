/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDevice.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:38 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// to be includes first: includes wxWindows too...
#include "mafDefines.h"

// base includes
#include "mafDevice.h"
#include "mmuIdFactory.h"

// GUI
#include "mafGUI.h"

// serialization
#include "mafStorageElement.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDevice::DEVICE_NAME_CHANGED)
MAF_ID_IMP(mafDevice::DEVICE_STARTED)
MAF_ID_IMP(mafDevice::DEVICE_STOPPED)

mafCxxTypeMacro(mafDevice)

//------------------------------------------------------------------------------
mafDevice::mafDevice()
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
mafDevice::~mafDevice()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafDevice::SetName(const char *name)
//------------------------------------------------------------------------------
{
  Superclass::SetName(name);
  InvokeEvent(DEVICE_NAME_CHANGED); // send event to device manager (up)
}

//------------------------------------------------------------------------------
int mafDevice::InternalInitialize()
//------------------------------------------------------------------------------
{
  int ret=Superclass::InternalInitialize();
  m_AutoStart = 1; // enable auto starting of device

  // update the GUI if present
  return ret;
}

//------------------------------------------------------------------------------
int mafDevice::Start()
//------------------------------------------------------------------------------
{
  if (Initialize())
    return MAF_ERROR;

  // send an event to advise interactors this device has been started
  InvokeEvent(DEVICE_STARTED,MCH_INPUT);

  return MAF_OK;
}

//------------------------------------------------------------------------------
void mafDevice::Stop()
//------------------------------------------------------------------------------
{
  if (!m_Initialized)
    return;

  Shutdown();

  // send an event to advise interactors this device has been stopped
  InvokeEvent(DEVICE_STOPPED,MCH_INPUT);
}

//----------------------------------------------------------------------------
mafGUI *mafDevice::GetGui()
//----------------------------------------------------------------------------
{
  if (!m_Gui)
    CreateGui();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafDevice::CreateGui()
//----------------------------------------------------------------------------
{
  /*  //SIL. 07-jun-2006 : 
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->String(ID_NAME,"name",&m_Name);
  m_Gui->Divider();
  m_Gui->Button(ID_ACTIVATE,"activate device");
  m_Gui->Button(ID_SHUTDOWN,"shutdown device");
  m_Gui->Bool(ID_AUTO_START,"auto start",&m_AutoStart,0,"automatically start device on application startup");
  m_Gui->Enable(ID_ACTIVATE,!IsInitialized());
  m_Gui->Enable(ID_SHUTDOWN,IsInitialized()!=0);
  */

  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->String(ID_NAME,"name",&m_Name);
  m_Gui->Divider();
  m_Gui->Bool(ID_ACTIVATE,"start",&m_Start,0,"activate/deactivate this device");
  m_Gui->Bool(ID_AUTO_START,"auto start",&m_AutoStart,0,"automatically activate device on application startup");
  //m_Gui->Enable(ID_ACTIVATE,!IsInitialized());
  //m_Gui->Enable(ID_SHUTDOWN,IsInitialized()!=0);

	m_Gui->Divider();
}

//----------------------------------------------------------------------------
void mafDevice::UpdateGui()
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
void mafDevice::OnEvent(mafEventBase *e)
//----------------------------------------------------------------------------
{
  mafEvent *ev = mafEvent::SafeDownCast(e);
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
            mafErrorMessage("Cannot Initialize Device","I/O Error");
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
int mafDevice::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if (node->StoreText("Name",m_Name)==MAF_OK && \
      node->StoreInteger("ID",(m_ID-MIN_DEVICE_ID))==MAF_OK && \
      node->StoreInteger("AutoStart",m_AutoStart)==MAF_OK)  
  return MAF_OK;

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafDevice::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{  
  // Device Name
  if (node->RestoreText("Name",m_Name)==MAF_OK)
  {
    int dev_id;
    node->RestoreInteger("ID",dev_id);
    SetID(dev_id+MIN_DEVICE_ID);
    int flag;
    // AutoStart flag (optional)
    if (node->RestoreInteger("AutoStart",flag)==MAF_OK)
    {
      SetAutoStart(flag!=0);      
    }

    // the ID???
    return MAF_OK;
  }

  return MAF_ERROR;
}
