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

#include "vtkALBAEllipseSource.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkMath.h"

#include <math.h>
vtkCxxRevisionMacro(vtkALBAEllipseSource, "$Revision: 1.44 $");
vtkStandardNewMacro(vtkALBAEllipseSource);

vtkALBAEllipseSource::vtkALBAEllipseSource(int res)
{
  this->Center[0] =  0.0;
  this->Center[1] =  0.0;
  this->Center[2] =  0.0;

	Theta = 0.0;
	MajorAxis = MinorAxis = 0.0;
	
	this->Resolution = res;
	
	m_Plane = 0;
}

void vtkALBAEllipseSource::Execute()
{
	int A = 0, B = 1, C = 2;

	if (m_Plane == 0) { A = 0; B = 1; C = 2; } //XY
	else if (m_Plane == 1) { A = 1; B = 2; C = 0; } //YZ
	else if (m_Plane == 2) { A = 0; B = 2; C = 1; } //XZ

	int numLines = this->Resolution;
	int numPts = this->Resolution + 1;
	double x[3], tc[3];
	int i, j;
	vtkPoints *newPoints;
	vtkFloatArray *newTCoords;
	vtkCellArray *newLines;
	vtkPolyData *output = this->GetOutput();

	vtkDebugMacro(<< "Creating line");

	newPoints = vtkPoints::New();
	newPoints->Allocate(numPts);
	newTCoords = vtkFloatArray::New();
	newTCoords->SetNumberOfComponents(2);
	newTCoords->Allocate(2 * numPts);

	newLines = vtkCellArray::New();
	newLines->Allocate(newLines->EstimateSize(numLines, 2));

	double fullAngle = vtkMath::Pi() * 2.0;
	double currentAngle;
	
	tc[B] = 0.0;
	tc[C] = 0.0;
	x[C] = Center[C];
	for (i = 0; i < numPts; i++)
	{
		tc[A] = ((double)i / this->Resolution);
	
		currentAngle = fullAngle*tc[A];

		// x(t) = cx + a搾os(t)搾os(theta) - b新in(t)新in(theta)
		x[A] = Center[A] + MajorAxis * cos(currentAngle) * cos(Theta) - MinorAxis * sin(currentAngle) * sin(Theta);

		// y(t) = cy + a搾os(t)新in(theta) + b新in(t)搾os(theta)
		x[B] = Center[B] + MajorAxis * cos(currentAngle) * sin(Theta) + MinorAxis * sin(currentAngle) * cos(Theta);

		newPoints->InsertPoint(i, x);
		newTCoords->InsertTuple(i, tc);
	}
	//
	//  Generate lines
	//
	newLines->InsertNextCell(numPts);
	for (i = 0; i < numPts; i++)
	{
		newLines->InsertCellPoint(i);
	}
	//
	// Update ourselves and release memory
	//
	output->SetPoints(newPoints);
	newPoints->Delete();

	output->GetPointData()->SetTCoords(newTCoords);
	newTCoords->Delete();

	output->SetLines(newLines);
	newLines->Delete();
}

void vtkALBAEllipseSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Resolution: " << this->Resolution << "\n";

  os << indent << "Center: (" << this->Center[0] << ", "
                               << this->Center[1] << ", "
                               << this->Center[2] << ")\n";
	os << indent << "MajorAxis: " << this->MajorAxis << "\n";

	os << indent << "MinorAxis: " << this->MinorAxis << "\n";
	
	os << indent << "Theta: " << this->Theta << "\n";
}

//----------------------------------------------------------------------------
bool vtkALBAEllipseSource::IsPointInEllipse(double point[3])
{
	// Translate point relative to ellipse center
	double translatedPoint[3];
	translatedPoint[0] = point[0] - Center[0];
	translatedPoint[1] = point[1] - Center[1];
	translatedPoint[2] = point[2] - Center[2];

	// Determine axes based on plane orientation
	int A = 0, B = 1, C = 2;
	if (m_Plane == 0) { A = 0; B = 1; C = 2; } //XY
	else if (m_Plane == 1) { A = 1; B = 2; C = 0; } //YZ
	else if (m_Plane == 2) { A = 0; B = 2; C = 1; } //XZ

	// Rotate point back by -Theta to align with ellipse axes
	double cosTheta = cos(-Theta);
	double sinTheta = sin(-Theta);

	double rotatedA = translatedPoint[A] * cosTheta - translatedPoint[B] * sinTheta;
	double rotatedB = translatedPoint[A] * sinTheta + translatedPoint[B] * cosTheta;

	// Check ellipse equation: (x/a)^2 + (y/b)^2 <= 1
	if (MajorAxis > 0.0 && MinorAxis > 0.0)
	{
		double normalizedA = rotatedA / MajorAxis;
		double normalizedB = rotatedB / MinorAxis;
		return (normalizedA * normalizedA + normalizedB * normalizedB) <= 1.0;
	}

	return false;
}

//----------------------------------------------------------------------------
void vtkALBAEllipseSource::GetBounds(double bounds[6])
{
	// Determine axes based on plane orientation
	int A = 0, B = 1, C = 2;
	if (m_Plane == 0) { A = 0; B = 1; C = 2; } //XY
	else if (m_Plane == 1) { A = 1; B = 2; C = 0; } //YZ
	else if (m_Plane == 2) { A = 0; B = 2; C = 1; } //XZ

	// Calculate the extent of rotated ellipse in the plane
	double cosTheta = cos(Theta);
	double sinTheta = sin(Theta);

	// Maximum extent from center in A and B directions
	double extentA = sqrt(MajorAxis * MajorAxis * cosTheta * cosTheta + 
	                       MinorAxis * MinorAxis * sinTheta * sinTheta);
	double extentB = sqrt(MajorAxis * MajorAxis * sinTheta * sinTheta + 
	                       MinorAxis * MinorAxis * cosTheta * cosTheta);

	// Set bounds in each direction
	double minA = Center[A] - extentA;
	double maxA = Center[A] + extentA;
	double minB = Center[B] - extentB;
	double maxB = Center[B] + extentB;
	double minC = Center[C];
	double maxC = Center[C];

	// Map back to XYZ coordinates
	bounds[2 * A] = minA;
	bounds[2 * A + 1] = maxA;
	bounds[2 * B] = minB;
	bounds[2 * B + 1] = maxB;
	bounds[2 * C] = minC;
	bounds[2 * C + 1] = maxC;
}

//----------------------------------------------------------------------------
double vtkALBAEllipseSource::GetPerimeter()
{
	// Handle degenerate cases
	if (MajorAxis <= 0.0 || MinorAxis <= 0.0)
	{
		return 0.0;
	}


	// Use Ramanujan's approximation for ellipse perimeter
	// P = pi(a + b)[1 + 3h/(10 + sqrt(4 - 3h))]
	// where h = (a-b)^2/(a+b)^2
	
	double a = MajorAxis;
	double b = MinorAxis;
	double sumAB = a + b;
	double diffAB = a - b;
	
	double h = (diffAB * diffAB) / (sumAB * sumAB);
	double denominator = 10.0 + sqrt(4.0 - 3.0 * h);
	double perimeter = vtkMath::Pi() * sumAB * (1.0 + (3.0 * h) / denominator);
	
	return perimeter;
}
