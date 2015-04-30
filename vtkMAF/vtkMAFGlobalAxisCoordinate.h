/*=========================================================================

 Program: MAF2
 Module: vtkMAFGlobalAxisCoordinate
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFGlobalAxisCoordinate_h
#define __vtkMAFGlobalAxisCoordinate_h

#include "mafConfigure.h"

#include "vtkCoordinate.h"
#include "vtkProp3D.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"

class vtkViewport;
class vtkRenderer;
class vtkRenderWindow;
class vtkCamera;

/**
class name: vtkMAFGlobalAxisCoordinate.
*/
class MAF_EXPORT  vtkMAFGlobalAxisCoordinate : public vtkCoordinate
{
public:
  //vtkTypeRevisionMacro(vtkMAFGlobalAxisCoordinate,vtkCoordinate);
  /** destructor */
  virtual ~vtkMAFGlobalAxisCoordinate();
  /** create an instance of the object */
  static vtkMAFGlobalAxisCoordinate* New();

  /** used only when the coordinate system is VTK_USERDEFINED  */
  virtual double *GetComputedUserDefinedValue(vtkViewport *viewport);

protected:
  /** constructor */
  vtkMAFGlobalAxisCoordinate();

private:
	vtkRenderWindow *Renderwindow;
	vtkRenderer *Renderer;
	vtkCamera *Camera;

  /** Copy Constructor , not implemented */
  vtkMAFGlobalAxisCoordinate(const vtkMAFGlobalAxisCoordinate&);
  /** operator =, not implemented */
  void operator=(const vtkMAFGlobalAxisCoordinate&);
};
#endif


