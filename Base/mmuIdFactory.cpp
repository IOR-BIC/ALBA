/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mmuIdFactory.cpp,v $
Language:  C++
Date:      $Date: 2005-01-11 17:35:03 $
Version:   $Revision: 1.1 $

=========================================================================*/

#include "mmuIdFactory.h"

#include <map>
#include <vector>
#include <string>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------

// this is used as a dictionary for event IDs from their name.
// A lot of problems bacause of initialization problems: the mmuIdFactoryDictionary seems to be
// initialized after the events ID objects 
class mmuIdFactoryDictionaryType
{
  public:
  static std::map<std::string,mafID> *m_Ids;
  static std::vector<std::string> *m_IdNames;

  mmuIdFactoryDictionaryType() {if (m_Ids==NULL) m_Ids=new std::map<std::string,mafID>;}
  ~mmuIdFactoryDictionaryType() { \
    if (m_Ids) \
      delete m_Ids; \
    if (m_IdNames) \
      delete m_IdNames; \
  } // this is to allow memory deallocation
}; 

std::map<std::string,mafID> *mmuIdFactoryDictionaryType::m_Ids=NULL;
std::vector<std::string> *mmuIdFactoryDictionaryType::m_IdNames=NULL;
static mmuIdFactoryDictionaryType mmuIdFactoryDictionary;

//static mmuIdFactory::mmuIdFactoryDictionaryType mmuIdFactoryDictionary; 

// This is for allocating unique  Ids.
mafID mmuIdFactory::m_IdCounter = 1;

//------------------------------------------------------------------------------
mafID mmuIdFactory::AllocIdGroup(const char *idname,int num)
//------------------------------------------------------------------------------
{
  mafID id=m_IdCounter;
  m_IdCounter+=num;

  if (mmuIdFactoryDictionary.m_Ids==NULL)
  {
    mmuIdFactoryDictionary.m_Ids=new std::map<std::string,mafID>;
    mmuIdFactoryDictionary.m_IdNames=new std::vector<std::string>;
  }

  (*mmuIdFactoryDictionary.m_Ids)[idname]=id;

  return id;
}

//------------------------------------------------------------------------------
mafID mmuIdFactory::GetNextId(const char *idname)
//------------------------------------------------------------------------------
{
  mafID id=m_IdCounter++;

  if (mmuIdFactoryDictionary.m_Ids==NULL)
  {
    mmuIdFactoryDictionary.m_Ids=new std::map<std::string,mafID>;
    mmuIdFactoryDictionary.m_IdNames=new std::vector<std::string>;
  }

  (*mmuIdFactoryDictionary.m_Ids)[idname]=id;

  //if ((*mmuIdFactoryDictionary.m_IdNames).size()<=id)
    (*mmuIdFactoryDictionary.m_IdNames).resize(id+1);
    
  (*mmuIdFactoryDictionary.m_IdNames)[id]=idname;

  return id;
}

//------------------------------------------------------------------------------
mafID mmuIdFactory::GetId(const char *idname)
//------------------------------------------------------------------------------
{
  mafID id=0;
  
  std::map<std::string,mafID>::iterator  it=(*mmuIdFactoryDictionary.m_Ids).find(idname);

  if (it!=(*mmuIdFactoryDictionary.m_Ids).end())
  {
    id=it->second;
  }
  
  return id;
}

//------------------------------------------------------------------------------
const char *mmuIdFactory::GetIdName(mafID id)
//------------------------------------------------------------------------------
{
  return (id<(*mmuIdFactoryDictionary.m_IdNames).size())?(*mmuIdFactoryDictionary.m_IdNames)[id].c_str():NULL;
}

