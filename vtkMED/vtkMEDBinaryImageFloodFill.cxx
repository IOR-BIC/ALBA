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


#include "vtkMEDBinaryImageFloodFill.h"
#include "vtkMAFSmartPointer.h"

#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
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

vtkCxxRevisionMacro(vtkMEDBinaryImageFloodFill, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMEDBinaryImageFloodFill);



//----------------------------------------------------------------------------
vtkMEDBinaryImageFloodFill::vtkMEDBinaryImageFloodFill()
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
vtkMEDBinaryImageFloodFill::~vtkMEDBinaryImageFloodFill()
//----------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void vtkMEDBinaryImageFloodFill::Execute()
//------------------------------------------------------------------------------
{
  // get input
  vtkStructuredPoints *input = (vtkStructuredPoints*)this->GetInput();
  input->Update();

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
  
  vtkStructuredPoints *intermediate_output = NULL;
  
  // call the right flood fill template function
  switch(image_dimension)
  {
    case 0:
      {
        return;
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
  vtkStructuredPoints *output = this->GetOutput();
  output->DeepCopy(intermediate_output);
  output->UpdateData();
  output->Update();
  this->SetOutput(output);
  intermediate_output->Delete();
}
//------------------------------------------------------------------------------
template <unsigned int ImageDimension>
vtkStructuredPoints *vtkMEDBinaryImageFloodFill::FloodFill(vtkStructuredPoints *input)
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
  vtkMAFSmartPointer<vtkImageCast> caster;
  caster->SetOutputScalarTypeToUnsignedChar();
  caster->SetInput(input);
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
    // differnce filter to subtract original image with flood filled one
    Difference::Pointer difference = Difference::New();
    difference->SetInput1(vtk2Itk->GetOutput());
    difference->SetInput2(connectedThreshold->GetOutput());
    difference->Update();
    itk2Vtk->SetInput(difference->GetOutput());
  }
  
  itk2Vtk->Update();

  vtkStructuredPoints *output = vtkStructuredPoints::New();
  output->CopyInformation(input);
  output->CopyStructure(input);
  output->DeepCopy(itk2Vtk->GetOutput());

  // Get the id of the center of the filled region:
  // the center is represented by the point inside the region at the maximum distance
  SignedMaurerDistanceMap::Pointer distance = SignedMaurerDistanceMap::New();
  distance->SetInput(connectedThreshold->GetOutput());
  distance->Update();

  ItkDouble2Vtk::Pointer itkDouble2Vtk = ItkDouble2Vtk::New();
  itkDouble2Vtk->SetInput(distance->GetOutput());
  itkDouble2Vtk->Update();

  vtkStructuredPoints *distanceImage = (vtkStructuredPoints *)itkDouble2Vtk->GetOutput();
  distanceImage->Update();
  vtkDoubleArray *distanceScalars = (vtkDoubleArray *)distanceImage->GetPointData()->GetScalars();

  // Iterate over scalars and get the id of the minimum
  double minDistance = VTK_DOUBLE_MAX;
  for(vtkIdType s = 0; s < distanceScalars->GetNumberOfTuples(); s++)
  {
    double curDistance = distanceScalars->GetTuple1(s);
    if(curDistance < minDistance)
    {
      Center = s;
      minDistance = curDistance;
    }
  }
  return output;
}

//------------------------------------------------------------------------------
void vtkMEDBinaryImageFloodFill::ComputeItkSeed()
//------------------------------------------------------------------------------
{
  // get input
  vtkStructuredPoints *input = (vtkStructuredPoints*)this->GetInput();
  input->Update();

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