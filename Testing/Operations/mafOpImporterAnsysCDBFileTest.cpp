/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysCDBFileTest.cpp
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

#include "mafOpImporterAnsysCDBFileTest.h"
#include "mafOpImporterAnsysCDBFile.h"
#include "mafVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"
#include "mafVMEStorage.h"

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::TestAnsysCDBFileBlock()
{
  ImportCDBFile("tet10_2elem.cdb", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::TestAnsysCDBFileNoBlock()
{
  ImportCDBFile("mid_one.cdb", 57, 485, 15);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::TestAnsysHyperMeshFileSmall()
{
  ImportCDBFile("mesh_3_elementi.cdb", 18, 0, 3);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::TestAnsysHyperMeshFileMed()
{
  ImportCDBFile("mesh_22_elementi.cdb", 60, 0, 22);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::TestAnsysENotation()
{
  ImportCDBFile("tet10_2elem_ENotation.cdb", 14, 2, 2);
}

//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::TestFileName()
{
  mafOpImporterAnsysCDBFile *importer=new mafOpImporterAnsysCDBFile("importer");
  importer->TestModeOn();

  mafString filename= MAF_DATA_ROOT;
  filename<<"/mafOpImporterAnsysCDBFileTest/tet10_2elem.cdb";
  importer->SetFileName(filename.GetCStr());
  
  // File name
  CPPUNIT_ASSERT(importer->GetFileName() == filename.GetCStr());

  cppDEL(importer);
}
//----------------------------------------------------------------------------------------
void mafOpImporterAnsysCDBFileTest::ImportCDBFile(mafString fileName, int numPntsExpected, int numMatsExpected, int numElemsExpected)
{
	mafVMEStorage *storage = mafVMEStorage::New();
	mafVME *root = (mafVME *)storage->GetRoot();

  mafOpImporterAnsysCDBFile *importer=new mafOpImporterAnsysCDBFile("importer");
  importer->TestModeOn();

  mafString filePath= MAF_DATA_ROOT;
  filePath<<"/mafOpImporterAnsysCDBFileTest/" << fileName;
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