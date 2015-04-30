/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFInteractorStyleEnableRotation.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2014
University of Bedfordshire
=========================================================================*/

#include "vtkMAFInteractorStyleEnableRotation.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFInteractorStyleEnableRotation, "$Revision: 1.61 $");
vtkStandardNewMacro(vtkMAFInteractorStyleEnableRotation);


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
vtkMAFInteractorStyleEnableRotation::vtkMAFInteractorStyleEnableRotation()
  : m_RotationEnabled(true)
{

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
vtkMAFInteractorStyleEnableRotation::~vtkMAFInteractorStyleEnableRotation()
{

}


//------------------------------------------------------------------------------
// Rotation
//------------------------------------------------------------------------------
void vtkMAFInteractorStyleEnableRotation::Rotate()
{
  if (m_RotationEnabled)
    vtkInteractorStyleTrackballCamera::Rotate() ;
}
