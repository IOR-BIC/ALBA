/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExtrusionHolesTest.cpp,v $
Language:  C++
Date:      $Date: 2008-04-28 09:00:28 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medOpExtrusionHolesTest.h"
#include "medOpExtrusionHoles.h"

#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafOpImporterVTK.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------
void medOpExtrusionHolesTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	medOpExtrusionHoles *extrusion = new medOpExtrusionHoles();
	mafDEL(extrusion);
}
//-----------------------------------------------------------
void medOpExtrusionHolesTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
	medOpExtrusionHoles extrusion; 
}
//-----------------------------------------------------------
void medOpExtrusionHolesTest::TestSetGetExtrusionFactor() 
//-----------------------------------------------------------
{
	medOpExtrusionHoles *extrusion = new medOpExtrusionHoles();
	extrusion->SetExtrusionFactor(4.0);
	CPPUNIT_ASSERT(extrusion->GetExtrusionFactor()==4.0);
	mafDEL(extrusion);
}
//-----------------------------------------------------------
void medOpExtrusionHolesTest::TestExtractFreeEdge() 
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
	mafString filename=MED_DATA_ROOT;
	filename<<"/Extrusion/extrusion_cube.vtk";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	mafVMESurface *cube=mafVMESurface::SafeDownCast(importer->GetOutput());
	cube->GetOutput()->GetVTKData()->Update();
	cube->Update();

	medOpExtrusionHoles *extrusion = new medOpExtrusionHoles();
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
void medOpExtrusionHolesTest::TestExtrude() 
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
	mafString filename=MED_DATA_ROOT;
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

	medOpExtrusionHoles *extrusion = new medOpExtrusionHoles();
	extrusion->TestModeOn();
	extrusion->SetInput(cube);
	extrusion->SetExtrusionFactor(4.0);
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
	CPPUNIT_ASSERT(bounds[1]-bounds[0] < 4+epsilon && bounds[1]-bounds[0] > 4-epsilon && bounds[3]-bounds[2] < 4+epsilon && bounds[3]-bounds[2] > 4-epsilon && bounds[5]-bounds[4] < 8+epsilon && bounds[5]-bounds[4] > 8-epsilon);
	
	cube = NULL;

	delete wxLog::SetActiveTarget(NULL);
	mafDEL(extrusion);
	mafDEL(importer);
	mafDEL(storage);
}
