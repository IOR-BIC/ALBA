/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBAInteractorStyleEnableRotation.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2014
University of Bedfordshire
=========================================================================*/

#include "vtkALBAInteractorStyleEnableRotation.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkALBAInteractorStyleEnableRotation);


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
vtkALBAInteractorStyleEnableRotation::vtkALBAInteractorStyleEnableRotation()
  : m_RotationEnabled(true)
{

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
vtkALBAInteractorStyleEnableRotation::~vtkALBAInteractorStyleEnableRotation()
{

}


//------------------------------------------------------------------------------
// Rotation
//------------------------------------------------------------------------------
void vtkALBAInteractorStyleEnableRotation::Rotate()
{
  if (m_RotationEnabled)
    vtkInteractorStyleTrackballCamera::Rotate() ;
}
