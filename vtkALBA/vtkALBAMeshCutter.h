/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshCutter
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAMeshCutter_h
#define __vtkALBAMeshCutter_h

#include "albaConfigure.h"
#include "vtk3DLinearGridPlaneCutter.h"


//----------------------------------------------------------------------------
// forward declarations
class vtkPlane;
class vtkLinearTransform;

/**
class name:  vtkALBAMeshCutter
specialization of vtk3DLinearGridPlaneCutter  in which after execution if the number of points is zero, it will added one point
that is the center of the input.

Warning: vtk3DLinearGridPlaneCutter needs a linearized mesh in input.
*/
class ALBA_EXPORT vtkALBAMeshCutter : public vtk3DLinearGridPlaneCutter
{
public:
  vtkTypeMacro(vtkALBAMeshCutter,vtk3DLinearGridPlaneCutter);
  /** constructor */
           vtkALBAMeshCutter();
  /** destructor */
  virtual ~vtkALBAMeshCutter();
  /** create an instance of the object */
  static vtkALBAMeshCutter* New();
  
  /** Overload standard modified time function. If cut function is modified,
  then this object is modified as well. */
	virtual vtkMTimeType GetMTime();
	
  /** Redefine of SetPlane in order to store the external Plane instead of SetPlane 
      CutFunction will be set on RequestData */
	virtual void SetPlane(vtkPlane *plane);

	vtkGetObjectMacro(InputTransform, vtkLinearTransform);
	virtual void SetInputTransform(vtkLinearTransform *transform) {InputTransform = transform;}

protected:
  /** reimplement execute fixing the algorithm when the number of points of the cutter output is zero.*/
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  vtkPlane *ExternalPlane;
  vtkPlane *LocalPlane;

  vtkLinearTransform *InputTransform;
private:
    /** Copy Constructor , not implemented */
	vtkALBAMeshCutter(const vtkALBAMeshCutter&);
  /** operator =, not implemented */
  void operator=(const vtkALBAMeshCutter&);
};
#endif
