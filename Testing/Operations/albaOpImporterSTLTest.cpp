/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterSTLTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaOpImporterSTLTest.h"
#include "albaOpImporterSTL.h"
#include "albaString.h"

#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVME.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMESurface.h"
#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"

#include "vtkDataSet.h"

#include <vector>
#include <iostream>

void albaOpImporterSTLTest::Test()
{
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	albaOpImporterSTL *importer1 = new albaOpImporterSTL("importer");
	importer1->TestModeOn();
  importer1->SetInput(storage->GetRoot());
	albaString filename = ALBA_DATA_ROOT;
  filename<<"/STL/Cubo.stl";
	importer1->SetFileName(filename);
	importer1->OpRun();
  std::vector<albaVMESurface*> importedSTL;
  importer1->GetImportedSTL(importedSTL);
  albaVMESurface *node = importedSTL[0];
	
	CPPUNIT_ASSERT(node->IsA("albaVMESurface"));
	vtkDataSet *data = node->GetOutput()->GetVTKData();
	data->Update();
	int cells = data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells == 4);
	int points=data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points==4);

	albaString filename_binary=ALBA_DATA_ROOT;
	filename_binary << "/STL/Cubo_Binary.stl";
	albaOpImporterSTL *importer2 = new albaOpImporterSTL();
	importer2->TestModeOn();
	importer2->SetInput(storage->GetRoot());
	importer2->SetFileName(filename_binary.GetCStr());
	importer2->OpRun();
	importer2->GetImportedSTL(importedSTL);
  node = importedSTL[0];

	CPPUNIT_ASSERT(node->IsA("albaVMESurface"));
	data = node->GetOutput()->GetVTKData();
	data->Update();
	cells = data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells == 4);
	points = data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points == 4);

	node = NULL;
	data = NULL;
	albaDEL(importer1);
	albaDEL(importer2);
	albaDEL(storage);
}
