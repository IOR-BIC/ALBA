/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObject.h,v $
  Language:  C++
  Date:      $Date: 2004-12-02 13:28:59 $
  Version:   $Revision: 1.10 $
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
  RTTI APIs. MAF objects can be created either on the stack or dynamically
  in the heap. The the latter you can use both "new" or "::New()" for instantiacion
  and "delete" or "Delete()" for deletion. There's no difference between the two.
  For smart objects these two methods behave differently.
  @sa mafSmartObject
*/
class MAF_EXPORT mafObject
{
public:
  mafObject();
  virtual ~mafObject();

  /** the same as delete obj, implemented for syntax compatibility */
  virtual void Delete() {delete this;};
  
  /** Return the name of this type (static function) */
  static const char *GetTypeName();

  /** Return the class name of this instance */
  virtual const char *GetClassName() const; 
  
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

  /** Return TypeId for this type (static function, i.e. the pointer type) */ 
  static const mafTypeID &GetTypeId();

  /** Return TypeId for this object instance (the real type) */ 
  virtual const mafTypeID &GetClassId() const;

  mafObject(const mafObject& c) {}

#ifdef _WIN32
    // avoid dll boundary problems
  void* operator new( size_t tSize );
  void operator delete( void* p );
#endif

protected:
  /** Internally used to mark objects created on the Heap with New() */
  bool HeapFlag;
};

#endif /* __mafObject_h */
