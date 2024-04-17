/*=========================================================================

Program: ALBA
Module: albaOpImporterAnsysInputFileTest.cpp
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include "albaOpImporterAnsysInputFileTest.h"
#include "albaOpImporterAnsysInputFile.h"
#include "albaVMEStorage.h"
#include "albaVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::TestAnsysInputFile1()
{
  ImportInputFile("tet10_v0.inp", 14, 2, 2);

  ImportInputFile("tet10_v1.inp", 14, 2, 2);

  ImportInputFile("tet10_v2.inp", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::TestAnsysInputFile2()
{
  ImportInputFile("tet10_v3.inp", 14, 2, 2);

  ImportInputFile("tet10_v4.inp", 14, 2, 2);

  ImportInputFile("tet10_v5.inp", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::TestAnsysInputFile3()
{
  ImportInputFile("Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar.inp", 86, 4, 17);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::TestAnsysInputFile4()
{
  ImportInputFile("Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarRotatedNodes.inp", 86, 4, 17);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::TestAnsysHypermeshInputFile()
{
  ImportInputFile("mesh_3_elementi.inp", 18, 0, 3);

  ImportInputFile("mesh_22_elementi.inp", 60, 0, 22);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::TestFileName()
{
  albaOpImporterAnsysInputFile *importer=new albaOpImporterAnsysInputFile("importer");
  importer->TestModeOn();

  albaString filename= ALBA_DATA_ROOT;
  filename<<"/albaOpImporterAnsysInputFileTest/tet10_v0.inp";
  importer->SetFileName(filename.GetCStr());
  
  // File name
  CPPUNIT_ASSERT(importer->GetFileName() == filename.GetCStr());

  cppDEL(importer);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysInputFileTest::ImportInputFile(albaString fileName, int numPntsExpected, int numMatsExpected, int numElemsExpected)
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

  albaOpImporterAnsysInputFile *importer=new albaOpImporterAnsysInputFile("importer");
  importer->TestModeOn();

  albaString filePath= ALBA_DATA_ROOT;
  filePath<<"/albaOpImporterAnsysInputFileTest/" << fileName;
  importer->SetFileName(filePath.GetCStr());
	importer->SetInput(root);
  importer->Import();

  // Imported Mesh
  albaVMEMesh *importedMesh = albaVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedMesh != NULL);

  importedMesh->Update();
  importedMesh->TestModeOn();

  importedMesh->GetUnstructuredGridOutput()->Update();
  vtkUnstructuredGrid *inputUGrid = importedMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  // Nodes
  vtkIntArray *nodesIDArray = importedMesh->GetNodesIDArray();

  CPPUNIT_ASSERT(nodesIDArray != NULL);

  int numPoints = inputUGrid->GetNumberOfPoints();

  CPPUNIT_ASSERT(numPoints == numPntsExpected); // Total points

	// Materials
	if (numMatsExpected != 0)
	{
		vtkDataArray *materialsIDArray = NULL;

		// try field data
		materialsIDArray = inputUGrid->GetFieldData()->GetArray("material_id");

		CPPUNIT_ASSERT(materialsIDArray != NULL);

		int numberOfMaterials = materialsIDArray->GetNumberOfTuples();

		CPPUNIT_ASSERT(numberOfMaterials == numMatsExpected); // Total materials
	}
  // Elements
  int numberOfElements = inputUGrid->GetNumberOfCells();

  CPPUNIT_ASSERT(numberOfElements == numElemsExpected); // Total elements

  cppDEL(importer);
	albaDEL(storage);
}