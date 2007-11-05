/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mmuIdFactory.cpp,v $
Language:  C++
Date:      $Date: 2007-11-05 12:43:02 $
Version:   $Revision: 1.4 $

=========================================================================*/

#include "mmuIdFactory.h"
#include "mafDecl.h"
#include <map>
#include <vector>
#include <string>

std::map<std::string,mafID> *mmuIdFactoryMap=NULL;
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
mafID mmuIdFactory::m_IdCounter = MAF_BASE_ID;

/* not used
//------------------------------------------------------------------------------
mafID mmuIdFactory::AllocIdGroup(const char *idname,int num)
//------------------------------------------------------------------------------
{
  mafID id=m_IdCounter;
  m_IdCounter+=num;

  if (mmuIdFactoryMap==NULL)
  {
    mmuIdFactoryMap   = new std::map<std::string,mafID>;
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
mafID mmuIdFactory::GetNextId(const char *idname)
//------------------------------------------------------------------------------
{
  mafID id=m_IdCounter++;

  if (mmuIdFactoryMap==NULL)
  {
    mmuIdFactoryMap   = new std::map<std::string,mafID>;
    mmuIdFactoryNames = new std::vector<std::string>;
  }

  (*mmuIdFactoryMap)[idname]=id;
  
  (*mmuIdFactoryNames).resize(id+1);  
  (*mmuIdFactoryNames)[id]=idname;

  return id;
}

//------------------------------------------------------------------------------
mafID mmuIdFactory::GetId(const char *idname)
//------------------------------------------------------------------------------
{
  mafID id=0;
  
  std::map<std::string,mafID>::iterator  it=(*mmuIdFactoryMap).find(idname);

  if (it!=(*mmuIdFactoryMap).end())
  {
    id=it->second;
  }
  
  return id;
}

//------------------------------------------------------------------------------
const char *mmuIdFactory::GetIdName(mafID id)
//------------------------------------------------------------------------------
{
  return (id<(*mmuIdFactoryNames).size())?(*mmuIdFactoryNames)[id].c_str():NULL;
}

