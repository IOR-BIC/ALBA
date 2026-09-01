/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAGlobalAxisCoordinate
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAGlobalAxisCoordinate_h
#define __vtkALBAGlobalAxisCoordinate_h

#include "albaConfigure.h"

#include "vtkCoordinate.h"
#include "vtkProp3D.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"

class vtkViewport;
class vtkRenderer;
class vtkRenderWindow;
class vtkCamera;

/**
class name: vtkALBAGlobalAxisCoordinate.
*/
class ALBA_EXPORT  vtkALBAGlobalAxisCoordinate : public vtkCoordinate
{
public:
  //vtkTypeMacro(vtkALBAGlobalAxisCoordinate,vtkCoordinate);
  /** destructor */
  virtual ~vtkALBAGlobalAxisCoordinate();
  /** create an instance of the object */
  static vtkALBAGlobalAxisCoordinate* New();

  /** used only when the coordinate system is VTK_USERDEFINED  */
  virtual double *GetComputedUserDefinedValue(vtkViewport *viewport);

protected:
  /** constructor */
  vtkALBAGlobalAxisCoordinate();

private:
	vtkRenderWindow *Renderwindow;
	vtkRenderer *Renderer;
	vtkCamera *Camera;

  /** Copy Constructor , not implemented */
  vtkALBAGlobalAxisCoordinate(const vtkALBAGlobalAxisCoordinate&);
  /** operator =, not implemented */
  void operator=(const vtkALBAGlobalAxisCoordinate&);
};
#endif
