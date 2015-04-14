/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVTKTest
 Authors: Matteo Giacomoni
 
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
#include "mafOperationsTests.h"

#include "mafOpImporterVTKTest.h"

#include "mafString.h"
#include "mafVME.h"
#include "mafOpImporterVTK.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;

  m_ImporterVTK = new mafOpImporterVTK();
  m_ImporterVTK->TestModeOn();
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImporterVTK);
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();
  
  result = m_ImporterVTK->Accept(dummyVme);
  TEST_RESULT;

  delete dummyVme;
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKPolyline()
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();

  result = m_ImporterVTK->Accept(dummyVme);
  TEST_RESULT;

  m_ImporterVTK->SetInput(dummyVme);
  mafString Filename=MAF_DATA_ROOT;
  Filename<<"/Test_ImporterVTK/Polyline.vtk";
  m_ImporterVTK->SetFileName(Filename);
  m_ImporterVTK->ImportVTK();

  result = dummyVme->GetChild(0)->IsA("mafVMEPolyline");
  TEST_RESULT;

  delete dummyVme;
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKSurface()
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();

  result = m_ImporterVTK->Accept(dummyVme);
  TEST_RESULT;

  m_ImporterVTK->SetInput(dummyVme);
  mafString Filename=MAF_DATA_ROOT;
  Filename<<"/Test_ImporterVTK/Surface.vtk";
  m_ImporterVTK->SetFileName(Filename);
  m_ImporterVTK->ImportVTK();

  result = dummyVme->GetChild(0)->IsA("mafVMESurface");
  TEST_RESULT;

  delete dummyVme;
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKVolume()
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();

  result = m_ImporterVTK->Accept(dummyVme);
  TEST_RESULT;

  m_ImporterVTK->SetInput(dummyVme);
  mafString Filename=MAF_DATA_ROOT;
  Filename<<"/Test_ImporterVTK/Volume.vtk";
  m_ImporterVTK->SetFileName(Filename);
  m_ImporterVTK->ImportVTK();

  result = dummyVme->GetChild(0)->IsA("mafVMEVolume");
  TEST_RESULT;

  delete dummyVme;
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKMesh()
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();

  result = m_ImporterVTK->Accept(dummyVme);
  TEST_RESULT;

  m_ImporterVTK->SetInput(dummyVme);
  mafString Filename=MAF_DATA_ROOT;
  Filename<<"/Test_ImporterVTK/Mesh.vtk";
  m_ImporterVTK->SetFileName(Filename);
  m_ImporterVTK->ImportVTK();

  result = dummyVme->GetChild(0)->IsA("mafVMEMesh");
  TEST_RESULT;

  delete dummyVme;
  delete wxLog::SetActiveTarget(NULL);
}
