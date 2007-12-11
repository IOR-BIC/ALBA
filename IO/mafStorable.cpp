/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorable.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:25:08 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorable.h"
#include "mafObject.h"
#include "mafStorageElement.h"

#include <vector>

//------------------------------------------------------------------------------
int mafStorable::Store(mafStorageElement *parent)
//------------------------------------------------------------------------------
{
  return InternalStore(parent);
}

//------------------------------------------------------------------------------
int mafStorable::Restore(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  return InternalRestore(element);
}
//------------------------------------------------------------------------------
mafStorable* mafStorable::SafeCastToObject(mafObject *o)
//------------------------------------------------------------------------------
{
  try 
  {
    return dynamic_cast<mafStorable *>(o);
  } 
  
  catch (std::bad_cast) 
  { 
    return NULL;
  } 
}

//------------------------------------------------------------------------------
mafObject *mafStorable::CastToObject()
//------------------------------------------------------------------------------
{
  try 
  {
    return dynamic_cast<mafObject *>(this);
  } 
  
  catch (std::bad_cast) 
  { 
    return NULL;
  }

}
