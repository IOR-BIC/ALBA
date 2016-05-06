/*=========================================================================

 Program: MAF2
 Module: mafOpImporterSTLTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpImporterSTLTest.h"
#include "mafOpImporterSTL.h"
#include "mafString.h"

#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVME.h"
#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafVMESurface.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"

#include <vector>
#include <iostream>

void mafOpImporterSTLTest::Test()
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterSTL *importer1 = new mafOpImporterSTL("importer");
	importer1->TestModeOn();
  importer1->SetInput(storage->GetRoot());
	mafString filename = MAF_DATA_ROOT;
  filename<<"/STL/Cubo.stl";
	importer1->SetFileName(filename.GetCStr());
	importer1->OpRun();
  std::vector<mafVMESurface*> importedSTL;
  importer1->GetImportedSTL(importedSTL);
  mafVMESurface *node = importedSTL[0];
	
	CPPUNIT_ASSERT(node->IsA("mafVMESurface"));
	vtkDataSet *data = node->GetOutput()->GetVTKData();
	data->Update();
	int cells = data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells == 4);
	int points=data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points==4);

	mafString filename_binary=MAF_DATA_ROOT;
	filename_binary << "/STL/Cubo_Binary.stl";
	mafOpImporterSTL *importer2 = new mafOpImporterSTL();
	importer2->TestModeOn();
	importer2->SetInput(storage->GetRoot());
	importer2->SetFileName(filename_binary.GetCStr());
	importer2->OpRun();
	importer2->GetImportedSTL(importedSTL);
  node = importedSTL[0];

	CPPUNIT_ASSERT(node->IsA("mafVMESurface"));
	data = node->GetOutput()->GetVTKData();
	data->Update();
	cells = data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells == 4);
	points = data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points == 4);

	node = NULL;
	data = NULL;
	mafDEL(importer1);
	mafDEL(importer2);
	mafDEL(storage);
}
