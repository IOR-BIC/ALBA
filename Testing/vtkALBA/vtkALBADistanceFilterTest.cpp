/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADistanceFilterTest.cpp
 Authors: Nicola Vanella
 
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

#include "vtkALBADistanceFilterTest.h"
#include "vtkALBADistanceFilter.h"
#include "vtkSphereSource.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaOpImporterVTK.h"
#include "albaVMEVolumeGray.h"
#include "vtkPolyDataNormals.h"
#include "vtkALBASmartPointer.h"
#include "vtkDataSetReader.h"

//-----------------------------------------------------------
void vtkALBADistanceFilterTest::TestDynamicAllocation()
{
	vtkALBADistanceFilter *filter;
	vtkNEW(filter);
	vtkDEL(filter);
}

//----------------------------------------------------------------------------
void vtkALBADistanceFilterTest::TestInput()
{
	vtkALBASmartPointer<vtkALBADistanceFilter> filter;

	CPPUNIT_ASSERT(filter->GetTotalNumberOfInputConnections() == 0);

	vtkALBASmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	vtkALBASmartPointer<vtkPolyDataNormals> normals;
	normals->SetInputConnection(sphere->GetOutputPort());

	filter->SetInputConnection(normals->GetOutputPort());
	filter->Update();

	CPPUNIT_ASSERT(filter->GetTotalNumberOfInputConnections() == 1);
}

//----------------------------------------------------------------------------
void vtkALBADistanceFilterTest::TestGetSet()
{
	vtkALBASmartPointer<vtkALBADistanceFilter> filter;

	// Distance Mode
	filter->SetDistanceMode(VTK_SCALAR);	
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_SCALAR);

	filter->SetDistanceModeToVector();
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_VECTOR);
	CPPUNIT_ASSERT(!strcmp(filter->GetDistanceModeAsString(), "Vector"));

	filter->SetDistanceModeToScalar();
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_SCALAR);
	CPPUNIT_ASSERT(!strcmp(filter->GetDistanceModeAsString(), "Scalar"));

	// Filter Mode
	filter->SetFilterMode(VTK_DISTANCE_MODE);
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DISTANCE_MODE);

	filter->SetFilterModeToDensity();
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DENSITY_MODE);
	CPPUNIT_ASSERT(!strcmp(filter->GetFilterModeAsString(), "Density"));

	filter->SetFilterModeToDistance();
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DISTANCE_MODE);
	CPPUNIT_ASSERT(!strcmp(filter->GetFilterModeAsString(), "Distance"));
}

//----------------------------------------------------------------------------
void vtkALBADistanceFilterTest::TestFilter_Scalar_Density()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root = storage->GetRoot();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";

	albaOpImporterVTK *importer = new albaOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->ReparentTo(root);
	volume->Update();
	
	vtkALBASmartPointer<vtkSphereSource> sphere;

	sphere->Update();

	vtkALBASmartPointer<vtkPolyDataNormals> normals;
	normals->SetInputConnection(sphere->GetOutputPort());

	// Create Filter
	vtkALBASmartPointer<vtkALBADistanceFilter> filter;

	filter->SetDistanceModeToScalar();
	filter->SetFilterModeToDensity();

	filter->SetSource(volume->GetOutput()->GetVTKData());
	filter->SetInputConnection(normals->GetOutputPort());
	filter->Update();

	//	
	vtkDataSet *filterOutput = filter->GetOutput();
	CPPUNIT_ASSERT(filterOutput->GetNumberOfPoints() == 66);
	CPPUNIT_ASSERT(filterOutput->GetNumberOfCells() == 96);
	CPPUNIT_ASSERT(filterOutput->GetPointData()->GetNumberOfTuples() == 66);
	
	vtkDataArray *vectors = filterOutput->GetPointData()->GetVectors();
	vtkDataArray *scalars = filterOutput->GetPointData()->GetScalars();
	
  CPPUNIT_ASSERT(vectors == NULL && scalars != NULL);

	double val = 0.88789987564086914;
	CPPUNIT_ASSERT(scalars->GetTuple(5)[1] == val);   // 0.88789987564086914
	CPPUNIT_ASSERT(scalars->GetTuple(10)[1] == val);  // 0.88789987564086914
	CPPUNIT_ASSERT(scalars->GetTuple(57)[1] == -val); //-0.88789987564086914
	CPPUNIT_ASSERT(scalars->GetTuple(22)[2] == val);  // 0.88789987564086914
	CPPUNIT_ASSERT(scalars->GetTuple(55)[2] == val);  // 0.88789987564086914

	val = 0.88789993524551392;
	CPPUNIT_ASSERT(scalars->GetTuple(28)[1] == -val); //-0.88789993524551392
	CPPUNIT_ASSERT(scalars->GetTuple(50)[1] == val);  // 0.88789993524551392
	CPPUNIT_ASSERT(scalars->GetTuple(16)[2] == val);  // 0.88789993524551392
	CPPUNIT_ASSERT(scalars->GetTuple(40)[2] == -val); //-0.88789993524551392

	val = 0.88789981603622437;
	CPPUNIT_ASSERT(scalars->GetTuple(34)[1] == -val); //-0.88789981603622437
	CPPUNIT_ASSERT(scalars->GetTuple(46)[2] == -val); //-0.88789981603622437

	//
	volume->ReparentTo(NULL);
	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void vtkALBADistanceFilterTest::TestFilter_Vector_Distance()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root = storage->GetRoot();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";

	albaOpImporterVTK *importer = new albaOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->ReparentTo(root);
	volume->Update();

	vtkALBASmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	vtkALBASmartPointer<vtkPolyDataNormals> normals;
	normals->SetInputConnection(sphere->GetOutputPort());

	// Create Filter
	vtkALBASmartPointer<vtkALBADistanceFilter> filter;

	filter->SetDistanceModeToVector();
	filter->SetFilterModeToDistance();

	filter->SetSource(volume->GetOutput()->GetVTKData());
	filter->SetInputConnection(normals->GetOutputPort());
	filter->Update();

	//	
	vtkDataArray *vectors = filter->GetOutput()->GetPointData()->GetVectors();
	vtkDataArray *scalars = filter->GetOutput()->GetPointData()->GetScalars();

	CPPUNIT_ASSERT(vectors != NULL && scalars == NULL);

	double val = 8.8789997100830078;
	CPPUNIT_ASSERT(vectors->GetTuple3(28)[0] == -val); //-8.8789997100830078
	CPPUNIT_ASSERT(vectors->GetTuple3(50)[0] == val);  // 8.8789997100830078
	CPPUNIT_ASSERT(vectors->GetTuple3(16)[1] == val);  // 8.8789997100830078
	CPPUNIT_ASSERT(vectors->GetTuple3(40)[1] == -val); //-8.8789997100830078
	CPPUNIT_ASSERT(vectors->GetTuple3(61)[1] == -val); //-8.8789997100830078

	CPPUNIT_ASSERT(vectors->GetTuple3(0)[2] == 10.0);
	CPPUNIT_ASSERT(vectors->GetTuple3(1)[2] == -10.0);

	//
	volume->ReparentTo(NULL);
	cppDEL(importer);
	albaDEL(storage);
}
