/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFAbstractPointsSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMAFAbstractPointsSource.h"

#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <float.h>
#include <math.h>

//----------------------------------------------------------------------------
vtkMAFAbstractPointsSource::vtkMAFAbstractPointsSource()
{
  this->NumberOfPoints = 0;
	points=vtkPoints::New();
}

//----------------------------------------------------------------------------
vtkMAFAbstractPointsSource::~vtkMAFAbstractPointsSource()
{
	points->Delete();
}


//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::Execute()
{
  vtkIdType i;
  vtkCellArray *newVerts;
	vtkPoints *newPoints;
  vtkPolyData *output = this->GetOutput();

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
}

//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::ExecuteInformation()
{
}


//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::PrintSelf(ostream& os, vtkIndent indent)
{
}

//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::AddPoint(double point[3])
{
	NumberOfPoints++;
	points->InsertNextPoint(point);
	Modified();
}


//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::SetPoint(int nPoint, double point[3])
{
	points->SetPoint(nPoint,point);
	Modified();
}


//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::GetPoint(int nPoint, double point[3])
{
	points->GetPoint(nPoint,point); 
}


//----------------------------------------------------------------------------
void vtkMAFAbstractPointsSource::ClearPoints()
{
	NumberOfPoints=0;
	points->SetNumberOfPoints(0);
	Modified();
}

