/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObject.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-25 11:29:35 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafObject.h"
#include <map>
#include <string>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
/** This class is used as a dictionary to inference event IDs from class names. */
class mafObjectDictionaryType
{
  public:
  static std::map<std::string,mafID> *m_TypeIDs;

  mafObjectDictionaryType() {if (m_TypeIDs==NULL) m_TypeIDs=new std::map<std::string,mafID>;}
  ~mafObjectDictionaryType() {if (m_TypeIDs) delete m_TypeIDs;} // this is to allow memory deallocation
}; 

//------------------------------------------------------------------------------
// Static Objects
//------------------------------------------------------------------------------
std::map<std::string,mafID> * mafObjectDictionaryType::m_TypeIDs=NULL;
mafObjectDictionaryType mafObject::m_TypesDictionary;
mafID mafObject::m_TypeIdCounter = 0; // This is for allocating unique Object IDs.

mafID mafObject::m_TypeId = GetNextTypeId("mafObject");
//------------------------------------------------------------------------------
mafObject::mafObject()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafObject::~mafObject()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
const char *mafObject::GetTypeName()
//------------------------------------------------------------------------------
{
  return "mafObject";
}

//------------------------------------------------------------------------------
const char *mafObject::GetClassName() const
//------------------------------------------------------------------------------
{
  return "mafObject";
}

//------------------------------------------------------------------------------
int mafObject::IsTypeOf(const char *type_name)
//------------------------------------------------------------------------------
{
  return ( !strcmp("mafObject",type_name) )? 1 : 0;
}

//------------------------------------------------------------------------------
int mafObject::IsTypeOf(const mafID type_id)
//------------------------------------------------------------------------------
{
  return ( type_id==mafObject::m_TypeId ) ? 1 : 0;
}

//------------------------------------------------------------------------------
int mafObject::IsA(const mafID type_id) const
//------------------------------------------------------------------------------
{
  return IsTypeOf(type_id);
}

//------------------------------------------------------------------------------
int mafObject::IsA(const char *type_name) const
//------------------------------------------------------------------------------
{
  return IsTypeOf(type_name);
}

//------------------------------------------------------------------------------
mafObject *mafObject::NewObjectInstance()
//------------------------------------------------------------------------------
{
  return new mafObject;
}

//------------------------------------------------------------------------------
mafObject *mafObject::NewInternalInstance() const
//------------------------------------------------------------------------------
{
  return new mafObject;
}

//------------------------------------------------------------------------------
mafObject *mafObject::NewInstance() const
//------------------------------------------------------------------------------
{
  return NewObjectInstance();
}

//------------------------------------------------------------------------------
mafID mafObject::GetTypeId()
//------------------------------------------------------------------------------
{
  return mafObject::m_TypeId;
}

//------------------------------------------------------------------------------
mafID mafObject::GetClassId() const
//------------------------------------------------------------------------------
{
  return mafObject::m_TypeId;
}

//------------------------------------------------------------------------------
mafID mafObject::GetNextTypeId(const char *classname)
//------------------------------------------------------------------------------
{
  mafID id=m_TypeIdCounter++;

  if (m_TypesDictionary.m_TypeIDs==NULL)
    m_TypesDictionary.m_TypeIDs=new std::map<std::string,mafID>;

  (*m_TypesDictionary.m_TypeIDs)[classname]=id;

  return id;
}

//------------------------------------------------------------------------------
mafID mafObject::GetTypeId(const char *classname)
//------------------------------------------------------------------------------
{
  mafID id=0;
  
  std::map<std::string,mafID>::iterator  it=(*m_TypesDictionary.m_TypeIDs).find(classname);

  if (it!=(*m_TypesDictionary.m_TypeIDs).end())
  {
    id=it->second;
  }
  
  return id;
}
