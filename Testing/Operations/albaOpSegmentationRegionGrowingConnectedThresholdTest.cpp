/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationRegionGrowingConnectedThresholdTest
 Authors: Matteo Giacomoni, Di Cosmo Grazia, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaOpSegmentationRegionGrowingConnectedThresholdTest.h"

#include "albaOpSegmentationRegionGrowingConnectedThreshold.h"
#include "albaVMEVolumeGray.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSetReader.h"
#include "vtkImageData.h"
#include "vtkDataSetWriter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"
#include "albaOpImporterVTK.h"
#include "albastring.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"


//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestStaticAllocation()
{
  albaOpSegmentationRegionGrowingConnectedThreshold op;
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestDynamicAllocation()
{
  albaOpSegmentationRegionGrowingConnectedThreshold *op = new albaOpSegmentationRegionGrowingConnectedThreshold();
  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestAccept()
{

  albaOpSegmentationRegionGrowingConnectedThreshold *op = new albaOpSegmentationRegionGrowingConnectedThreshold();

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::CompareImageData(vtkImageData *imFile,vtkImageData *imOP)
{
  CPPUNIT_ASSERT( imFile->GetNumberOfPoints() == imOP->GetNumberOfPoints() );
  CPPUNIT_ASSERT( imFile->GetNumberOfCells() == imOP->GetNumberOfCells() );

	bool sameTuple=true;

  if ( imFile->GetPointData()->GetScalars() != NULL && imOP->GetPointData()->GetScalars() != NULL )
  {
    for (int i=0;i<imFile->GetNumberOfPoints();i++)
    {
      if( imFile->GetPointData()->GetScalars()->GetTuple1(i) != imOP->GetPointData()->GetScalars()->GetTuple1(i) )
				sameTuple=false;
    }
  }
  else if ( imFile->GetCellData()->GetScalars() != NULL && imOP->GetCellData()->GetScalars() != NULL )
  {
    for (int i=0;i<imFile->GetNumberOfCells();i++)
    {
      if (imFile->GetCellData()->GetScalars()->GetTuple1(i) != imOP->GetCellData()->GetScalars()->GetTuple1(i) )
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
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestAlgorithmID()
//----------------------------------------------------------------------------
{
	int seed[3] = { 51,18,38 };
	double threshold[2] = { 30000,34000 };
	TestAlgorithm("volume.vtk", "ConnectedThresholdID.vtk", seed, threshold);
}

//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestAlgorithmRG()
{
	int seed[3] = { 106,74,5 };
  double threshold[2] = { 1000,1800 };
  TestAlgorithm("LabeledVolumeTest.vtk", "ConnectedThresholdRG.vtk", seed, threshold);
}

//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThresholdTest::TestAlgorithm(char *inputVol, char *compareVol, int seed[3], double threshold[2])
{
	//import the data of input and use it as input of the operation
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	//Read the data of input and use it as input of the operation
	albaOpImporterVTK *importerVTK = new albaOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
	importerVTK->SetInput(storage->GetRoot());

	albaString absPathFilename = ALBA_DATA_ROOT;
	absPathFilename << "/VTK_Volumes/";
	absPathFilename.Append(inputVol);
	importerVTK->SetFileName(absPathFilename);
	importerVTK->OpRun();

	albaVMEVolumeGray *inputVolume = albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	inputVolume->ReparentTo(storage->GetRoot());
	inputVolume->Update();
	inputVolume->GetOutput()->Update();
	int k = inputVolume->GetOutput()->GetVTKData()->GetNumberOfPoints();


	CPPUNIT_ASSERT(inputVolume != NULL);

	albaOpSegmentationRegionGrowingConnectedThreshold *op = new albaOpSegmentationRegionGrowingConnectedThreshold();
	op->TestModeOn();
	op->SetInput(inputVolume);
	op->SetLowerThreshold(threshold[0]);
	op->SetUpperThreshold(threshold[1]);
	op->SetSeed(seed);
	op->Algorithm();
	albaVMEVolumeGray *volumeOperationOutput = albaVMEVolumeGray::SafeDownCast(op->GetOutputVolume());
	volumeOperationOutput->GetOutput()->Update();
	volumeOperationOutput->Update();

	//read the result expected
	vtkALBASmartPointer<vtkDataSetReader> outputRead;
	albaString fileNameOut = ALBA_DATA_ROOT;
	fileNameOut << "/VTK_Volumes/" << compareVol;
	outputRead->SetFileName(fileNameOut.GetCStr());
	outputRead->Update();

	//Compare the two results
	vtkALBASmartPointer<vtkImageData> imFile;
	imFile->DeepCopy(vtkImageData::SafeDownCast(outputRead->GetOutput()));
	vtkALBASmartPointer<vtkImageData> imOP;
	imOP->DeepCopy(vtkImageData::SafeDownCast(volumeOperationOutput->GetOutput()->GetVTKData()));

	CompareImageData(imFile, imOP);

	albaDEL(op);
}
