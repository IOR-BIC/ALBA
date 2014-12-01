/*=========================================================================

 Program: MAF2Medical
 Module: mafOpExtrusionHolesTest
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
#include "mafOpExtrusionHolesTest.h"
#include "mafOpExtrusionHoles.h"

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafOpImporterVTK.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------
void mafOpExtrusionHolesTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	mafOpExtrusionHoles *extrusion = new mafOpExtrusionHoles();
	mafDEL(extrusion);
}
//-----------------------------------------------------------
void mafOpExtrusionHolesTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
	mafOpExtrusionHoles extrusion; 
}
//-----------------------------------------------------------
void mafOpExtrusionHolesTest::TestSetGetExtrusionFactor() 
//-----------------------------------------------------------
{
	mafOpExtrusionHoles *extrusion = new mafOpExtrusionHoles();
	extrusion->SetExtrusionFactor(4.0);
	CPPUNIT_ASSERT(extrusion->GetExtrusionFactor()==4.0);
	mafDEL(extrusion);
}
//-----------------------------------------------------------
void mafOpExtrusionHolesTest::TestExtractFreeEdge() 
//-----------------------------------------------------------
{
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	//Imput data is a cube with 2 holes
	mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Extrusion/extrusion_cube.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	mafVMESurface *cube=mafVMESurface::SafeDownCast(importer->GetOutput());
	cube->GetOutput()->GetVTKData()->Update();
	cube->Update();

	mafOpExtrusionHoles *extrusion = new mafOpExtrusionHoles();
	extrusion->TestModeOn();
	extrusion->SetInput(cube);
	extrusion->OpRun();
	extrusion->ExtractFreeEdge();

	//In the cube there is an hole with 4 points
	CPPUNIT_ASSERT(extrusion->GetExtractFreeEdgesNumeberOfPoints()==4);

	delete wxLog::SetActiveTarget(NULL);

	mafDEL(extrusion);
	mafDEL(importer);
	mafDEL(storage);
}
//-----------------------------------------------------------
void mafOpExtrusionHolesTest::TestExtrude() 
//-----------------------------------------------------------
{
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *root=storage->GetRoot();

	//Imput data is a cube with 1 hole
	mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
	importer->TestModeOn();
	importer->SetInput(root);
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Extrusion/extrusion_cube.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	mafVMESurface *cube=mafVMESurface::SafeDownCast(importer->GetOutput());
	cube->GetOutput()->GetVTKData()->Update();
	cube->Update();

	double bounds[6];

	vtkPolyData *polydata = vtkPolyData::SafeDownCast(cube->GetOutput()->GetVTKData());
	polydata->Update();
	
	polydata->GetBounds(bounds);

	mafOpExtrusionHoles *extrusion = new mafOpExtrusionHoles();
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
	resultPolydata->Update();

	resultPolydata->GetBounds(bounds);

  double epsilon = 0.01;
	CPPUNIT_ASSERT(bounds[1]-bounds[0] < 4+epsilon && bounds[1]-bounds[0] > 4-epsilon && bounds[3]-bounds[2] < 4+epsilon && bounds[3]-bounds[2] > 4-epsilon && bounds[5]-bounds[4] < 24.37+epsilon && bounds[5]-bounds[4] > 24.37-epsilon);
	
	cube = NULL;

	delete wxLog::SetActiveTarget(NULL);
	mafDEL(extrusion);
	mafDEL(importer);
	mafDEL(storage);
}
