/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionFactory
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

#include "albaInteractionFactory.h"
#include "albaDevice.h"
#include "albaAvatar.h"
#include "albaIndent.h"
#include <string>
#include <ostream>

//----------------------------------------------------------------------------
// static variables
bool albaInteractionFactory::m_Initialized=false;
// albaInteractionFactory *albaInteractionFactory::m_Instance=NULL;
std::set<std::string> albaInteractionFactory::glo_DeviceNames; 
std::set<std::string> albaInteractionFactory::glo_AvatarNames;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractionFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int albaInteractionFactory::Initialize()
{
  if (!m_Initialized)
  {
    //m_Instance=albaInteractionFactory::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return ALBA_OK;  
    }
    else
    {
      m_Initialized=false;
      return ALBA_ERROR;
    }
  }
  
  return ALBA_OK;
}

//------------------------------------------------------------------------
albaInteractionFactory::albaInteractionFactory()
{
  //m_Instance = NULL;
  
  //
  // Plug here Devices in this factory
  //
  //albaPlugDeviceMacro(mmdViconTracker,"tracker set device for Vicon systems");
  
  
  //
  // Plug here Devices that should not stay in the list (sub devices)
  //
  //albaPlugObjectMacro(mmdViconTool,"a tracker device representing the single body tracked by a vicon system");
  
  //
  // Plug here Avatars in this factory
  //
  //albaPlugAvatarMacro(albaAvatar3DCone,"an avatar represented as a 3D cone");
}


//------------------------------------------------------------------------------
const char* albaInteractionFactory::GetDescription() const
{
  return "Factory for ALBA Devices and Avatars";
}

//------------------------------------------------------------------------------
albaDevice *albaInteractionFactory::CreateDeviceInstance(const char *type_name)
{
  return albaDevice::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void albaInteractionFactory::RegisterNewDevice(const char* node_name, const char* description, albaCreateObjectFunction createFunction)
{
  glo_DeviceNames.insert(node_name);
  RegisterNewObject(node_name,description,createFunction);
}

//------------------------------------------------------------------------------
albaAvatar *albaInteractionFactory::CreateAvatarInstance(const char *type_name)
{
  return albaAvatar::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void albaInteractionFactory::RegisterNewAvatar(const char* node_name, const char* description, albaCreateObjectFunction createFunction)
{
  glo_AvatarNames.insert(node_name);
  RegisterNewObject(node_name,description,createFunction);
}

//------------------------------------------------------------------------------
const char *albaInteractionFactory::GetDeviceTypeName(const char *device_name)
//------------------------------------------------------------------------------
{
  // check if that device exists
  if (glo_DeviceNames.find(device_name)!=glo_DeviceNames.end())
  {
    mmuOverRideMap::iterator pos = m_OverrideMap->find(device_name);
    if ( pos != m_OverrideMap->end() )
    {
      return (*pos).second.m_TypeName.c_str();
    }
  }
  
  return NULL;
}

//------------------------------------------------------------------------------
const char *albaInteractionFactory::GetAvatarDescription(const char *avatar_name)
//------------------------------------------------------------------------------
{
  // check if that device exists
  if (glo_AvatarNames.find(avatar_name)!=glo_DeviceNames.end())
  {
    mmuOverRideMap::iterator pos = m_OverrideMap->find(avatar_name);
    if ( pos != m_OverrideMap->end() )
    {
      return (*pos).second.m_TypeName.c_str();
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
const std::set<std::string> *albaInteractionFactory::GetDeviceNames()
//------------------------------------------------------------------------------ 
{
  return &glo_DeviceNames;
}

//------------------------------------------------------------------------------
const std::set<std::string> *albaInteractionFactory::GetAvatarNames()
//------------------------------------------------------------------------------
{
  return &glo_AvatarNames;
}

//------------------------------------------------------------------------------
int albaInteractionFactory::GetNumberOfDevices()
//------------------------------------------------------------------------------
{
  return glo_DeviceNames.size();
}

//------------------------------------------------------------------------------
int albaInteractionFactory::GetNumberOfAvatars()
//------------------------------------------------------------------------------
{
  return glo_AvatarNames.size();
}

//------------------------------------------------------------------------------
const char *albaInteractionFactory::GetDeviceName(int idx)
//------------------------------------------------------------------------------
{
  std::set<std::string>::iterator it = glo_DeviceNames.begin();
  for (int i=0;i<idx;i++)
    it++;
  return it->c_str();
}

//------------------------------------------------------------------------------
const char *albaInteractionFactory::GetAvatarName(int idx)
//------------------------------------------------------------------------------
{
  std::set<std::string>::iterator it = glo_AvatarNames.begin();
  for (int i=0;i<idx;i++)
    it++;
  return it->c_str();
}
//------------------------------------------------------------------------------
albaInteractionFactory* albaInteractionFactory::GetInstance()
//------------------------------------------------------------------------------
{
  static albaInteractionFactory &istance = *(albaInteractionFactory::New());
  Initialize();
  return &istance;
}
