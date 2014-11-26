/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDInteractorStyleEnableRotation.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2014
University of Bedfordshire
=========================================================================*/


#ifndef __vtkMEDInteractorStyleEnableRotation_h
#define __vtkMEDInteractorStyleEnableRotation_h

#include "mafConfigure.h"
#include "vtkInteractorStyleTrackballCamera.h"



//------------------------------------------------------------------------------
/// vtkMEDInteractorStyleEnableRotation. \n
/// Interactor style in which rotation can be enabled or disabled.
//
// Version 14.3.14
//------------------------------------------------------------------------------
class MAF_EXPORT vtkMEDInteractorStyleEnableRotation : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkMEDInteractorStyleEnableRotation *New();
  vtkTypeRevisionMacro(vtkMEDInteractorStyleEnableRotation, vtkInteractorStyleTrackballCamera);

  void EnableRotation(bool enable) {m_RotationEnabled = enable ;  this->Modified() ;}
  virtual void Rotate();

protected:
  vtkMEDInteractorStyleEnableRotation();
  ~vtkMEDInteractorStyleEnableRotation();

private:
  bool m_RotationEnabled ;
};

#endif