/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafInteractionFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:12 $
  Version:   $Revision: 1.1 $

=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafInteractionFactory.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mafInteractionFactory.h"
#include "mflCoreFactory.h"
#include "vtkVersion.h"
#include "mafAvatar.h"
#include "mafDevice.h"

#include <vector>
#include <string>
#include <utility>

typedef struct std::pair<std::string,std::string> list_item;

class mafInteractionFactory::PIMPL : public vtkObjectBase
{
public:
  static mafInteractionFactory::PIMPL *New() {return new mafInteractionFactory::PIMPL;}

  std::vector<list_item> DevicesList;
  std::vector<list_item> AvatarsList;
};

mafInteractionFactory::PIMPL *mafInteractionFactory::Internals = NULL;

//---------------------------------------------------------------------------
// Static Variables
//----------------------------------------------------------------------------
mafID DeviceCounter = 0; // Counter of Devices in the factory
mafID AvatarCounter = 0; // Counter of Avatars in the factory

// Factory pointer
mafInteractionFactory * mafInteractionFactory::InteractionFactory = NULL;

//----------------------------------------------------------------------------
mafInteractionFactory *mafInteractionFactory::GetInstance()
//----------------------------------------------------------------------------
{
  if (InteractionFactory==NULL)
    Initialize();
  
  return InteractionFactory;
} 

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
//------------------------------------------------------------------------
void mafInteractionFactory::Initialize()
{
  InteractionFactory=mafInteractionFactory::New();

  if (InteractionFactory)
  {
    InteractionFactory->RegisterFactory(InteractionFactory);
    InteractionFactory->Delete();  
  }
}


//------------------------------------------------------------------------
mafInteractionFactory* mafInteractionFactory::New()
//------------------------------------------------------------------------
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("mafInteractionFactory");
  
  if(ret)
  {
    return (mafInteractionFactory*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new mafInteractionFactory;
}

//------------------------------------------------------------------------
void mafInteractionFactory::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------
{
  os << indent << "MAF Interaction objects factory" << endl;
  os << indent << "Number of Devices = " << DeviceCounter << endl;
  os << indent << "Number of Avatars = " << AvatarCounter << endl;
}

//------------------------------------------------------------------------
// Table of create functions for factory's classes
//------------------------------------------------------------------------
//VTK_CREATE_CREATE_FUNCTION(mmdP5Glove);//SIL. 26-5-2004: 


//------------------------------------------------------------------------
mafInteractionFactory::mafInteractionFactory()
//------------------------------------------------------------------------
{
  if (Internals==NULL)
  {
    Internals = mafInteractionFactory::PIMPL::New(); 
  }
  else
  {
    Internals->Register(this);
  }
  //RegisterNewDevice(mmdP5Glove,"driver for essential reality P5 glove");//SIL. 26-5-2004: 
}

//------------------------------------------------------------------------
mafInteractionFactory::~mafInteractionFactory()
//------------------------------------------------------------------------
{
  Internals->UnRegister(this);
}
//------------------------------------------------------------------------
const char* mafInteractionFactory::GetVTKSourceVersion()
//------------------------------------------------------------------------
{
  return VTK_SOURCE_VERSION;
}

//------------------------------------------------------------------------
const char* mafInteractionFactory::GetDescription()
//------------------------------------------------------------------------
{
  return "MAF Interaction Objects Factory";
}

//------------------------------------------------------------------------
mafID mafInteractionFactory::RegisterNewDevice(const char* type, const char* name, CreateFunction createFunction)
//------------------------------------------------------------------------
{ 
  RegisterNewObject(type,name,createFunction); 
  Internals->DevicesList.push_back(std::make_pair(type,name));
  return Internals->DevicesList.size();
}

//------------------------------------------------------------------------
mafID mafInteractionFactory::RegisterNewAvatar(const char* type, const char* name, CreateFunction createFunction)
//------------------------------------------------------------------------
{
  RegisterNewObject(type,name,createFunction);
  Internals->AvatarsList.push_back(std::make_pair(type,name));
  return Internals->AvatarsList.size();
}

//------------------------------------------------------------------------
int mafInteractionFactory::GetNumberOfDevices()
//------------------------------------------------------------------------
{
  return Internals->DevicesList.size();
}

//------------------------------------------------------------------------
int mafInteractionFactory::GetNumberOfAvatars()
//------------------------------------------------------------------------
{
  return Internals->AvatarsList.size();
}

//------------------------------------------------------------------------
const char *mafInteractionFactory::GetDeviceName(mafID idx)
//------------------------------------------------------------------------
{
  return Internals->DevicesList[idx].second.c_str();
}

//------------------------------------------------------------------------
const char *mafInteractionFactory::GetDeviceName(const char *type)
//------------------------------------------------------------------------
{
  for (int i=0;i<Internals->DevicesList.size();i++)
    if (Internals->DevicesList[i].first == type)
      return Internals->DevicesList[i].second.c_str();
  return NULL;
}

//------------------------------------------------------------------------
const char *mafInteractionFactory::GetDeviceType(mafID idx)
//------------------------------------------------------------------------
{
  return Internals->DevicesList[idx].first.c_str();
}
//------------------------------------------------------------------------
const char *mafInteractionFactory::GetAvatarName(mafID idx)
//------------------------------------------------------------------------
{
  return Internals->AvatarsList[idx].second.c_str();
}

//------------------------------------------------------------------------
const char *mafInteractionFactory::GetAvatarName(const char *type)
//------------------------------------------------------------------------
{
  for (int i=0;i<Internals->AvatarsList.size();i++)
    if (Internals->AvatarsList[i].first == type)
      return Internals->AvatarsList[i].second.c_str();
  return NULL;
}

//------------------------------------------------------------------------
const char *mafInteractionFactory::GetAvatarType(mafID idx)
//------------------------------------------------------------------------
{
  return Internals->AvatarsList[idx].first.c_str();
}
//------------------------------------------------------------------------
mafAvatar *mafInteractionFactory::CreateAvatarInstance(const char *name)
//------------------------------------------------------------------------
{
  return mafAvatar::SafeDownCast(GetInstance()->CreateObject(name));
}
//------------------------------------------------------------------------
mafDevice *mafInteractionFactory::CreateDeviceInstance(const char *name)
//------------------------------------------------------------------------
{
  return mafDevice::SafeDownCast(GetInstance()->CreateObject(name));
}
