/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFLineStripSource.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFLineStripSource - create a random cloud of points
// .SECTION Description
// vtkMAFLineStripSource is a source object that creates a user-specified number 
// of points within a specified radius about a specified center point. 
// By default location of the points is random within the sphere. It is
// also possible to generate random points only on the surface of the
// sphere.

#ifndef __vtkMAFLineStripSource_h
#define __vtkMAFLineStripSource_h

#include "vtkMAFAbstractPointsSource.h"

class vtkPoints;
class vtkPolyData;
class vtkCellArray;

class VTK_GRAPHICS_EXPORT vtkMAFLineStripSource : public vtkMAFAbstractPointsSource 
{
public:
  static vtkMAFLineStripSource *New();
  vtkTypeRevisionMacro(vtkMAFLineStripSource,vtkMAFAbstractPointsSource);

protected:
  vtkMAFLineStripSource();
  ~vtkMAFLineStripSource();
  
private:
	void SetCellArrayToOutput(vtkPolyData *output, vtkCellArray *cellArray);

  vtkMAFLineStripSource(const vtkMAFLineStripSource&);  // Not implemented.
  void operator=(const vtkMAFLineStripSource&);  // Not implemented.
};

#endif
