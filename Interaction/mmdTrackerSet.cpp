/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdTrackerSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-19 16:27:40 $
  Version:   $Revision: 1.2 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmdTrackerSet.h"
#include "mmdTracker.h"
#include "mafTransform.h"

//------------------------------------------------------------------------------
mmdTrackerSet::mmdTrackerSet()
{  
}

//------------------------------------------------------------------------------
mmdTracker *mmdTrackerSet::GetDevice(unsigned long id)
//------------------------------------------------------------------------------
{
  return (mmdTracker *)this->Superclass::GetDevice(id);
}

//------------------------------------------------------------------------------
mmdTracker *mmdTrackerSet::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  return (mmdTracker *)this->Superclass::GetDevice(name);
}

//------------------------------------------------------------------------------
void mmdTrackerSet::AddDevice(mmdTracker *device)
//------------------------------------------------------------------------------
{
  Superclass::AddDevice(device);
}
