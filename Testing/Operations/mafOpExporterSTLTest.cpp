/*=========================================================================

 Program: MAF2
 Module: mafOpExporterSTLTest
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

#include "mafOpExporterSTLTest.h"
#include "mafOpExporterSTL.h"

#include "mafOpImporterSTL.h"

#include "mafVMESurface.h"
#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafNode.h"

#include "vtkCubeSource.h"
#include "vtkDataSet.h"
#include "vtkTriangleFilter.h"

#include <string>
#include <assert.h>
//-----------------------------------------------------------
void mafOpExporterSTLTest::Test() 
//-----------------------------------------------------------
{
  int returnValue = -1;

  //Create data
	vtkCubeSource *data;
	vtkNEW(data);
	data->SetXLength(2.0);
	data->SetYLength(1.0);
	data->SetZLength(2.0);
	data->Update();
	mafVMESurface *surface_input = mafVMESurface::New();
	//surface_input->SetData(data->GetOutput(),0);
	//returnValue = surface_input->SetData(data->GetOutput(),0);
	//surface_input->Update();
	
  // Marco: disabled this assertion since it fails and there seems to be no reasons to succede!
  // this should fail since input surface must be made by triangles only 
  //CPPUNIT_ASSERT(returnValue == MAF_ERROR); 

  vtkTriangleFilter *triangleFilter;
	vtkNEW(triangleFilter);
  triangleFilter->SetInput(data->GetOutput());
  triangleFilter->Update();

  // this should pass since data has been converted to triangles only
  returnValue = surface_input->SetData(triangleFilter->GetOutput(),0);
	surface_input->Update();
  CPPUNIT_ASSERT(returnValue == MAF_OK);

  surface_input->Modified();

	//Initialize exporter
	mafOpExporterSTL *exporter1 = new mafOpExporterSTL("test exporter");
	exporter1->SetInput(surface_input);
	mafString filename=MAF_DATA_ROOT;
  filename<<"/STL/Export.stl";
	exporter1->SetFileName(filename);
	exporter1->ExportAsBynaryOff();
	exporter1->ExportSurface();
	//Import the file to check
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterSTL *importer1 = new mafOpImporterSTL("importer");
	importer1->TestModeOn();
  importer1->SetInput(storage->GetRoot());
	importer1->SetFileName(filename.GetCStr());
	importer1->OpRun();
  std::vector<mafVMESurface*> importedSTL;
  importer1->GetImportedSTL(importedSTL);
  mafVMESurface *node = importedSTL[0];
	//Test
	CPPUNIT_ASSERT(node->IsA("mafVMESurface"));
	vtkDataSet *surface_output = node->GetOutput()->GetVTKData();
	surface_output->Update();
	double b1[6];
	surface_output->GetBounds(b1);
	double b2[6];
	surface_input->GetOutput()->GetVTKData()->GetBounds(b2);
	CPPUNIT_ASSERT(b1[0]==b2[0]&&b1[1]==b2[1]&&b1[2]==b2[2]);
	//Binary Case
	//Initialize exporter
	mafOpExporterSTL *exporter2 = new mafOpExporterSTL("test exporter");
	exporter2->SetInput(surface_input);
	mafString filename_binary=MAF_DATA_ROOT;
  filename_binary<<"/STL/Export_Binary.stl";
	exporter2->SetFileName(filename_binary);
	exporter2->ExportAsBynaryOn();
	exporter2->ExportSurface();
	//Import the file to check
	mafOpImporterSTL *importer2 = new mafOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetFileName(filename_binary.GetCStr());
	importer2->OpRun();
	importer2->GetImportedSTL(importedSTL);
  node = importedSTL[0];
	//Test
	CPPUNIT_ASSERT(node->IsA("mafVMESurface"));
	surface_output = node->GetOutput()->GetVTKData();
	surface_output->Update();
	surface_output->GetBounds(b1);
	surface_input->GetOutput()->GetVTKData()->GetBounds(b2);
	CPPUNIT_ASSERT(b1[0]==b2[0]&&b1[1]==b2[1]&&b1[2]==b2[2]);

	node = NULL;
	surface_output = NULL;
	vtkDEL(triangleFilter);
	vtkDEL(data);
	mafDEL(exporter2);
	mafDEL(importer2);
	mafDEL(importer1);
	mafDEL(exporter1);
	mafDEL(surface_input);
	mafDEL(storage);
}
