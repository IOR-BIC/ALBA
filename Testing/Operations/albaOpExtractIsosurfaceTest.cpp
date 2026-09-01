/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtractIsosurfaceTest
 Authors: Roberto Mucci
 
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
#include "albaOpExtractIsosurfaceTest.h"

#include "albaOpExtractIsosurface.h"
#include "vtkRectilinearGridReader.h"
#include "vtkStructuredPointsReader.h"
#include "albaVMESurface.h"
#include "vtkDataSet.h"
#include "albaString.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEGenericAbstract.h"
#include "vtkALBASmartPointer.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaOpExtractIsosurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpExtractIsosurface *extract = new albaOpExtractIsosurface();
  albaDEL(extract);
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurfaceTest::TestExtractRG()
//----------------------------------------------------------------------------
{
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/CropTestVolumeRG.vtk";
  vtkRectilinearGridReader *preader = vtkRectilinearGridReader::New();
  preader->SetFileName(filename);
  preader->Update();

  vtkRectilinearGrid *data = preader->GetOutput();

  albaSmartPointer<albaVMEVolumeGray> Input;
  Input->SetDataByDetaching(data,0);
  Input->Update();

  //clean on, triangulate on
  albaOpExtractIsosurface *extract = new albaOpExtractIsosurface();
  extract->TestModeOn();
  extract->SetInput(Input);
  extract->CreateVolumePipeline();
  extract->SetIsoValue(300);
  extract->UpdateSurface();
  extract->ExtractSurface();

  albaVMESurface *output = albaVMESurface::SafeDownCast(extract->GetOutput());
  albaString number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("10336") == 0;

  TEST_RESULT;
  albaDEL(output);
  cppDEL(extract);

  albaOpExtractIsosurface *extract1 = new albaOpExtractIsosurface();
  //clean on, triangulate off
  extract1->TestModeOn();
  extract1->SetInput(Input);
  extract1->SetTriangulate(false);
  extract1->CreateVolumePipeline();
  extract1->SetIsoValue(300);
  extract1->UpdateSurface();
  extract1->ExtractSurface();

  output = albaVMESurface::SafeDownCast(extract1->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("10336") == 0;

  TEST_RESULT;
  albaDEL(output);
  cppDEL(extract1);

  albaOpExtractIsosurface *extract2 = new albaOpExtractIsosurface();
  //clean off, triangulate off
  extract2->TestModeOn();
  extract2->SetInput(Input);
  extract2->SetTriangulate(false);
  extract2->SetClean(false);
  extract2->CreateVolumePipeline();
  extract2->SetIsoValue(300);
  extract2->UpdateSurface();
  extract2->ExtractSurface();

  output = albaVMESurface::SafeDownCast(extract2->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("10336") == 0;

  TEST_RESULT;
  cppDEL(extract2);
  albaDEL(output);
  albaDEL(preader);
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurfaceTest::TestExtractSP()
//----------------------------------------------------------------------------
{
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/CropTestVolumeSP.vtk";
  vtkStructuredPointsReader *preader = vtkStructuredPointsReader::New();
  preader->SetFileName(filename);
  preader->Update();

  vtkImageData *data = preader->GetOutput();

  albaSmartPointer<albaVMEVolumeGray> Input;
  Input->SetDataByDetaching(data,0);
  Input->Update();

  //clean on, triangulate on
  albaOpExtractIsosurface *extract = new albaOpExtractIsosurface();
  extract->TestModeOn();
  extract->SetInput(Input);
	extract->SetConnectivity(false);
  extract->CreateVolumePipeline();
  extract->SetIsoValue(200);
  extract->UpdateSurface();

  extract->ExtractSurface();

  albaVMESurface *output = albaVMESurface::SafeDownCast(extract->GetOutput());
  albaString number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("656") == 0;

  TEST_RESULT;
  albaDEL(output);
  cppDEL(extract);

  albaOpExtractIsosurface *extract1 = new albaOpExtractIsosurface();
  //clean on, triangulate off
  extract1->TestModeOn();
  extract1->SetInput(Input);
  extract1->SetTriangulate(false);
	extract1->SetConnectivity(false);
  extract1->CreateVolumePipeline();
  extract1->SetIsoValue(200);
  extract1->UpdateSurface();
  extract1->ExtractSurface();

  output = albaVMESurface::SafeDownCast(extract1->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("656") == 0;

  TEST_RESULT;
  albaDEL(output);
  cppDEL(extract1);

  albaOpExtractIsosurface *extract2 = new albaOpExtractIsosurface();
  //clean off, triangulate off
  extract2->TestModeOn();
  extract2->SetInput(Input);
  extract2->SetTriangulate(false);
	extract2->SetConnectivity(false);
  extract2->SetClean(false);
  extract2->CreateVolumePipeline();
  extract2->SetIsoValue(200);
  extract2->UpdateSurface();
  extract2->ExtractSurface();

  output = albaVMESurface::SafeDownCast(extract2->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("672") == 0;

  TEST_RESULT;
  cppDEL(extract2);
  albaDEL(output);
  albaDEL(preader);
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurfaceTest::TestExtractConnectivity()
{
	albaString filename = ALBA_DATA_ROOT;
	filename << "/VTK_Volumes/CropTestVolumeSP.vtk";
	vtkStructuredPointsReader *preader = vtkStructuredPointsReader::New();
	preader->SetFileName(filename);
	preader->Update();

	vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutput());

	albaSmartPointer<albaVMEVolumeGray> Input;
	Input->SetDataByDetaching(data, 0);
	Input->Update();

	//clean on, triangulate on
	albaOpExtractIsosurface *extract = new albaOpExtractIsosurface();
	extract->TestModeOn();
	extract->SetInput(Input);
	extract->SetConnectivity(true);
	extract->CreateVolumePipeline();
	extract->SetIsoValue(200);
	extract->UpdateSurface();

	extract->ExtractSurface();

	albaVMESurface *output = albaVMESurface::SafeDownCast(extract->GetOutput());
	albaString number = output->GetSurfaceOutput()->GetNumberOfTriangles();

	result = number.Compare("64") == 0;

	TEST_RESULT;
	albaDEL(output);
	cppDEL(extract);

	albaOpExtractIsosurface *extract1 = new albaOpExtractIsosurface();
	//clean on, triangulate off
	extract1->TestModeOn();
	extract1->SetInput(Input);
	extract1->SetTriangulate(false);
	extract1->SetConnectivity(true);
	extract1->CreateVolumePipeline();
	extract1->SetIsoValue(200);
	extract1->UpdateSurface();
	extract1->ExtractSurface();

	output = albaVMESurface::SafeDownCast(extract1->GetOutput());
	number = output->GetSurfaceOutput()->GetNumberOfTriangles();

	result = number.Compare("64") == 0;

	TEST_RESULT;
	albaDEL(output);
	cppDEL(extract1);

	albaOpExtractIsosurface *extract2 = new albaOpExtractIsosurface();
	//clean off, triangulate off
	extract2->TestModeOn();
	extract2->SetInput(Input);
	extract2->SetTriangulate(false);
	extract2->SetConnectivity(true);
	extract2->SetClean(false);
	extract2->CreateVolumePipeline();
	extract2->SetIsoValue(200);
	extract2->UpdateSurface();
	extract2->ExtractSurface();

	output = albaVMESurface::SafeDownCast(extract2->GetOutput());
	number = output->GetSurfaceOutput()->GetNumberOfTriangles();

	result = number.Compare("64") == 0;

	TEST_RESULT;
	cppDEL(extract2);
	albaDEL(output);
	albaDEL(preader);
}
