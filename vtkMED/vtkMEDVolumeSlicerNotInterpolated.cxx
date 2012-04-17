/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkMEDVolumeSlicerNotInterpolated.cxx,v $
  Language:  C++
  Date:      $Date: 2012-04-17 14:52:28 $
  Version:   $Revision: 1.1.2.2 $
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
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkCharArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

vtkCxxRevisionMacro(vtkMEDVolumeSlicerNotInterpolated, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMEDVolumeSlicerNotInterpolated);

//----------------------------------------------------------------------------
vtkMEDVolumeSlicerNotInterpolated::vtkMEDVolumeSlicerNotInterpolated() 
//----------------------------------------------------------------------------
{
  // Initialize attributes
  SliceOrigin[0] = SliceOrigin[1] = SliceOrigin[2] = .0;
  SliceAxis = SLICE_Z;
  NumberOfComponents = 0;
  BaseIndex = -1;
  SliceDimensions[0] = SliceDimensions[1] = 0;
  InputDimensions[0] = InputDimensions[1] = InputDimensions[2] = 0;
  InputSpacing[0] = InputSpacing[1] = InputSpacing[2] = 1.; 
}
//----------------------------------------------------------------------------
vtkMEDVolumeSlicerNotInterpolated::~vtkMEDVolumeSlicerNotInterpolated() 
//----------------------------------------------------------------------------
{
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
    // Input is an image data
    imageData->GetDimensions(InputDimensions);
    imageData->GetSpacing(InputSpacing);
    int multiplyFactor = InputDimensions[1] * InputDimensions[0];

    // Fill slice dimensions (output image is always defined in xy plane)
    switch(SliceAxis)
    {
      case SLICE_Z:
        {
          SliceDimensions[0] = InputDimensions[0];
          SliceDimensions[1] = InputDimensions[1];
        } break;
      case SLICE_Y:
        {
          multiplyFactor = InputDimensions[0];
          SliceDimensions[0] = InputDimensions[0];
          SliceDimensions[1] = InputDimensions[2];
        } break;
      case SLICE_X:
        {
          multiplyFactor = 1;
          SliceDimensions[0] = InputDimensions[1];
          SliceDimensions[1] = InputDimensions[2];
        } break;
    }
    // Get the first point index where axis is nearest to SliceOrigin[SliceAxis]
    double minDist = VTK_DOUBLE_MAX;
    int nearestIndex = -1;
    for(int i = 0; i < InputDimensions[SliceAxis]; i++)
    {
      int curIndex = i * multiplyFactor;
      double xyz[3];
      imageData->GetPoint(curIndex,xyz);
      int dist = 0;
      if(((dist = abs(xyz[SliceAxis] - SliceOrigin[SliceAxis])) < minDist) /*&& ((xyz[SliceAxis] - SliceOrigin[2]) <= 0)*/)
      {
        nearestIndex = curIndex;
        minDist = dist;
      }
      else if(dist > minDist && nearestIndex != -1)
      {
        break;
      }
    }
    // BaseIndex is the current slice first point index
    BaseIndex = nearestIndex;
  }
  else if((rectilinearGrid = vtkRectilinearGrid::SafeDownCast(input)) != NULL)
  {
    // Input is a rectilinear grid
    rectilinearGrid->GetDimensions(InputDimensions);
    vtkDoubleArray *coords = NULL;

    // Fill slice dimensions (output image is always in xy plane)
    switch(SliceAxis)
      {
      case SLICE_Z:
        {
          coords = (vtkDoubleArray *)rectilinearGrid->GetZCoordinates();
          SliceDimensions[0] = InputDimensions[0];
          SliceDimensions[1] = InputDimensions[1];
        } break;
      case SLICE_Y:
        {
          coords = (vtkDoubleArray *)rectilinearGrid->GetYCoordinates();
          SliceDimensions[0] = InputDimensions[0];
          SliceDimensions[1] = InputDimensions[2];
        } break;
      case SLICE_X:
        {
          coords = (vtkDoubleArray *)rectilinearGrid->GetXCoordinates();
          SliceDimensions[0] = InputDimensions[1];
          SliceDimensions[1] = InputDimensions[2];
        } break;
     }

    // Get the first point index where axis is nearest to SliceOrigin[axis]
    double minDist = VTK_DOUBLE_MAX;
    int nearestIndex = -1;
    for(int i = 0; i < InputDimensions[SliceAxis]; i++)
    {
      double tupleVal = coords->GetTuple1(i);
      int dist = 0;
      if(((dist = abs(tupleVal - SliceOrigin[SliceAxis])) < minDist) /*&& ((tupleVal - SliceOrigin[2]) <= 0)*/)
      {
        int ijk[3] = {0,0,0};
        ijk[SliceAxis] = i;
        nearestIndex = rectilinearGrid->ComputePointId(ijk);
        minDist = dist;
      }
      else if(dist > minDist && nearestIndex != -1)
      {
        break;
      }
    }

    // BaseIndex is the current slice first point index
    BaseIndex = nearestIndex;

    // NOT COMPLETELY IMPLEMENTED FOR RG!
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
      scalars =vtkDoubleArray::New();
      break;
    }
  default:
    return;
  }
  scalars->SetNumberOfComponents(inputScalars->GetNumberOfComponents());
  scalars->SetName("SCALARS");

  // Fill output scalars with the right values
  for(int j = 0; j < SliceDimensions[1]; j++)
  {
    for(int i = 0; i < SliceDimensions[0]; i++)
    {
      double *tuple = inputScalars->GetTuple(i * iFactor + j * jFactor + BaseIndex);
      vtkIdType sid = scalars->InsertNextTuple(tuple);
      // NOT COMPLETELY IMPLEMENTED FOR RG!
    }
  }

  // Prepare and generate output image
  output->SetSpacing(InputSpacing[0],InputSpacing[1],InputSpacing[2]);
  output->SetExtent(0, (SliceDimensions[0] - 1) , 0, (SliceDimensions[1] - 1), 0, 0);
  output->SetOrigin(0,0,0);

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