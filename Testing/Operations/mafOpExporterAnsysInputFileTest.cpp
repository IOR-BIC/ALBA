/*=========================================================================

Program: MAF2
Module: mafOpExporterAnsysInputFileTest.cpp
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

#include "mafOpExporterAnsysInputFileTest.h"
#include "mafOpImporterAnsysInputFile.h"
#include "mafOpExporterAnsysInputFile.h"
#include "mafVMEStorage.h"
#include "mafVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysInputFileTest::TestAnsysINPFile_FileExist()
{
  Check_INPFile("mesh1.inp");
}

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysInputFileTest::TestAnsysINPFile_ReadData()
{
  CompareINPFilesData("mesh3.inp");
}

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysInputFileTest::Check_INPFile(mafString fileName)
{
	mafVMEStorage *storage = mafVMEStorage::New();
	mafNode *root = (mafNode *)storage->GetRoot();

  mafString filePath= MAF_DATA_ROOT;
  filePath<<"/mafOpExporterAnsysInputFileTest/";

  // Import
  mafOpImporterAnsysInputFile *importer = new mafOpImporterAnsysInputFile("importer");
  importer->TestModeOn();
  importer->SetFileName((filePath << fileName).GetCStr());
	importer->SetInput(root);
  importer->Import();

  mafVMEMesh *importedData = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData != NULL);

  importedData->TestModeOn();
  importedData->Update();

  // Export
  mafOpExporterAnsysInputFile *exporter = new mafOpExporterAnsysInputFile();
  exporter->TestModeOn();

  filePath="";
  filePath << MAF_DATA_ROOT << "/mafOpExporterAnsysInputFileTest/";

  exporter->SetOutputFileName((filePath << "exported_" << fileName).GetCStr());
  exporter->SetInput(importedData);
  int result = exporter->Write();

  CPPUNIT_ASSERT(result == MAF_OK);

  // Import
  importer->SetFileName(filePath.GetCStr());
  importer->Import();

  mafVMEMesh *importedData2 = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData2 != NULL);

  cppDEL(importer);
  cppDEL(exporter);
	mafDEL(storage);
}

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysInputFileTest::CompareINPFilesData(mafString fileName)
{
	mafVMEStorage *storage = mafVMEStorage::New();
	mafNode *root = (mafNode *)storage->GetRoot();

  mafString filePath= MAF_DATA_ROOT;
  filePath<<"/mafOpExporterAnsysInputFileTest/";

  // Import
  mafOpImporterAnsysInputFile *importer = new mafOpImporterAnsysInputFile("importer");
  importer->TestModeOn();
  importer->SetFileName((filePath << fileName).GetCStr());
	importer->SetInput(root);
  importer->Import();

  mafVMEMesh *importedData = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData != NULL);

  importedData->TestModeOn();
  importedData->Update();

  // Export
  mafOpExporterAnsysInputFile *exporter = new mafOpExporterAnsysInputFile();
  exporter->TestModeOn();

  filePath="";
  filePath << MAF_DATA_ROOT << "/mafOpExporterAnsysInputFileTest/";

  exporter->SetOutputFileName((filePath << "exported2_" << fileName).GetCStr());
  exporter->SetInput(importedData);
  int result = exporter->Write();

  CPPUNIT_ASSERT(result == MAF_OK);

  // Import
  importer->SetFileName(filePath.GetCStr());
  importer->Import();

  mafVMEMesh *importedData2 = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData2 != NULL);

  //////////////////////////////////////////////////////////////////////////

  importedData->GetUnstructuredGridOutput()->Update();
  vtkUnstructuredGrid *inputUGrid = importedData->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  inputUGrid->Update();

  // Nodes
  vtkIntArray *nodesIDArray = importedData->GetNodesIDArray();

  int numPoints = inputUGrid->GetNumberOfPoints();

  // Materials
  vtkDataArray *materialsIDArray = NULL;

  // try field data
  materialsIDArray = inputUGrid->GetFieldData()->GetArray("material_id");

  int numberOfMaterials = materialsIDArray->GetNumberOfTuples();

  // Elements
  int numberOfElements = inputUGrid->GetNumberOfCells();

  //
  importedData2->GetUnstructuredGridOutput()->Update();
  vtkUnstructuredGrid *inputUGrid2 = importedData2->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  inputUGrid2->Update();

  // Nodes
  vtkIntArray *nodesIDArray2 = importedData2->GetNodesIDArray();

  int numPoints2 = inputUGrid2->GetNumberOfPoints();

  // Materials
  vtkDataArray *materialsIDArray2 = NULL;

  // try field data
  materialsIDArray2 = inputUGrid2->GetFieldData()->GetArray("material_id");

  int numberOfMaterials2 = materialsIDArray2->GetNumberOfTuples();

  // Elements
  int numberOfElements2 = inputUGrid2->GetNumberOfCells();

  CPPUNIT_ASSERT(numPoints == numPoints2);
  CPPUNIT_ASSERT(numberOfMaterials == numberOfMaterials2);
  CPPUNIT_ASSERT(numberOfElements == numberOfElements2);

  cppDEL(importer);
  cppDEL(exporter);
	mafDEL(storage);
}
