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

#include "vtkALBACircleSource.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkMath.h"

#include <math.h>
#include "vtkInformationVector.h"
#include "vtkInformation.h"
vtkStandardNewMacro(vtkALBACircleSource);

vtkALBACircleSource::vtkALBACircleSource(int res)
{
  this->Center[0] =  0.0;
  this->Center[1] =  0.0;
  this->Center[2] =  0.0;

	this->AngleRange[0] = 0;
	this->AngleRange[1] = vtkMath::Pi() *2.0;

  this->Resolution = (res < 3 ? 3 : res);

	m_Plane = 0;
}

int vtkALBACircleSource::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	int A = 0, B = 1, C = 2;

	if (m_Plane == 0) { A = 0; B = 1; C = 2; }; //XY
	if (m_Plane == 1) { A = 1; B = 2; C = 0; }; //YZ
	if (m_Plane == 2) { A = 0; B = 2; C = 1; }; //XZ

	int numLines = this->Resolution;
	int numPts = this->Resolution + 1;
	double x[3], tc[3];
	int i, j;
	vtkPoints *newPoints;
	vtkFloatArray *newTCoords;
	vtkCellArray *newLines;

	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkDebugMacro(<< "Creating line");

	newPoints = vtkPoints::New();
	newPoints->Allocate(numPts);
	newTCoords = vtkFloatArray::New();
	newTCoords->SetNumberOfComponents(2);
	newTCoords->Allocate(2 * numPts);

	newLines = vtkCellArray::New();
	newLines->Allocate(newLines->EstimateSize(numLines, 2));

	double angleSize = AngleRange[1] - AngleRange[0];
	double currentAngle;
	
	tc[B] = 0.0;
	tc[C] = 0.0;
	x[C] = Center[A];
	for (i = 0; i < numPts; i++)
	{
		tc[A] = ((double)i / this->Resolution);
	
		currentAngle = AngleRange[0] + angleSize*tc[A];

		x[A] = Center[A] + sin(currentAngle)*Radius;
		x[B] = Center[B] + cos(currentAngle)*Radius;

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

	return 1;
}

void vtkALBACircleSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Resolution: " << this->Resolution << "\n";

  os << indent << "Center: (" << this->Center[0] << ", "
                               << this->Center[1] << ", "
                               << this->Center[2] << ")\n";

  os << indent << "AngleRange: (" << this->AngleRange[0] << ", "
                               << this->AngleRange[1] << ")\n";

	os << indent << "Radius: " << this->Resolution << "\n";

}
