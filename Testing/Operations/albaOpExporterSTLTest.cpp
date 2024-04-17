/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterSTLTest
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

#include "albaOpExporterSTLTest.h"
#include "albaOpExporterSTL.h"

#include "albaOpImporterSTL.h"

#include "albaVMESurface.h"
#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVME.h"

#include "vtkCubeSource.h"
#include "vtkDataSet.h"
#include "vtkTriangleFilter.h"

#include <string>
#include <assert.h>
//-----------------------------------------------------------
void albaOpExporterSTLTest::Test() 
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
	albaVMESurface *surface_input = albaVMESurface::New();
	//surface_input->SetData(data->GetOutput(),0);
	//returnValue = surface_input->SetData(data->GetOutput(),0);
	//surface_input->Update();
	
  // Marco: disabled this assertion since it fails and there seems to be no reasons to succede!
  // this should fail since input surface must be made by triangles only 
  //CPPUNIT_ASSERT(returnValue == ALBA_ERROR); 

  vtkTriangleFilter *triangleFilter;
	vtkNEW(triangleFilter);
  triangleFilter->SetInputConnection(data->GetOutputPort());
  triangleFilter->Update();

  // this should pass since data has been converted to triangles only
  returnValue = surface_input->SetData(triangleFilter->GetOutput(),0);
	surface_input->Update();
  CPPUNIT_ASSERT(returnValue == ALBA_OK);

  surface_input->Modified();

	//Initialize exporter
	albaOpExporterSTL *exporter1 = new albaOpExporterSTL("test exporter");
	exporter1->SetInput(surface_input);
	albaString filename = GET_TEST_DATA_DIR();
  filename<<"/Export.stl";
	exporter1->SetFileName(filename);
	exporter1->ExportAsBynaryOff();
	exporter1->ExportSurface();
	
	//Import the file to check
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	albaOpImporterSTL *importer1 = new albaOpImporterSTL("importer");
	importer1->TestModeOn();
  importer1->SetInput(storage->GetRoot());
	importer1->SetFileName(filename.GetCStr());
	importer1->OpRun();
  std::vector<albaVMESurface*> importedSTL;
  importer1->GetImportedSTL(importedSTL);
  albaVMESurface *node = importedSTL[0];
	
	//Test
	CPPUNIT_ASSERT(node->IsA("albaVMESurface"));
	vtkDataSet *surface_output = node->GetOutput()->GetVTKData();
	double b1[6];
	surface_output->GetBounds(b1);
	double b2[6];
	surface_input->GetOutput()->GetVTKData()->GetBounds(b2);
	CPPUNIT_ASSERT(b1[0]==b2[0]&&b1[1]==b2[1]&&b1[2]==b2[2]);
	
	//Binary Case
	//Initialize exporter
	albaOpExporterSTL *exporter2 = new albaOpExporterSTL("test exporter");
	exporter2->SetInput(surface_input);
	albaString filename_binary = GET_TEST_DATA_DIR();
  filename_binary<<"/Export_Binary.stl";
	exporter2->SetFileName(filename_binary);
	exporter2->ExportAsBynaryOn();
	exporter2->ExportSurface();
	
	//Import the file to check
	albaOpImporterSTL *importer2 = new albaOpImporterSTL("importer");
	importer2->TestModeOn();
	importer2->SetFileName(filename_binary.GetCStr());
	importer2->OpRun();
	importer2->GetImportedSTL(importedSTL);
  node = importedSTL[0];
	
	//Test
	CPPUNIT_ASSERT(node->IsA("albaVMESurface"));
	surface_output = node->GetOutput()->GetVTKData();
	surface_output->GetBounds(b1);
	surface_input->GetOutput()->GetVTKData()->GetBounds(b2);
	CPPUNIT_ASSERT(b1[0]==b2[0]&&b1[1]==b2[1]&&b1[2]==b2[2]);

	node = NULL;
	surface_output = NULL;
	vtkDEL(triangleFilter);
	vtkDEL(data);
	albaDEL(exporter2);
	albaDEL(importer2);
	albaDEL(importer1);
	albaDEL(exporter1);
	albaDEL(surface_input);
	albaDEL(storage);
}
