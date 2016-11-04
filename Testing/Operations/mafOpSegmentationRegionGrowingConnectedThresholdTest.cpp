/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentationRegionGrowingConnectedThresholdTest
 Authors: Matteo Giacomoni, Di Cosmo Grazia
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpSegmentationRegionGrowingConnectedThresholdTest.h"

#include "mafOpSegmentationRegionGrowingConnectedThreshold.h"
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
#include "vtkRectilinearGrid.h"
#include "medOpImporterVTK.h"
#include "mafstring.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"


#define ITK_IMAGE_DIMENSION 3
typedef itk::Image< float, ITK_IMAGE_DIMENSION > RealImage;
typedef itk::ImageFileReader< RealImage > RealReaderType;
typedef itk::ImageFileWriter< RealImage > RealWriterType;

//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpSegmentationRegionGrowingConnectedThreshold op;
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpSegmentationRegionGrowingConnectedThreshold *op = new mafOpSegmentationRegionGrowingConnectedThreshold();
  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::TestAccept()
//----------------------------------------------------------------------------
{

  mafOpSegmentationRegionGrowingConnectedThreshold *op = new mafOpSegmentationRegionGrowingConnectedThreshold();

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::CompareImageData(vtkImageData *imITK,vtkImageData *imOP)
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT( imITK->GetNumberOfPoints() == imOP->GetNumberOfPoints() );
  CPPUNIT_ASSERT( imITK->GetNumberOfCells() == imOP->GetNumberOfCells() );

	bool sameTuple=true;

  if ( imITK->GetPointData()->GetScalars() != NULL && imOP->GetPointData()->GetScalars() != NULL )
  {
    for (int i=0;i<imITK->GetNumberOfPoints();i++)
    {
      if( imITK->GetPointData()->GetScalars()->GetTuple1(i) != imOP->GetPointData()->GetScalars()->GetTuple1(i) )
				sameTuple=false;
    }
  }
  else if ( imITK->GetCellData()->GetScalars() != NULL && imOP->GetCellData()->GetScalars() != NULL )
  {
    for (int i=0;i<imITK->GetNumberOfCells();i++)
    {
      if (imITK->GetCellData()->GetScalars()->GetTuple1(i) != imOP->GetCellData()->GetScalars()->GetTuple1(i) )
				sameTuple=false;
    }
  }
  else
  {
    sameTuple = false;//There aren't any scalars
  }

	CPPUNIT_ASSERT(sameTuple);
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::TestAlgorithm()
//----------------------------------------------------------------------------
{
  //Read the data of input and use it as input of the operation
  vtkMAFSmartPointer<vtkDataSetReader> r;
  mafString fileNameIn = MAF_DATA_ROOT;
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
  mafOpSegmentationRegionGrowingConnectedThreshold *op = new mafOpSegmentationRegionGrowingConnectedThreshold();
  op->TestModeOn();
  op->SetInput(volume);
  op->SetLowerThreshold(30000);
  op->SetUpperThreshold(34000);
  op->SetSeed(seed);
  op->Algorithm();
  mafVMEVolumeGray *volumeOperationOutput = mafVMEVolumeGray::SafeDownCast(op->GetOutputVolume());
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
	mafString fileNameOut = GET_TEST_DATA_DIR();
  fileNameOut<<"/Segmentation.vtk";
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
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingConnectedThresholdTest::TestAlgorithmRG()
//----------------------------------------------------------------------------
{
  //import the data of input and use it as input of the operation
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  medOpImporterVTK *importerVTK=new medOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());

  mafString absPathFilename=MAF_DATA_ROOT;
  absPathFilename<<"/VTK_Volumes/";
  absPathFilename.Append("LabeledVolumeTest.vtk");
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  mafVMEVolumeGray *inputVolume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
  inputVolume->ReparentTo(storage->GetRoot());
  inputVolume->Update();
  inputVolume->GetOutput()->Update();
  inputVolume->GetOutput()->GetVTKData()->Update();
  int k=inputVolume->GetOutput()->GetVTKData()->GetNumberOfPoints();


  CPPUNIT_ASSERT(inputVolume!=NULL);
  
  int seed[3] = {106,74,5};
  mafOpSegmentationRegionGrowingConnectedThreshold *op = new mafOpSegmentationRegionGrowingConnectedThreshold();
  op->TestModeOn();
  op->SetInput(inputVolume);
  op->SetLowerThreshold(1000);
  op->SetUpperThreshold(1800);
  op->SetSeed(seed);
  op->Algorithm();
  mafVMEVolumeGray *volumeOperationOutput = mafVMEVolumeGray::SafeDownCast(op->GetOutputVolume());
  volumeOperationOutput->GetOutput()->Update();
  volumeOperationOutput->Update();

  //read the result expected
  vtkMAFSmartPointer<vtkDataSetReader> outputRead;
  mafString fileNameOut = MAF_DATA_ROOT;
  fileNameOut<<"/VTK_Volumes/ConnectedThreshold.vtk";
  outputRead->SetFileName(fileNameOut.GetCStr());
  outputRead->Update();
 
  //Compare the two results
  vtkMAFSmartPointer<vtkImageData> imFile;
  imFile->DeepCopy(vtkImageData::SafeDownCast(outputRead->GetOutput()));
  imFile->Update();
  vtkMAFSmartPointer<vtkImageData> imOP;
  imOP->DeepCopy(vtkImageData::SafeDownCast(volumeOperationOutput->GetOutput()->GetVTKData()));
  imOP->Update();

  CompareImageData(imFile,imOP);

  mafDEL(op);
}