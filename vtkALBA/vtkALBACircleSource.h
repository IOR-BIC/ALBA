/*=========================================================================

Program: ALBA
Module: vtkALBACircleSource
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBACircleSource_h
#define __vtkALBACircleSource_h

#include "albaConfigure.h"
#include "vtkPolyDataSource.h"

class ALBA_EXPORT vtkALBACircleSource : public vtkPolyDataSource
{
public:
  static vtkALBACircleSource *New();
  vtkTypeRevisionMacro(vtkALBACircleSource,vtkPolyDataSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set position center
  vtkSetVector3Macro(Center,double);
  vtkGetVectorMacro(Center,double,3);

	// Description:
	// Set AngleRange
	vtkSetVector2Macro(AngleRange, double);
	vtkGetVectorMacro(AngleRange, double, 2);

	// Description
	// Set Radius
	vtkSetMacro(Radius, double);
	vtkGetMacro(Radius, double);

  // Description:
  // Divide line into resolution number of pieces.
  vtkSetClampMacro(Resolution,int,1,VTK_LARGE_INTEGER);
  vtkGetMacro(Resolution,int);

	/* 0=XY, 1=YZ, 2=XZ*/
	void SetPlane(int plane) { m_Plane = plane; };

protected:
  vtkALBACircleSource(int res=1);
  ~vtkALBACircleSource() {};

  void Execute();
  double Center[3];

	double AngleRange[2];

	double Radius;

  int Resolution;

	int m_Plane;

private:
  vtkALBACircleSource(const vtkALBACircleSource&);  // Not implemented.
  void operator=(const vtkALBACircleSource&);  // Not implemented.
};

#endif


