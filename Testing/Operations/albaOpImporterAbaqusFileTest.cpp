/*=========================================================================
Program:   ALBA
Module:    albaOpImporterAbaqusFileTest.cpp
Language:  C++
Date:      $Date: 2009-05-19 14:29:53 $
Version:   $Revision: 1.1 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "albaOpImporterAbaqusFileTest.h"
#include "albaOpImporterAbaqusFile.h"
#include "albaVMEStorage.h"
#include "albaVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::TestAbaqusFile1()
{
	ImportInputFile("tet10_v0.inp", 14, 1, 2);

	ImportInputFile("tet10_v1.inp", 14, 1, 2);

	ImportInputFile("tet10_v2.inp", 14, 1, 2);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::TestAbaqusFile2()
{
	ImportInputFile("tet10_v3.inp", 14, 1, 2);

	ImportInputFile("tet10_v4.inp", 14, 1, 2);

	ImportInputFile("tet10_v5.inp", 14, 1, 2);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::TestAbaqusFile3()
{
  ImportInputFile("Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar.inp", 86, 4, 17);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::TestAbaqusFile4()
{
  ImportInputFile("Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarRotatedNodes.inp", 86, 4, 17);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::TestAnsysHypermeshInputFile()
{
  ImportInputFile("mesh_3_elementi.inp", 18, 0, 3);

  ImportInputFile("mesh_22_elementi.inp", 60, 0, 22);

	ImportInputFile("mesh_10_elementi_generate.inp", 18, 2, 10);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::TestFileName()
{
	albaOpImporterAbaqusFile *importer=new albaOpImporterAbaqusFile("importer");
  importer->TestModeOn();

  albaString filename= ALBA_DATA_ROOT;
  filename<<"/albaOpImporterAbaqusFileTest/tet10_v0.inp";
  importer->SetFileName(filename.GetCStr());
  
  // File name
  CPPUNIT_ASSERT(importer->GetFileName() == filename.GetCStr());

  cppDEL(importer);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAbaqusFileTest::ImportInputFile(albaString fileName, int numPntsExpected, int numMatsExpected, int numElemsExpected)
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaOpImporterAbaqusFile *importer=new albaOpImporterAbaqusFile("importer");
  importer->TestModeOn();

  albaString filePath= ALBA_DATA_ROOT;
  filePath<<"/albaOpImporterAbaqusFileTest/" << fileName;
  importer->SetFileName(filePath.GetCStr());
	importer->SetInput(root);
  importer->ImportFile();

  // Imported Mesh
  albaVMEMesh *importedMesh = albaVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedMesh != NULL);

  importedMesh->Update();
  importedMesh->TestModeOn();

  importedMesh->GetUnstructuredGridOutput()->Update();
  vtkUnstructuredGrid *inputUGrid = importedMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  inputUGrid->Update();

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