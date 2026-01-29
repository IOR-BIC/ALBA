/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTKTest
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

#include "albaOpImporterVTKTest.h"

#include "albaString.h"
#include "albaVME.h"
#include "albaOpImporterVTK.h"
#include "albaVMEPolyline.h"
#include "vtkPolyData.h"
#include "albaVMEVolumeGray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMESurface.h"
#include "albaVMEMesh.h"
#include "vtkUnstructuredGrid.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaOpImporterVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpImporterVTKTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_ImporterVTK = new albaOpImporterVTK();
  m_ImporterVTK->TestModeOn();
}
//----------------------------------------------------------------------------
void albaOpImporterVTKTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_ImporterVTK);
}
//----------------------------------------------------------------------------
void albaOpImporterVTKTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void albaOpImporterVTKTest::TestAccept()
//----------------------------------------------------------------------------
{
  DummyVme *dummyVme = new DummyVme();
  
  result = m_ImporterVTK->Accept(dummyVme);
  TEST_RESULT;

  delete dummyVme;
}
//-----------------------------------------------------------
void albaOpImporterVTKTest::TestImportVTKSurface()
//-----------------------------------------------------------
{
	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/Surface/sphere.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportFile();
	albaVMESurface *surface = albaVMESurface::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(surface != NULL);
	surface->Modified();
	surface->Update();

	vtkPolyData *polydata = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(polydata != NULL);
	CPPUNIT_ASSERT(polydata->GetNumberOfPoints() == 82);
	CPPUNIT_ASSERT(polydata->GetNumberOfPolys() == 160);
}

//----------------------------------------------------------------------------
void albaOpImporterVTKTest::TestImportVTKMesh()
//----------------------------------------------------------------------------
{
	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/FEM/tet4.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportFile();
	albaVMEMesh *surface = albaVMEMesh::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(surface != NULL);
	surface->Modified();
	surface->Update();

	vtkUnstructuredGrid *ug = vtkUnstructuredGrid::SafeDownCast(surface->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(ug != NULL);
	CPPUNIT_ASSERT(ug->GetNumberOfPoints() == 5);
	CPPUNIT_ASSERT(ug->GetNumberOfCells() == 2);
}

//-----------------------------------------------------------
void albaOpImporterVTKTest::TestSetFileName()
{
	char *fileName = { "file name" };
	m_ImporterVTK->SetFileName(fileName);

	CPPUNIT_ASSERT(strcmp(m_ImporterVTK->GetFileName(), fileName) == 0);
}

//-----------------------------------------------------------
void albaOpImporterVTKTest::TestImportVTKPolyline()
{
	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/PolylineGraph/PolylineGraph.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportFile();
	albaVMEPolyline *polyline = albaVMEPolyline::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(polyline != NULL);

	polyline->Modified();
	polyline->Update();

	vtkPolyData *polyData = vtkPolyData::SafeDownCast(polyline->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(polyData != NULL);
	CPPUNIT_ASSERT(polyData->GetNumberOfPoints() == 5);
	CPPUNIT_ASSERT(polyData->GetNumberOfLines() == 4);
}
//-----------------------------------------------------------
void albaOpImporterVTKTest::TestImportVTKVolumeSP()
{
	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/VTK_Volumes/volume.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportFile();
	albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(volume != NULL);
	volume->Modified();
	volume->Update();

	vtkImageData *sp = vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(sp != NULL);
	CPPUNIT_ASSERT(sp->GetNumberOfPoints() == 517440);
	CPPUNIT_ASSERT(sp->GetNumberOfCells() == 498332);

}
//-----------------------------------------------------------
void albaOpImporterVTKTest::TestImportVTKVolumeRG()
{
	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/VTK_Volumes/volumeRG.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportFile();
	albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(volume != NULL);
	volume->Modified();
	volume->Update();

	vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(rg != NULL);
	CPPUNIT_ASSERT(rg->GetNumberOfPoints() == 1000);
	CPPUNIT_ASSERT(rg->GetNumberOfCells() == 729);
}

//-----------------------------------------------------------
void albaOpImporterVTKTest::TestImportVTKPointSet()
{
	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/PointSet/pointset.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportFile();
	albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(lmc != NULL);
	lmc->Modified();
	lmc->Update();

	lmc->GetOutput()->Update();
	vtkPolyData *pts = vtkPolyData::SafeDownCast(lmc->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(pts != NULL);
	CPPUNIT_ASSERT(pts->GetNumberOfPoints() == 2);

	//On import the point visibility is set to false
	CPPUNIT_ASSERT(pts->GetNumberOfCells() == 0);
}