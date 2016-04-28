/*=========================================================================

Program: MAF2
Module: mafOpExporterAnsysCDBFileTest.cpp
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

#include "mafOpExporterAnsysCDBFileTest.h"
#include "mafOpImporterAnsysCDBFile.h"
#include "mafOpExporterAnsysCDBFile.h"
#include "mafVMEStorage.h"
#include "mafVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysCDBFileTest::TestAnsysCDBFile_FileExist()
{
  Check_CDBFile("mesh1.cdb");
}

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysCDBFileTest::TestAnsysCDBFile_ReadData()
{
  CompareCDBFilesData("mesh2.cdb");
}

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysCDBFileTest::TestAnsysCDBFile_AutoFillData()
{
  CompareCDBFilesData("mesh3.cdb");
}

//----------------------------------------------------------------------------------------
void mafOpExporterAnsysCDBFileTest::Check_CDBFile(mafString fileName)
{
	mafVMEStorage *storage = mafVMEStorage::New();
	mafVME *root = (mafVME *)storage->GetRoot();

  mafString filePath= MAF_DATA_ROOT;
  filePath<<"/mafOpExporterAnsysCDBFileTest/";

  // Import
  mafOpImporterAnsysCDBFile *importer = new mafOpImporterAnsysCDBFile("importer");
  importer->TestModeOn();
  importer->SetFileName((filePath << fileName).GetCStr());
	importer->SetInput(root);
  importer->Import();

  mafVMEMesh *importedData = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData != NULL);

  importedData->TestModeOn();
  importedData->Update();

  // Export
  mafOpExporterAnsysCDBFile *exporter = new mafOpExporterAnsysCDBFile();
  exporter->TestModeOn();

  filePath="";
  filePath << MAF_DATA_ROOT << "/mafOpExporterAnsysCDBFileTest/";

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
void mafOpExporterAnsysCDBFileTest::CompareCDBFilesData(mafString fileName)
{
	mafVMEStorage *storage = mafVMEStorage::New();
	mafVME *root = (mafVME *)storage->GetRoot();

  mafString filePath= MAF_DATA_ROOT;
  filePath<<"/mafOpExporterAnsysCDBFileTest/";

  // Import
  mafOpImporterAnsysCDBFile *importer = new mafOpImporterAnsysCDBFile("importer");
  importer->TestModeOn();
  importer->SetFileName((filePath << fileName).GetCStr());
	importer->SetInput(root);
  importer->Import();

  mafVMEMesh *importedData = mafVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData != NULL);

  importedData->TestModeOn();
  importedData->Update();

  // Export
  mafOpExporterAnsysCDBFile *exporter = new mafOpExporterAnsysCDBFile();
  exporter->TestModeOn();

  filePath="";
  filePath << MAF_DATA_ROOT << "/mafOpExporterAnsysCDBFileTest/";

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
  materialsIDArray = importedData->GetMaterialsIDArray();

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
  materialsIDArray2 = importedData2->GetMaterialsIDArray();

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