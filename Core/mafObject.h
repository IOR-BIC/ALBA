/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObject.h,v $
  Language:  C++
  Date:      $Date: 2004-11-30 18:18:21 $
  Version:   $Revision: 1.9 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafObject_h
#define __mafObject_h

#include "mafDefines.h"

class mafObjectDictionaryType;

//------------------------------------------------------------------------------
// mafObject
//------------------------------------------------------------------------------
/** Abstract superclass for all MAF classes implementing RTTI APIs.
  mafObject is a superclass implementing some useful features like 
  RTTI APIs.
*/
class MAF_EXPORT mafObject
{
public:
  mafObject();
  virtual ~mafObject();
  
  /** Return the name of this type (static function) */
  static const char *GetTypeName();

  /** Return the class name of this instance */
  const char *GetClassName() const; 
  
  /** This is used by IsA to check the class name */
  static bool IsTypeOf(const char *type_name);
  
  /** This is used by IsA to check the class type id */
  static bool IsTypeOf(const mafTypeID &type_id);

  /** Check the class name of this instance */
  virtual bool IsA(const char *type_name) const;
  
  /** Check the type id of this instance */
  virtual bool IsA(const mafTypeID &type_id) const;

  /** Return a new instance of the same type */
  virtual mafObject *NewInternalInstance() const = 0;

  /** Return TypeId for this type (static function) */ 
  static const mafTypeID &GetTypeId();

  /** Return TypeId for this class instance */ 
  const mafTypeID &GetClassId() const;

  /** Return the ID for a specific classname */
  //static mafTypeID &GetTypeId(const char *classname);

  mafObject(const mafObject& c) {}
  //void operator=(const mafObject&) {}

#ifdef _WIN32
    // avoid dll boundary problems
  void* operator new( size_t tSize );
  void operator delete( void* p );
#endif

protected:
  /** Internally used to keep a unique ID for all the object */
  //static mafID GetNextTypeId(const char *classname);
  bool HeapFlag;

private:
//  static mafID m_TypeId;
//  static mafID m_TypeIdCounter;
//  static mafObjectDictionaryType m_TypesDictionary;
};

#endif /* __mafObject_h */
