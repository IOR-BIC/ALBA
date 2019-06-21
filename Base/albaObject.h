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

#ifndef __albaObject_h
#define __albaObject_h

#include "albaDefines.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// albaObject
//------------------------------------------------------------------------------
/** Abstract superclass for all ALBA classes implementing RTTI APIs.
  albaObject is a superclass implementing some useful features like 
  RTTI APIs. ALBA objects can be created either on the stack or dynamically
  in the heap. The the latter you can use both "new" or "::New()" for instantiacion
  and "delete" or "Delete()" for deletion. There's no difference between the two.
  For reference counted objects these two methods behave differently.
  @sa albaReferenceCounted
*/
class ALBA_EXPORT albaObject
{
public:
  albaObject();
  virtual ~albaObject();

  /** the same as delete obj, implemented for syntax compatibility */
  virtual void Delete() {delete this;};
  
  /** Return the name of this type (static function) */
  static const char *GetStaticTypeName();

  /** Return the class name of this instance */
  virtual const char *GetTypeName() const; 
  
  /** This is used by IsA to check the class name */
  static bool IsStaticType(const char *type_name);
  
  /** This is used by IsA to check the class type id */
  static bool IsStaticType(const albaTypeID &type_id);

  /** Check the class name of this instance */
  virtual bool IsA(const char *type_name) const;
  
  /** Check the type id of this instance */
  virtual bool IsA(const albaTypeID &type_id) const;

  /** Return a new instance of the same type */
  virtual albaObject *NewObjectInstance() const = 0;

  /** Return TypeId for this type (static function, i.e. the pointer type) */ 
  static const albaTypeID &GetStaticTypeId();

  /** Return TypeId for this object instance (the real type) */ 
  virtual const albaTypeID &GetTypeId() const;

  /** Cast with dynamic type checking. This is used for casting from (albaObject *) */
  static albaObject* SafeDownCast(albaObject *o);

  /** print debug information for this object */
  virtual void Print(std::ostream &os, const int indent=0) const;

  albaObject(const albaObject& c) {}

#ifndef _DEBUG
	#ifdef _WIN32
		// avoid dll boundary problems
	  void* operator new( size_t tSize );
	  void operator delete( void* p );
	#endif
#endif

protected:
  bool m_HeapFlag; ///< Internally used to mark objects created on the Heap with New()
};
#endif /* __albaObject_h */
