/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttribute.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-17 00:42:45 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafAttribute.h"
#include "assert.h"

//-------------------------------------------------------------------------
void mafAttribute::operator=(const mafAttribute &a)
//-------------------------------------------------------------------------
{
  DeepCopy(&a);
}

//-------------------------------------------------------------------------
bool mafAttribute::operator==(const mafAttribute &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
void mafAttribute::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{
  assert(a->IsA(GetTypeId()));
}

//-------------------------------------------------------------------------
bool mafAttribute::Equals(const mafAttribute *a) const
//-------------------------------------------------------------------------
{
  return a->IsA(GetTypeId());
}
