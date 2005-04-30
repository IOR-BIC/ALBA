/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdTrackerSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:57 $
  Version:   $Revision: 1.1 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mmdTrackerSet.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmdTrackerSet.h"
#include "mmdTracker.h"
#include "mflTransform.h"
#include "vtkTemplatedList.txx"

#include "vtkObjectFactory.h"

#define NUMBER_OF_VERTEX_FOR_SCREEN_PLANE 2


//------------------------------------------------------------------------------
vtkStandardNewMacro(mmdTrackerSet)

//------------------------------------------------------------------------------
mmdTrackerSet::mmdTrackerSet()
{  
}

//------------------------------------------------------------------------------
mmdTrackerSet::~mmdTrackerSet()
//------------------------------------------------------------------------------
{
  if (this->Initialized)
  { 
    this->RemoveAllDevices();
  } 
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
