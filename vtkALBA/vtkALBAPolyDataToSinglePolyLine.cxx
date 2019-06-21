/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataToSinglePolyLine
 Authors: Alessandro Chiarini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAPolyDataToSinglePolyLine.h"
#include "vtkPoints.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkSplineFilter.h"
#include "vtkCardinalSpline.h"
//vtkCxxRevisionMacro(vtkALBAFixedCutter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkALBAPolyDataToSinglePolyLine);

//----------------------------------------------------------------------------
vtkALBAPolyDataToSinglePolyLine::vtkALBAPolyDataToSinglePolyLine() : vtkPolyDataToPolyDataFilter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
vtkALBAPolyDataToSinglePolyLine::~vtkALBAPolyDataToSinglePolyLine()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkALBAPolyDataToSinglePolyLine::Execute()
//----------------------------------------------------------------------------
{
	vtkPoints* InPts = this->GetInput()->GetPoints();
	vtkPolyData* InPD = this->GetInput();
	vtkPolyData *output = this->GetOutput();

	int numLines = this->GetInput()->GetNumberOfLines();
  if(numLines == 0) return;
	int numPts = numLines+1;
	double /*x[3],*/ tc[3]/*, v[3]*/;
	int i/*, j*/;

	vtkPoints *newPoints; 
	vtkFloatArray *newTCoords; 
	vtkCellArray *newLines;
	

	vtkDebugMacro(<<"Creating line");

	newPoints = vtkPoints::New();
	newPoints->Allocate(numPts);
	newTCoords = vtkFloatArray::New();
	newTCoords->SetNumberOfComponents(2);
	newTCoords->Allocate(2*numPts);

	newLines = vtkCellArray::New();
	newLines->Allocate(newLines->EstimateSize(numLines,2));
	//
	// Generate points and texture coordinates
	//

	tc[1] = 0.0;
	tc[2] = 0.0;

	for (i=0; i<numPts; i++) 
	{
		double Point1[3];
		InPts->GetPoint(i,Point1);
		tc[0] = ((double)i/numLines);//this may be wrong...
		newPoints->InsertPoint(i,Point1);
		newTCoords->InsertTuple(i,tc);
	}

	//
	//  Generate lines
	//
	newLines->InsertNextCell(numPts);
	for (i=0; i < numPts; i++) 
	{
		newLines->InsertCellPoint (i);
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

  if(output->GetPointData()->GetScalars())
    output->GetPointData()->SetScalars(vtkFloatArray::New());
}
