/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeStamped.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 20:41:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
//
// Initialize static member
//
#include "mafTimeStamped.h"

//-------------------------------------------------------------------------
void mafTimeStamped::Modified()
//-------------------------------------------------------------------------
{
  m_MTime.Modified();
}

//-------------------------------------------------------------------------
unsigned long mafTimeStamped::GetMTime()
//-------------------------------------------------------------------------
{
  return m_MTime.GetMTime();
}







