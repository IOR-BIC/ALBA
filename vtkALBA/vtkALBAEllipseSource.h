/*=========================================================================

Program: ALBA
Module: vtkALBAEllipseSource
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAEllipseSource_h
#define __vtkALBAEllipseSource_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"

class ALBA_EXPORT vtkALBAEllipseSource : public vtkPolyDataAlgorithm
{
public:
  static vtkALBAEllipseSource *New();
  vtkTypeMacro(vtkALBAEllipseSource, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set position center
  vtkSetVector3Macro(Center,double);
  vtkGetVectorMacro(Center,double,3);

	// Description
  // Set MajorAxis
	vtkSetMacro(MajorAxis, double);
	vtkGetMacro(MajorAxis, double);
	
	// Description
	// Set MajorAxis
	vtkSetMacro(MinorAxis, double);
	vtkGetMacro(MinorAxis, double);

	// Description
	// Set Ellipse orientation
	vtkSetMacro(Theta, double);
	vtkGetMacro(Theta, double);


  // Description:
  // Divide line into resolution number of pieces.
  vtkSetClampMacro(Resolution,int,1,VTK_INT_MAX);
  vtkGetMacro(Resolution,int);

	/* 0=XY, 1=YZ, 2=XZ*/
	void SetPlane(int plane) { m_Plane = plane; };

protected:
  vtkALBAEllipseSource(int res=1);
  ~vtkALBAEllipseSource() {};

  /** re-implement execute fixing the algorithm when the number of points of the cutter output is zero.*/
	int RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  double Center[3];
  double MajorAxis;
	double MinorAxis;
	double Theta;

  int m_Plane;
	int Resolution;

private:
  vtkALBAEllipseSource(const vtkALBAEllipseSource&);  // Not implemented.
  void operator=(const vtkALBAEllipseSource&);  // Not implemented.
};

#endif


