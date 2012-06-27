/*=========================================================================

 Program: MAF2
 Module: mafOpBooleanSurfaceTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "mafOpBooleanSurfaceTest.h"
#include "mafOpBooleanSurface.h"
#include "mafOpImporterSTL.h"

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"

#include "vtkPolyData.h"

#define EPSILON 0.01

//-----------------------------------------------------------
void mafOpBooleanSurfaceTest::TestUnion() 
//-----------------------------------------------------------
{
	//UNION
	
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	mafOpImporterSTL *importer1=new mafOpImporterSTL("importer");
	importer1->TestModeOn();
	importer1->SetInput(root);
	mafString filename1=MAF_DATA_ROOT;
	filename1<<"/STL/BoolCubo1.stl";
	importer1->SetFileName(filename1.GetCStr());
	importer1->OpRun();
  std::vector<mafVMESurface*> importedSTL1;
  importer1->GetImportedSTL(importedSTL1);
	mafVMESurface *cubo1 = importedSTL1[0];
	
	mafOpImporterSTL *importer2 = new mafOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetInput(root);
	mafString filename2=MAF_DATA_ROOT;
	filename2<<"/STL/BoolCubo2.stl";
	importer2->SetFileName(filename2.GetCStr());
	importer2->OpRun();
  std::vector<mafVMESurface*> importedSTL2;
	importer2->GetImportedSTL(importedSTL2);
  mafVMESurface *cubo2 = importedSTL2[0];

	vtkPolyData *poly1 = vtkPolyData::SafeDownCast(cubo1->GetOutput()->GetVTKData());
	poly1->Update();
	vtkPolyData *poly2 = vtkPolyData::SafeDownCast(cubo2->GetOutput()->GetVTKData());
	poly2->Update();
	
	CPPUNIT_ASSERT(poly1->GetNumberOfPoints()==poly2->GetNumberOfPoints());
	int numPoints1=poly1->GetNumberOfPoints();
	int numPoints2=poly2->GetNumberOfPoints();

	mafOpBooleanSurface *booleanSurface=new mafOpBooleanSurface();
	booleanSurface->TestModeOn();
	booleanSurface->SetInput(cubo1);
	booleanSurface->SetFactor1(cubo1);
	booleanSurface->SetFactor2(cubo2);
	booleanSurface->Union();
	cubo1->GetOutput()->GetVTKData()->Update();
	cubo1->Update();

	CPPUNIT_ASSERT(poly1->GetNumberOfPoints()==numPoints1+numPoints2);

	mafDEL(booleanSurface);
	mafDEL(importer1);
	mafDEL(importer2);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpBooleanSurfaceTest::TestIntersection() 
//-----------------------------------------------------------
{
	//INTERSECTION
	
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	mafOpImporterSTL *importer1=new mafOpImporterSTL("importer");
	importer1->TestModeOn();
	importer1->SetInput(root);
	mafString filename1=MAF_DATA_ROOT;
	filename1<<"/STL/BoolCubo1.stl";
	importer1->SetFileName(filename1.GetCStr());
	importer1->OpRun();
  std::vector<mafVMESurface*> importedSTL1;
  importer1->GetImportedSTL(importedSTL1);
	mafVMESurface *cubo1 = importedSTL1[0];

	mafOpImporterSTL *importer2=new mafOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetInput(root);
	mafString filename2=MAF_DATA_ROOT;
	filename2<<"/STL/BoolCubo2.stl";
	importer2->SetFileName(filename2.GetCStr());
	importer2->OpRun();
  std::vector<mafVMESurface*> importedSTL2;
  importer2->GetImportedSTL(importedSTL2);
  mafVMESurface *cubo2 = importedSTL2[0];

	vtkPolyData *poly1 = vtkPolyData::SafeDownCast(cubo1->GetOutput()->GetVTKData());
	poly1->Update();
	vtkPolyData *poly2 = vtkPolyData::SafeDownCast(cubo2->GetOutput()->GetVTKData());
	poly2->Update();

	double bounds1[6],bounds2[6];
	poly1->GetBounds(bounds1);
	poly2->GetBounds(bounds2);

	mafOpBooleanSurface *booleanSurface=new mafOpBooleanSurface();
	booleanSurface->TestModeOn();
	booleanSurface->SetInput(cubo1);
	booleanSurface->SetFactor1(cubo1);
	booleanSurface->SetFactor2(cubo2);
	booleanSurface->Intersection();
	cubo1->GetOutput()->GetVTKData()->Update();
	cubo1->Update();

	double boundsOutput[6];
	poly1->GetBounds(boundsOutput);

	//control done only with cube1
	CPPUNIT_ASSERT(boundsOutput[0]+EPSILON >= bounds1[1] || boundsOutput[0]-EPSILON <= bounds1[1]);	
	CPPUNIT_ASSERT(boundsOutput[1]+EPSILON >= bounds1[1] || boundsOutput[1]-EPSILON <= bounds1[1]);
	
	mafDEL(booleanSurface);
	mafDEL(importer1);
	mafDEL(importer2);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpBooleanSurfaceTest::TestDifference() 
//-----------------------------------------------------------
{
	//DIFFERENCE
	
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	mafOpImporterSTL *importer1=new mafOpImporterSTL("importer");
	importer1->TestModeOn();
	importer1->SetInput(root);
	mafString filename1=MAF_DATA_ROOT;
	filename1<<"/STL/BoolCubo1.stl";
	importer1->SetFileName(filename1.GetCStr());
	importer1->OpRun();
  std::vector<mafVMESurface*> importedSTL1;
  importer1->GetImportedSTL(importedSTL1);
	mafVMESurface *cubo1 = importedSTL1[0];
	
  mafOpImporterSTL *importer2=new mafOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetInput(root);
	mafString filename2=MAF_DATA_ROOT;
	filename2<<"/STL/BoolCubo2.stl";
	importer2->SetFileName(filename2.GetCStr());
	importer2->OpRun();
  std::vector<mafVMESurface*> importedSTL2;
  importer2->GetImportedSTL(importedSTL2);
	mafVMESurface *cubo2 = importedSTL2[0];

	vtkPolyData *poly1 = vtkPolyData::SafeDownCast(cubo1->GetOutput()->GetVTKData());
	poly1->Update();
	vtkPolyData *poly2 = vtkPolyData::SafeDownCast(cubo2->GetOutput()->GetVTKData());
	poly2->Update();

	double bounds1[6],bounds2[6];
	poly1->GetBounds(bounds1);
	poly2->GetBounds(bounds2);

	mafOpBooleanSurface *booleanSurface=new mafOpBooleanSurface();
	booleanSurface->TestModeOn();
	booleanSurface->SetInput(cubo1);
	booleanSurface->SetFactor1(cubo1);
	booleanSurface->SetFactor2(cubo2);
	booleanSurface->Difference();
	cubo1->GetOutput()->GetVTKData()->Update();
	cubo1->Update();

	double boundsOutput[6];
	poly1->GetBounds(boundsOutput);

	CPPUNIT_ASSERT(boundsOutput[0]+EPSILON >= bounds1[0] || boundsOutput[0]-EPSILON <= bounds1[0]);	
	CPPUNIT_ASSERT(boundsOutput[1]+EPSILON >= bounds2[0] || boundsOutput[1]-EPSILON <= bounds2[0]);

	mafDEL(booleanSurface);
	mafDEL(importer1);
	mafDEL(importer2);
	mafDEL(storage);
}
