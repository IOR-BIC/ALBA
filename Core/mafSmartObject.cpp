/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSmartObject.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-25 11:29:36 $
  Version:   $Revision: 1.1 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafSmartObject.h"
#include <malloc.h>
#include <sstream>

mafCxxTypeMacro(mafSmartObject);

#ifdef _WIN32
//------------------------------------------------------------------------------
// avoid dll boundary problems
void* mafSmartObject::operator new(size_t nSize)
//------------------------------------------------------------------------------
{
  void* p=malloc(nSize);
  return p;
}

void mafSmartObject::operator delete( void *p )
{
  free(p);
}
#endif 

//------------------------------------------------------------------------------
// Create an object with Debug turned off and modified time initialized 
// to zero.
mafSmartObject::mafSmartObject()
//------------------------------------------------------------------------------
{
  this->ReferenceCount = 1;
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
    mafWarningMacro(<< "Trying to delete object with non-zero reference count.");
  }
}

//------------------------------------------------------------------------------
// Delete a vtk object. This method should always be used to delete an object 
// when the new operator was used to create it. Using the C++ delete method
// will not work with reference counting.
void mafSmartObject::Delete()
//------------------------------------------------------------------------------
{
  this->UnRegister();
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
void mafSmartObject::Register()
//------------------------------------------------------------------------------
{
  this->ReferenceCount++;
  if (this->ReferenceCount <= 0)
    {
    delete this;
    }
}

//------------------------------------------------------------------------------
// Decrease the reference count (release by another object).
void mafSmartObject::UnRegister()
//------------------------------------------------------------------------------
{
  if (--this->ReferenceCount <= 0)
  {
    // invoke the delete method
    // Here we should call delete method
    delete this;
  }
}
