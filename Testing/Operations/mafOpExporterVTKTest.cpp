/*=========================================================================

 Program: MAF2
 Module: mafOpExporterVTKTest
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

#include "mafOpExporterVTKTest.h"
#include "mafOpExporterVTK.h"

#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEMesh.h"
#include "mafVMEPolyline.h"

#include "vtkSphereSource.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGridReader.h"
#include "vtkUnstructuredGridReader.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::setUp()
//----------------------------------------------------------------------------
{
  m_ExporterVTK = new mafOpExporterVTK("VTK");
	m_ExporterVTK->TestModeOn();
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_ExporterVTK);
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestAccept() 
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();
  result = m_ExporterVTK->Accept((mafNode*)dummyVme);

  CPPUNIT_ASSERT(result);

  delete dummyVme;
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestOpExportPolydata()
//----------------------------------------------------------------------------
{
  vtkSphereSource *sphere;
  vtkNEW(sphere);
  sphere->Update();

  mafVMESurface *surface;
  mafNEW(surface);

  result = surface->SetData((vtkPolyData*)sphere->GetOutput(),0.0) == MAF_OK;
  CPPUNIT_ASSERT(result);
  surface->Update();

  m_ExporterVTK->SetInput(surface);

  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_ExporterVTK/PolydataToCheck.vtk";

  m_ExporterVTK->SetFileName(filename);
  m_ExporterVTK->SaveVTKData();

  vtkPolyDataReader *reader;
  vtkNEW(reader);
  reader->SetFileName(filename);
  reader->Update();

  result = reader->GetOutput() != NULL;
  CPPUNIT_ASSERT(result);

  vtkDEL(reader);
  mafDEL(surface);
  vtkDEL(sphere);
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestOpExportPolydataWithApplyABSMatrix()
//----------------------------------------------------------------------------
{
  vtkSphereSource *sphere;
  vtkNEW(sphere);
  sphere->Update();

  double start_bounds[6];
  sphere->GetOutput()->GetBounds(start_bounds);

  // Create a mafVMESurface
  mafVMESurface *surface;
  mafNEW(surface);
  surface->SetData((vtkPolyData*)sphere->GetOutput(),0.0) == MAF_OK;
  surface->Update();

  // Apply a transformation
  mafMatrix m;
  m.SetElement(0, 3, 2.1);
  m.SetElement(1, 3, 2.1);
  m.SetElement(2, 3, 2.1);
  surface->SetAbsMatrix(m);

  // set the ABSMatrix flat On
  m_ExporterVTK->ApplyABSMatrixOn();
  m_ExporterVTK->SetInput(surface);

  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_ExporterVTK/PolydataToCheckTransformation.vtk";

  m_ExporterVTK->SetFileName(filename);
  m_ExporterVTK->SaveVTKData();

  vtkPolyDataReader *reader;
  vtkNEW(reader);
  reader->SetFileName(filename);
  reader->Update();

  result = reader->GetOutput() != NULL;
  CPPUNIT_ASSERT(result);

  double end_bounds[6];
  reader->GetOutput()->GetBounds(end_bounds);

  result = mafEquals(start_bounds[0], end_bounds[0]) &&
          mafEquals(start_bounds[1], end_bounds[1]) &&
          mafEquals(start_bounds[2], end_bounds[2]) &&
          mafEquals(start_bounds[3], end_bounds[3]) &&
          mafEquals(start_bounds[4], end_bounds[4]) &&
          mafEquals(start_bounds[5], end_bounds[5]);
  
  // check that bounds are not equals.
  CPPUNIT_ASSERT(!result);
  
  vtkDEL(reader);
  mafDEL(surface);
  vtkDEL(sphere);
}

//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestOpExportMesh() 
//----------------------------------------------------------------------------
{
  vtkUnstructuredGridReader *reader;
  vtkNEW(reader);

  mafString filenameIn=MAF_DATA_ROOT;
  filenameIn<<"/Test_ExporterVTK/MeshInput.vtk";
  reader->SetFileName(filenameIn);
  reader->Update();

  mafVMEMesh *mesh;
  mafNEW(mesh);
  result = mesh->SetData(reader->GetOutput(),0.0) == MAF_OK;
  CPPUNIT_ASSERT(result);
  mesh->Update();

  mafString filenameOut=MAF_DATA_ROOT;
  filenameOut<<"/Test_ExporterVTK/MeshToCheck.vtk";
  m_ExporterVTK->SetInput(mesh);
  m_ExporterVTK->SetFileName(filenameOut);
  m_ExporterVTK->SaveVTKData();

  reader->SetFileName(filenameOut);
  reader->Update();

  result = reader->GetOutput() != NULL;
  CPPUNIT_ASSERT(result);

  mafDEL(mesh);
  vtkDEL(reader);
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestOpExportVolume() 
//----------------------------------------------------------------------------
{
  vtkRectilinearGridReader *reader;
  vtkNEW(reader);

  mafString filenameIn=MAF_DATA_ROOT;
  filenameIn<<"/Test_ExporterVTK/VolumeInput.vtk";
  reader->SetFileName(filenameIn);
  reader->Update();

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  result = volume->SetData(reader->GetOutput(),0.0) == MAF_OK;
  CPPUNIT_ASSERT(result);
  volume->Update();

  mafString filenameOut=MAF_DATA_ROOT;
  filenameOut<<"/Test_ExporterVTK/VolumeToCheck.vtk";
  m_ExporterVTK->SetInput(volume);
  m_ExporterVTK->SetFileName(filenameOut);
  m_ExporterVTK->SaveVTKData();

  reader->SetFileName(filenameOut);
  reader->Update();

  result = reader->GetOutput() != NULL;
  CPPUNIT_ASSERT(result);

  mafDEL(volume);
  vtkDEL(reader);
}
//----------------------------------------------------------------------------
void mafOpExporterVTKTest::TestOpExportPolyline() 
//----------------------------------------------------------------------------
{
  vtkPolyDataReader *reader;
  vtkNEW(reader);

  mafString filenameIn=MAF_DATA_ROOT;
  filenameIn<<"/Test_ExporterVTK/PolylineInput.vtk";
  reader->SetFileName(filenameIn);
  reader->Update();

  mafVMEPolyline *polyline;
  mafNEW(polyline);
  result = polyline->SetData(reader->GetOutput(),0.0) == MAF_OK;
  CPPUNIT_ASSERT(result);
  polyline->Update();

  mafString filenameOut=MAF_DATA_ROOT;
  filenameOut<<"/Test_ExporterVTK/PolylineToCheck.vtk";
  m_ExporterVTK->SetInput(polyline);
  m_ExporterVTK->SetFileName(filenameOut);
  m_ExporterVTK->SaveVTKData();

  reader->SetFileName(filenameOut);
  reader->Update();

  result = reader->GetOutput() != NULL;
  CPPUNIT_ASSERT(result);

  mafVMEPolyline *polylineToCheck;
  mafNEW(polylineToCheck);
  result = polylineToCheck->SetData(reader->GetOutput(),0.0) == MAF_OK;
  CPPUNIT_ASSERT(result);
  polylineToCheck->Update();

  mafDEL(polylineToCheck);
  mafDEL(polyline);
  vtkDEL(reader);
}
