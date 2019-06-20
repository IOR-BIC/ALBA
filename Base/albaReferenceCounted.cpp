/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaReferenceCounted
 Authors: based on vtkObjectBase (www.vtk.org), adapted Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaReferenceCounted.h"
#include <malloc.h>
#include <assert.h>

albaCxxAbstractTypeMacro(albaReferenceCounted)

//------------------------------------------------------------------------------
void albaReferenceCounted::Delete()
//------------------------------------------------------------------------------
{
  UnRegister(NULL);
}

//------------------------------------------------------------------------------
albaReferenceCounted::albaReferenceCounted()
//------------------------------------------------------------------------------
{
  // Create an object with Debug turned off and modified time initialized 
  // to zero.
  // objects not created with New() cannot be reference counted...
  m_HeapFlag=false;

  // initial reference count = 0, objects must be refernced explicitelly by
  // their creators.
  m_ReferenceCount = 0;
}

//------------------------------------------------------------------------------
albaReferenceCounted::~albaReferenceCounted() 
//------------------------------------------------------------------------------
{
  // warn user if reference counting is on and the object is being referenced
  // by another object
  if ( m_ReferenceCount > 0)
  {
    albaErrorMacro(<< "Trying to delete object with non-zero reference count.");
    assert(true);
  }
}

//------------------------------------------------------------------------------
void albaReferenceCounted::SetReferenceCount(int ref)
//------------------------------------------------------------------------------
{
  // Sets the reference count (use with care)
  m_ReferenceCount = ref;
}

//------------------------------------------------------------------------------
void albaReferenceCounted::Register(void *obj)
//------------------------------------------------------------------------------
{
  // Increase the reference count (mark as used by another object).
  if (!m_HeapFlag)
  {
    albaErrorMacro(<< "Trying to Register a non-dynamically allocated object.");
    return;
  }

  m_ReferenceCount++;

  if (m_ReferenceCount <= 0)
  {
    delete this;
  }
}

//------------------------------------------------------------------------------
void albaReferenceCounted::UnRegister(void *obj)
//------------------------------------------------------------------------------
{
  // Decrease the reference count (release by another object).
  if (!m_HeapFlag)
  {
    albaErrorMacro("Trying to UnRegister a non-dynamically allocated object.");
    return;
  }

  if (--m_ReferenceCount <= 0)
  {
    // invoke the delete method
    // Here we should call delete method
    delete this;
  }
}


