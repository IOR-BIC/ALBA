/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVMEDataSetAttributesTest
 Authors: Stefano Perticoni
 
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

#include "mafOpImporterVMEDataSetAttributesTest.h"
#include "mafOpImporterMesh.h"
#include "mafOpImporterVMEDataSetAttributes.h"

#include "mafString.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEMesh.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafSmartPointer.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"

#include <iostream>


void mafOpImporterVMEDataSetAttributesTest::TestFixture()
{
}


void mafOpImporterVMEDataSetAttributesTest::TestImportAttributes()
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/GenericMesh/";

  mafString nodesFileName = dirPrefix;
  nodesFileName << "NLISTtet10.lis";

  mafString elementsFileName = dirPrefix;
  elementsFileName << "ELISTtet10.lis";

  mafOpImporterMesh *meshImporter=new mafOpImporterMesh("mesh importer");
	meshImporter->TestModeOn();

  meshImporter->SetNodesFileName(nodesFileName.GetCStr());
  meshImporter->SetElementsFileName(elementsFileName.GetCStr());

  meshImporter->Read();

	mafNode *node=meshImporter->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("mafVMEMesh"));

  mafVMEMesh *mesh = mafVMEMesh::SafeDownCast(node);

  vtkDataSet* data = NULL;
  data = mesh->GetUnstructuredGridOutput()->GetVTKData();
	data->Update();
	
  int cells=data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells==2);

	int points=data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points==14);

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_element.lis";

  mafOpImporterVMEDataSetAttributes* attributesImporter = NULL;
  attributesImporter = new mafOpImporterVMEDataSetAttributes("attributes importer");
  attributesImporter->TestModeOn();

  attributesImporter->SetInput(mesh);
  attributesImporter->SetFileName(attributesFileName.GetCStr());
  attributesImporter->SetUseTSFile(false);
  attributesImporter->SetDataTypeToCellData();
  attributesImporter->Read();

  node=attributesImporter->GetOutput();

  CPPUNIT_ASSERT(node->IsA("mafVMEMesh"));

  mesh = mafVMEMesh::SafeDownCast(node);

  data = NULL;
  data = mesh->GetUnstructuredGridOutput()->GetVTKData();
  data->Update();

  cells=data->GetNumberOfCells();
  CPPUNIT_ASSERT(cells==2);

  points=data->GetNumberOfPoints();
  CPPUNIT_ASSERT(points==14);

  CPPUNIT_ASSERT(data->GetCellData()->GetArray("EPTO1"));
  CPPUNIT_ASSERT(data->GetCellData()->GetArray("EPTO1")->GetNumberOfTuples() == 2);

  node = NULL;
	data = NULL;
	
  cppDEL(meshImporter);
  cppDEL(attributesImporter);
	mafDEL(storage);
}

void mafOpImporterVMEDataSetAttributesTest::TestConstructor()
{

  mafOpImporterVMEDataSetAttributes *meshImporter=new mafOpImporterVMEDataSetAttributes("mesh importer");
  meshImporter->TestModeOn();
  CPPUNIT_ASSERT(meshImporter->GetOutput() == NULL);

  delete meshImporter;
}
