/*=========================================================================

 Program: MAF2
 Module: vtkMAFDistanceFilterTest.cpp
 Authors: Nicola Vanella
 
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

#include "vtkMAFDistanceFilterTest.h"
#include "vtkMAFDistanceFilter.h"
#include "vtkSphereSource.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafOpImporterVTK.h"
#include "mafVMEVolumeGray.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFSmartPointer.h"


//-----------------------------------------------------------
void vtkMAFDistanceFilterTest::TestDynamicAllocation()
{
	vtkMAFDistanceFilter *filter;
	vtkNEW(filter);
	vtkDEL(filter);
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestInput()
{
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	CPPUNIT_ASSERT(filter->GetNumberOfInputs() == 0);

	vtkMAFSmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	vtkMAFSmartPointer<vtkPolyDataNormals> normals;
	normals->SetInput(sphere->GetOutput());

	filter->SetInput((vtkDataSet *)normals->GetOutput());
	filter->Update();

	CPPUNIT_ASSERT(filter->GetNumberOfInputs() == 1);
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestGetSet()
{
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	// Distance Mode
	filter->SetDistanceMode(VTK_SCALAR);	
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_SCALAR);

	filter->SetDistanceModeToVector();
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_VECTOR);
	CPPUNIT_ASSERT(filter->GetDistanceModeAsString() == "Vector");

	filter->SetDistanceModeToScalar();
	CPPUNIT_ASSERT(filter->GetDistanceMode() == VTK_SCALAR);
	CPPUNIT_ASSERT(filter->GetDistanceModeAsString() == "Scalar");

	// Filter Mode
	filter->SetFilterMode(VTK_DISTANCE_MODE);
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DISTANCE_MODE);

	filter->SetFilterModeToDensity();
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DENSITY_MODE);
	CPPUNIT_ASSERT(filter->GetFilterModeAsString() == "Density");

	filter->SetFilterModeToDistance();
	CPPUNIT_ASSERT(filter->GetFilterMode() == VTK_DISTANCE_MODE);
	CPPUNIT_ASSERT(filter->GetFilterModeAsString() == "Distance");
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestFilter_Scalar_Density()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root = storage->GetRoot();

	mafString filename = MAF_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";

	mafOpImporterVTK *importer = new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->ReparentTo(root);
	volume->Update();
	
	vtkMAFSmartPointer<vtkSphereSource> sphere;
// 	sphere->SetThetaResolution(6);
// 	sphere->SetPhiResolution(6);
// 	sphere->SetCenter(0, 0, 0);
// 	sphere->SetRadius(1.0);
	sphere->Update();

	vtkMAFSmartPointer<vtkPolyDataNormals> normals;
	normals->SetInput(sphere->GetOutput());

	// Create Filter
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	filter->SetDistanceModeToScalar();
	filter->SetFilterModeToDensity();

	filter->SetSource(volume->GetOutput()->GetVTKData());
	filter->SetInput((vtkDataSet *)normals->GetOutput());
	filter->Update();

	//	
	CPPUNIT_ASSERT(filter->GetOutput()->GetNumberOfPoints() == 66);
	CPPUNIT_ASSERT(filter->GetOutput()->GetNumberOfCells() == 96);
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetNumberOfTuples() == 66);
	
	vtkDataArray *vectors = filter->GetOutput()->GetPointData()->GetVectors();
	vtkDataArray *scalars = filter->GetOutput()->GetPointData()->GetScalars();
	
  CPPUNIT_ASSERT(vectors == NULL && scalars != NULL);

	double val = 0.88789987564086914;
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(5)[1] == val);   // 0.88789987564086914
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(10)[1] == val);  // 0.88789987564086914
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(57)[1] == -val); //-0.88789987564086914
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(22)[2] == val);  // 0.88789987564086914
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(55)[2] == val);  // 0.88789987564086914

	val = 0.88789993524551392;
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(28)[1] == -val); //-0.88789993524551392
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(50)[1] == val);  // 0.88789993524551392
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(16)[2] == val);  // 0.88789993524551392
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(40)[2] == -val); //-0.88789993524551392

	val = 0.88789981603622437;
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(34)[1] == -val); //-0.88789981603622437
	CPPUNIT_ASSERT(filter->GetOutput()->GetPointData()->GetTuple(46)[2] == -val); //-0.88789981603622437

	//
	volume->ReparentTo(NULL);
	cppDEL(importer);
	mafDEL(storage);
}

//----------------------------------------------------------------------------
void vtkMAFDistanceFilterTest::TestFilter_Vector_Distance()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root = storage->GetRoot();

	mafString filename = MAF_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";

	mafOpImporterVTK *importer = new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->ReparentTo(root);
	volume->Update();

	vtkMAFSmartPointer<vtkSphereSource> sphere;
	sphere->Update();

	vtkMAFSmartPointer<vtkPolyDataNormals> normals;
	normals->SetInput(sphere->GetOutput());

	// Create Filter
	vtkMAFSmartPointer<vtkMAFDistanceFilter> filter;

	filter->SetDistanceModeToVector();
	filter->SetFilterModeToDistance();

	filter->SetSource(volume->GetOutput()->GetVTKData());
	filter->SetInput((vtkDataSet *)normals->GetOutput());
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
	mafDEL(storage);
}
