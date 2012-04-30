/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkMEDVolumeSlicerNotInterpolated.cxx,v $
  Language:  C++
  Date:      $Date: 2012-04-30 15:43:16 $
  Version:   $Revision: 1.1.2.4 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "medDefines.h"
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

vtkCxxRevisionMacro(vtkMEDVolumeSlicerNotInterpolated, "$Revision: 1.1.2.4 $");
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

  vtkImageData* imageData = NULL;
  vtkRectilinearGrid * rectilinearGrid = NULL;
  if ((imageData = vtkImageData::SafeDownCast(input)) != NULL) 
  {
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
    SliceDimensions[0] = InputDimensions[AxisX];
    SliceDimensions[1] = InputDimensions[AxisY];
    SliceSpacing[0] = InputSpacing[AxisX];
    SliceSpacing[1] = InputSpacing[AxisY];
    SliceSpacing[2] = 1;

    // Get the first point index where axis is nearest to Origin[SliceAxis]
    double minDist = VTK_DOUBLE_MAX;
    int nearestIndex = -1;
    for(int i = 0; i < InputDimensions[SliceAxis]; i++)
    {
      int curIndex = i * multiplyFactor;
      double xyz[3];
      imageData->GetPoint(curIndex,xyz);
      int dist = 0;
      if(((dist = abs(xyz[SliceAxis] - Origin[SliceAxis])) < minDist) /*&& ((xyz[SliceAxis] - Origin[2]) <= 0)*/)
      {
        nearestIndex = curIndex;
        minDist = dist;
        SliceOrigin[0] = xyz[AxisX];
        SliceOrigin[1] = xyz[AxisY];
      }
      else if(dist > minDist && nearestIndex != -1)
      {
        break;
      }
    }
    // BaseIndex is the current slice first point index
    BaseIndex = nearestIndex;

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

    SliceDimensions[0] = InputDimensions[AxisX];
    SliceDimensions[1] = InputDimensions[AxisY];

    // Get the first point index where axis is nearest to Origin[axis]
    double minDist = VTK_DOUBLE_MAX;
    int nearestIndex = -1;
    for(int i = 0; i < InputDimensions[SliceAxis]; i++)
    {
      double tupleVal = coordsZ->GetTuple1(i);
      int dist = 0;
      if(((dist = abs(tupleVal - Origin[SliceAxis])) < minDist) /*&& ((tupleVal - Origin[2]) <= 0)*/)
      {
        int ijk[3] = {0,0,0};
        ijk[SliceAxis] = i;
        nearestIndex = rectilinearGrid->ComputePointId(ijk);
        double xyz[3];
        rectilinearGrid->GetPoint(nearestIndex,xyz);
        SliceOrigin[0] = xyz[AxisX];
        SliceOrigin[1] = xyz[AxisY];
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
    // If spacing is regular the output can be only the image data.
    OutputDataType = VTK_IMAGE_DATA;
    for(int i = 0; i < 2; i++)
    {
      for(int c = 0; c < CoordsXY[i]->GetNumberOfTuples() - 2; c++)
      {
        double spac1 = CoordsXY[i]->GetTuple1(c + 1) - CoordsXY[i]->GetTuple1(c);
        double spac2 = CoordsXY[i]->GetTuple1(c + 2) - CoordsXY[i]->GetTuple1(c + 1);
        SliceSpacing[i] = spac1;
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
  }

  // Now nearest index is the "origin" of the slice so all points in the slice can be evaluated as 
  // x + y * InputDimensions[0] + BaseIndex for SLICE_Z
  // x + z * InputDimensions[0] * InputDimensions[1] + BaseIndex for SLICE_Y
  // y * InputDimensions[0] + z * InputDimensions[1] * InputDimensions[0] + BaseIndex for SLICE_X
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
  vtkDataSet* input = NULL;
  if ((input = GetInput()) == NULL || this->GetNumberOfOutputs() == 0)
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
  scalars->SetNumberOfComponents(inputScalars->GetNumberOfComponents());
  scalars->SetName("SCALARS");


  int iLenght = SliceDimensions[0];
  int jLenght = SliceDimensions[1];
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
      int index[2] = {i,j};
      double *tuple = inputScalars->GetTuple(index[0] * iFactor + index[1] * jFactor + BaseIndex);
      vtkIdType sid = scalars->InsertNextTuple(tuple);
    }
  }
  int dimensions[3];
  dimensions[0] = InputDimensions[0];
  dimensions[1] = InputDimensions[1];
  dimensions[2] = InputDimensions[2];
  dimensions[SliceAxis] = 1;

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
  // Prepare and generate output image
  output->SetSpacing(InputSpacing[0],InputSpacing[1],InputSpacing[2]);
  output->SetExtent(0, (dimensions[0] - 1) , 0, (dimensions[1] - 1), 0, (dimensions[2] - 1));
  output->SetOrigin(Origin[0],Origin[1],Origin[2]);

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