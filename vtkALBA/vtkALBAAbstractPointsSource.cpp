/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAAbstractPointsSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkALBAAbstractPointsSource.h"

#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <float.h>
#include <math.h>
#include "vtkInformationVector.h"
#include "vtkInformation.h"

//----------------------------------------------------------------------------
vtkALBAAbstractPointsSource::vtkALBAAbstractPointsSource()
{
  this->NumberOfPoints = 0;
	points=vtkPoints::New();
	this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
vtkALBAAbstractPointsSource::~vtkALBAAbstractPointsSource()
{
	points->Delete();
}


//----------------------------------------------------------------------------
int vtkALBAAbstractPointsSource::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (output == NULL)
	{
		vtkErrorMacro(<< "Output polydata cannot be NULL.");
		return 0;
	}

  vtkIdType i;
  vtkCellArray *newVerts;
	vtkPoints *newPoints;

	newPoints = vtkPoints::New();
	if(NumberOfPoints>0)
		newPoints->Allocate(this->NumberOfPoints);
  
	newVerts = vtkCellArray::New();
  newVerts->Allocate(newVerts->EstimateSize(1,this->NumberOfPoints));

  newVerts->InsertNextCell(this->NumberOfPoints);

  for (i=0; i<this->NumberOfPoints; i++)
  {
    newVerts->InsertCellPoint(newPoints->InsertNextPoint(points->GetPoint(i)));
  }
    
	//
	// Update ourselves and release memory
	//
	if(NumberOfPoints>0)
		output->SetPoints(newPoints);
	newPoints->Delete();
  
	SetCellArrayToOutput(output,newVerts);
  newVerts->Delete();

	return 1;
}


//----------------------------------------------------------------------------
void vtkALBAAbstractPointsSource::PrintSelf(ostream& os, vtkIndent indent)
{
}

//----------------------------------------------------------------------------
void vtkALBAAbstractPointsSource::AddPoint(double point[3])
{
	NumberOfPoints++;
	points->InsertNextPoint(point);
	Modified();
}


//----------------------------------------------------------------------------
void vtkALBAAbstractPointsSource::SetPoint(int nPoint, double point[3])
{
	points->SetPoint(nPoint,point);
	Modified();
}


//----------------------------------------------------------------------------
void vtkALBAAbstractPointsSource::GetPoint(int nPoint, double point[3])
{
	points->GetPoint(nPoint,point); 
}


//----------------------------------------------------------------------------
void vtkALBAAbstractPointsSource::ClearPoints()
{
	NumberOfPoints=0;
	points->SetNumberOfPoints(0);
	Modified();
}


