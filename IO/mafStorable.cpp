/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorable.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-10 00:18:06 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorable.h"
#include "mafObject.h"

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
mafStorable* mafStorable::SafeDownCast(mafObject *o)
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
