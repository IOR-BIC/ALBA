/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImageFillHolesRemoveIslands
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkALBAImageFillHolesRemoveIslands.h"
#include "vtkALBABinaryImageFloodFill.h"

#include "vtkALBASmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

#include <cassert>


#define PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS 1

vtkStandardNewMacro(vtkALBAImageFillHolesRemoveIslands);

//----------------------------------------------------------------------------
vtkALBAImageFillHolesRemoveIslands::vtkALBAImageFillHolesRemoveIslands()
//----------------------------------------------------------------------------
{
  SetAlgorithmToFillHoles(); // Algorithm = FILL_HOLES; DiscriminationPixelValue = 255;
  EdgeSize = 1;
  RemovePeninsulaRegions = false;
}

//----------------------------------------------------------------------------
vtkALBAImageFillHolesRemoveIslands::~vtkALBAImageFillHolesRemoveIslands()
//----------------------------------------------------------------------------
{ 
}

//----------------------------------------------------------------------------
void vtkALBAImageFillHolesRemoveIslands::SetAlgorithm(int algorithm)
//----------------------------------------------------------------------------
{
  // set the filter algorithm 
  assert(algorithm < INVALID_ALGORITHM && algorithm >= 0);
  Algorithm = algorithm;
  switch (Algorithm)
  {
  case FILL_HOLES:
    {
      SetAlgorithmToFillHoles();
      break;
    }
  case REMOVE_ISLANDS:
    {
      SetAlgorithmToRemoveIslands();
      break;
    }
  }
}

//------------------------------------------------------------------------------
int vtkALBAImageFillHolesRemoveIslands::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkImageData  *input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));


  output->DeepCopy(input);


  int recognitionSquareEdge = EdgeSize + 2; // Number of pixels of the recognition square

  int dims[3];
  input->GetDimensions(dims);
  
  // Flood fill external region of the shape to allow fill big holes inside the shape
  vtkALBABinaryImageFloodFill *flood_fill = vtkALBABinaryImageFloodFill::New();
  flood_fill->SetInputData(input);
  flood_fill->Update();

  // get flood fill scalars
  vtkUnsignedCharArray* filled_scalars = (vtkUnsignedCharArray*)input->GetPointData()->GetScalars();
  // get output scalars
  vtkUnsignedCharArray* output_scalars = (vtkUnsignedCharArray*)output->GetPointData()->GetScalars();

  while(recognitionSquareEdge >= 3)
  {
    // (x0, y0) origin of the recognition square
    for(int y0 = 0; y0 < dims[1] - recognitionSquareEdge; y0++)
    {

      for(int x0 = 0; x0 < dims[0] - recognitionSquareEdge; x0++)
      {
        bool isolatedRegion = true;
        int peninsulaConerNumberOfPixels = 0;

        // check "y" sides
        for(int y = y0; y < y0 + recognitionSquareEdge; y = y + recognitionSquareEdge - 1)
        {
          for(int x = x0; x < x0 + recognitionSquareEdge; x++)
          {
            if((filled_scalars->GetTuple1(y * dims[0] + x) != DiscriminationPixelValue))
            {
              if(((x == x0 && y == y0) || (x == x0 + recognitionSquareEdge - 1 && y == y0) || (x == x0 && y == y0  + recognitionSquareEdge - 1) || (x == x0 + recognitionSquareEdge - 1 && y == y0  + recognitionSquareEdge - 1)) && RemovePeninsulaRegions) // Corner pixels
              {
                peninsulaConerNumberOfPixels++; // update the number of "corner pixel"
              }
              else
              {
                isolatedRegion = false;
                break;
              }
            }
          }
          if(!isolatedRegion)
          {
            break;
          }
        }
        if(peninsulaConerNumberOfPixels > PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS && isolatedRegion == true)
        {
          isolatedRegion = false; // not isolated region is not a peninsula
        }

        // check x sides
        if(isolatedRegion)
        {
          peninsulaConerNumberOfPixels = 0;
          for(int x = x0; x < x0 + recognitionSquareEdge; x = x + recognitionSquareEdge - 1)
          {
            for(int y = y0; y < y0 + recognitionSquareEdge; y++)
            {
              if((filled_scalars->GetTuple1(y * dims[0] + x) != DiscriminationPixelValue))
              {
                if(!(((x == x0 && y == y0) || (x == x0 + recognitionSquareEdge - 1 && y == y0) || (x == x0 && y == y0  + recognitionSquareEdge - 1) || (x == x0 + recognitionSquareEdge - 1 && y == y0  + recognitionSquareEdge - 1)) && RemovePeninsulaRegions)) // Corner pixels
                {
                  isolatedRegion = false;
                  break;
                }
              }
            }
            if(!isolatedRegion)
            {
              break;
            }
          }
        }

        if(isolatedRegion)
        {
          // fill all the recognition square with discrimination value
          for(int y = y0 + 1; y < y0 + recognitionSquareEdge - 1; y++)
          {
            for(int x = x0 + 1; x < x0 + recognitionSquareEdge - 1; x++)
            {
              if(filled_scalars->GetTuple1(y * dims[0] + x) != DiscriminationPixelValue)
              {
                output_scalars->SetTuple1(y * dims[0] + x, DiscriminationPixelValue);
              }
            }
          }
        }
      }
    }
    recognitionSquareEdge --;
  }
  // set output's scalars and filter's output
  output->GetPointData()->SetScalars(output_scalars);
  output->GetPointData()->Update();
  output->GetPointData()->Modified();
  flood_fill->Delete();
 
	return 1;
}
