/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBABinaryImageFloodFill
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkALBABinaryImageFloodFill.h"
#include "vtkALBASmartPointer.h"

#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkImageCast.h"
#include "vtkDoubleArray.h"

#include <cassert>

#include "itkVTKImageToImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkImageToVTKImageFilter.h"

vtkStandardNewMacro(vtkALBABinaryImageFloodFill);



//----------------------------------------------------------------------------
vtkALBABinaryImageFloodFill::vtkALBABinaryImageFloodFill()
//----------------------------------------------------------------------------
{
  // initialize attributes to default
  ReplaceValue = ON_PIXEL;
  Seed = 0;
  ItkSeed[0] = 0;
  ItkSeed[1] = 0;
  ItkSeed[2] = 0;
  Threshold[0] = OFF_PIXEL;
  Threshold[1] = (ON_PIXEL - OFF_PIXEL) / 2;
  Erase = false;
  Center = 0;
}

//----------------------------------------------------------------------------
vtkALBABinaryImageFloodFill::~vtkALBABinaryImageFloodFill()
//----------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
int vtkALBABinaryImageFloodFill::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkImageData  *input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  
  // Get the image dimensions based on input
  int dims[3];
  input->GetDimensions(dims);
  int image_dimension = 0;
  
  // calculate image dimensionality
  for(int i = 0; i < 3; i++)
  {
    if(dims[i] > 1)
    {
      image_dimension++;
    }
  }
  
  vtkImageData *intermediate_output = NULL;
  
  // call the right flood fill template function
  switch(image_dimension)
  {
    case 0:
      {
        return 1;
      } break;
    case 1:
      {
        intermediate_output = FloodFill< 1 >(input); // 1D image
      } break;
    case 2:
      {
        intermediate_output = FloodFill< 2 >(input); // 2D image
      } break;
    case 3:
      {
        intermediate_output = FloodFill< 3 >(input); // 3D image ( = Volume)
      } break;
  }
  
  // prepare output
  output->DeepCopy(intermediate_output);

	intermediate_output->Delete();
	
	return 1;
}
//------------------------------------------------------------------------------
template <unsigned int ImageDimension>
vtkImageData *vtkALBABinaryImageFloodFill::FloodFill(vtkImageData *input)
 //------------------------------------------------------------------------------
{
  ComputeItkSeed();

  // define itk types
  typedef itk::Image < unsigned char, ImageDimension > UChar;
  typedef itk::Image < double, ImageDimension > Double;
  typedef itk::VTKImageToImageFilter < UChar > Vtk2Itk;
  typedef itk::ConnectedThresholdImageFilter < UChar, UChar > ConnectedThreshold;
  typedef itk::SignedMaurerDistanceMapImageFilter <UChar, Double> SignedMaurerDistanceMap;
  typedef itk::OrImageFilter < UChar, UChar, UChar > Or;
  typedef itk::SubtractImageFilter < UChar, UChar, UChar > Difference;
  typedef itk::ImageToVTKImageFilter < UChar > Itk2Vtk;
  typedef itk::ImageToVTKImageFilter < Double > ItkDouble2Vtk;

  // must cast the image before pass it to the itk pipeline
  vtkALBASmartPointer<vtkImageCast> caster;
  caster->SetOutputScalarTypeToUnsignedChar();
  caster->SetInputData(input);
  caster->Update();

  // Convert vtk image to itk
  Vtk2Itk::Pointer vtk2Itk = Vtk2Itk::New();
  vtk2Itk->SetInput(caster->GetOutput());
  vtk2Itk->Update();

  // flood fill filter
  ConnectedThreshold::Pointer connectedThreshold = ConnectedThreshold::New();
  connectedThreshold->SetLower(Threshold[0]);
  connectedThreshold->SetUpper(Threshold[1]);
  connectedThreshold->SetReplaceValue(ReplaceValue);
  
  UChar::IndexType seed;
  for(int i = 0; i < ImageDimension; i++)
  {
    seed[i] = ItkSeed[i];
  }
  connectedThreshold->SetSeed(seed);
  connectedThreshold->SetInput(vtk2Itk->GetOutput());
  connectedThreshold->Update();

  // convert itk to vtk
  Itk2Vtk::Pointer itk2Vtk = Itk2Vtk::New();

  if(!Erase)
  {
    // or filter to sum original image with flood filled one
    Or::Pointer or = Or::New();
    or->SetInput1(vtk2Itk->GetOutput());
    or->SetInput2(connectedThreshold->GetOutput());
    or->Update();
    itk2Vtk->SetInput(or->GetOutput());
  }
  else
  {
    // differEnce filter to subtract original image with flood filled one
    Difference::Pointer difference = Difference::New();
    difference->SetInput1(vtk2Itk->GetOutput());
    difference->SetInput2(connectedThreshold->GetOutput());
    difference->Update();
    itk2Vtk->SetInput(difference->GetOutput());
  }
  
  itk2Vtk->Update();

  vtkImageData *output = vtkImageData::New();
  output->CopyStructure(input);
  output->DeepCopy(itk2Vtk->GetOutput());

  // Get the id of the center of the filled region:
  // the center is represented by the point inside the region at the maximum distance
//   SignedMaurerDistanceMap::Pointer distance = SignedMaurerDistanceMap::New();
//   distance->SetInput(connectedThreshold->GetOutput());
//   distance->Update();
// 
//   ItkDouble2Vtk::Pointer itkDouble2Vtk = ItkDouble2Vtk::New();
//   itkDouble2Vtk->SetInput(distance->GetOutput());
//   itkDouble2Vtk->Update();
// 
//   vtkImageData *distanceImage = (vtkImageData *)itkDouble2Vtk->GetOutput();
//   distanceImage->Update();
//   vtkDoubleArray *distanceScalars = (vtkDoubleArray *)distanceImage->GetPointData()->GetScalars();
// 
//   // Iterate over scalars and get the id of the minimum
//   double minDistance = VTK_DOUBLE_MAX;
//   for(vtkIdType s = 0; s < distanceScalars->GetNumberOfTuples(); s++)
//   {
//     double curDistance = distanceScalars->GetTuple1(s);
//     if(curDistance < minDistance)
//     {
//       Center = s;
//       minDistance = curDistance;
//     }
//   }
//   if(Center == 0)
//   {
//     Center = Seed;
//   }
  return output;
}

//------------------------------------------------------------------------------
void vtkALBABinaryImageFloodFill::ComputeItkSeed()
//------------------------------------------------------------------------------
{
  // get input
  vtkImageData *input = (vtkImageData*)this->GetInput();

  // Get the image dimensions based on input
  int dims[3];
  input->GetDimensions(dims);

  double seed[3];
  double origin[3];
  input->GetPoint(Seed,seed);
  input->GetPoint(0,origin);

  double spacing[3];
  input->GetSpacing(spacing);
  ItkSeed[0] = (seed[0] - origin[0]) / spacing[0];
  ItkSeed[1] = (seed[1] - origin[1]) / spacing[1];
  ItkSeed[2] = (seed[2] - origin[2]) / spacing[2];
}
