/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDInteractorStyleEnableRotation.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2014
University of Bedfordshire
=========================================================================*/

#include "vtkMEDInteractorStyleEnableRotation.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMEDInteractorStyleEnableRotation, "$Revision: 1.61 $");
vtkStandardNewMacro(vtkMEDInteractorStyleEnableRotation);


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
vtkMEDInteractorStyleEnableRotation::vtkMEDInteractorStyleEnableRotation()
  : m_RotationEnabled(true)
{

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
vtkMEDInteractorStyleEnableRotation::~vtkMEDInteractorStyleEnableRotation()
{

}


//------------------------------------------------------------------------------
// Rotation
//------------------------------------------------------------------------------
void vtkMEDInteractorStyleEnableRotation::Rotate()
{
  if (m_RotationEnabled)
    vtkInteractorStyleTrackballCamera::Rotate() ;
}
