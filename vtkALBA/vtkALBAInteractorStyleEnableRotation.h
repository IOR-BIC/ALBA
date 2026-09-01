/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBAInteractorStyleEnableRotation.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2014
University of Bedfordshire
=========================================================================*/


#ifndef __vtkALBAInteractorStyleEnableRotation_h
#define __vtkALBAInteractorStyleEnableRotation_h

#include "albaConfigure.h"
#include "vtkInteractorStyleTrackballCamera.h"



//------------------------------------------------------------------------------
/// vtkALBAInteractorStyleEnableRotation. \n
/// Interactor style in which rotation can be enabled or disabled.
//
// Version 14.3.14
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAInteractorStyleEnableRotation : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkALBAInteractorStyleEnableRotation *New();
  vtkTypeMacro(vtkALBAInteractorStyleEnableRotation, vtkInteractorStyleTrackballCamera);

  void EnableRotation(bool enable) {m_RotationEnabled = enable ;  this->Modified() ;}
  virtual void Rotate();

protected:
  vtkALBAInteractorStyleEnableRotation();
  ~vtkALBAInteractorStyleEnableRotation();

private:
  bool m_RotationEnabled ;
};

#endif
