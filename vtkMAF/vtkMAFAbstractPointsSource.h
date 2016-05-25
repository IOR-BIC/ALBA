/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFAbstractPointsSource.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFAbstractPointsSource - create a random cloud of points
// .SECTION Description
// vtkMAFAbstractPointsSource is a source object that creates a user-specified number 
// of points within a specified radius about a specified center point. 
// By default location of the points is random within the sphere. It is
// also possible to generate random points only on the surface of the
// sphere.

#ifndef __vtkMAFAbstractPointsSource_h
#define __vtkMAFAbstractPointsSource_h

#include "vtkPolyDataSource.h"

class vtkPoints;
class vtkCellArray;
class vtkPolyData;

class VTK_GRAPHICS_EXPORT vtkMAFAbstractPointsSource : public vtkPolyDataSource 
{
public:
    
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set the number of points to generate.
  vtkGetMacro(NumberOfPoints,vtkIdType);

	void AddPoint(double point[3]);

	void SetPoint(int nPoint, double point[3]);

	void GetPoint(int nPoint, double point[3]);
 
	void ClearPoints();

protected:
  vtkMAFAbstractPointsSource();
  ~vtkMAFAbstractPointsSource();

  void Execute();
  void ExecuteInformation();

  vtkIdType NumberOfPoints;
	vtkPoints *points;
  
private:
	virtual void SetCellArrayToOutput(vtkPolyData *output, vtkCellArray *cellArray) = 0;

  vtkMAFAbstractPointsSource(const vtkMAFAbstractPointsSource&);  // Not implemented.
  void operator=(const vtkMAFAbstractPointsSource&);  // Not implemented.
};

#endif
