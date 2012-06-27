/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMeshTest
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
#include "mafOpImporterMeshTest.h"

#include "mafOpImporterMesh.h"
#include "mafString.h"

#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEMesh.h"
#include "mafNode.h"
#include "mafNodeIterator.h"

#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"

#include <iostream>


void mafOpImporterMeshTest::TestFixture()
{
}

void mafOpImporterMeshTest::setUp()
{
}

void mafOpImporterMeshTest::tearDown()
{
}

void mafOpImporterMeshTest::TestImportMesh()
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet4/";

  mafString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  mafString elementsFileName = dirPrefix;
  elementsFileName << "ELIST.lis";

  mafString materialsFileName = dirPrefix;
  materialsFileName << "MPLIST.lis";

  mafOpImporterMesh *meshImporter=new mafOpImporterMesh("mesh importer");
	meshImporter->TestModeOn();

  meshImporter->SetNodesFileName(nodesFileName.GetCStr());
  meshImporter->SetElementsFileName(elementsFileName.GetCStr());
  meshImporter->SetMaterialsFileName(materialsFileName.GetCStr());

  meshImporter->Read();

	mafNode *node=meshImporter->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("mafVMEMesh"));
  vtkDataSet* data;

  data = mafVMEMesh::SafeDownCast(node)->GetUnstructuredGridOutput()->GetVTKData();
	data->Update();
	
  int cells=data->GetNumberOfCells();
	CPPUNIT_ASSERT(cells==2);

	int points=data->GetNumberOfPoints();
	CPPUNIT_ASSERT(points==5);

	node = NULL;
	data = NULL;
	
  mafDEL(meshImporter);
	mafDEL(storage);
}

void mafOpImporterMeshTest::TestConstructor()
{

  mafOpImporterMesh *meshImporter=new mafOpImporterMesh("mesh importer");
  meshImporter->TestModeOn();
  CPPUNIT_ASSERT(meshImporter->GetOutput() == NULL);

  delete meshImporter;
}
