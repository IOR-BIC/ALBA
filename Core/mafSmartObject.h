/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartObject.h,v $
  Language:  C++
  Date:      $Date: 2004-12-02 13:28:59 $
  Version:   $Revision: 1.4 $
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
  
  Reference counting works only if the smart object is allocated dynamically with
  the New() static function, in all other cases trying to use Register/UnRegister
  simply throws an error. When allocated with New, the object should also be 
  deallocated with Delete() or UnRegister(). */
class MAF_EXPORT mafSmartObject: public mafObject 
{
public:
  mafAbstractTypeMacro(mafSmartObject,mafObject);

  mafSmartObject(); 
  virtual ~mafSmartObject(); 

  /**
    Delete a MAF object.  This method should be used to delete
    an object when the New() method was used to create it. Using the
    C++ delete method will not work with reference counting. This is 
    the same as UnRegister(NULL) */
  virtual void Delete();

  /** Increase the reference count (mark as used by another object). */
  void Register(void *obj);

  /**
    Decrease the reference count (release by another object). This
    has the same effect as invoking Delete() (i.e., it reduces the
    reference count by 1). */
  virtual void UnRegister(void *obj);

  /** Return the current reference count of this object. */
  int  GetReferenceCount() {return this->ReferenceCount;}

  /** Sets the reference count. (This is very dangerous, use with care.) */
  void SetReferenceCount(int);
  
protected:
  int ReferenceCount;      ///< Number of uses of this object by other objects
};

#endif

