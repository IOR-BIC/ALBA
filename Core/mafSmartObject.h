/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartObject.h,v $
  Language:  C++
  Date:      $Date: 2004-11-29 09:33:04 $
  Version:   $Revision: 1.2 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafSmartObject_h
#define __mafSmartObject_h

#include "mafObject.h"

/** mafSmartObject - abstract base class for MAF objects with reference counting.
  mafSmartObject is the base class for all reference counted classes
  in the MAF. mafSmartObjects are also mafObjects, thus implementing all RTTI APIs.
  mafSmartObject performs reference counting: objects that are
  reference counted exist as long as another object uses them. Once
  the last reference to a reference counted object is removed, the
  object will spontaneously destruct.
  
  Constructor and destructor of the subclasses of mafSmartObject
  should be protected, so that only New() and UnRegister() actually
  call them. Debug leaks can be used to see if there are any objects
  left with nonzero reference count.

  Note: Objects of subclasses of mafSmartObject should always be
  created with the New() method and deleted with the Delete()
  method. They cannot be allocated off the stack (i.e., automatic
  objects) because the constructor is a protected method.
 
  @sa  mafObject */
class MAF_EXPORT mafSmartObject: public mafObject 
{
public:
  mafTypeMacro(mafSmartObject,mafObject);

  mafSmartObject(); 
  virtual ~mafSmartObject(); 


  /**
    Delete a MAF object.  This method should always be used to delete
    an object when the New() method was used to create it. Using the
    C++ delete method will not work with reference counting. */
  //virtual void Delete();

  /**
    Create an object with Debug turned off, modified time initialized 
    to zero, and reference counting on. */
  //static mafSmartObject *New() {return new mafSmartObject;}

  /** Increase the reference count (mark as used by another object). */
  void Register();

  /**
    Decrease the reference count (release by another object). This
    has the same effect as invoking Delete() (i.e., it reduces the
    reference count by 1). */
  virtual void UnRegister();

  /** Return the current reference count of this object. */
  int  GetReferenceCount() {return this->ReferenceCount;}

  /** Sets the reference count. (This is very dangerous, use with care.) */
  void SetReferenceCount(int);
  
#ifdef _WIN32
    // avoid dll boundary problems
  void* operator new( size_t tSize );
  void operator delete( void* p );
#endif 
  
protected:

  int ReferenceCount;      ///< Number of uses of this object by other objects
  bool DynamicObject;
};

#endif

