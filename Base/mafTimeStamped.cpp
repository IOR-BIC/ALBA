/*=========================================================================

 Program: MAF2
 Module: mafTimeStamped
 Authors: Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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







