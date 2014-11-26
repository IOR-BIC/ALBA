/*=========================================================================

 Program: MAF2
 Module: vtkMAFLocalAxisCoordinate
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __vtkMAFLocalAxisCoordinate_h
#define __vtkMAFLocalAxisCoordinate_h

#include "mafConfigure.h"

#include "vtkCoordinate.h"
#include "vtkProp3D.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"

class vtkViewport;

/**
class name: vtkMAFLocalAxisCoordinate.
*/
class MAF_EXPORT vtkMAFLocalAxisCoordinate : public vtkCoordinate
{
public:
  //vtkTypeRevisionMacro(vtkMAFLocalAxisCoordinate,vtkCoordinate);
  /** destructor */
  virtual ~vtkMAFLocalAxisCoordinate();
  /** create an instance of the object */
  static vtkMAFLocalAxisCoordinate* New();

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
  vtkMAFLocalAxisCoordinate();

private:
  vtkDataSet *DataSet;
  vtkMatrix4x4 *Matrix;
  
  /** Copy Constructor , not implemented */
  vtkMAFLocalAxisCoordinate(const vtkMAFLocalAxisCoordinate&);
  /** operator =, not implemented */
  void operator=(const vtkMAFLocalAxisCoordinate&);
};
#endif
