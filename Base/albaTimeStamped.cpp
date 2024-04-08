/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeStamped
 Authors: Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// Initialize static member
//
#include "albaTimeStamped.h"

//-------------------------------------------------------------------------
void albaTimeStamped::Modified()
//-------------------------------------------------------------------------
{
  m_MTime.Modified();
}

//-------------------------------------------------------------------------
vtkMTimeType albaTimeStamped::GetMTime()
//-------------------------------------------------------------------------
{
  return m_MTime.GetMTime();
}







