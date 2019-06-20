/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mmuIdFactory.cpp,v $
Language:  C++
Date:      $Date: 2007-11-05 12:43:02 $
Version:   $Revision: 1.4 $

=========================================================================*/

#include "mmuIdFactory.h"
#include "albaDecl.h"
#include <map>
#include <vector>
#include <string>

std::map<std::string,albaID> *mmuIdFactoryMap=NULL;
std::vector<std::string> *mmuIdFactoryNames=NULL;

/** this class is used to destroy Id factory */
class mmuIdFactoryDestroyer
{
  public:
  mmuIdFactoryDestroyer() {}
  ~mmuIdFactoryDestroyer() { \
    if (mmuIdFactoryMap) \
      delete mmuIdFactoryMap; \
    if (mmuIdFactoryNames) \
      delete mmuIdFactoryNames; \
  } // this is to allow memory deallocation
}; 

static mmuIdFactoryDestroyer mmuIdFactoryDestroyerSingleton;

// This is for allocating unique  Ids.
albaID mmuIdFactory::m_IdCounter = ALBA_BASE_ID;

/* not used
//------------------------------------------------------------------------------
albaID mmuIdFactory::AllocIdGroup(const char *idname,int num)
//------------------------------------------------------------------------------
{
  albaID id=m_IdCounter;
  m_IdCounter+=num;

  if (mmuIdFactoryMap==NULL)
  {
    mmuIdFactoryMap   = new std::map<std::string,albaID>;
    mmuIdFactoryNames = new std::vector<std::string>;
  }

  (*mmuIdFactoryMap)[idname]=id;

 /////////////////////
  (*mmuIdFactoryNames).resize(id+num);  
  for (int n = 0; n < num; n++)
  {
    (*mmuIdFactoryNames)[id + n]=idname;
  }
///////////////////////

  return id;
}*/

//------------------------------------------------------------------------------
albaID mmuIdFactory::GetNextId(const char *idname)
//------------------------------------------------------------------------------
{
  albaID id=m_IdCounter++;

  if (mmuIdFactoryMap==NULL)
  {
    mmuIdFactoryMap   = new std::map<std::string,albaID>;
    mmuIdFactoryNames = new std::vector<std::string>;
  }

  (*mmuIdFactoryMap)[idname]=id;
  
  (*mmuIdFactoryNames).resize(id+1);  
  (*mmuIdFactoryNames)[id]=idname;

  return id;
}

//------------------------------------------------------------------------------
albaID mmuIdFactory::GetId(const char *idname)
//------------------------------------------------------------------------------
{
  albaID id=0;
  
  std::map<std::string,albaID>::iterator  it=(*mmuIdFactoryMap).find(idname);

  if (it!=(*mmuIdFactoryMap).end())
  {
    id=it->second;
  }
  
  return id;
}

//------------------------------------------------------------------------------
const char *mmuIdFactory::GetIdName(albaID id)
//------------------------------------------------------------------------------
{
  return (id<(*mmuIdFactoryNames).size())?(*mmuIdFactoryNames)[id].c_str():NULL;
}

