/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObject.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-02 13:28:59 $
  Version:   $Revision: 1.10 $
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

#ifdef _WIN32
//------------------------------------------------------------------------------
// avoid dll boundary problems
void* mafObject::operator new(size_t nSize)
//------------------------------------------------------------------------------
{
  void* p=malloc(nSize);
  return p;
}

//------------------------------------------------------------------------------
void mafObject::operator delete( void *p )
//------------------------------------------------------------------------------
{
  free(p);
}
#endif 

//------------------------------------------------------------------------------
mafObject::mafObject():HeapFlag(0)
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
bool mafObject::IsTypeOf(const char *type_name)
//------------------------------------------------------------------------------
{
  return ( !strcmp("mafObject",type_name) )? true : false;
}

//------------------------------------------------------------------------------
bool mafObject::IsTypeOf(const mafTypeID &type_id)
//------------------------------------------------------------------------------
{
  return ( type_id==typeid(mafObject) ) ? true : false;
}

//------------------------------------------------------------------------------
bool mafObject::IsA(const mafTypeID &type_id) const
//------------------------------------------------------------------------------
{
  return IsTypeOf(type_id);
}

//------------------------------------------------------------------------------
bool mafObject::IsA(const char *type_name) const
//------------------------------------------------------------------------------
{
  return IsTypeOf(type_name);
}

//------------------------------------------------------------------------------
const mafTypeID &mafObject::GetTypeId()
//------------------------------------------------------------------------------
{
  return typeid(mafObject);
}

//------------------------------------------------------------------------------
const mafTypeID &mafObject::GetClassId() const
//------------------------------------------------------------------------------
{
  return typeid(mafObject);
}
