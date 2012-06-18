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

#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include <cassert>
#include "vtkMAFSmartPointer.h"
#include "vtkImageCast.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "vtkStructuredPointsWriter.h"

#define PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS 1

vtkCxxRevisionMacro(vtkMEDImageFillHolesRemoveIslands, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMEDImageFillHolesRemoveIslands);

typedef itk::Image< double, 2 > DoubleImage;
typedef itk::Image< unsigned char, 2 > UCharImage;
typedef itk::ConnectedThresholdImageFilter <UCharImage, UCharImage > ConnectedThresholdFilter;
typedef itk::OrImageFilter<UCharImage,UCharImage,UCharImage> OrFilter;

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
  vtkStructuredPoints *outputImage = this->GetOutput();
  this->GetInput()->Update();
  outputImage->DeepCopy(this->GetInput());
  outputImage->UpdateData();
  outputImage->Update();

  int imgDims[3];
  outputImage->GetDimensions(imgDims);

  vtkUnsignedCharArray* imgScalars = (vtkUnsignedCharArray*)outputImage->GetPointData()->GetScalars();

  int recognitionSquareEdge = EdgeSize + 2; // Number of pixels of the recognition square
  // @ToDo: test image bounds?

  // @Todo: OPTIMIZATION:
  // Start from first coordinates (xs,ys) where the value is different from discrimination value (not necessary depending on the same pixel)
  // End from last coordinates (xl,yl) where the value is different from discrimination value (not necessary depending on the same pixel)

  //-------------------------------------------------------------------------------------------------
  vtkMAFSmartPointer<vtkImageCast> caster;
  caster->SetOutputScalarTypeToUnsignedChar();
  caster->SetInput(outputImage);
  caster->Update();

  int dims[3];

  caster->GetOutput()->GetDimensions(dims);
  ////

  //// Convert vtk image to itk image
  typedef itk::VTKImageToImageFilter<UCharImage> vtkImageDataToitkUcharImageFilter;
  vtkImageDataToitkUcharImageFilter::Pointer vtkimage_converter = vtkImageDataToitkUcharImageFilter::New();
  vtkimage_converter->SetInput(caster->GetOutput());
  vtkimage_converter->Update();

  caster->GetOutput()->GetDimensions(dims);

  //// Flood Fill the exter of the shape
  ConnectedThresholdFilter::Pointer connectedThreshold = ConnectedThresholdFilter::New();
  connectedThreshold->SetLower(0);
  connectedThreshold->SetUpper(100);
 
  connectedThreshold->SetReplaceValue(DiscriminationPixelValue);
 
  // Seed 1: (25, 35)
  UCharImage::IndexType seed1;
  seed1[0] = 0;
  seed1[1] = 0;
  connectedThreshold->SetSeed(seed1);
  connectedThreshold->SetInput(vtkimage_converter->GetOutput());
  connectedThreshold->Update();

  OrFilter::Pointer or = OrFilter::New();
  or->SetInput1(vtkimage_converter->GetOutput());
  or->SetInput2(connectedThreshold->GetOutput());

  //// Convert itk image to vtk image
  typedef itk::ImageToVTKImageFilter<UCharImage> itkDoubleImageTovtkImageDataFilter;
  itkDoubleImageTovtkImageDataFilter::Pointer itkimage_converter = itkDoubleImageTovtkImageDataFilter::New();
  itkimage_converter->SetInput(or->GetOutput()); //binary_dilatator->GetOutput());
  itkimage_converter->Update();
  

  //// Get the output image
  vtkStructuredPoints *imageMap = ((vtkStructuredPoints*)itkimage_converter->GetOutput());
  imageMap->Update();
  //-------------------------------------------------------------------------------------------------

  vtkUnsignedCharArray* imgMapScalars = (vtkUnsignedCharArray*)imageMap->GetPointData()->GetScalars();

  /*
  outputImage->GetPointData()->SetScalars(imgMapScalars);
  outputImage->GetPointData()->Modified();
  outputImage->GetPointData()->Update();
  outputImage->UpdateData();
  outputImage->Update();
  return;
  */

  while(recognitionSquareEdge >= 3)
  {
    // (x0, y0) origin of the recognition square
    for(int y0 = 0; y0 < imgDims[1] - recognitionSquareEdge; y0++)
    {

      for(int x0 = 0; x0 < imgDims[0] - recognitionSquareEdge; x0++)
      {

        // imgScalars->SetTuple1(y0 * imgDims[0] + x0, 255);
        bool isolatedRegion = true;
        int peninsulaConerNumberOfPixels = 0;

        // check "y" sides
        for(int y = y0; y < y0 + recognitionSquareEdge; y = y + recognitionSquareEdge - 1)
        {
          for(int x = x0; x < x0 + recognitionSquareEdge; x++)
          {
            //if(imgMapScalars->GetTuple1(y * imgDims[0] + x) <= 0)
            {
              if((imgMapScalars->GetTuple1(y * imgDims[0] + x) != DiscriminationPixelValue))
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
              //if(imgMapScalars->GetTuple1(y * imgDims[0] + x) <= 0)
              {
                if((imgMapScalars->GetTuple1(y * imgDims[0] + x) != DiscriminationPixelValue))
                {
                  if(!(((x == x0 && y == y0) || (x == x0 + recognitionSquareEdge - 1 && y == y0) || (x == x0 && y == y0  + recognitionSquareEdge - 1) || (x == x0 + recognitionSquareEdge - 1 && y == y0  + recognitionSquareEdge - 1)) && RemovePeninsulaRegions)) // Corner pixels
                  {
                    isolatedRegion = false;
                    break;
                  }
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
          //mafLogMessage("Square x0,y0 %d,%d IS an island",x0,y0);
          // fill all the recognition square with discrimination value
          for(int y = y0 + 1; y < y0 + recognitionSquareEdge - 1; y++)
          {
            for(int x = x0 + 1; x < x0 + recognitionSquareEdge - 1; x++)
            {
              if(imgMapScalars->GetTuple1(y * imgDims[0] + x) != DiscriminationPixelValue)
              {
                imgScalars->SetTuple1(y * imgDims[0] + x, DiscriminationPixelValue);
              }
            }
          }
        }
      }
    }
    recognitionSquareEdge --;
  }
  outputImage->GetPointData()->SetScalars(imgScalars);
  outputImage->GetPointData()->Modified();
  outputImage->GetPointData()->Update();
  outputImage->UpdateData();
  outputImage->Update();
  this->SetOutput(outputImage);
}