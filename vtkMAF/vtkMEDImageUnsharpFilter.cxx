/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMEDImageUnsharpFilter.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMEDImageUnsharpFilter.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "itkLaplacianSharpeningImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include <math.h>

vtkCxxRevisionMacro(vtkMEDImageUnsharpFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMEDImageUnsharpFilter);

//----------------------------------------------------------------------------
vtkMEDImageUnsharpFilter::vtkMEDImageUnsharpFilter()
{
}

//----------------------------------------------------------------------------
vtkMEDImageUnsharpFilter::~vtkMEDImageUnsharpFilter()
{
}

//----------------------------------------------------------------------------
void vtkMEDImageUnsharpFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  // int idx;  

  //os << indent << "BoundaryRescale: " << this->BoundaryRescale << "\n";

  /*os << indent << "Dimensionality: " << this->Dimensionality << "\n";

  os << indent << "RadiusFactors: ( "
     << this->RadiusFactors[0] << ", "
     << this->RadiusFactors[1] << ", "
     << this->RadiusFactors[2] << " )\n";

  os << indent << "StandardDeviations: ( "
     << this->StandardDeviations[0] << ", "
     << this->StandardDeviations[1] << ", "
     << this->StandardDeviations[2] << " )\n";*/
}
//----------------------------------------------------------------------------
// This method decomposes the gaussian and smooths along each axis.
void vtkMEDImageUnsharpFilter::ThreadedExecute(vtkImageData *inData, 
                                             vtkImageData *outData,
                                             int outExt[6], int id)
{
  typedef  double InputPixelType;
  typedef  itk::Image< InputPixelType, 3 > InputImageType;
  typedef  double OutputPixelType;
  typedef  itk::Image< OutputPixelType, 3 > OutputImageType; 
  typedef  itk::LaplacianSharpeningImageFilter< InputImageType, OutputImageType > MyLaplacianFilter;

  MyLaplacianFilter::Pointer spLaplFilt = MyLaplacianFilter::New();
  //c->SetInput

  typedef itk::VTKImageToImageFilter< OutputImageType > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New(); // i prefer the smart pointer...

  vtkTOitk->SetInput( GetInput() );

  try
  {
    vtkTOitk->Update();
  }

  catch (...)
  {
    //wxMessageBox("An error has been occurred during vtk to itk conversion");
    GetOutput()->DeepCopy(GetInput());
    return;
  }
  spLaplFilt->SetInput(vtkTOitk->GetOutput());

  typedef itk::RescaleIntensityImageFilter<OutputImageType, OutputImageType> RescaleFilter;

  //Setting the IO
  RescaleFilter::Pointer rescale = RescaleFilter::New();

  //Setting the ITK pipeline filter



  // Rescale and cast to unsigned char
  rescale->SetInput( spLaplFilt->GetOutput() );

  rescale->SetOutputMinimum(   0 );
  rescale->SetOutputMaximum( 65536 ); //16 bit..


  // i prefer the smart pointer...->Update();
  try
  {
    rescale->Update();
  }
  catch (...)
  {
    GetOutput()->DeepCopy(GetInput());

    //wxMessageBox("An error has been occurred during unsharp computation");
    return;
  }

  typedef itk::ImageToVTKImageFilter< OutputImageType > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New(); // i prefer the smart pointer...
  itkTOvtk->SetInput( rescale->GetOutput() );
  itkTOvtk->Update();
  GetOutput()->DeepCopy(vtkImageData::SafeDownCast(itkTOvtk->GetOutput()));

  //set the output of the filter
}