/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorable.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:40:53 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
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
