/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObject.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-02 15:01:02 $
  Version:   $Revision: 1.3 $
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
/** 
  This class is used as a dictionary to inference event IDs from class names.
*/
class mafObjectDictionaryType
{
  public:
  static std::map<std::string,mafID> *TypeIDs;

  mafObjectDictionaryType() {if (TypeIDs==NULL) TypeIDs=new std::map<std::string,mafID>;}
  ~mafObjectDictionaryType() {if (TypeIDs) delete TypeIDs;} // this is to allow memory deallocation
}; 

//------------------------------------------------------------------------------
// Static Objects
//------------------------------------------------------------------------------
std::map<std::string,mafID> * mafObjectDictionaryType::TypeIDs=NULL;
mafObjectDictionaryType mafObject::TypesDictionary;
mafID mafObject::TypeIdCounter = 0; // This is for allocating unique Object IDs.

mafID mafObject::TypeId = GetNextTypeId("mafObject");
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
  return ( type_id==mafObject::TypeId ) ? 1 : 0;
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
  return mafObject::TypeId;
}

//------------------------------------------------------------------------------
mafID mafObject::GetClassId() const
//------------------------------------------------------------------------------
{
  return mafObject::TypeId;
}

//------------------------------------------------------------------------------
mafID mafObject::GetNextTypeId(const char *classname)
//------------------------------------------------------------------------------
{
  mafID id=TypeIdCounter++;

  if (TypesDictionary.TypeIDs==NULL)
    TypesDictionary.TypeIDs=new std::map<std::string,mafID>;

  (*TypesDictionary.TypeIDs)[classname]=id;

  return id;
}

//------------------------------------------------------------------------------
mafID mafObject::GetTypeId(const char *classname)
//------------------------------------------------------------------------------
{
  mafID id=0;
  
  std::map<std::string,mafID>::iterator  it=(*TypesDictionary.TypeIDs).find(classname);

  if (it!=(*TypesDictionary.TypeIDs).end())
  {
    id=it->second;
  }
  
  return id;
}
