/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVMEDataSetAttributesTest
 Authors: Stefano Perticoni
 
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

#include "albaOpImporterVMEDataSetAttributesTest.h"
#include "albaOpImporterMesh.h"
#include "albaOpImporterVMEDataSetAttributes.h"

#include "albaString.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEMesh.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaSmartPointer.h"

#include "vtkALBASmartPointer.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"

#include <iostream>


void albaOpImporterVMEDataSetAttributesTest::TestFixture()
{
}


void albaOpImporterVMEDataSetAttributesTest::TestImportAttributes()
{
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/GenericMesh/";

  albaString nodesFileName = dirPrefix;
  nodesFileName << "NLISTtet10.lis";

  albaString elementsFileName = dirPrefix;
  elementsFileName << "ELISTtet10.lis";

  albaOpImporterMesh *meshImporter=new albaOpImporterMesh("mesh importer");
	meshImporter->TestModeOn();

  meshImporter->SetNodesFileName(nodesFileName.GetCStr());
  meshImporter->SetElementsFileName(elementsFileName.GetCStr());
	meshImporter->SetImportMaterials(false);
  meshImporter->Read();

	albaVME *node=meshImporter->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("albaVMEMesh"));

  albaVMEMesh *mesh = albaVMEMesh::SafeDownCast(node);

  vtkDataSet* data = NULL;
  data = mesh->GetUnstructuredGridOutput()->GetVTKData();
	
  int cells=data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells==2);

	int points=data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points==14);

  albaString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_element.lis";

  albaOpImporterVMEDataSetAttributes* attributesImporter = NULL;
  attributesImporter = new albaOpImporterVMEDataSetAttributes("attributes importer");
  attributesImporter->TestModeOn();

  attributesImporter->SetInput(mesh);
  attributesImporter->SetFileName(attributesFileName.GetCStr());
  attributesImporter->SetUseTSFile(false);
  attributesImporter->SetDataTypeToCellData();
  attributesImporter->Read();

  node=attributesImporter->GetOutput();

  CPPUNIT_ASSERT(node->IsA("albaVMEMesh"));

  mesh = albaVMEMesh::SafeDownCast(node);

  data = NULL;
  data = mesh->GetUnstructuredGridOutput()->GetVTKData();

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
	albaDEL(storage);
}

void albaOpImporterVMEDataSetAttributesTest::TestConstructor()
{

  albaOpImporterVMEDataSetAttributes *meshImporter=new albaOpImporterVMEDataSetAttributes("mesh importer");
  meshImporter->TestModeOn();
  CPPUNIT_ASSERT(meshImporter->GetOutput() == NULL);

  delete meshImporter;
}
