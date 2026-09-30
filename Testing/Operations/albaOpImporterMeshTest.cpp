/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMeshTest
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
#include "albaOpImporterMeshTest.h"

#include "albaOpImporterMesh.h"
#include "albaString.h"

#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEMesh.h"
#include "albaVME.h"
#include "albaVMEIterator.h"

#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"

#include "vtkDataSet.h"

#include <iostream>


void albaOpImporterMeshTest::TestFixture()
{
}

void albaOpImporterMeshTest::TestImportGenericMesh()
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/GenericMesh/";

  albaString nodesFileName = dirPrefix;
  nodesFileName << "NLISTtet4.lis";

  albaString elementsFileName = dirPrefix;
  elementsFileName << "ELISTtet4.lis";

  albaOpImporterMesh *meshImporter=new albaOpImporterMesh("mesh importer");
  meshImporter->TestModeOn();

  meshImporter->SetNodesFileName(nodesFileName.GetCStr());
  meshImporter->SetElementsFileName(elementsFileName.GetCStr());
	meshImporter->SetImportMaterials(false);

  meshImporter->Read();

  albaVME *node=meshImporter->GetOutput();

  CPPUNIT_ASSERT(node->IsA("albaVMEMesh"));
  vtkDataSet* data;

  data = albaVMEMesh::SafeDownCast(node)->GetUnstructuredGridOutput()->GetVTKData();

  int cells=data->GetNumberOfCells();
  CPPUNIT_ASSERT(cells==2);

  int points=data->GetNumberOfPoints();
  CPPUNIT_ASSERT(points==5);

  node = NULL;
  data = NULL;

  albaDEL(meshImporter);
  albaDEL(storage);
}


void albaOpImporterMeshTest::TestConstructor()
{
  albaOpImporterMesh *meshImporter=new albaOpImporterMesh("mesh importer");
  meshImporter->TestModeOn();
  CPPUNIT_ASSERT(meshImporter->GetOutput() == NULL);

  delete meshImporter;
}
