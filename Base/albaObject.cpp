/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObject
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaObject.h"
#include "albaIndent.h"
#include <ostream>
#include <map>
#include <string>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
/** This class is used as a dictionary to inference event IDs from class names. */
class albaObjectDictionaryType
{
  public:
  static std::map<std::string,albaID> *m_TypeIDs;

  albaObjectDictionaryType() {if (m_TypeIDs==NULL) m_TypeIDs=new std::map<std::string,albaID>;}
  ~albaObjectDictionaryType() {if (m_TypeIDs) delete m_TypeIDs;} // this is to allow memory deallocation
}; 
#ifndef _DEBUG
  #ifdef _WIN32
  //------------------------------------------------------------------------------
  // avoid dll boundary problems
  void* albaObject::operator new(size_t nSize)
  //------------------------------------------------------------------------------
  {
    void* p=malloc(nSize);
    return p;
  }

  //------------------------------------------------------------------------------
  void albaObject::operator delete( void *p )
  //------------------------------------------------------------------------------
  {
    free(p);
  }
  #endif 
#endif

//------------------------------------------------------------------------------
albaObject::albaObject():m_HeapFlag(0)
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaObject::~albaObject()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
const char *albaObject::GetStaticTypeName()
//------------------------------------------------------------------------------
{
  return "albaObject";
}

//------------------------------------------------------------------------------
const char *albaObject::GetTypeName() const
//------------------------------------------------------------------------------
{
  return "albaObject";
}

//------------------------------------------------------------------------------
bool albaObject::IsStaticType(const char *type_name)
//------------------------------------------------------------------------------
{
  return ( !strcmp("albaObject",type_name) )? true : false;
}

//------------------------------------------------------------------------------
bool albaObject::IsStaticType(const albaTypeID &type_id)
//------------------------------------------------------------------------------
{
  return ( type_id==typeid(albaObject) ) ? true : false;
}

//------------------------------------------------------------------------------
bool albaObject::IsA(const albaTypeID &type_id) const
//------------------------------------------------------------------------------
{
  return IsStaticType(type_id);
}

//------------------------------------------------------------------------------
bool albaObject::IsA(const char *type_name) const
//------------------------------------------------------------------------------
{
  return IsStaticType(type_name);
}

//------------------------------------------------------------------------------
const albaTypeID &albaObject::GetStaticTypeId()
//------------------------------------------------------------------------------
{
  return typeid(albaObject);
}

//------------------------------------------------------------------------------
const albaTypeID &albaObject::GetTypeId() const
//------------------------------------------------------------------------------
{
  return typeid(albaObject);
}

//------------------------------------------------------------------------------
albaObject* albaObject::SafeDownCast(albaObject *o)
//------------------------------------------------------------------------------
{
  try 
  { 
    return dynamic_cast<albaObject *>(o);
  }
  catch (std::bad_cast)
  { 
    return NULL;
  }
}

//------------------------------------------------------------------------------
void albaObject::Print(std::ostream &os, const int indent) const
//------------------------------------------------------------------------------
{
  os << albaIndent(indent) << "Object Type Name: " << GetTypeName() << std::endl;
}
