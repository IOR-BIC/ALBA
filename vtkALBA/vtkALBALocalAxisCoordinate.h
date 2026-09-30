/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALocalAxisCoordinate
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkALBALocalAxisCoordinate_h
#define __vtkALBALocalAxisCoordinate_h

#include "albaConfigure.h"

#include "vtkCoordinate.h"
#include "vtkProp3D.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"

class vtkViewport;

/**
class name: vtkALBALocalAxisCoordinate.
*/
class ALBA_EXPORT vtkALBALocalAxisCoordinate : public vtkCoordinate
{
public:
  /** destructor */
  virtual ~vtkALBALocalAxisCoordinate();
  /** create an instance of the object */
  static vtkALBALocalAxisCoordinate* New();

  /** macro Set for DataSet member */
  vtkSetObjectMacro(DataSet,vtkDataSet);
  /** macro Get for DataSet member */
  vtkGetObjectMacro(DataSet,vtkDataSet);

  /** macro Set for Mactrix member */
  vtkSetObjectMacro(Matrix,vtkMatrix4x4);
  /** macro Get for Mactrix member */
  vtkGetObjectMacro(Matrix,vtkMatrix4x4);

  /** used only when the coordinate system is VTK_USERDEFINED  */
  virtual double *GetComputedUserDefinedValue(vtkViewport *viewport);

protected:
  /** constructor */
  vtkALBALocalAxisCoordinate();

private:
  vtkDataSet *DataSet;
  vtkMatrix4x4 *Matrix;
  
  /** Copy Constructor , not implemented */
  vtkALBALocalAxisCoordinate(const vtkALBALocalAxisCoordinate&);
  /** operator =, not implemented */
  void operator=(const vtkALBALocalAxisCoordinate&);
};
#endif
