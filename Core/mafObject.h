/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObject.h,v $
  Language:  C++
  Date:      $Date: 2004-10-29 11:25:36 $
  Version:   $Revision: 1.2 $
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
/** Superclass for all MAF classes implementing RTTI APIs.
  mafObject is a superclass implementing some useful features like 
  RTTI APIs.
*/
class mafObject
{
public:
  mafObject();
  virtual ~mafObject();
  
  /** Return the name of this type (static function) */
  static const char *GetTypeName();

  /** Return the class name of this instance */
  virtual const char *GetClassName() const; 
  
  /** This is used by IsA to check the class name */
  static int IsTypeOf(const char *type_name);
  
  /** This is used by IsA to check the class type id */
  static int IsTypeOf(const mafID type_id);

  /** Check the class name of this instance */
  virtual int IsA(const char *type_name) const;
  
  /** Check the type id of this instance */
  virtual int IsA(const mafID type_id) const;
  
  /** Return a new instance of the same type */
  static mafObject *NewObjectInstance();

  /** Return a new instance of the same type */
  virtual mafObject *NewInternalInstance() const;
 
  /** Return a new instance of the same type */
  mafObject *NewInstance() const;

  /** Return TypeId for this type (static function) */ 
  static mafID GetTypeId();

  /** Return TypeId for this class instance */ 
  virtual mafID GetClassId() const;

  /** Return the ID for a specific classname */
  static mafID GetTypeId(const char *classname);

  /** Internally used to keep a unique ID for all the object */
  static mafID GetNextTypeId(const char *classname);

  mafObject(const mafObject& c) {}
  //void operator=(const mafObject&) {}

private:
  static mafID TypeId;
  static mafID TypeIdCounter;
  static mafObjectDictionaryType TypesDictionary;
};

#endif /* __mafObject_h */
