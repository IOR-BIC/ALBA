/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractionFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 07:57:08 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafInteractionFactory.h"
#include "mafDevice.h"
#include "mafAvatar.h"
#include "mafVersion.h"
#include "mafIndent.h"
#include <string>
#include <ostream>

//----------------------------------------------------------------------------
// static variables
//----------------------------------------------------------------------------
mafInteractionFactory *mafInteractionFactory::m_Instance=NULL;
std::set<std::string> mafInteractionFactory::m_DeviceNames; 
std::set<std::string> mafInteractionFactory::m_AvatarNames;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractionFactory);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafInteractionFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafInteractionFactory::New();

    if (m_Instance)
    {
      m_Instance->RegisterFactory(m_Instance);
      return MAF_OK;  
    }
    else
    {
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
mafInteractionFactory::mafInteractionFactory()
//------------------------------------------------------------------------------
{
  m_Instance = NULL;
  
  //
  // Plug here Devices in this factory
  //
  //mafPlugDeviceMacro(mmdViconTracker,"tracker set device for Vicon systems");
  
  
  //
  // Plug here Devices that should not stay in the list (sub devices)
  //
  //mafPlugObjectMacro(mmdViconTool,"a tracker device representing the single body tracked by a vicon system");
  
  //
  // Plug here Avatars in this factory
  //
  //mafPlugAvatarMacro(mafAvatar3DCone,"an avatar represented as a 3D cone");
}

//------------------------------------------------------------------------------
const char* mafInteractionFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafInteractionFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF Devices and Avatars";
}

//------------------------------------------------------------------------------
mafDevice *mafInteractionFactory::CreateDeviceInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafDevice::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void mafInteractionFactory::RegisterNewDevice(const char* node_name, const char* description, mafCreateObjectFunction createFunction)
//------------------------------------------------------------------------------
{
  m_DeviceNames.insert(node_name);
  RegisterNewObject(node_name,description,createFunction);
}

//------------------------------------------------------------------------------
mafAvatar *mafInteractionFactory::CreateAvatarInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafAvatar::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void mafInteractionFactory::RegisterNewAvatar(const char* node_name, const char* description, mafCreateObjectFunction createFunction)
//------------------------------------------------------------------------------
{
  m_AvatarNames.insert(node_name);
  RegisterNewObject(node_name,description,createFunction);
}

//------------------------------------------------------------------------------
const char *mafInteractionFactory::GetDeviceDescription(const char *device_name)
//------------------------------------------------------------------------------
{
  // check if that device exists
  if (m_DeviceNames.find(device_name)!=m_DeviceNames.end())
  {
    mmuOverRideMap::iterator pos = m_OverrideMap->find(device_name);
    if ( pos != m_OverrideMap->end() )
    {
      return (*pos).second.m_Description.c_str();
    }
  }
  
  return NULL;
}

//------------------------------------------------------------------------------
const char *mafInteractionFactory::GetAvatarDescription(const char *avatar_name)
//------------------------------------------------------------------------------
{
  // check if that device exists
  if (m_AvatarNames.find(avatar_name)!=m_DeviceNames.end())
  {
    mmuOverRideMap::iterator pos = m_OverrideMap->find(avatar_name);
    if ( pos != m_OverrideMap->end() )
    {
      return (*pos).second.m_Description.c_str();
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
const std::set<std::string> *mafInteractionFactory::GetDeviceNames()
//------------------------------------------------------------------------------ 
{
  return &m_DeviceNames;
}

//------------------------------------------------------------------------------
const std::set<std::string> *mafInteractionFactory::GetAvatarNames()
//------------------------------------------------------------------------------
{
  return &m_AvatarNames;
}
