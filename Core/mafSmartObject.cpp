/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartObject.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-18 22:07:43 $
  Version:   $Revision: 1.4 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafSmartObject.h"
#include <malloc.h>
#include <sstream>
#include <assert.h>

mafCxxAbstractTypeMacro(mafSmartObject);

//------------------------------------------------------------------------------
void mafSmartObject::Delete()
//------------------------------------------------------------------------------
{
  UnRegister(NULL);
}

//------------------------------------------------------------------------------
// Create an object with Debug turned off and modified time initialized 
// to zero.
mafSmartObject::mafSmartObject()
//------------------------------------------------------------------------------
{
  this->ReferenceCount = 0;
  // initial reference count = 1 and reference counting on.
}

//------------------------------------------------------------------------------
mafSmartObject::~mafSmartObject() 
//------------------------------------------------------------------------------
{
  // warn user if reference counting is on and the object is being referenced
  // by another object
  if ( this->ReferenceCount > 0)
  {
    mafErrorMacro(<< "Trying to delete object with non-zero reference count.");
    assert(true);
  }
}

//------------------------------------------------------------------------------
// Sets the reference count (use with care)
void mafSmartObject::SetReferenceCount(int ref)
//------------------------------------------------------------------------------
{
  this->ReferenceCount = ref;
}

//------------------------------------------------------------------------------
// Increase the reference count (mark as used by another object).
void mafSmartObject::Register(void *obj)
//------------------------------------------------------------------------------
{
  if (!HeapFlag)
  {
    mafErrorMacro(<< "Trying to Register a non-dynamically allocated object.");
    return;
  }

  this->ReferenceCount++;

  if (this->ReferenceCount <= 0)
  {
    delete this;
  }
}

//------------------------------------------------------------------------------
// Decrease the reference count (release by another object).
void mafSmartObject::UnRegister(void *obj)
//------------------------------------------------------------------------------
{
  if (!HeapFlag)
  {
    mafErrorMacro("Trying to UnRegister a non-dynamically allocated object.");
    return;
  }

  if (--this->ReferenceCount <= 0)
  {
    // invoke the delete method
    // Here we should call delete method
    delete this;
  }
}
