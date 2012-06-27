/*=========================================================================

 Program: MAF2
 Module: mafOpExtractIsosurfaceTest
 Authors: Roberto Mucci
 
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
#include "mafOpExtractIsosurfaceTest.h"

#include "mafOpExtractIsosurface.h"
#include "vtkRectilinearGridReader.h"
#include "vtkStructuredPointsReader.h"
#include "mafVMESurface.h"
#include "vtkDataSet.h"
#include "mafString.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEGenericAbstract.h"
#include "vtkMAFSmartPointer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafOpExtractIsosurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExtractIsosurfaceTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExtractIsosurfaceTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExtractIsosurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpExtractIsosurface *extract = new mafOpExtractIsosurface();
  mafDEL(extract);
}

//----------------------------------------------------------------------------
void mafOpExtractIsosurfaceTest::TestExtractRG()
//----------------------------------------------------------------------------
{
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/CropTestVolumeRG.vtk";
  vtkRectilinearGridReader *preader = vtkRectilinearGridReader::New();
  preader->SetFileName(filename);
  preader->Update();

  vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);

  mafSmartPointer<mafVMEVolumeGray> Input;
  Input->SetDataByDetaching(data,0);
  Input->Update();
  Input->GetOutput()->GetVTKData()->Update();

  //clean on, triangulate on
  mafOpExtractIsosurface *extract = new mafOpExtractIsosurface();
  extract->TestModeOn();
  extract->SetInput(Input);
  extract->CreateVolumePipeline();
  extract->SetIsoValue(300);
  extract->UpdateSurface();
  extract->ExtractSurface();

  mafVMESurface *output = mafVMESurface::SafeDownCast(extract->GetOutput());
  mafString number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("7018") == 0;

  TEST_RESULT;
  mafDEL(output);
  cppDEL(extract);

  mafOpExtractIsosurface *extract1 = new mafOpExtractIsosurface();
  //clean on, triangulate off
  extract1->TestModeOn();
  extract1->SetInput(Input);
  extract1->SetTriangulate(false);
  extract1->CreateVolumePipeline();
  extract1->SetIsoValue(300);
  extract1->UpdateSurface();
  extract1->ExtractSurface();

  output = mafVMESurface::SafeDownCast(extract1->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("7018") == 0;

  TEST_RESULT;
  mafDEL(output);
  cppDEL(extract1);

  mafOpExtractIsosurface *extract2 = new mafOpExtractIsosurface();
  //clean off, triangulate off
  extract2->TestModeOn();
  extract2->SetInput(Input);
  extract2->SetTriangulate(false);
  extract2->SetClean(false);
  extract2->CreateVolumePipeline();
  extract2->SetIsoValue(300);
  extract2->UpdateSurface();
  extract2->ExtractSurface();

  output = mafVMESurface::SafeDownCast(extract2->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("7018") == 0;

  TEST_RESULT;
  cppDEL(extract2);
  mafDEL(output);
  mafDEL(preader);
}

//----------------------------------------------------------------------------
void mafOpExtractIsosurfaceTest::TestExtractSP()
//----------------------------------------------------------------------------
{
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/CropTestVolumeSP.vtk";
  vtkStructuredPointsReader *preader = vtkStructuredPointsReader::New();
  preader->SetFileName(filename);
  preader->Update();

  vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);

  mafSmartPointer<mafVMEVolumeGray> Input;
  Input->SetDataByDetaching(data,0);
  Input->Update();
  Input->GetOutput()->GetVTKData()->Update();

  //clean on, triangulate on
  mafOpExtractIsosurface *extract = new mafOpExtractIsosurface();
  extract->TestModeOn();
  extract->SetInput(Input);
  extract->CreateVolumePipeline();
  extract->SetIsoValue(200);
  extract->UpdateSurface();

  extract->ExtractSurface();

  mafVMESurface *output = mafVMESurface::SafeDownCast(extract->GetOutput());
  mafString number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("656") == 0;

  TEST_RESULT;
  mafDEL(output);
  cppDEL(extract);

  mafOpExtractIsosurface *extract1 = new mafOpExtractIsosurface();
  //clean on, triangulate off
  extract1->TestModeOn();
  extract1->SetInput(Input);
  extract1->SetTriangulate(false);
  extract1->CreateVolumePipeline();
  extract1->SetIsoValue(200);
  extract1->UpdateSurface();
  extract1->ExtractSurface();

  output = mafVMESurface::SafeDownCast(extract1->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("656") == 0;

  TEST_RESULT;
  mafDEL(output);
  cppDEL(extract1);

  mafOpExtractIsosurface *extract2 = new mafOpExtractIsosurface();
  //clean off, triangulate off
  extract2->TestModeOn();
  extract2->SetInput(Input);
  extract2->SetTriangulate(false);
  extract2->SetClean(false);
  extract2->CreateVolumePipeline();
  extract2->SetIsoValue(200);
  extract2->UpdateSurface();
  extract2->ExtractSurface();

  output = mafVMESurface::SafeDownCast(extract2->GetOutput());
  number = output->GetSurfaceOutput()->GetNumberOfTriangles();

  result = number.Compare("656") == 0;

  TEST_RESULT;
  cppDEL(extract2);
  mafDEL(output);
  mafDEL(preader);
}