/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDVolumeSlicerNotInterpolated
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkMEDVolumeSlicerNotInterpolated.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
/*#include "vtkRectilinearGrid.h"*/
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCharArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkStructuredPointsWriter.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

vtkCxxRevisionMacro(vtkMEDVolumeSlicerNotInterpolated, "$Revision: 1.1.2.3 $");
vtkStandardNewMacro(vtkMEDVolumeSlicerNotInterpolated);

//----------------------------------------------------------------------------
vtkMEDVolumeSlicerNotInterpolated::vtkMEDVolumeSlicerNotInterpolated() 
//----------------------------------------------------------------------------
{
  // Initialize attributes
  Origin[0] = Origin[1] = Origin[2] = .0;
  SliceOrigin[0] = SliceOrigin[1] = 0.;
  SliceAxis = SLICE_Z;
  NumberOfComponents = 0;
  BaseIndex = -1;
  SliceDimensions[0] = SliceDimensions[1] = 0;
  InputDimensions[0] = InputDimensions[1] = InputDimensions[2] = 0;
  InputSpacing[0] = InputSpacing[1] = InputSpacing[2] = 1.; 
  SliceSpacing[0] = SliceSpacing[1] = SliceSpacing[2] = 1.;
  InputDataType = VTK_IMAGE_DATA;
  OutputDataType = VTK_IMAGE_DATA;
  CoordsXY[0] = CoordsXY[1] = NULL;
  OutputRectilinearGrid = NULL;
  AxisX = 0;
  AxisY = 1;

  NumberOfPieces = 1;
}
//----------------------------------------------------------------------------
vtkMEDVolumeSlicerNotInterpolated::~vtkMEDVolumeSlicerNotInterpolated() 
//----------------------------------------------------------------------------
{
  if(OutputRectilinearGrid)
  {
    OutputRectilinearGrid->Delete();
    OutputRectilinearGrid = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMEDVolumeSlicerNotInterpolated::ExecuteInformation() 
//----------------------------------------------------------------------------
{
  vtkDataSet* input = NULL;
  if ((input = GetInput()) == NULL || this->GetNumberOfOutputs() == 0)
    return; // No input or no output

  if ((NumberOfComponents = input->GetPointData()->GetNumberOfComponents()) == 0)
    return; // Nothing to display

  input->GetBounds(Bounds);

  vtkImageData* imageData = NULL;
  vtkRectilinearGrid * rectilinearGrid = NULL;
  SliceSpacing[SliceAxis] = 1;
  if ((imageData = vtkImageData::SafeDownCast(input)) != NULL) 
  {
    imageData->GetOrigin(SliceOrigin);
    InputDataType = VTK_IMAGE_DATA;

    // Input is an image data
    imageData->GetDimensions(InputDimensions);
    imageData->GetSpacing(InputSpacing);
    int multiplyFactor = InputDimensions[1] * InputDimensions[0];

    AxisX = 0;
    AxisY = 1;

    // Fill slice dimensions (output image is always defined in xy plane)
    switch(SliceAxis)
    {
      case SLICE_Z:
        {
        } break;
      case SLICE_Y:
        {
          multiplyFactor = InputDimensions[0];
          AxisX = 0;
          AxisY = 2;
        } break;
      case SLICE_X:
        {
          multiplyFactor = 1;
          AxisX = 1;
          AxisY = 2;
        } break;
    }
    if(InputDimensions[AxisX] <= 1 || InputDimensions[AxisY] <= 1)
    {
      OutputDataType = VTK_IMAGE_DATA;
      NumberOfPieces = 0;
      return;
    }

    SliceDimensions[0] = InputDimensions[AxisX];
    SliceDimensions[1] = InputDimensions[AxisY];
    SliceSpacing[AxisX] = InputSpacing[AxisX];
    SliceSpacing[AxisY] = InputSpacing[AxisY];

    // Get the first point index where axis is nearest to Origin[SliceAxis]
    double minDist = VTK_DOUBLE_MAX;
    int nearestIndex = -1;
    for(int i = 0; i < InputDimensions[SliceAxis]; i++)
    {
      int curIndex = i * multiplyFactor;
      double xyz[3];
      imageData->GetPoint(curIndex,xyz);
      double dist = 0;
      if(((dist = abs(xyz[SliceAxis] - Origin[SliceAxis])) < minDist) /*&& ((xyz[SliceAxis] - Origin[2]) <= 0)*/)
      {
        nearestIndex = curIndex;
        minDist = dist;
        SliceOrigin[SliceAxis] = xyz[SliceAxis];
      }
      else if(dist > minDist && nearestIndex != -1)
      {
        break;
      }
    }
    // BaseIndex is the current slice first point index
    BaseIndex = nearestIndex;

    // Number of pieces for image data is = 1
    NumberOfPieces = 1;
    SlicePieceDimensions[0][0] = SliceDimensions[0];
    SlicePieceDimensions[0][1] = SliceDimensions[1];
    SlicePieceSpacings[0][0] = SliceSpacing[AxisX];
    SlicePieceSpacings[0][1] = SliceSpacing[AxisY];
    SlicePieceOrigins[0][AxisX] = Origin[AxisX];
    SlicePieceOrigins[0][AxisY] = Origin[AxisY];
    SlicePieceOrigins[0][SliceAxis] = Origin[SliceAxis];

    OutputDataType = VTK_IMAGE_DATA;
  }
  else if((rectilinearGrid = vtkRectilinearGrid::SafeDownCast(input)) != NULL)
  {
    InputDataType = VTK_RECTILINEAR_GRID;

    // Input is a rectilinear grid
    rectilinearGrid->GetDimensions(InputDimensions);
    vtkDoubleArray *coordsZ = NULL;

    AxisX = 0;
    AxisY = 1;

    // Fill slice dimensions (output image is always in xy plane)
    switch(SliceAxis)
      {
      case SLICE_Z:
        {
          coordsZ = (vtkDoubleArray *)rectilinearGrid->GetZCoordinates();
          CoordsXY[0] = (vtkDoubleArray *)rectilinearGrid->GetXCoordinates();
          CoordsXY[1] = (vtkDoubleArray *)rectilinearGrid->GetYCoordinates();
        } break;
      case SLICE_Y:
        {
          coordsZ = (vtkDoubleArray *)rectilinearGrid->GetYCoordinates();
          CoordsXY[0] = (vtkDoubleArray *)rectilinearGrid->GetXCoordinates();
          CoordsXY[1] = (vtkDoubleArray *)rectilinearGrid->GetZCoordinates();
          AxisX = 0;
          AxisY = 2;
        } break;
      case SLICE_X:
        {
          coordsZ = (vtkDoubleArray *)rectilinearGrid->GetXCoordinates();
          CoordsXY[0] = (vtkDoubleArray *)rectilinearGrid->GetYCoordinates();
          CoordsXY[1] = (vtkDoubleArray *)rectilinearGrid->GetZCoordinates();
          AxisX = 1;
          AxisY = 2;
        } break;
     }
    if(CoordsXY[0]->GetNumberOfTuples() <= 1 || CoordsXY[1]->GetNumberOfTuples() <= 1)
    {
      OutputDataType = VTK_IMAGE_DATA;
      NumberOfPieces = 0;
      return;
    }
    SliceDimensions[0] = InputDimensions[AxisX];
    SliceDimensions[1] = InputDimensions[AxisY];

    // Get the first point index where axis is nearest to Origin[axis]
    double minDist = VTK_DOUBLE_MAX;
    int nearestIndex = -1;
    for(int i = 0; i < InputDimensions[SliceAxis]; i++)
    {
      double tupleVal = coordsZ->GetTuple1(i);
      double dist = 0;
      if(((dist = abs(tupleVal - Origin[SliceAxis])) < minDist) /*&& ((tupleVal - Origin[2]) <= 0)*/)
      {
        int ijk[3] = {0,0,0};
        ijk[SliceAxis] = i;
        nearestIndex = rectilinearGrid->ComputePointId(ijk);
        double xyz[3];
        rectilinearGrid->GetPoint(nearestIndex,xyz);
        SliceOrigin[SliceAxis] = tupleVal;
        minDist = dist;
      }
      else if(dist > minDist && nearestIndex != -1)
      {
        break;
      }
    }

    // BaseIndex is the current slice first point index
    BaseIndex = nearestIndex;  
    
    SliceSpacing[0] = SliceSpacing[1] = SliceSpacing[2] = 1.; // Dummy spacing
    // Loop on coordinates to determine if spacing on this slice plane is regular.
    // If spacing is regular the output can be simply an image data.
    OutputDataType = VTK_IMAGE_DATA;
    double spacing[2] = {1.,1.};
    for(int i = 0; i < 2; i++)
    {
      for(int c = 0; c < CoordsXY[i]->GetNumberOfTuples() - 2; c++)
      {
        double spac1 = CoordsXY[i]->GetTuple1(c + 1) - CoordsXY[i]->GetTuple1(c);
        double spac2 = CoordsXY[i]->GetTuple1(c + 2) - CoordsXY[i]->GetTuple1(c + 1);
        spacing[i] = spac1;
        if(abs(spac1 - spac2)/(spac1 + spac2) > 0.001)
        {
          OutputDataType = VTK_RECTILINEAR_GRID;
          break;
        }
      }
      if(OutputDataType == VTK_RECTILINEAR_GRID)
      {
        break;
      }
    }
    SliceSpacing[AxisX] = spacing[0];
    SliceSpacing[AxisY] = spacing[1];
    SliceOrigin[AxisX] = Bounds[AxisX * 2];
    SliceOrigin[AxisY] = Bounds[AxisY * 2];

    // New implementation for rg
    int pieceDimensions[MAX_NUMBER_OF_PIECES][2];
    double pieceSpacings[MAX_NUMBER_OF_PIECES][2];
    int numberOfPieces[2] = {0,0};

    // Get the pieces that have the same spacing:
    // so rg can be visualized as "a couple" of image data
    for(int i = 0; i < 2; i++)
    {
      double pieceSpacing = -1; // invalid value
      double pieceSize = 0;
      for(int c = 0; c < CoordsXY[i]->GetNumberOfTuples() - 1 && numberOfPieces[i] < MAX_NUMBER_OF_PIECES; c++)
      {
        // Get spacing and number of pieces
        double spacing = CoordsXY[i]->GetTuple1(c + 1) - CoordsXY[i]->GetTuple1(c);
        if(pieceSpacing == -1)
        {
          pieceSpacing = spacing;
        }
        if(abs(spacing - pieceSpacing) < 0.01) // necessary because of vtk conversion errors
        {
          if(spacing > pieceSpacing)
          {
            pieceSpacing = spacing;
          }
          pieceSize++;
        }
        else
        {
          pieceDimensions[ numberOfPieces[i] ][i] = pieceSize;
          pieceDimensions[ numberOfPieces[i] ][i]++;
          pieceSpacings[ numberOfPieces[i] ][i] = pieceSpacing;
          numberOfPieces[i]++;
          pieceSize = 0;
          pieceSpacing = spacing;
        }
      }
      pieceDimensions[ numberOfPieces[i] ][i] = pieceSize;
      pieceDimensions[ numberOfPieces[i] ][i]++;
      pieceSpacings[ numberOfPieces[i] ][i] = pieceSpacing;
      numberOfPieces[i]++;
      pieceSize = 0;
    }
    // Update global attributes
    NumberOfPieces = numberOfPieces[0] * numberOfPieces[1];

    double sliceOriginX[MAX_NUMBER_OF_PIECES];
    double sliceOriginY[MAX_NUMBER_OF_PIECES];

    if(NumberOfPieces < MAX_NUMBER_OF_PIECES)
    {
      for(int x = 0; x < numberOfPieces[0]; x++)
      {
        for(int y = 0; y < numberOfPieces[1]; y++)
        {
          // calculate piece origin
          int reachedDimension = 0;
          if(x > 0)
          {
            sliceOriginX[x] = 0;
            for(int x2 = 0; x2 < x; x2++)
            {
              if(pieceDimensions[x2][0]%2==0 || pieceDimensions[x2][0] == 1)
              {
                reachedDimension += pieceDimensions[x2][0]-1;
              }
              else
              {
                reachedDimension += pieceDimensions[x2][0];
              }
              sliceOriginX[x] = CoordsXY[0]->GetTuple1(reachedDimension);//((pieceDimensions[x2][0]-1) * pieceSpacings[x2][0]);
            }
          }
          else
          {
            sliceOriginX[x] = CoordsXY[0]->GetTuple1(0);
          }
          if(y > 0)
          {
            sliceOriginY[y] = 0;
            int reachedDimension = 0;
            for(int y2 = 0; y2 < y; y2++)
            {
              if(pieceDimensions[y2][1]%2==0 || pieceDimensions[y2][1] == 1)
              {
                reachedDimension += pieceDimensions[y2][1]-1;
              }
              else
              {
                reachedDimension += pieceDimensions[y2][1];
              }
              sliceOriginY[y] = CoordsXY[1]->GetTuple1(reachedDimension);//((pieceDimensions[y2][1]-1) * pieceSpacings[y2][1]);
            }
          }
          else
          {
            sliceOriginY[y] = CoordsXY[1]->GetTuple1(0);
          }
          // Update global attributes
          SlicePieceDimensions[x + numberOfPieces[0] * y][0] = pieceDimensions[x][0];
          SlicePieceDimensions[x + numberOfPieces[0] * y][1] = pieceDimensions[y][1];
          SlicePieceSpacings[x + numberOfPieces[0] * y][0] = pieceSpacings[x][0];
          SlicePieceSpacings[x + numberOfPieces[0] * y][1] = pieceSpacings[y][1];
          SlicePieceOrigins[x + numberOfPieces[0] * y][AxisX] = sliceOriginX[x];
          SlicePieceOrigins[x + numberOfPieces[0] * y][AxisY] = sliceOriginY[y];
          SlicePieceOrigins[x + numberOfPieces[0] * y][SliceAxis] = Origin[SliceAxis];
        }
      }
    }
  }
  if(NumberOfPieces < MAX_NUMBER_OF_PIECES)
  {
    // output data type is image data also for rg
    OutputDataType = VTK_IMAGE_DATA;
    SetNumberOfOutputs(NumberOfPieces);
  }
  else
  {
    // if number of pieces is greater than MAX_NUMBER_OF_PIECES use the old rg representation
    OutputDataType = VTK_RECTILINEAR_GRID;
    NumberOfPieces = 1;
  }

  // Now nearest index is the "origin" of the slice so all points in the slice can be evaluated as 
  // x + y * InputDimensions[0] + BaseIndex for SLICE_Z
  // x + z * InputDimensions[0] * InputDimensions[1] + BaseIndex for SLICE_Y
  // y * InputDimensions[0] + z * InputDimensions[1] * InputDimensions[0] + BaseIndex for SLICE_X
}

//----------------------------------------------------------------------------
void vtkMEDVolumeSlicerNotInterpolated::AddOutputsAttributes(int dimension, double spacing, int** dimensions, double** spacings, int size)
//----------------------------------------------------------------------------
{
  // Convenince method to add output attribute
  // Unused @ToDo remove it
  int *newDimensions = new int[size + 1];
  double *newSpacing = new double[size + 1];

  for(int i = 0; i < size; i++)
  {
    newDimensions[i] = *dimensions[i];
    newSpacing[i] = *spacings[i];
  }
  delete [] *dimensions;
  delete [] *spacings;
  newDimensions[size] = dimension;
  newSpacing[size] = spacing;
  size++;

  *dimensions = newDimensions;
  *spacings = newSpacing;
}

//----------------------------------------------------------------------------
void vtkMEDVolumeSlicerNotInterpolated::ExecuteData(vtkDataObject *output)
//----------------------------------------------------------------------------
{
  // Redirect on correct output type ExecuteData function
  if (vtkImageData::SafeDownCast(output) != NULL)
    this->ExecuteData((vtkImageData*)output);

  output->Modified();
}

//----------------------------------------------------------------------------
void vtkMEDVolumeSlicerNotInterpolated::ExecuteData(vtkImageData *output)
//----------------------------------------------------------------------------
{
  int iStart = 0;
  int jStart = 0;
  for(int idx = 0; idx < NumberOfPieces; idx++) // iterate over pieces
  {
    vtkDataSet* input = NULL;
    if ((input = GetInput()) == NULL || this->GetNumberOfOutputs() == 0 && OutputDataType == VTK_IMAGE_DATA)
      return; // No input or no output

    input->Update();
    vtkDataArray * inputScalars = input->GetPointData()->GetScalars();

    // Fill variables for scalar index recognition
    double iFactor = 1;
    double jFactor = InputDimensions[0];
    switch(SliceAxis)
    {
    case SLICE_Z:
      {
      } break;
    case SLICE_Y:
      {
        iFactor = 1;
        jFactor = InputDimensions[1] * InputDimensions[0];
      } break;
    case SLICE_X:
      {
        iFactor = InputDimensions[0];
        jFactor = InputDimensions[1] * InputDimensions[0];
      } break;
    }

    // Prepare output scalars
    // of the same type of the input scalars
    vtkDataArray *scalars = NULL;
    switch (inputScalars->GetDataType()) 
    {
    case VTK_CHAR:
      {
        scalars = vtkCharArray::New();
        break;
      }
    case VTK_UNSIGNED_CHAR:
      {
        scalars = vtkUnsignedCharArray::New();
        break;
      }
    case VTK_SHORT:
      {
        scalars = vtkShortArray::New();
        break;
      }
    case VTK_UNSIGNED_SHORT:
      {
        scalars = vtkUnsignedShortArray::New();
        break;
      }
    case VTK_FLOAT:
      {
        scalars = vtkFloatArray::New();
        break;
      }
    case VTK_DOUBLE:
      {
        scalars = vtkDoubleArray::New();
        break;
      }
    default:
      return;
    }

    // get dimensions
    int dimensions[3];
    dimensions[AxisX] = SlicePieceDimensions[idx][0];
    dimensions[AxisY] = SlicePieceDimensions[idx][1];
    dimensions[SliceAxis] = 1;

    // get the spacing
    double spacing[3];
    spacing[AxisX] = SlicePieceSpacings[idx][0];
    spacing[AxisY] = SlicePieceSpacings[idx][1];
    spacing[SliceAxis] = 1;

    // update scalars attribute
    scalars->SetNumberOfComponents(inputScalars->GetNumberOfComponents());
    scalars->SetName("SCALARS");

    int iLenght = dimensions[AxisX];
    int jLenght = dimensions[AxisY];

    if(OutputDataType == VTK_RECTILINEAR_GRID)
    {
      iLenght--;
      jLenght--;
    }

    // Fill output scalars with the right values
    for(int j = 0; j < jLenght; j++)
    {
      for(int i = 0; i < iLenght; i++)
      {
        // calculate right index
        int index[2] = {i + iStart,j + jStart};
        double *tuple = inputScalars->GetTuple(index[0] * iFactor + index[1] * jFactor + BaseIndex);
        vtkIdType sid = scalars->InsertNextTuple(tuple);
      }
    }
    
    // Indeces to computer the righ scalars
    iStart += (iLenght-1);
    if(iStart >= InputDimensions[AxisX] - 1)
    {
      iStart = 0;
    }
    jStart += (jLenght-1);
    if(jStart >= InputDimensions[AxisY] - 1)
    {
      jStart = 0;
    }

    // This is executed only if recognized image pieces are greater than MAX_NUMBER_OF_PIECES
    if(OutputDataType == VTK_RECTILINEAR_GRID)
    {
      // Create the rectilinear grid structure
      if(OutputRectilinearGrid)
      {
        OutputRectilinearGrid->Delete();
        OutputRectilinearGrid = NULL;
      }
      OutputRectilinearGrid = vtkRectilinearGrid::New();
      OutputRectilinearGrid->SetDimensions(dimensions[0],dimensions[1],dimensions[2]);

      vtkDoubleArray *CoordsZ = vtkDoubleArray::New();
      CoordsZ->SetNumberOfComponents(1);
      CoordsZ->InsertNextTuple1(0.);

      switch(SliceAxis)
      {
      case SLICE_Z:
        {
          OutputRectilinearGrid->SetXCoordinates(CoordsXY[0]);
          OutputRectilinearGrid->SetYCoordinates(CoordsXY[1]);
          OutputRectilinearGrid->SetZCoordinates(CoordsZ);
        } break;
      case SLICE_Y:
        {
          OutputRectilinearGrid->SetXCoordinates(CoordsXY[0]);
          OutputRectilinearGrid->SetZCoordinates(CoordsXY[1]);
          OutputRectilinearGrid->SetYCoordinates(CoordsZ);
        } break;
      case SLICE_X:
        {
          OutputRectilinearGrid->SetYCoordinates(CoordsXY[0]);
          OutputRectilinearGrid->SetZCoordinates(CoordsXY[1]);
          OutputRectilinearGrid->SetXCoordinates(CoordsZ);
        } break;
      }
      CoordsZ->Delete();

      // Set Scalars
      OutputRectilinearGrid->GetCellData()->RemoveArray("SCALARS");
      OutputRectilinearGrid->GetCellData()->SetScalars(scalars);
      OutputRectilinearGrid->GetCellData()->SetActiveScalars("SCALARS");
      OutputRectilinearGrid->GetCellData()->Update();

      OutputRectilinearGrid->Modified();
      OutputRectilinearGrid->Update();
    }
    else
    {
      vtkImageData *output = this->GetOutput(idx);
      if (output == NULL)
      {
        this->Outputs[idx] = output = vtkImageData::New();
      }
      // Prepare and generate output image
      output->SetSpacing(spacing[0],spacing[1],spacing[2]);
      output->SetExtent(0, (dimensions[0] - 1) , 0, (dimensions[1] - 1), 0, (dimensions[2] - 1));
      output->SetOrigin(SlicePieceOrigins[idx][0],SlicePieceOrigins[idx][1],SlicePieceOrigins[idx][2]);

      // Set the image scalars
      output->SetScalarType(inputScalars->GetDataType());
      output->GetPointData()->RemoveArray("SCALARS");
      output->GetPointData()->SetScalars(scalars);
      output->GetPointData()->SetActiveScalars("SCALARS");
      output->GetPointData()->Update();
      scalars->Delete();

      output->Modified();
      output->Update();
    }
  }
}