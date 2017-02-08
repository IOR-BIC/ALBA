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
#include "mafVMEPolyline.h"
#include "vtkPolyData.h"
#include "mafVMEVolumeGray.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMESurface.h"
#include "mafVMEMesh.h"
#include "vtkUnstructuredGrid.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_ImporterVTK = new mafOpImporterVTK();
  m_ImporterVTK->TestModeOn();
}
//----------------------------------------------------------------------------
void mafOpImporterVTKTest::AfterTest()
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
//-----------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKSurface()
//-----------------------------------------------------------
{
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/Surface/sphere.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportVTK();
	mafVMESurface *surface = mafVMESurface::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(surface != NULL);
	surface->Modified();
	surface->Update();

	vtkPolyData *polydata = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(polydata != NULL);

	polydata->Modified();
	polydata->Update();

	CPPUNIT_ASSERT(polydata->GetNumberOfPoints() == 82);
	CPPUNIT_ASSERT(polydata->GetNumberOfPolys() == 160);
}

//----------------------------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKMesh()
//----------------------------------------------------------------------------
{
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/FEM/tet4.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportVTK();
	mafVMEMesh *surface = mafVMEMesh::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(surface != NULL);
	surface->Modified();
	surface->Update();

	vtkUnstructuredGrid *ug = vtkUnstructuredGrid::SafeDownCast(surface->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(ug != NULL);

	ug->Modified();
	ug->Update();

	CPPUNIT_ASSERT(ug->GetNumberOfPoints() == 5);
	CPPUNIT_ASSERT(ug->GetNumberOfCells() == 2);
}

//-----------------------------------------------------------
void mafOpImporterVTKTest::TestSetFileName()
{
	char *fileName = { "file name" };
	m_ImporterVTK->SetFileName(fileName);

	CPPUNIT_ASSERT(strcmp(m_ImporterVTK->GetFileName(), fileName) == 0);
}

//-----------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKPolyline()
{
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/PolylineGraph/PolylineGraph.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportVTK();
	mafVMEPolyline *polyline = mafVMEPolyline::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(polyline != NULL);

	polyline->Modified();
	polyline->Update();

	vtkPolyData *polyData = vtkPolyData::SafeDownCast(polyline->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(polyData != NULL);

	polyData->Modified();
	polyData->Update();

	CPPUNIT_ASSERT(polyData->GetNumberOfPoints() == 5);
	CPPUNIT_ASSERT(polyData->GetNumberOfLines() == 4);
}
//-----------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKVolumeSP()
{
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/VTK_Volumes/volume.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportVTK();
	mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(volume != NULL);
	volume->Modified();
	volume->Update();

	vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(volume->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(sp != NULL);

	sp->Modified();
	sp->Update();

	CPPUNIT_ASSERT(sp->GetNumberOfPoints() == 517440);
	CPPUNIT_ASSERT(sp->GetNumberOfCells() == 498332);

}
//-----------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKVolumeRG()
{
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/VTK_Volumes/volumeRG.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportVTK();
	mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(volume != NULL);
	volume->Modified();
	volume->Update();

	vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(rg != NULL);

	rg->Modified();
	rg->Update();

	CPPUNIT_ASSERT(rg->GetNumberOfPoints() == 1000);
	CPPUNIT_ASSERT(rg->GetNumberOfCells() == 729);
}

//-----------------------------------------------------------
void mafOpImporterVTKTest::TestImportVTKPointSet()
{
	mafString fileName = MAF_DATA_ROOT;
	fileName << "/PointSet/pointset.vtk";
	m_ImporterVTK->SetFileName(fileName);
	m_ImporterVTK->ImportVTK();
	mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(m_ImporterVTK->GetOutput());

	CPPUNIT_ASSERT(lmc != NULL);
	lmc->Modified();
	lmc->Update();

	lmc->GetOutput()->Update();
	vtkPolyData *pts = vtkPolyData::SafeDownCast(lmc->GetOutput()->GetVTKData());

	CPPUNIT_ASSERT(pts != NULL);

	pts->Modified();
	pts->Update();

	CPPUNIT_ASSERT(pts->GetNumberOfPoints() == 2);

	//On import the point visibility is set to false
	CPPUNIT_ASSERT(pts->GetNumberOfCells() == 0);
}