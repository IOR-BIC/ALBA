/*===========================================================================
Program:   Medical
Module:    $RCSfile: vtkMEDImageFillHolesRemoveIslands.cxx,v $
Language:  C++
Date:      $Date: 2012-01-26 13:44:58 $
Version:   $Revision: 1.1.2.2 $
Authors:   Alberto Losi
=============================================================================
Copyright (c) 2010
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
===========================================================================*/


#include "vtkMEDImageFillHolesRemoveIslands.h"
#include "vtkMEDBinaryImageFloodFill.h"

#include "vtkMAFSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

#include <cassert>


#define PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS 1

vtkCxxRevisionMacro(vtkMEDImageFillHolesRemoveIslands, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMEDImageFillHolesRemoveIslands);

//----------------------------------------------------------------------------
vtkMEDImageFillHolesRemoveIslands::vtkMEDImageFillHolesRemoveIslands()
//----------------------------------------------------------------------------
{
  SetAlgorithmToFillHoles(); // Algorithm = FILL_HOLES; DiscriminationPixelValue = 255;
  EdgeSize = 1;
  RemovePeninsulaRegions = false;
}

//----------------------------------------------------------------------------
vtkMEDImageFillHolesRemoveIslands::~vtkMEDImageFillHolesRemoveIslands()
//----------------------------------------------------------------------------
{ 
}

//----------------------------------------------------------------------------
void vtkMEDImageFillHolesRemoveIslands::SetAlgorithm(int algorithm)
//----------------------------------------------------------------------------
{
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
void vtkMEDImageFillHolesRemoveIslands::Execute()
//------------------------------------------------------------------------------
{
  // get input
  vtkStructuredPoints *input = (vtkStructuredPoints*)this->GetInput();
  input->Update();

  // prepare output
  vtkStructuredPoints *output = this->GetOutput();
  output->DeepCopy(input);
  output->UpdateData();
  output->Update();

  int recognitionSquareEdge = EdgeSize + 2; // Number of pixels of the recognition square

  int dims[3];
  input->GetDimensions(dims);
  
  // Flood fill external region of the shape to allow fill big holes inside the shape
  vtkMEDBinaryImageFloodFill *flood_fill = vtkMEDBinaryImageFloodFill::New();
  flood_fill->SetInput(input);
  flood_fill->Update();

  vtkUnsignedCharArray* filled_scalars = (vtkUnsignedCharArray*)flood_fill->GetOutput()->GetPointData()->GetScalars();
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
                peninsulaConerNumberOfPixels++;
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
          isolatedRegion = false;
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
  output->UpdateData();
  output->Update();
  this->SetOutput(output);
}