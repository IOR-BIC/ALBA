/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterVTKTest
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
#include "albaOperationsTests.h"

#include "albaOpExporterVTKTest.h"
#include "albaOpExporterVTK.h"

#include "albaVMERoot.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEMesh.h"
#include "albaVMEPolyline.h"

#include "vtkSphereSource.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGridReader.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkRectilinearGrid.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_ExporterVTK = new albaOpExporterVTK("VTK");
	m_ExporterVTK->TestModeOn();
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_ExporterVTK);
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestAccept()
//----------------------------------------------------------------------------
{
	DummyVme *dummyVme = new DummyVme();
	result = m_ExporterVTK->Accept(dummyVme);

	CPPUNIT_ASSERT(result);

	delete dummyVme;
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestOpExportPolydata()
//----------------------------------------------------------------------------
{
	vtkSphereSource *sphere;
	vtkNEW(sphere);
	sphere->Update();

	albaVMESurface *surface;
	albaNEW(surface);

	result = surface->SetData((vtkPolyData*)sphere->GetOutput(), 0.0) == ALBA_OK;
	CPPUNIT_ASSERT(result);
	surface->Update();

	m_ExporterVTK->SetInput(surface);

	albaString filename = GET_TEST_DATA_DIR();
	filename << "/PolydataToCheck.vtk";

	m_ExporterVTK->SetFileName(filename);
	m_ExporterVTK->SaveVTKData();

	vtkPolyDataReader *reader;
	vtkNEW(reader);
	reader->SetFileName(filename);
	reader->Update();

	result = reader->GetOutput() != NULL;
	CPPUNIT_ASSERT(result);

	vtkDEL(reader);
	albaDEL(surface);
	vtkDEL(sphere);
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestOpExportPolydataWithApplyABSMatrix()
//----------------------------------------------------------------------------
{
	vtkSphereSource *sphere;
	vtkNEW(sphere);
	sphere->Update();

	double start_bounds[6];
	sphere->GetOutput()->GetBounds(start_bounds);

	// Create a albaVMESurface
	albaVMESurface *surface;
	albaNEW(surface);
	surface->SetData((vtkPolyData*)sphere->GetOutput(), 0.0) == ALBA_OK;
	surface->Update();

	// Apply a transformation
	albaMatrix m;
	m.SetElement(0, 3, 2.1);
	m.SetElement(1, 3, 2.1);
	m.SetElement(2, 3, 2.1);
	surface->SetAbsMatrix(m);

	// set the ABSMatrix flat On
	m_ExporterVTK->ApplyABSMatrixOn();
	m_ExporterVTK->SetInput(surface);

	albaString filename = GET_TEST_DATA_DIR();
	filename << "/PolydataToCheckTransformation.vtk";

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

	result = albaEquals(start_bounds[0], end_bounds[0]) &&
		albaEquals(start_bounds[1], end_bounds[1]) &&
		albaEquals(start_bounds[2], end_bounds[2]) &&
		albaEquals(start_bounds[3], end_bounds[3]) &&
		albaEquals(start_bounds[4], end_bounds[4]) &&
		albaEquals(start_bounds[5], end_bounds[5]);

	// check that bounds are not equals.
	CPPUNIT_ASSERT(!result);

	vtkDEL(reader);
	albaDEL(surface);
	vtkDEL(sphere);
}

//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestOpExportMesh()
//----------------------------------------------------------------------------
{
	vtkUnstructuredGridReader *reader;
	vtkNEW(reader);

	albaString filenameIn = ALBA_DATA_ROOT;
	filenameIn << "/Test_ExporterVTK/MeshInput.vtk";
	reader->SetFileName(filenameIn);
	reader->Update();

	albaVMEMesh *mesh;
	albaNEW(mesh);
	result = mesh->SetData(reader->GetOutput(), 0.0) == ALBA_OK;
	CPPUNIT_ASSERT(result);
	mesh->Update();

	albaString filenameOut = GET_TEST_DATA_DIR();
	filenameOut << "/MeshToCheck.vtk";
	m_ExporterVTK->SetInput(mesh);
	m_ExporterVTK->SetFileName(filenameOut);
	m_ExporterVTK->SaveVTKData();

	reader->SetFileName(filenameOut);
	reader->Update();

	result = reader->GetOutput() != NULL;
	CPPUNIT_ASSERT(result);

	albaDEL(mesh);
	vtkDEL(reader);
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestOpExportVolume()
//----------------------------------------------------------------------------
{
	vtkRectilinearGridReader *reader;
	vtkNEW(reader);

	albaString filenameIn = ALBA_DATA_ROOT;
	filenameIn << "/Test_ExporterVTK/VolumeInput.vtk";
	reader->SetFileName(filenameIn);
	reader->Update();

	albaVMEVolumeGray *volume;
	albaNEW(volume);
	result = volume->SetData((vtkRectilinearGrid*)reader->GetOutput(), 0.0) == ALBA_OK;
	CPPUNIT_ASSERT(result);
	volume->Update();

	albaString filenameOut = GET_TEST_DATA_DIR();
	filenameOut << "/VolumeToCheck.vtk";
	m_ExporterVTK->SetInput(volume);
	m_ExporterVTK->SetFileName(filenameOut);
	m_ExporterVTK->SaveVTKData();

	reader->SetFileName(filenameOut);
	reader->Update();

	result = reader->GetOutput() != NULL;
	CPPUNIT_ASSERT(result);

	albaDEL(volume);
	vtkDEL(reader);
}
//----------------------------------------------------------------------------
void albaOpExporterVTKTest::TestOpExportPolyline()
//----------------------------------------------------------------------------
{
	vtkPolyDataReader *reader;
	vtkNEW(reader);

	albaString filenameIn = ALBA_DATA_ROOT;
	filenameIn << "/Test_ExporterVTK/PolylineInput.vtk";
	reader->SetFileName(filenameIn);
	reader->Update();

	albaVMEPolyline *polyline;
	albaNEW(polyline);
	result = polyline->SetData(reader->GetOutput(), 0.0) == ALBA_OK;
	CPPUNIT_ASSERT(result);
	polyline->Update();

	albaString filenameOut = GET_TEST_DATA_DIR();
	filenameOut << "/PolylineToCheck.vtk";
	m_ExporterVTK->SetInput(polyline);
	m_ExporterVTK->SetFileName(filenameOut);
	m_ExporterVTK->SaveVTKData();

	reader->SetFileName(filenameOut);
	reader->Update();

	result = reader->GetOutput() != NULL;
	CPPUNIT_ASSERT(result);

	albaVMEPolyline *polylineToCheck;
	albaNEW(polylineToCheck);
	result = polylineToCheck->SetData(reader->GetOutput(), 0.0) == ALBA_OK;
	CPPUNIT_ASSERT(result);
	polylineToCheck->Update();

	albaDEL(polylineToCheck);
	albaDEL(polyline);
	vtkDEL(reader);
}
