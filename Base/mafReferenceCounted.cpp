/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafReferenceCounted.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:35:01 $
  Version:   $Revision: 1.1 $
  Authors:   based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafReferenceCounted.h"
#include <malloc.h>
#include <sstream>
#include <assert.h>

mafCxxAbstractTypeMacro(mafReferenceCounted)

//------------------------------------------------------------------------------
void mafReferenceCounted::Delete()
//------------------------------------------------------------------------------
{
  UnRegister(NULL);
}

//------------------------------------------------------------------------------
// Create an object with Debug turned off and modified time initialized 
// to zero.
mafReferenceCounted::mafReferenceCounted()
//------------------------------------------------------------------------------
{
  // objects not created with New() cannot be reference counted...
  m_HeapFlag=0;

  // initial reference count = 0, objects must be refernced explicitelly by
  // their creators.
  m_ReferenceCount = 0;
}

//------------------------------------------------------------------------------
mafReferenceCounted::~mafReferenceCounted() 
//------------------------------------------------------------------------------
{
  // warn user if reference counting is on and the object is being referenced
  // by another object
  if ( m_ReferenceCount > 0)
  {
    mafErrorMacro(<< "Trying to delete object with non-zero reference count.");
    assert(true);
  }
}

//------------------------------------------------------------------------------
// Sets the reference count (use with care)
void mafReferenceCounted::SetReferenceCount(int ref)
//------------------------------------------------------------------------------
{
  m_ReferenceCount = ref;
}

//------------------------------------------------------------------------------
// Increase the reference count (mark as used by another object).
void mafReferenceCounted::Register(void *obj)
//------------------------------------------------------------------------------
{
  if (!m_HeapFlag)
  {
    mafErrorMacro(<< "Trying to Register a non-dynamically allocated object.");
    return;
  }

  m_ReferenceCount++;

  if (m_ReferenceCount <= 0)
  {
    delete this;
  }
}

//------------------------------------------------------------------------------
// Decrease the reference count (release by another object).
void mafReferenceCounted::UnRegister(void *obj)
//------------------------------------------------------------------------------
{
  if (!m_HeapFlag)
  {
    mafErrorMacro("Trying to UnRegister a non-dynamically allocated object.");
    return;
  }

  if (--m_ReferenceCount <= 0)
  {
    // invoke the delete method
    // Here we should call delete method
    delete this;
  }
}


