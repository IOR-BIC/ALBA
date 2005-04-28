/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDevice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:10 $
  Version:   $Revision: 1.1 $
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

// serialization
#include "mafStorageElement.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDevice::DEVICE_NAME_CHANGED)
MAF_ID_IMP(mafDevice::DEVICE_STARTED)
MAF_ID_IMP(mafDevice::DEVICE_STOPPED)

//------------------------------------------------------------------------------
mafDevice::mafDevice()
//------------------------------------------------------------------------------
{
  m_ID        = 0;
  m_Settings  = NULL;
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
  ForwardEvent(DEVICE_NAME_CHANGED); // send event to device manager (up)
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
  ForwardEvent(DEVICE_STARTED,MCH_INPUT);

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
  ForwardEvent(DEVICE_STOPPED,MCH_INPUT);
}

//------------------------------------------------------------------------------
int mafDevice::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if (node->StoreText("Name",m_Name)==MAF_OK && \
      node->StoreInteger("ID",(m_ID-MIN_DEVICE_ID)) && \
      node->StoreInteger("AutoStart",m_AutoStart))  
  return MAF_OK;

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafDevice::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{  
  // Device Name
  if (node->RestoreText("Name",m_Name))
  {
    int flag;
    // AutoStart flag (optional)
    if (node->RestoreInteger("AutoStart",flag))
    {
      SetAutoStart(flag!=0);      
    }

    // the ID???
    return MAF_OK;
  }

  return MAF_ERROR;
}