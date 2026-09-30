/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtrusionHolesTest
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
#include "albaOpExtrusionHolesTest.h"
#include "albaOpExtrusionHoles.h"

#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"
#include "albaOpImporterVTK.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------
void albaOpExtrusionHolesTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	albaOpExtrusionHoles *extrusion = new albaOpExtrusionHoles();
	albaDEL(extrusion);
}
//-----------------------------------------------------------
void albaOpExtrusionHolesTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
	albaOpExtrusionHoles extrusion; 
}
//-----------------------------------------------------------
void albaOpExtrusionHolesTest::TestSetGetExtrusionFactor() 
//-----------------------------------------------------------
{
	albaOpExtrusionHoles *extrusion = new albaOpExtrusionHoles();
	extrusion->SetExtrusionFactor(4.0);
	CPPUNIT_ASSERT(extrusion->GetExtrusionFactor()==4.0);
	albaDEL(extrusion);
}
//-----------------------------------------------------------
void albaOpExtrusionHolesTest::TestExtractFreeEdge() 
//-----------------------------------------------------------
{
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	//Imput data is a cube with 2 holes
	albaOpImporterVTK *importer=new albaOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Extrusion/extrusion_cube.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	albaVMESurface *cube=albaVMESurface::SafeDownCast(importer->GetOutput());
	cube->Update();

	albaOpExtrusionHoles *extrusion = new albaOpExtrusionHoles();
	extrusion->TestModeOn();
	extrusion->SetInput(cube);
	extrusion->OpRun();
	extrusion->ExtractFreeEdge();

	//In the cube there is an hole with 4 points
	CPPUNIT_ASSERT(extrusion->GetExtractFreeEdgesNumeberOfPoints()==4);

	albaDEL(extrusion);
	albaDEL(importer);
	albaDEL(storage);
}
//-----------------------------------------------------------
void albaOpExtrusionHolesTest::TestExtrude() 
//-----------------------------------------------------------
{
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *root=storage->GetRoot();

	//Imput data is a cube with 1 hole
	albaOpImporterVTK *importer=new albaOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Extrusion/extrusion_cube.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	albaVMESurface *cube=albaVMESurface::SafeDownCast(importer->GetOutput());
	cube->Update();

	double bounds[6];

	vtkPolyData *polydata = vtkPolyData::SafeDownCast(cube->GetOutput()->GetVTKData());
	
	polydata->GetBounds(bounds);

	albaOpExtrusionHoles *extrusion = new albaOpExtrusionHoles();
	extrusion->TestModeOn();
	extrusion->SetInput(cube);
	extrusion->SetExtrusionFactor(4.0);//diameter is 5,09
	extrusion->OpRun();
	extrusion->ExtractFreeEdge();
	//Select 1° hole with point ID 1
	extrusion->SelectHole(1);
	extrusion->Extrude();
	extrusion->SaveExtrusion();
	vtkPolyData *resultPolydata=extrusion->GetExtrutedSurface();

	resultPolydata->GetBounds(bounds);

  double epsilon = 0.01;
	CPPUNIT_ASSERT(bounds[1]-bounds[0] < 4+epsilon && bounds[1]-bounds[0] > 4-epsilon && bounds[3]-bounds[2] < 4+epsilon && bounds[3]-bounds[2] > 4-epsilon && bounds[5]-bounds[4] < 24.37+epsilon && bounds[5]-bounds[4] > 24.37-epsilon);
	
	albaDEL(extrusion);
	albaDEL(importer);
	albaDEL(storage);
}
