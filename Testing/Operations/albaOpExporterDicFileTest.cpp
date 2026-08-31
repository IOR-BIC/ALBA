/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterDicFileTest
 Authors: Gianluigi Crimi

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

#include "albaOpExporterDicFileTest.h"
#include "albaOpExporterDicFile.h"
#include "albaOpImporterDICFile.h"

#include "albaString.h"
#include "albaVMEPointCloud.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaSmartPointer.h"

#include <string>
#include <assert.h>
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

//-----------------------------------------------------------
void albaOpExporterDicFileTest::TestExportDicFile()
{
  albaVMEStorage *storage = albaVMEStorage::New();
  albaVME *root = (albaVME *)storage->GetRoot();

  // Import the original DIC file
  albaOpImporterDicFile *importer = new albaOpImporterDicFile("importer");
  importer->TestModeOn();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/DIC/DicData.dat";
  importer->SetFileName(filename.GetCStr());
  importer->SetInput(root);
  importer->ImportFile();
  albaVMEPointCloud *inputNode = (albaVMEPointCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(inputNode != NULL);
  CPPUNIT_ASSERT(inputNode->GetNumberOfPoints() > 0);

  // Export the imported data
  albaOpExporterDicFile *exporter = new albaOpExporterDicFile("exporter");
  exporter->TestModeOn();

  albaString exportFilename = GET_TEST_DATA_DIR();
  exportFilename << "/DicExport.dat";
  exporter->SetFileName(exportFilename.GetCStr());
  exporter->SetInput(inputNode);
  exporter->ExportDIC();

  // Verify the exported file exists
  std::ifstream exportedFile(exportFilename.GetCStr());
  CPPUNIT_ASSERT(exportedFile.good());
  exportedFile.close();

  // Re-import the exported file
  albaOpImporterDicFile *reimporter = new albaOpImporterDicFile("reimporter");
  reimporter->TestModeOn();
  reimporter->SetFileName(exportFilename.GetCStr());

  albaVMEStorage *storage2 = albaVMEStorage::New();
  albaVME *root2 = (albaVME *)storage2->GetRoot();
  reimporter->SetInput(root2);
  reimporter->ImportFile();
  albaVMEPointCloud *outputNode = (albaVMEPointCloud *)reimporter->GetOutput();

  // Verify the data is preserved
  CPPUNIT_ASSERT(outputNode->GetNumberOfPoints() == inputNode->GetNumberOfPoints());

  // Check some point coordinates
  double inputXyz[3], outputXyz[3];
  for (int i = 0; i < inputNode->GetNumberOfPoints(); i++)
  {
    inputNode->GetPoint(i, inputXyz);
    outputNode->GetPoint(i, outputXyz);

    CPPUNIT_ASSERT(fabs(inputXyz[0] - outputXyz[0]) < 0.01);
    CPPUNIT_ASSERT(fabs(inputXyz[1] - outputXyz[1]) < 0.01);
    CPPUNIT_ASSERT(fabs(inputXyz[2] - outputXyz[2]) < 0.01);
  }

  albaDEL(storage);
  albaDEL(storage2);
  delete importer;
  delete exporter;
  delete reimporter;
}

//-----------------------------------------------------------
void albaOpExporterDicFileTest::TestExportDicFileWithABSMatrix()
{
  albaVMEStorage *storage = albaVMEStorage::New();
  albaVME *root = (albaVME *)storage->GetRoot();

  // Import the original DIC file
  albaOpImporterDicFile *importer = new albaOpImporterDicFile("importer");
  importer->TestModeOn();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/DIC/DicData.dat";
  importer->SetFileName(filename.GetCStr());
  importer->SetInput(root);
  importer->ImportFile();
  albaVMEPointCloud *inputNode = (albaVMEPointCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(inputNode != NULL);

  // Export with ABS Matrix flag
  albaOpExporterDicFile *exporter = new albaOpExporterDicFile("exporter");
  exporter->TestModeOn();
  exporter->ApplyABSMatrixOn();

  albaString exportFilename = GET_TEST_DATA_DIR();
  exportFilename << "/DicExportABS.dat";
  exporter->SetFileName(exportFilename.GetCStr());
  exporter->SetInput(inputNode);
  exporter->ExportDIC();

  // Verify the exported file exists
  std::ifstream exportedFile(exportFilename.GetCStr());
  CPPUNIT_ASSERT(exportedFile.good());
  exportedFile.close();

  albaDEL(storage);
  delete importer;
  delete exporter;
}

//-----------------------------------------------------------
void albaOpExporterDicFileTest::TestExportAndReimport()
{
  albaVMEStorage *storage = albaVMEStorage::New();
  albaVME *root = (albaVME *)storage->GetRoot();

  // Import the original DIC file
  albaOpImporterDicFile *importer = new albaOpImporterDicFile("importer");
  importer->TestModeOn();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/DIC/DicData.dat";
  importer->SetFileName(filename.GetCStr());
  importer->SetInput(root);
  importer->ImportFile();
  albaVMEPointCloud *inputNode = (albaVMEPointCloud *)importer->GetOutput();

  // Get scalar data from original
  inputNode->GetOutput()->Update();
  vtkPolyData *inputPolyData = vtkPolyData::SafeDownCast(inputNode->GetOutput()->GetVTKData());

  vtkDataArray *inputScalars = inputPolyData->GetPointData()->GetScalars("Deformed Coordinates");
  CPPUNIT_ASSERT(inputScalars != NULL);
  int numScalars = inputScalars->GetNumberOfTuples();

  // Export the data
  albaOpExporterDicFile *exporter = new albaOpExporterDicFile("exporter");
  exporter->TestModeOn();

  albaString exportFilename = GET_TEST_DATA_DIR();
  exportFilename << "/DicExportScalars.dat";
  exporter->SetFileName(exportFilename.GetCStr());
  exporter->SetInput(inputNode);
  exporter->ExportDIC();

  // Re-import and verify scalar preservation
  albaOpImporterDicFile *reimporter = new albaOpImporterDicFile("reimporter");
  reimporter->TestModeOn();
  reimporter->SetFileName(exportFilename.GetCStr());

  albaVMEStorage *storage2 = albaVMEStorage::New();
  albaVME *root2 = (albaVME *)storage2->GetRoot();
  reimporter->SetInput(root2);
  reimporter->ImportFile();
  albaVMEPointCloud *outputNode = (albaVMEPointCloud *)reimporter->GetOutput();

  // Verify scalars are preserved
  outputNode->GetOutput()->Update();
  vtkPolyData *outputPolyData = vtkPolyData::SafeDownCast(outputNode->GetOutput()->GetVTKData());

  albaString scalarNames[] = { "Deformed Coordinates", "Displacements", "Displ Magnitudo", "Index X", "Index Y" };

  // Verify all scalars are preserved with correct values
  for (int scalarIdx = 0; scalarIdx < 5; scalarIdx++)
  {
    vtkDataArray *inputScalars = inputPolyData->GetPointData()->GetScalars(scalarNames[scalarIdx].GetCStr());
    vtkDataArray *outputScalars = outputPolyData->GetPointData()->GetScalars(scalarNames[scalarIdx].GetCStr());
    
    CPPUNIT_ASSERT(inputScalars != NULL);
    CPPUNIT_ASSERT(outputScalars != NULL);
    CPPUNIT_ASSERT(outputScalars->GetNumberOfTuples() == inputScalars->GetNumberOfTuples());
    CPPUNIT_ASSERT(outputScalars->GetNumberOfComponents() == inputScalars->GetNumberOfComponents());

    int numTuples = inputScalars->GetNumberOfTuples();
    int numComponents = inputScalars->GetNumberOfComponents();

    // Verify each tuple and component
    for (int i = 0; i < numTuples; i++)
    {
      for (int comp = 0; comp < numComponents; comp++)
      {
        double inputValue = inputScalars->GetComponent(i, comp);
        double outputValue = outputScalars->GetComponent(i, comp);
        CPPUNIT_ASSERT(fabs(inputValue - outputValue) < 0.00001);
      }
    }
  }

  albaDEL(storage);
  albaDEL(storage2);
  delete importer;
  delete exporter;
  delete reimporter;
}