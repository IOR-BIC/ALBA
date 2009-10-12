/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSegmentationRegionGrowingConnectedThresholdTest.cpp,v $
Language:  C++
Date:      $Date: 2009-10-12 13:40:35 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medOpSegmentationRegionGrowingConnectedThresholdTest.h"

#include "medOpSegmentationRegionGrowingConnectedThreshold.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSetReader.h"
#include "vtkImageCast.h"
#include "vtkDataSetWriter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"

#define ITK_IMAGE_DIMENSION 3
typedef itk::Image< float, ITK_IMAGE_DIMENSION > RealImage;
typedef itk::ImageFileReader< RealImage > RealReaderType;
typedef itk::ImageFileWriter< RealImage > RealWriterType;

//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::tearDown()
//----------------------------------------------------------------------------
{
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  medOpSegmentationRegionGrowingConnectedThreshold op;
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medOpSegmentationRegionGrowingConnectedThreshold *op = new medOpSegmentationRegionGrowingConnectedThreshold();
  mafDEL(op);
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::TestAccept()
//----------------------------------------------------------------------------
{

  medOpSegmentationRegionGrowingConnectedThreshold *op = new medOpSegmentationRegionGrowingConnectedThreshold();

  mafDEL(op);
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::CompareImageData(vtkImageData *imITK,vtkImageData *imOP)
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT( imITK->GetNumberOfPoints() == imOP->GetNumberOfPoints() );
  CPPUNIT_ASSERT( imITK->GetNumberOfCells() == imOP->GetNumberOfCells() );

  if ( imITK->GetPointData()->GetScalars() != NULL && imOP->GetPointData()->GetScalars() != NULL )
  {
    for (int i=0;i<imITK->GetNumberOfPoints();i++)
    {
      CPPUNIT_ASSERT( imITK->GetPointData()->GetScalars()->GetTuple1(i) == imOP->GetPointData()->GetScalars()->GetTuple1(i) );
    }
  }
  else if ( imITK->GetCellData()->GetScalars() != NULL && imOP->GetCellData()->GetScalars() != NULL )
  {
    for (int i=0;i<imITK->GetNumberOfCells();i++)
    {
      CPPUNIT_ASSERT( imITK->GetCellData()->GetScalars()->GetTuple1(i) == imOP->GetCellData()->GetScalars()->GetTuple1(i) );
    }
  }
  else
  {
    CPPUNIT_ASSERT( false );//There aren't any scalars
  }
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThresholdTest::TestAlgorithm()
//----------------------------------------------------------------------------
{
  //Read the data of input and use it as input of the operation
  vtkMAFSmartPointer<vtkDataSetReader> r;
  mafString fileNameIn = MED_DATA_ROOT;
  fileNameIn<<"/VTK_Volumes/volume.vtk";
  r->SetFileName(fileNameIn.GetCStr());
  r->Update();

  vtkMAFSmartPointer<vtkImageCast> imageToFloat;
  imageToFloat->SetInput(vtkImageData::SafeDownCast(r->GetOutput()));
  imageToFloat->SetOutputScalarTypeToFloat();
  imageToFloat->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(vtkImageData::SafeDownCast(imageToFloat->GetOutput()),0.0);
  volume->Update();

  int seed[3] = {51,18,38};
  medOpSegmentationRegionGrowingConnectedThreshold *op = new medOpSegmentationRegionGrowingConnectedThreshold();
  op->TestModeOn();
  op->SetInput(volume);
  op->SetLowerThreshold(30000);
  op->SetUpperThreshold(34000);
  op->SetSeed(seed);
  op->Algorithm();
  mafVMEVolumeGray *volumeOperationOutput = mafVMEVolumeGray::SafeDownCast(op->GetOutput());
  volumeOperationOutput->GetOutput()->Update();
  volumeOperationOutput->Update();

  //Read the data of input and use it as input of the itk filter
  RealReaderType::Pointer reader = RealReaderType::New();

  reader->SetFileName( fileNameIn.GetCStr() );
  try
  {
    reader->Update();
  }
  catch ( itk::ExceptionObject &err)
  {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    CPPUNIT_ASSERT( false );
  }


  typedef itk::ConnectedThresholdImageFilter<RealImage, RealImage> ITKConnectedThresholdFilter;
  ITKConnectedThresholdFilter::Pointer connectedThreshold = ITKConnectedThresholdFilter::New();

  connectedThreshold->SetLower(30000);
  connectedThreshold->SetUpper(34000);
  connectedThreshold->SetReplaceValue(255);

  RealImage::IndexType seedITK;
  for (int i=0;i<ITK_IMAGE_DIMENSION;i++)
  {
    seedITK[i] = seed[i];
  }
  connectedThreshold->AddSeed(seedITK);

  connectedThreshold->SetInput( ((RealImage*)reader->GetOutput()) );

  try
  {
    connectedThreshold->Update();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl;

    CPPUNIT_ASSERT( false );
  }

  //Write the result of the itk filter
  RealWriterType::Pointer writer = RealWriterType::New();
  mafString fileNameOut = MED_DATA_ROOT;
  fileNameOut<<"/VTK_Volumes/Segmentation.vtk";
  writer->SetFileName( fileNameOut.GetCStr() );
  writer->SetInput(connectedThreshold->GetOutput());

  try
  {
    writer->Write();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl;

    CPPUNIT_ASSERT( false );
  }

  r->SetFileName(fileNameOut.GetCStr());
  r->Update();

  //Compare the two results
  vtkMAFSmartPointer<vtkImageData> imITK;
  imITK->DeepCopy(vtkImageData::SafeDownCast(r->GetOutput()));
  imITK->Update();
  vtkMAFSmartPointer<vtkImageData> imOP;
  imOP->DeepCopy(vtkImageData::SafeDownCast(volumeOperationOutput->GetOutput()->GetVTKData()));
  imOP->Update();

  CompareImageData(imITK,imOP);

  mafDEL(op);

}
