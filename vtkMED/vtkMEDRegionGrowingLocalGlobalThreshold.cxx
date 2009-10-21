/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkMEDRegionGrowingLocalGlobalThreshold.cxx,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMEDRegionGrowingLocalGlobalThreshold.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkDataSetWriter.h"

#include <math.h>

#define APLHA 1.0

vtkCxxRevisionMacro(vtkMEDRegionGrowingLocalGlobalThreshold, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMEDRegionGrowingLocalGlobalThreshold);

//----------------------------------------------------------------------------
vtkMEDRegionGrowingLocalGlobalThreshold::vtkMEDRegionGrowingLocalGlobalThreshold()
//----------------------------------------------------------------------------
{
  UpperLabel = 1;
  LowerLabel = -1;

  UpperThreshold = -1;
  LowerThreshold = 1;

  Output = vtkImageData::New();

  OutputScalarType = VTK_UNSIGNED_SHORT;
}

//----------------------------------------------------------------------------
vtkMEDRegionGrowingLocalGlobalThreshold::~vtkMEDRegionGrowingLocalGlobalThreshold()
//----------------------------------------------------------------------------
{
  Output->Delete();
}

//----------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThreshold::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os, indent);
}
//----------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThreshold::ComputeIndexNearstPoints(int index, int indexNearest[26] , int &error, vtkImageData *imBordered)
//----------------------------------------------------------------------------
{
  vtkImageData *image = NULL;
  if (imBordered != NULL)
  {
    image = imBordered;
  }
  else
  {
    error = TRUE;
    return;
  }

  vtkImageData *inputIM = Input;
  inputIM->Update();

  int dims[3],dimsBordered[3];
  inputIM->GetDimensions(dims);
  image->GetDimensions(dimsBordered);

  //Check if the index no excede the max value
  if (index > dims[0]*dims[1]*dims[2] )
  {
    error = TRUE;
    return;
  }

  vtkDataArray *scalars = image->GetPointData()->GetScalars();

  int z = (int)index/(dims[1]*dims[0]);
  int y = (int)(index%(dims[1]*dims[0]))/dims[1];
  int x = (int)((index%(dims[1]*dims[0]))%dims[1]);

  int zBordered = z+1;
  int yBordered = y+1;
  int xBordered = x+1;

  //Check if the index isn't a point at the border of the ImBordered
  if (zBordered==0 || zBordered==dimsBordered[2]-1 || yBordered==0 || yBordered==dimsBordered[1]-1 || xBordered==0 || xBordered==dimsBordered[0]-1)
  {
    error = TRUE;
    return;
  }

  indexNearest[0] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered-1;
  indexNearest[1] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered+1;
  indexNearest[2] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered;
  indexNearest[3] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered;
  indexNearest[4] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered-1;
  indexNearest[5] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered+1;
  indexNearest[6] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered-1;
  indexNearest[7] = zBordered*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered+1;

  indexNearest[8] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered;  
  indexNearest[9] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered-1;  
  indexNearest[10] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered+1;  
  indexNearest[11] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered;
  indexNearest[12] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered;
  indexNearest[13] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered-1;
  indexNearest[14] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered+1;
  indexNearest[15] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered-1;
  indexNearest[16] = (zBordered-1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered+1;

  indexNearest[17] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered;
  indexNearest[18] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered-1;
  indexNearest[19] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*yBordered)+xBordered+1;
  indexNearest[20] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered;
  indexNearest[21] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered;
  indexNearest[22] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered-1;
  indexNearest[23] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered+1;
  indexNearest[24] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered+1))+xBordered-1;
  indexNearest[25] = (zBordered+1)*((dimsBordered[1]*dimsBordered[0]))+(dimsBordered[0]*(yBordered-1))+xBordered+1;

  error = FALSE;
}
//----------------------------------------------------------------------------
double vtkMEDRegionGrowingLocalGlobalThreshold::ComputeStandardDeviation(int index, int indexNearest[26] , double mean, int &error, vtkImageData *imBordered)
//----------------------------------------------------------------------------
{
  vtkImageData *image = NULL;
  if (imBordered != NULL)
  {
    image = imBordered;
  }
  else
  {
    error = TRUE;
    return 0.0;
  }

  vtkDataArray *scalars = image->GetPointData()->GetScalars();

  double stdDev = 0.0;
  //Compute the standard deviation using the 26 point value nearest
  for (int i=0;i<26;i++)
  {
    stdDev += ( (mean - scalars->GetTuple1(indexNearest[i])) * (mean - scalars->GetTuple1(indexNearest[i])) );
  }

  stdDev /= 26;
  stdDev = sqrt(stdDev);

  error = FALSE;

  return stdDev;
}
//----------------------------------------------------------------------------
double vtkMEDRegionGrowingLocalGlobalThreshold::ComputeMeanValue(int index, int indexNearest[26] , int &error, vtkImageData *imBordered)
//----------------------------------------------------------------------------
{
  vtkImageData *image = NULL;
  if (imBordered != NULL)
  {
    image = imBordered;
  }
  else
  {
    error = TRUE;
    return 0.0;
  }

  vtkDataArray *scalars = image->GetPointData()->GetScalars();

  double means = 0.0;

  //Compute the mean using the 26 point value nearest
  for (int i=0;i<26;i++)
  {
    means += scalars->GetTuple1(indexNearest[i]);
  }

  means /= 26;

  error = FALSE;
  return means;
}
//----------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThreshold::BorderCreate(vtkImageData *imToApplyBorder)
//----------------------------------------------------------------------------
{
  vtkImageData *inputIM = Input;
  inputIM->Update();

  int scalarsType = inputIM->GetScalarType();
  vtkDataArray *scalars = inputIM->GetPointData()->GetScalars();

  int dims[3],dimsBordered[3];
  inputIM->GetDimensions(dims);
  double spacing[3];
  inputIM->GetSpacing(spacing);

  //the dims of the bordered image data should be increase of 2 for the border
  dimsBordered[0] = dims[0]+2;
  dimsBordered[1] = dims[1]+2;
  dimsBordered[2] = dims[2]+2;

  vtkImageData *imWithBorder = NULL;
  if (imToApplyBorder != NULL)
  {
    imWithBorder = imToApplyBorder;
  }
  else
  {
    return;
  }

  imWithBorder->SetDimensions(dimsBordered);
  imWithBorder->SetSpacing(spacing);
  imWithBorder->Update();

  vtkDataArray *scalarsBordered = NULL;
  switch(scalarsType)
  {
  case VTK_DOUBLE:
    {
      scalarsBordered = vtkDoubleArray::New();
    }
    break;
  case VTK_FLOAT:
    {
      scalarsBordered = vtkFloatArray::New();
    }
    break;
  case VTK_UNSIGNED_SHORT:
    {
      scalarsBordered = vtkUnsignedShortArray::New();
    }
    break;
  case VTK_SHORT:
    {
      scalarsBordered = vtkShortArray::New();
    }
    break;
  case VTK_CHAR:
    {
      scalarsBordered = vtkCharArray::New();
    }
    break;
  case VTK_UNSIGNED_CHAR:
    {
      scalarsBordered = vtkUnsignedCharArray::New();
    }
    break;
  }

  scalarsBordered->SetNumberOfTuples(dimsBordered[0]*dimsBordered[1]*dimsBordered[2]);

  //Copy of the scalars value
  //&
  //Filling of the border with the nearest image value

  int ixBordered = 0;
  int iyBordered = 0;
  int izBordered = 0;

  int sliceDimBordered = dimsBordered[0]*dimsBordered[1];
  int sliceDim = dims[0]*dims[1];

  for (izBordered = 0;izBordered<dimsBordered[2];izBordered++)
  {
    for (iyBordered = 0;iyBordered<dimsBordered[1];iyBordered++)
    {
      for (ixBordered = 0;ixBordered<dimsBordered[0];ixBordered++)
      {
        double value;

        int ix = ixBordered>0 ? ixBordered-1:0;
        int iy = iyBordered>0 ? iyBordered-1:0;
        int iz = izBordered>0 ? izBordered-1:0;

        ix = ixBordered==dimsBordered[0]-1 ? dims[0]-1:ix;
        iy = iyBordered==dimsBordered[1]-1 ? dims[1]-1:iy;
        iz = izBordered==dimsBordered[2]-1 ? dims[2]-1:iz;

        value = scalars->GetTuple1((iz*sliceDim)+(dims[0]*iy)+ix);

        scalarsBordered->SetTuple1((izBordered*sliceDimBordered)+(dimsBordered[0]*iyBordered)+ixBordered,value);
      }
    }
  }


  scalarsBordered->SetName("Scalars");

  imWithBorder->GetPointData()->AddArray(scalarsBordered);
  imWithBorder->GetPointData()->SetActiveScalars("Scalars");

  imWithBorder->Update();

  scalarsBordered->Delete();
}
//----------------------------------------------------------------------------
void vtkMEDRegionGrowingLocalGlobalThreshold::Update()
//----------------------------------------------------------------------------
{
  vtkImageData *inputIM = Input;
  inputIM->Update();

  Output->CopyStructure(Input);
  Output->Update();

  if (inputIM->GetPointData()->GetScalars())
  {
    vtkDataArray *scalarsOutput = NULL;
    switch(OutputScalarType)
    {
    case VTK_DOUBLE:
      {
        scalarsOutput = vtkDoubleArray::New();
      }
      break;
    case VTK_FLOAT:
      {
        scalarsOutput = vtkFloatArray::New();
      }
      break;
    case VTK_UNSIGNED_SHORT:
      {
        scalarsOutput = vtkUnsignedShortArray::New();
      }
      break;
    case VTK_SHORT:
      {
        scalarsOutput = vtkShortArray::New();
      }
      break;
    case VTK_CHAR:
      {
        scalarsOutput = vtkCharArray::New();
      }
      break;
    case VTK_UNSIGNED_CHAR:
      {
        scalarsOutput = vtkUnsignedCharArray::New();
      }
      break;
    }

    scalarsOutput->SetName("Scalars");
    scalarsOutput->SetNumberOfTuples(inputIM->GetNumberOfPoints());

    vtkImageData *imBordered = vtkImageData::New();

    BorderCreate(imBordered);
    
    for (int i=0;i<inputIM->GetPointData()->GetScalars()->GetNumberOfTuples();i++)//For each scalar
    {
      double scalarValue = inputIM->GetPointData()->GetScalars()->GetTuple1(i);

      if (scalarValue > LowerThreshold && scalarValue < UpperThreshold)
      {
        int error = FALSE;
        int indexNearest[26];
        ComputeIndexNearstPoints(i,indexNearest,error,imBordered);
        double mean = ComputeMeanValue(i,indexNearest,error,imBordered);
        if (error == TRUE)
        {
          return;
        }
        double stdDev = ComputeStandardDeviation(i,indexNearest,mean,error,imBordered);
        if (error == TRUE)
        {
          return;
        }

        //Condition of region growing
        if ( scalarValue < (mean - ( APLHA*stdDev )) )
        {
          scalarsOutput->SetTuple1(i,(double)LowerLabel);
        }
        else if( scalarValue >= (mean - ( APLHA*stdDev )) )
        {
          scalarsOutput->SetTuple1(i,(double)UpperLabel);
        }
      }
      else if (scalarValue <= LowerThreshold)
      {
        scalarsOutput->SetTuple1(i,(double)LowerLabel);
      }
      else if (scalarValue >= UpperThreshold)
      {
        scalarsOutput->SetTuple1(i,(double)UpperLabel);
      }
    }

    Output->GetPointData()->SetScalars(scalarsOutput);
    Output->GetPointData()->GetScalars()->Modified();
    Output->Update();

    imBordered->Delete();
    scalarsOutput->Delete();
  }
}