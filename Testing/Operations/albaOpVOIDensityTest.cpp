/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensityTest
 Authors: Matteo Giacomoni
 
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
#include "albaOpVOIDensityTest.h"
#include "albaOpVOIDensity.h"

#include "albaString.h"
#include "albaOpImporterSTL.h"
#include "albaOpImporterVTK.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaSmartPointer.h"

void albaOpVOIDensityTest::Test()
{
	//Initialize Storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import surface
	albaOpImporterSTL *importerSTL=new albaOpImporterSTL("importerSTL");
	importerSTL->TestModeOn();
  importerSTL->SetInput(storage->GetRoot());
	albaString filenameSTL=ALBA_DATA_ROOT;
  filenameSTL<<"/Test_VOI/Cubo.stl";
	importerSTL->SetFileName(filenameSTL.GetCStr());
	importerSTL->OpRun();
  std::vector<albaVMESurface*> STLs;
  importerSTL->GetImportedSTL(STLs);
  albaVMESurface *Surface = STLs[0];
	Surface->Update();
	//Import volume
	albaOpImporterVTK *importerVTK = new albaOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	albaString filenameVTK = ALBA_DATA_ROOT;
  filenameVTK << "/Test_VOI/Volume.vtk";
	importerVTK->SetFileName(filenameVTK.GetCStr());
	importerVTK->OpRun();
	albaVMEVolumeGray *Volume = albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	Volume->Update();
	vtkDataSet *VolumeData = Volume->GetOutput()->GetVTKData();
	VolumeData->Update();
  //Create operation VOI Density and initialize it
	albaOpVOIDensity *VOIDensity=new albaOpVOIDensity("VOI Density");
	VOIDensity->TestModeOn();
	VOIDensity->SetInput(Volume);
	VOIDensity->SetSurface(Surface);
	VOIDensity->OpRun();
	//Start VOI Density operation
	VOIDensity->EvaluateSurface();
	//Check output value
	double MinScalar=VOIDensity->GetMinScalar();
	CPPUNIT_ASSERT(MinScalar==6.0);
	double MaxScalar=VOIDensity->GetMaxScalar();
	CPPUNIT_ASSERT(MaxScalar==7.0);
	double MeanScalar=VOIDensity->GetMeanScalar();
	CPPUNIT_ASSERT(MeanScalar==6.5);
	int NumberScalars=VOIDensity->GetNumberScalars();
	CPPUNIT_ASSERT(NumberScalars==4);
	double StandardDeviationTest = sqrt(1/4.0);
	double StandardDeviation=VOIDensity->GetStandardDeviation();
	CPPUNIT_ASSERT(StandardDeviationTest==StandardDeviation);

	Volume = NULL;
	VolumeData = NULL;
	Surface = NULL;
	cppDEL(VOIDensity);
	cppDEL(importerSTL);
	cppDEL(importerVTK);
	albaDEL(storage);
}

void albaOpVOIDensityTest::TestNotAlignedVolume()
{
	//Initialize Storage
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	//Import surface
	albaOpImporterSTL *importerSTL = new albaOpImporterSTL("importerSTL");
	importerSTL->TestModeOn();
	importerSTL->SetInput(storage->GetRoot());
	albaString filenameSTL = ALBA_DATA_ROOT;
	filenameSTL << "/Test_VOI/Cubo.stl";
	importerSTL->SetFileName(filenameSTL.GetCStr());
	importerSTL->OpRun();
	std::vector<albaVMESurface*> STLs;
	importerSTL->GetImportedSTL(STLs);
	albaVMESurface *Surface = STLs[0];

	Surface->Update();
	//Import volume
	albaOpImporterVTK *importerVTK = new albaOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
	importerVTK->SetInput(storage->GetRoot());
	albaString filenameVTK = ALBA_DATA_ROOT;
	filenameVTK << "/Test_VOI/Volume.vtk";
	importerVTK->SetFileName(filenameVTK.GetCStr());
	importerVTK->OpRun();
	albaVMEVolumeGray *Volume = albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	Volume->Update();

	albaMatrix volumeMatrix = Volume->GetAbsMatrixPipe()->GetMatrix();

	albaMatrix rotateMatrix;
	rotateMatrix.Identity();
	rotateMatrix.SetElement(0, 0, 0.98);
	rotateMatrix.SetElement(0, 1, -0.09);
	rotateMatrix.SetElement(0, 2, 0.12);
	rotateMatrix.SetElement(1, 0, 0.11);
	rotateMatrix.SetElement(1, 1, 0.97);
	rotateMatrix.SetElement(1, 2, -0.17);
	rotateMatrix.SetElement(2, 0, -0.1);
	rotateMatrix.SetElement(2, 1, 0.18);
	rotateMatrix.SetElement(2, 2, 0.97);

	Volume->SetAbsMatrix(rotateMatrix);

	Volume->Update();
	vtkDataSet *VolumeData = Volume->GetOutput()->GetVTKData();
	VolumeData->Update();

	//Create operation VOI Density and initialize it
	albaOpVOIDensity *VOIDensity = new albaOpVOIDensity("VOI Density");
	VOIDensity->TestModeOn();
	VOIDensity->SetInput(Volume);
	VOIDensity->SetSurface(Surface);
	VOIDensity->OpRun();

	//Start VOI Density operation
	VOIDensity->EvaluateSurface();

	//Check output value
	double MinScalar = VOIDensity->GetMinScalar();
	CPPUNIT_ASSERT(MinScalar == 6.0);
	double MaxScalar = VOIDensity->GetMaxScalar();
	CPPUNIT_ASSERT(MaxScalar == 8.0);
	double MeanScalar = VOIDensity->GetMeanScalar();
	CPPUNIT_ASSERT(MeanScalar == 6.75);
	int NumberScalars = VOIDensity->GetNumberScalars();
	CPPUNIT_ASSERT(NumberScalars == 8);
	double StandardDeviationTest = sqrt(3.5 / 8.0);
	double StandardDeviation = VOIDensity->GetStandardDeviation();
	CPPUNIT_ASSERT(StandardDeviationTest == StandardDeviation);

	Volume = NULL;
	VolumeData = NULL;
	Surface = NULL;

	cppDEL(VOIDensity);
	cppDEL(importerSTL);
	cppDEL(importerVTK);
	albaDEL(storage);
}
