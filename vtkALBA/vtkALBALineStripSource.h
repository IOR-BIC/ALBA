/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBALineStripSource.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBALineStripSource - create a random cloud of points
// .SECTION Description
// vtkALBALineStripSource is a source object that creates a user-specified number 
// of points within a specified radius about a specified center point. 
// By default location of the points is random within the sphere. It is
// also possible to generate random points only on the surface of the
// sphere.

#ifndef __vtkALBALineStripSource_h
#define __vtkALBALineStripSource_h

#include "vtkALBAAbstractPointsSource.h"
#include "albaConfigure.h"

class vtkPoints;
class vtkPolyData;
class vtkCellArray;

class ALBA_EXPORT vtkALBALineStripSource : public vtkALBAAbstractPointsSource 
{
public:
  static vtkALBALineStripSource *New();
  vtkTypeMacro(vtkALBALineStripSource,vtkALBAAbstractPointsSource);

protected:
  vtkALBALineStripSource();
  ~vtkALBALineStripSource();
  
private:
	void SetCellArrayToOutput(vtkPolyData *output, vtkCellArray *cellArray);

  vtkALBALineStripSource(const vtkALBALineStripSource&);  // Not implemented.
  void operator=(const vtkALBALineStripSource&);  // Not implemented.
};

#endif
