/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixedCutter
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAFixedCutter_h
#define __vtkALBAFixedCutter_h

#include "albaConfigure.h"
#include "vtkCutter.h"


//----------------------------------------------------------------------------
// forward declarations
class vtkPlane;
class vtkLinearTransform;

/** vtkALBAFixedCutter` extends `vtkCutter` by allowing an input pose transformation to be specified. 
The transformation is used to account for the input dataset's position and orientation when computing the cut.

The output is generated in the same coordinate system and position as the input dataset, 
while the specified transformation is applied internally so that the cut is correctly 
computed according to the cutting plane.

The input transformation can be specified using `SetInputTransform()`.

vtkALBAFixedCutter also removes the normals of the output to maintain the material property when rendered 
(a black line is drawn elsewere).
*/
class ALBA_EXPORT vtkALBAFixedCutter : public vtkCutter
{
public:
  vtkTypeMacro(vtkALBAFixedCutter,vtkCutter);
  /** constructor */
           vtkALBAFixedCutter();
  /** destructor */
  virtual ~vtkALBAFixedCutter();
  /** create an instance of the object */
  static vtkALBAFixedCutter* New();

  /** Redefine of SetCutFunction in order to store the external cut function instead of CutFunction 
      CutFunction will be set on RequestData */
	virtual void SetCutFunction(vtkImplicitFunction *cutFunc);

	vtkGetObjectMacro(InputTransform, vtkLinearTransform);
	virtual void SetInputTransform(vtkLinearTransform *transform) {InputTransform = transform;}

protected:
  /** reimplement execute fixing the algorithm when the number of points of the cutter output is zero.*/
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  vtkImplicitFunction *ExternalCutFunc;
  vtkPlane *LocalCutFunction;

  vtkLinearTransform *InputTransform;
private:
    /** Copy Constructor , not implemented */
	vtkALBAFixedCutter(const vtkALBAFixedCutter&);
  /** operator =, not implemented */
  void operator=(const vtkALBAFixedCutter&);
};
#endif
