/*=========================================================================

Program: ALBA
Module: albaOpImporterAnsysCDBFileTest.cpp
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

#include "albaOpImporterAnsysCDBFileTest.h"
#include "albaOpImporterAnsysCDBFile.h"
#include "albaVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"
#include "albaVMEStorage.h"

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::TestAnsysCDBFileBlock()
{
  ImportCDBFile("tet10_2elem.cdb", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::TestAnsysCDBFileNoBlock()
{
  ImportCDBFile("mid_one.cdb", 57, 485, 15);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::TestAnsysHyperMeshFileSmall()
{
  ImportCDBFile("mesh_3_elementi.cdb", 18, 0, 3);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::TestAnsysHyperMeshFileMed()
{
  ImportCDBFile("mesh_22_elementi.cdb", 60, 0, 22);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::TestAnsysENotation()
{
  ImportCDBFile("tet10_2elem_ENotation.cdb", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::TestFileName()
{
  albaOpImporterAnsysCDBFile *importer=new albaOpImporterAnsysCDBFile("importer");
  importer->TestModeOn();

  albaString filename= ALBA_DATA_ROOT;
  filename<<"/albaOpImporterAnsysCDBFileTest/tet10_2elem.cdb";
  importer->SetFileName(filename.GetCStr());
  
  // File name
  CPPUNIT_ASSERT(importer->GetFileName() == filename.GetCStr());

  cppDEL(importer);
}
//----------------------------------------------------------------------------------------
void albaOpImporterAnsysCDBFileTest::ImportCDBFile(albaString fileName, int numPntsExpected, int numMatsExpected, int numElemsExpected)
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

  albaOpImporterAnsysCDBFile *importer=new albaOpImporterAnsysCDBFile("importer");
  importer->TestModeOn();

  albaString filePath= ALBA_DATA_ROOT;
  filePath<<"/albaOpImporterAnsysCDBFileTest/" << fileName;
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