/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysInputFileTest.cpp
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

#include "albaOpExporterAnsysInputFileTest.h"
#include "albaOpImporterAnsysInputFile.h"
#include "albaOpExporterAnsysInputFile.h"
#include "albaVMEStorage.h"
#include "albaVMEMesh.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"

//----------------------------------------------------------------------------------------
void albaOpExporterAnsysInputFileTest::TestAnsysINPFile_FileExist()
{
  Check_INPFile("mesh1.inp");
}

//----------------------------------------------------------------------------------------
void albaOpExporterAnsysInputFileTest::TestAnsysINPFile_ReadData()
{
  CompareINPFilesData("mesh3.inp");
}

//----------------------------------------------------------------------------------------
void albaOpExporterAnsysInputFileTest::Check_INPFile(albaString fileName)
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

  albaString filePath= ALBA_DATA_ROOT;
  filePath<<"/albaOpExporterAnsysInputFileTest/";

  // Import
  albaOpImporterAnsysInputFile *importer = new albaOpImporterAnsysInputFile("importer");
  importer->TestModeOn();
  importer->SetFileName((filePath << fileName).GetCStr());
	importer->SetInput(root);
  importer->Import();

  albaVMEMesh *importedData = albaVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData != NULL);

  importedData->TestModeOn();
  importedData->Update();

  // Export
  albaOpExporterAnsysInputFile *exporter = new albaOpExporterAnsysInputFile();
  exporter->TestModeOn();

  filePath="";
  filePath << GET_TEST_DATA_DIR();

  exporter->SetOutputFileName((filePath << "/exported_" << fileName).GetCStr());
  exporter->SetInput(importedData);
	exporter->Init();
	int result = exporter->Write();

  CPPUNIT_ASSERT(result == ALBA_OK);

  // Import
  importer->SetFileName(filePath.GetCStr());
  importer->Import();

  albaVMEMesh *importedData2 = albaVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData2 != NULL);

  cppDEL(importer);
  cppDEL(exporter);
	albaDEL(storage);
}

//----------------------------------------------------------------------------------------
void albaOpExporterAnsysInputFileTest::CompareINPFilesData(albaString fileName)
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

  albaString filePath= ALBA_DATA_ROOT;
  filePath<<"/albaOpExporterAnsysInputFileTest/";

  // Import
  albaOpImporterAnsysInputFile *importer = new albaOpImporterAnsysInputFile("importer");
  importer->TestModeOn();
  importer->SetFileName((filePath << fileName).GetCStr());
	importer->SetInput(root);
  importer->Import();

  albaVMEMesh *importedData = albaVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData != NULL);

  importedData->TestModeOn();
  importedData->Update();

  // Export
  albaOpExporterAnsysInputFile *exporter = new albaOpExporterAnsysInputFile();
  exporter->TestModeOn();

  filePath="";
  filePath << GET_TEST_DATA_DIR();

  exporter->SetOutputFileName((filePath << "/exported2_" << fileName).GetCStr());
  exporter->SetInput(importedData);
	exporter->Init();
  int result = exporter->Write();

  CPPUNIT_ASSERT(result == ALBA_OK);

  // Import
  importer->SetFileName(filePath.GetCStr());
  importer->Import();

  albaVMEMesh *importedData2 = albaVMEMesh::SafeDownCast(importer->GetInput()->GetFirstChild());

  CPPUNIT_ASSERT(importedData2 != NULL);

  //////////////////////////////////////////////////////////////////////////

  importedData->GetUnstructuredGridOutput()->Update();
  vtkUnstructuredGrid *inputUGrid = importedData->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  inputUGrid->Update();

  // Nodes
  vtkIntArray *nodesIDArray = importedData->GetNodesIDArray();

  int numPoints = inputUGrid->GetNumberOfPoints();

  // Elements
  int numberOfElements = inputUGrid->GetNumberOfCells();

  //
  importedData2->GetUnstructuredGridOutput()->Update();
  vtkUnstructuredGrid *inputUGrid2 = importedData2->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  inputUGrid2->Update();

  // Nodes
  vtkIntArray *nodesIDArray2 = importedData2->GetNodesIDArray();

  int numPoints2 = inputUGrid2->GetNumberOfPoints();

  // Elements
  int numberOfElements2 = inputUGrid2->GetNumberOfCells();

  CPPUNIT_ASSERT(numPoints == numPoints2);
  CPPUNIT_ASSERT(numberOfElements == numberOfElements2);

	//
	vtkDataArray *arrayE = inputUGrid->GetCellData()->GetArray("EX");
	vtkDataArray *arrayMaterial = inputUGrid->GetCellData()->GetArray("Material");
	vtkDataArray *arrayPoisson = inputUGrid->GetCellData()->GetArray("NUXY");
	vtkDataArray *arrayDens = inputUGrid->GetCellData()->GetArray("DENS");

	vtkDataArray *arrayE2 = inputUGrid2->GetCellData()->GetArray("EX");
	vtkDataArray *arrayMaterial2 = inputUGrid2->GetCellData()->GetArray("Material");
	vtkDataArray *arrayPoisson2 = inputUGrid2->GetCellData()->GetArray("NUXY");
	vtkDataArray *arrayDens2 = inputUGrid2->GetCellData()->GetArray("DENS");

	//
	bool existProps = (arrayE != NULL && arrayE2 != NULL);
	CPPUNIT_ASSERT((arrayE == NULL && arrayE2 == NULL) || existProps);

	bool equalProp = true;

	if (existProps)
	{
		for (int i = 0; i < numberOfElements; i++)
		{
			equalProp = arrayE->GetTuple(i)[0] == arrayE2->GetTuple(i)[0];

			if (!equalProp)
				break;
		}

		CPPUNIT_ASSERT(equalProp);
	}

	//
	existProps = (arrayMaterial != NULL && arrayMaterial2 != NULL);
	CPPUNIT_ASSERT((arrayMaterial == NULL && arrayMaterial2 == NULL) || existProps);


	if (existProps)
	{
		equalProp = true;
		for (int i = 0; i < numberOfElements; i++)
		{
			equalProp = arrayMaterial->GetTuple(i)[0] == arrayMaterial2->GetTuple(i)[0];

			if (!equalProp)
				break;
		}

		CPPUNIT_ASSERT(equalProp);
	}

	//
	existProps = (arrayPoisson != NULL && arrayPoisson2 != NULL);
	CPPUNIT_ASSERT((arrayPoisson == NULL && arrayPoisson2 == NULL)|| existProps);

	if (existProps)
	{
		equalProp = true;
		for (int i = 0; i < numberOfElements; i++)
		{
			equalProp = arrayPoisson->GetTuple(i)[0] == arrayPoisson2->GetTuple(i)[0];

			if (!equalProp)
				break;
		}

		CPPUNIT_ASSERT(equalProp);
	}

	//
	existProps = (arrayDens != NULL && arrayDens2 != NULL);
	CPPUNIT_ASSERT((arrayDens == NULL && arrayDens2 == NULL) || existProps);

	if (existProps)
	{
		equalProp = true;
		for (int i = 0; i < numberOfElements; i++)
		{
			equalProp = arrayDens->GetTuple(i)[0] == arrayDens2->GetTuple(i)[0];

			if (!equalProp)
				break;
		}

		CPPUNIT_ASSERT(equalProp);
	}

  cppDEL(importer);
  cppDEL(exporter);
	albaDEL(storage);
}
