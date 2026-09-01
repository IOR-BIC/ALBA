/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpBooleanSurfaceTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "albaOpBooleanSurfaceTest.h"
#include "albaOpBooleanSurface.h"
#include "albaOpImporterSTL.h"

#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"

#include "vtkPolyData.h"

#define EPSILON 0.01

//-----------------------------------------------------------
void albaOpBooleanSurfaceTest::TestUnion() 
//-----------------------------------------------------------
{
	//UNION
	
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	albaOpImporterSTL *importer1=new albaOpImporterSTL("importer");
	importer1->TestModeOn();
	importer1->SetInput(root);
	albaString filename1=ALBA_DATA_ROOT;
	filename1<<"/STL/BoolCubo1.stl";
	importer1->SetFileName(filename1.GetCStr());
	importer1->OpRun();
  std::vector<albaVMESurface*> importedSTL1;
  importer1->GetImportedSTL(importedSTL1);
	albaVMESurface *cubo1 = importedSTL1[0];
	
	albaOpImporterSTL *importer2 = new albaOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetInput(root);
	albaString filename2=ALBA_DATA_ROOT;
	filename2<<"/STL/BoolCubo2.stl";
	importer2->SetFileName(filename2.GetCStr());
	importer2->OpRun();
  std::vector<albaVMESurface*> importedSTL2;
	importer2->GetImportedSTL(importedSTL2);
  albaVMESurface *cubo2 = importedSTL2[0];

	vtkPolyData *poly1 = vtkPolyData::SafeDownCast(cubo1->GetOutput()->GetVTKData());
	vtkPolyData *poly2 = vtkPolyData::SafeDownCast(cubo2->GetOutput()->GetVTKData());
	
	CPPUNIT_ASSERT(poly1->GetNumberOfPoints()==poly2->GetNumberOfPoints());
	int numPoints1=poly1->GetNumberOfPoints();
	int numPoints2=poly2->GetNumberOfPoints();

	albaOpBooleanSurface *booleanSurface=new albaOpBooleanSurface();
	booleanSurface->TestModeOn();
	booleanSurface->SetInput(cubo1);
	booleanSurface->SetFactor1(cubo1);
	booleanSurface->SetFactor2(cubo2);
	booleanSurface->Union();
	cubo1->Update();

	CPPUNIT_ASSERT(poly1->GetNumberOfPoints()==numPoints1+numPoints2);

	albaDEL(booleanSurface);
	albaDEL(importer1);
	albaDEL(importer2);
	albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpBooleanSurfaceTest::TestIntersection() 
//-----------------------------------------------------------
{
	//INTERSECTION
	
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	albaOpImporterSTL *importer1=new albaOpImporterSTL("importer");
	importer1->TestModeOn();
	importer1->SetInput(root);
	albaString filename1=ALBA_DATA_ROOT;
	filename1<<"/STL/BoolCubo1.stl";
	importer1->SetFileName(filename1.GetCStr());
	importer1->OpRun();
  std::vector<albaVMESurface*> importedSTL1;
  importer1->GetImportedSTL(importedSTL1);
	albaVMESurface *cubo1 = importedSTL1[0];

	albaOpImporterSTL *importer2=new albaOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetInput(root);
	albaString filename2=ALBA_DATA_ROOT;
	filename2<<"/STL/BoolCubo2.stl";
	importer2->SetFileName(filename2.GetCStr());
	importer2->OpRun();
  std::vector<albaVMESurface*> importedSTL2;
  importer2->GetImportedSTL(importedSTL2);
  albaVMESurface *cubo2 = importedSTL2[0];

	vtkPolyData *poly1 = vtkPolyData::SafeDownCast(cubo1->GetOutput()->GetVTKData());
	vtkPolyData *poly2 = vtkPolyData::SafeDownCast(cubo2->GetOutput()->GetVTKData());

	double bounds1[6],bounds2[6];
	poly1->GetBounds(bounds1);
	poly2->GetBounds(bounds2);

	albaOpBooleanSurface *booleanSurface=new albaOpBooleanSurface();
	booleanSurface->TestModeOn();
	booleanSurface->SetInput(cubo1);
	booleanSurface->SetFactor1(cubo1);
	booleanSurface->SetFactor2(cubo2);
	booleanSurface->Intersection();
	cubo1->Update();

	double boundsOutput[6];
	poly1->GetBounds(boundsOutput);

	//control done only with cube1
	CPPUNIT_ASSERT(boundsOutput[0]+EPSILON >= bounds1[1] || boundsOutput[0]-EPSILON <= bounds1[1]);	
	CPPUNIT_ASSERT(boundsOutput[1]+EPSILON >= bounds1[1] || boundsOutput[1]-EPSILON <= bounds1[1]);
	
	albaDEL(booleanSurface);
	albaDEL(importer1);
	albaDEL(importer2);
	albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpBooleanSurfaceTest::TestDifference() 
//-----------------------------------------------------------
{
	//DIFFERENCE
	
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	albaOpImporterSTL *importer1=new albaOpImporterSTL("importer");
	importer1->TestModeOn();
	importer1->SetInput(root);
	albaString filename1=ALBA_DATA_ROOT;
	filename1<<"/STL/BoolCubo1.stl";
	importer1->SetFileName(filename1.GetCStr());
	importer1->OpRun();
  std::vector<albaVMESurface*> importedSTL1;
  importer1->GetImportedSTL(importedSTL1);
	albaVMESurface *cubo1 = importedSTL1[0];
	
  albaOpImporterSTL *importer2=new albaOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetInput(root);
	albaString filename2=ALBA_DATA_ROOT;
	filename2<<"/STL/BoolCubo2.stl";
	importer2->SetFileName(filename2.GetCStr());
	importer2->OpRun();
  std::vector<albaVMESurface*> importedSTL2;
  importer2->GetImportedSTL(importedSTL2);
	albaVMESurface *cubo2 = importedSTL2[0];

	vtkPolyData *poly1 = vtkPolyData::SafeDownCast(cubo1->GetOutput()->GetVTKData());
	vtkPolyData *poly2 = vtkPolyData::SafeDownCast(cubo2->GetOutput()->GetVTKData());

	double bounds1[6],bounds2[6];
	poly1->GetBounds(bounds1);
	poly2->GetBounds(bounds2);

	albaOpBooleanSurface *booleanSurface=new albaOpBooleanSurface();
	booleanSurface->TestModeOn();
	booleanSurface->SetInput(cubo1);
	booleanSurface->SetFactor1(cubo1);
	booleanSurface->SetFactor2(cubo2);
	booleanSurface->Difference();
	cubo1->Update();

	double boundsOutput[6];
	poly1->GetBounds(boundsOutput);

	CPPUNIT_ASSERT(boundsOutput[0]+EPSILON >= bounds1[0] || boundsOutput[0]-EPSILON <= bounds1[0]);	
	CPPUNIT_ASSERT(boundsOutput[1]+EPSILON >= bounds2[0] || boundsOutput[1]-EPSILON <= bounds2[0]);

	albaDEL(booleanSurface);
	albaDEL(importer1);
	albaDEL(importer2);
	albaDEL(storage);
}
