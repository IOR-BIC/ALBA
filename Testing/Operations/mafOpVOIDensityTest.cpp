/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpVOIDensityTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 12:01:16 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpVOIDensityTest.h"
#include "mafOpVOIDensity.h"

#include "mafString.h"
#include "mafOpImporterSTL.h"
#include "mafOpImporterVTK.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafSmartPointer.h"

#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"

void mafOpVOIDensityTest::Test()
{
	//Initialize Storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	//Import surface
	mafOpImporterSTL *importerSTL=new mafOpImporterSTL("importerSTL");
	importerSTL->TestModeOn();
  importerSTL->SetInput(storage->GetRoot());
	mafString filenameSTL=MAF_DATA_ROOT;
  filenameSTL<<"/Test_VOI/Cubo.stl";
	importerSTL->SetFileName(filenameSTL.GetCStr());
	importerSTL->OpRun();
  std::vector<mafVMESurface*> STLs;
  importerSTL->GetImportedSTL(STLs);
  mafVMESurface *Surface = STLs[0];
	Surface->Update();
	//Import volume
	mafOpImporterVTK *importerVTK = new mafOpImporterVTK("importerVTK");
	importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());
	mafString filenameVTK = MAF_DATA_ROOT;
  filenameVTK << "/Test_VOI/Volume.vtk";
	importerVTK->SetFileName(filenameVTK.GetCStr());
	importerVTK->OpRun();
	mafVMEVolumeGray *Volume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
	Volume->Update();
	vtkDataSet *VolumeData = ((mafVME*)Volume)->GetOutput()->GetVTKData();
	VolumeData->Update();
  //Create operation VOI Density and initialize it
	mafOpVOIDensity *VOIDensity=new mafOpVOIDensity("VOI Density");
	VOIDensity->TestModeOn();
	VOIDensity->SetInput(Volume);
	VOIDensity->SetSurface(Surface);
	VOIDensity->OpRun();
	//Start VOI Density operation
	VOIDensity->ExtractVolumeScalars();
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
	mafDEL(storage);
}
