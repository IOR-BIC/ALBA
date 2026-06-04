/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATicksGenerator
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkALBATicksGenerator.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkALBATicksGenerator);

//----------------------------------------------------------------------------
vtkALBATicksGenerator::vtkALBATicksGenerator()
{
}

//----------------------------------------------------------------------------
vtkALBATicksGenerator::~vtkALBATicksGenerator()
{
}

//----------------------------------------------------------------------------
int vtkALBATicksGenerator::RequestData(vtkInformation* vtkNotUsed(request),
                                       vtkInformationVector** inputVector,
                                       vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet* input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!input || !output)
  {
    return 0;
  }

  vtkPoints* tickPoints = vtkPoints::New();
  vtkCellArray* tickCells = vtkCellArray::New();
  vtkIdType pointsId[2];
  int counter = 0;

  double bounds[6];
  input->GetBounds(bounds);

  double xmin = bounds[0];
  double xmax = bounds[1];
  double ymin = bounds[2];
  double ymax = bounds[3];
  double zmin = bounds[4];
  double zmax = bounds[5];

  vtkRectilinearGrid* rgData = vtkRectilinearGrid::SafeDownCast(input);
  if (rgData)
  {
    vtkDoubleArray* zFa = vtkDoubleArray::SafeDownCast(rgData->GetZCoordinates());
    if (zFa)
    {
      for (int i = 0; i < zFa->GetNumberOfTuples(); i++)
      {
        double zVal = zFa->GetValue(i);
        tickPoints->InsertNextPoint(xmax, ymax, zVal);
        tickPoints->InsertNextPoint(xmax + (xmax - xmin) / 30, ymax + (ymax - ymin) / 30, zVal);
        pointsId[0] = counter;
        pointsId[1] = counter + 1;
        counter += 2;
        tickCells->InsertNextCell(2, pointsId);
      }
    }
    else
    {
      vtkFloatArray* zFaF = vtkFloatArray::SafeDownCast(rgData->GetZCoordinates());
      if (zFaF)
      {
        for (int i = 0; i < zFaF->GetNumberOfTuples(); i++)
        {
          float zVal = zFaF->GetValue(i);
          tickPoints->InsertNextPoint(xmax, ymax, zVal);
          tickPoints->InsertNextPoint(xmax + (xmax - xmin) / 30, ymax + (ymax - ymin) / 30, zVal);
          pointsId[0] = counter;
          pointsId[1] = counter + 1;
          counter += 2;
          tickCells->InsertNextCell(2, pointsId);
        }
      }
    }
  }

  vtkImageData* spData = vtkImageData::SafeDownCast(input);
  if (spData)
  {
    int dim[3];
    double origin[3];
    double spacing[3];
    spData->GetDimensions(dim);
    spData->GetOrigin(origin);
    spData->GetSpacing(spacing);

    for (int i = 0; i < dim[2]; i++)
    {
      float zVal = origin[2] + i * spacing[2];
      tickPoints->InsertNextPoint(xmax, ymax, zVal);
      tickPoints->InsertNextPoint(xmax + (xmax - xmin) / 30, ymax + (ymax - ymin) / 30, zVal);
      pointsId[0] = counter;
      pointsId[1] = counter + 1;
      counter += 2;
      tickCells->InsertNextCell(2, pointsId);
    }
  }

  output->SetPoints(tickPoints);
  output->SetLines(tickCells);
  output->Modified();

  tickPoints->Delete();
  tickCells->Delete();

  return 1;
}