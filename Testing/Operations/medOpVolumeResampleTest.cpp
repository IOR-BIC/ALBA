/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpVolumeResampleTest.cpp,v $
Language:  C++
Date:      $Date: 2009-02-10 19:30:49 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
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

#include <cppunit/config/SourcePrefix.h>
#include "medOpVolumeResampleTest.h"

#include "medOpVolumeResample.h"
// log facilities
typedef medOpVolumeResample movr;

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "medOpImporterVTK.h"

#include "mafSmartPointer.h"
#include "mafAbsMatrixPipe.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkDataSetWriter.h"
#include "vtkTransform.h"
#include "vtkDataArray.h"

using namespace std;

bool DEBUG_MODE = true;


void medOpVolumeResampleTest::TestSetBounds()
{
  const char *inFileName = "volumeRG_dim_10_10_10_bounds_m5_5_m5_5_m5_5.vtk";

  const char *outVTKFileName = "resampled_volumeRG_dim_10_10_10_bounds_m5_5_m5_5_m5_5.vtk";

  TestResampleInternal(inFileName,  outVTKFileName );
  
}

void medOpVolumeResampleTest::TestResample()
{
  const char * inFileName= "volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk";

  const char * outVTKFileName= "resampled_volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk";

  TestResampleInternal(inFileName,  outVTKFileName );

}


void medOpVolumeResampleTest::TestResampleInternal( const char *inFileName, const char *outVTKFileName )
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  medOpImporterVTK *importerVTK=new medOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());

  mafString absPathFilename=MED_DATA_ROOT;
  absPathFilename<<"/Test_VolumeResample/";
  absPathFilename.Append(inFileName);
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  mafVMEVolumeGray *inputVolume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
  inputVolume->ReparentTo(storage->GetRoot());
  CPPUNIT_ASSERT(inputVolume!=NULL);

  medOpVolumeResample *opVolumeResample=new medOpVolumeResample("Volume Resample");
  opVolumeResample->SetInput(inputVolume);
  opVolumeResample->TestModeOn();

  vtkDataSet *inputDataSet = inputVolume->GetOutput()->GetVTKData();
  inputDataSet->Update();

  double outputSpacing[3];
  double inputDataOrigin[3];

  vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(inputDataSet);

  outputSpacing[0] = rg->GetXCoordinates()->GetComponent(1,0)-rg->GetXCoordinates()->GetComponent(0,0);
  outputSpacing[1] = rg->GetYCoordinates()->GetComponent(1,0)-rg->GetYCoordinates()->GetComponent(0,0);
  outputSpacing[2] = rg->GetZCoordinates()->GetComponent(1,0)-rg->GetZCoordinates()->GetComponent(0,0);

  movr::PrintDouble3(cout, outputSpacing, "Input RG estimated spacing");

  inputDataOrigin[0] = rg->GetXCoordinates()->GetComponent(0,0);
  inputDataOrigin[1] = rg->GetYCoordinates()->GetComponent(0,0);
  inputDataOrigin[2] = rg->GetZCoordinates()->GetComponent(0,0);

  movr::PrintDouble3(cout, inputDataOrigin, "Input RG origin");

  opVolumeResample->SetSpacing(outputSpacing);
  
  double outputBounds[6];
  inputVolume->GetOutput()->GetVMEBounds(outputBounds);
  opVolumeResample->SetBounds(outputBounds,medOpVolumeResample::CUSTOMBOUNDS);

  opVolumeResample->Resample();

  mafVME *Output = mafVME::SafeDownCast(opVolumeResample->GetOutput());
  Output->GetOutput()->GetVTKData()->Update();
  vtkStructuredPoints *outputVTKData=vtkStructuredPoints::SafeDownCast(Output->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT(outputVTKData!=NULL);

  double checkSpacing[3] = {0,0,0};
  outputVTKData->GetSpacing(checkSpacing);

  CPPUNIT_ASSERT(checkSpacing[0]==outputSpacing[0] && checkSpacing[1]==outputSpacing[1] && checkSpacing[2]==outputSpacing[2]);

  double checkBounds[6];
  outputVTKData->GetBounds(checkBounds);
  
  CPPUNIT_ASSERT(checkBounds[0]==outputBounds[0] && checkBounds[1]==outputBounds[1] && checkBounds[2]==outputBounds[2]);
  CPPUNIT_ASSERT(checkBounds[3]==outputBounds[3] && checkBounds[4]==outputBounds[4] && checkBounds[5]==outputBounds[5]);

  WriteVTKDatasetToFile(outputVTKData, outVTKFileName);

  outputVTKData = NULL;
  Output = NULL;
  inputVolume = NULL;
  mafDEL(opVolumeResample);
  mafDEL(importerVTK);
  mafDEL(storage); 

}

void medOpVolumeResampleTest::WriteVTKDatasetToFile( vtkDataSet * outputVolumeVTKData, const char *outputFilename )
{
  vtkDataSetWriter *writer = vtkDataSetWriter::New();
  writer->SetInput(outputVolumeVTKData);

  string fullPathOutputFilename;
  fullPathOutputFilename.append(MED_DATA_ROOT);
  fullPathOutputFilename.append("/Test_VolumeResample/");
  fullPathOutputFilename.append(outputFilename);
  if (DEBUG_MODE)
  {
    cout << fullPathOutputFilename;
  }

  writer->SetFileName(fullPathOutputFilename.c_str());
  writer->SetFileTypeToASCII();
  writer->Write();

  vtkDEL(writer);
}

void medOpVolumeResampleTest::TestSetGetSpacing()
{
  medOpVolumeResample *opVolumeResample=new medOpVolumeResample("Volume Resample");
  opVolumeResample->TestModeOn();
  
  double spacing[3] = {1,2,3};
  opVolumeResample->SetSpacing(spacing);

  double checkSpacing[3] = {1,2,3};
  opVolumeResample->GetSpacing(checkSpacing);
  
  CPPUNIT_ASSERT(checkSpacing[0] = spacing[0]);
  CPPUNIT_ASSERT(checkSpacing[1] = spacing[1]);
  CPPUNIT_ASSERT(checkSpacing[2] = spacing[2]);

  cppDEL(opVolumeResample);
}

