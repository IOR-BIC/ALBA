/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysInputFileTest.cpp
Authors: Nicola Vanella

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

#include "mafOpImporterAnsysInputFileTest.h"
#include "mafOpImporterAnsysInputFile.h"
#include "mafVMEStorage.h"
#include "mafVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::TestAnsysInputFile1()
{
  ImportInputFile("tet10_v0.inp", 14, 2, 2);

  ImportInputFile("tet10_v1.inp", 14, 2, 2);

  ImportInputFile("tet10_v2.inp", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::TestAnsysInputFile2()
{
  ImportInputFile("tet10_v3.inp", 14, 2, 2);

  ImportInputFile("tet10_v4.inp", 14, 2, 2);

  ImportInputFile("tet10_v5.inp", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::TestAnsysInputFile3()
{
  ImportInputFile("Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar.inp", 86, 4, 17);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::TestAnsysInputFile4()
{
  ImportInputFile("Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarRotatedNodes.inp", 86, 4, 17);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::TestAnsysHypermeshInputFile()
{
  ImportInputFile("mesh_3_elementi.inp", 18, 0, 3);

  ImportInputFile("mesh_22_elementi.inp", 60, 0, 22);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::TestFileName()
{
  mafOpImporterAnsysInputFile *importer=new mafOpImporterAnsysInputFile("importer");
  importer->TestModeOn();

  mafString filename= MAF_DATA_ROOT;
  filename<<"/mafOpImporterAnsysInputFileTest/tet10_v0.inp";
  importer->SetFileName(filename.GetCStr());
  
  // File name
  CPPUNIT_ASSERT(importer->GetFileName() == filename.GetCStr());

  cppDEL(importer);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysInputFileTest::ImportInputFile(mafString fileName, int numPntsExpected, int numMatsExpected, int numElemsExpected)
{
	mafVMEStorage *storage = mafVMEStorage::New();
	mafNode *root = (mafNode *)storage->GetRoot();

  mafOpImporterAnsysInputFile *importer=new mafOpImporterAnsysInputFile("importer");
  importer->TestModeOn();

  mafString filePath= MAF_DATA_ROOT;
  filePath<<"/mafOpImporterAnsysInputFileTest/" << fileName;
  importer->SetFileName(filePath.GetCStr());
	importer->SetInput(root);
  importer->Import();

  // Imported Mesh
  mafVMEMesh *importedMesh = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

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
  vtkDataArray *materialsIDArray = NULL;

  // try field data
  materialsIDArray = inputUGrid->GetFieldData()->GetArray("material_id");

  CPPUNIT_ASSERT(materialsIDArray != NULL);

  int numberOfMaterials = materialsIDArray->GetNumberOfTuples();

  CPPUNIT_ASSERT(numberOfMaterials == numMatsExpected); // Total materials

  // Elements
  int numberOfElements = inputUGrid->GetNumberOfCells();

  CPPUNIT_ASSERT(numberOfElements == numElemsExpected); // Total elements

  cppDEL(importer);
	mafDEL(storage);
}