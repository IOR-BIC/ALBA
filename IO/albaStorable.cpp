/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorable
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaStorable.h"
#include "albaObject.h"
#include "albaStorageElement.h"

#include <vector>

//------------------------------------------------------------------------------
int albaStorable::Store(albaStorageElement *parent)
//------------------------------------------------------------------------------
{
  return InternalStore(parent);
}

//------------------------------------------------------------------------------
int albaStorable::Restore(albaStorageElement *element)
//------------------------------------------------------------------------------
{
  return InternalRestore(element);
}
//------------------------------------------------------------------------------
albaStorable* albaStorable::SafeCastToObject(albaObject *o)
//------------------------------------------------------------------------------
{
  try 
  {
    return dynamic_cast<albaStorable *>(o);
  } 
  
  catch (std::bad_cast) 
  { 
    return NULL;
  } 
}

//------------------------------------------------------------------------------
albaObject *albaStorable::CastToObject()
//------------------------------------------------------------------------------
{
  try 
  {
    return dynamic_cast<albaObject *>(this);
  } 
  
  catch (std::bad_cast) 
  { 
    return NULL;
  }

}
