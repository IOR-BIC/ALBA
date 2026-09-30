/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMeshTest
 Authors: Simone Brazzale
 
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
#include "albaOperationsTests.h"

#include "albaOpExporterMeshTest.h"

#include "albaOpExporterMesh.h"
#include "albaOpImporterVTK.h"
#include "albaString.h"

#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEMesh.h"

#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"

#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 

#include <iostream>


void albaOpExporterMeshTest::TestExporterMesh()
{
  // Generate output files
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/pipemesh/";

  albaString vtkFileName = dirPrefix;
  vtkFileName << "hex8.vtk";

  albaString outPrefix = GET_TEST_DATA_DIR();

  albaString fileName = outPrefix;
  fileName << "/testMeshExporter.lis";

  albaString nodesFileName = outPrefix;
  nodesFileName << "/testMeshExporter_NODES.lis";

  albaString elementsFileName = outPrefix;
  elementsFileName << "/testMeshExporter_ELEMENTS.lis";

  albaString materialsFileName = outPrefix;
  materialsFileName << "/testMeshExporter_MATERIALS.lis";

  // Import a hex8 VTK file which will be used for testing the Operation
  albaOpImporterVTK* importerVTK = new albaOpImporterVTK();
  importerVTK->TestModeOn();

  // Use a dummy vme as input
  DummyVme *dummyVme = new DummyVme();
  importerVTK->SetInput(dummyVme);
  importerVTK->SetFileName(vtkFileName.GetCStr());
  importerVTK->ImportFile();

  // Check that the vme has been correctly generated
  CPPUNIT_ASSERT(dummyVme->GetChild(0) && dummyVme->GetChild(0)->IsA("albaVMEMesh"));
  albaVMEMesh *vmeMesh = albaVMEMesh::SafeDownCast(importerVTK->GetOutput());
	vmeMesh->Update();
	CPPUNIT_ASSERT(vmeMesh);
  CPPUNIT_ASSERT(vmeMesh->GetOutput()->GetVTKData()->GetNumberOfPoints()>0);
  CPPUNIT_ASSERT(vmeMesh->GetOutput()->GetVTKData()->GetNumberOfCells()>0);

  // Call the exporter
  albaOpExporterMesh *meshExporter = new albaOpExporterMesh("mesh exporter");
	meshExporter->TestModeOn();

  meshExporter->SetFileName(fileName.GetCStr());
  meshExporter->SetNodesFileName(nodesFileName.GetCStr());
  meshExporter->SetElementsFileName(elementsFileName.GetCStr());
  meshExporter->SetMaterialsFileName(materialsFileName.GetCStr());
	meshExporter->SetEnableBackCalculation(false);
  meshExporter->SetInput(vmeMesh);
  meshExporter->Write();

	// Test NODE file
  wxString file;
  wxString original;
  file.append(nodesFileName.GetCStr());
  original = ALBA_DATA_ROOT;
	original << "/FEM/ANSYS/hex8/NLIST.lis";

  wxFileInputStream inputFile( file );
  wxFileInputStream originalFile( original );
  wxTextInputStream text1( inputFile );
  wxTextInputStream text2( originalFile );
  wxString line1;
  wxString line2;

  while (!inputFile.Eof())
  {
    line1 = text1.ReadLine();
    line2 = text2.ReadLine();
    wxStringTokenizer tkz1(line1,wxT(' '),wxTOKEN_DEFAULT);
    wxStringTokenizer tkz2(line2,wxT(' '),wxTOKEN_DEFAULT);

    while (tkz1.HasMoreTokens())
    {
      albaString scalar1 = tkz1.GetNextToken();
      albaString scalar2 = tkz2.GetNextToken();
      int val_scalar1 = atof(scalar1);
      int val_scalar2 = atof(scalar2);

      CPPUNIT_ASSERT( val_scalar1 == val_scalar2);
    }
  }
  // -------------------

  // Test ELEMENT file
  wxString file_e;
  file_e.append(elementsFileName.GetCStr());
  wxFileInputStream inputFile_e( file_e );
  wxTextInputStream text_e( inputFile_e );
  wxString line;

  int n = 0;
  int val_scalar[2][14];
  while (!inputFile_e.Eof())
  {
    line = text_e.ReadLine();
    wxStringTokenizer tkz(line,wxT(' '),wxTOKEN_DEFAULT);

    int j = 0;
    while (tkz.HasMoreTokens())
    { 
      albaString scalar = tkz.GetNextToken();
      val_scalar[n][j] = atof(scalar);
      j++;
    }
    n++;
  }
  CPPUNIT_ASSERT( n == 3);
  CPPUNIT_ASSERT( val_scalar[0][0] == 1);
  CPPUNIT_ASSERT( val_scalar[1][0] == 2);
  CPPUNIT_ASSERT( val_scalar[0][1] == 1);
  CPPUNIT_ASSERT( val_scalar[1][1] == 1);
  CPPUNIT_ASSERT( val_scalar[0][4] == 6);
  CPPUNIT_ASSERT( val_scalar[1][4] == 12);
  CPPUNIT_ASSERT( val_scalar[0][6] == 12);
  CPPUNIT_ASSERT( val_scalar[1][6] == 3);
  CPPUNIT_ASSERT( val_scalar[0][9] == 5);
  CPPUNIT_ASSERT( val_scalar[1][9] == 11);
  // -------------------

  // Test MATERIAL file
  wxString file_m;
  file_m.append(materialsFileName.GetCStr());
  wxFileInputStream inputFile_m( file_m );
  wxTextInputStream text_m( inputFile_m );

	line = text_m.ReadLine();
	albaString header = line;
	CPPUNIT_ASSERT(header.Compare("MAT_N\tEx\tNUxy\tDens") == 0);

	line = text_m.ReadLine();
	albaString mat = line;
	CPPUNIT_ASSERT(mat.Compare("1\t200000\t0.33\t1.07") == 0);

	// -------------------


  albaDEL(meshExporter);
  albaDEL(importerVTK);

  delete dummyVme;
}

//----------------------------------------------------------------------------
void albaOpExporterMeshTest::TestBackCalculation()
{
	// Generate output files
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/pipemesh/";

	albaString vtkFileName = dirPrefix;
	vtkFileName << "hex8.vtk";

	albaString outPrefix = GET_TEST_DATA_DIR();

	albaString fileName = outPrefix;
	fileName << "/testMeshExporter.lis";

	albaString nodesFileName = outPrefix;
	nodesFileName << "/testMeshExporter_NODES.lis";

	albaString elementsFileName = outPrefix;
	elementsFileName << "/testMeshExporter_ELEMENTS.lis";

	albaString materialsFileName = outPrefix;
	materialsFileName << "/testMeshExporter_MATERIALS.lis";

	// Import a hex8 VTK file which will be used for testing the Operation
	albaOpImporterVTK* importerVTK = new albaOpImporterVTK();
	importerVTK->TestModeOn();

	// Use a dummy vme as input
	DummyVme *dummyVme = new DummyVme();
	importerVTK->SetInput(dummyVme);
	importerVTK->SetFileName(vtkFileName.GetCStr());
	importerVTK->ImportFile();

	albaVMEMesh *vmeMesh = albaVMEMesh::SafeDownCast(importerVTK->GetOutput());
	vmeMesh->Update();


	// Call the exporter
	albaOpExporterMesh *meshExporter = new albaOpExporterMesh("mesh exporter");
	meshExporter->TestModeOn();

	meshExporter->SetFileName(fileName.GetCStr());
	meshExporter->SetNodesFileName(nodesFileName.GetCStr());
	meshExporter->SetElementsFileName(elementsFileName.GetCStr());
	meshExporter->SetMaterialsFileName(materialsFileName.GetCStr());
	meshExporter->SetEnableBackCalculation(true);
	meshExporter->SetInput(vmeMesh);
	meshExporter->Write();


	// Test MATERIAL file
	wxString line;
	wxString file_m;
	file_m.append(materialsFileName.GetCStr());
	wxFileInputStream inputFile_m(file_m);
	wxTextInputStream text_m(inputFile_m);

	line = text_m.ReadLine();
	albaString header = line;
	CPPUNIT_ASSERT(header.Compare("MAT_N\tEx\tNUxy\tDens") == 0);

	line = text_m.ReadLine();
	albaString mat = line;
	CPPUNIT_ASSERT(mat.Compare("1\t200000\t0.33\t200000") == 0);

	albaDEL(meshExporter);
	albaDEL(importerVTK);
	delete dummyVme;
}

//----------------------------------------------------------------------------
void albaOpExporterMeshTest::TestBackCalculationSetValues()
{
	// Generate output files
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/pipemesh/";

	albaString vtkFileName = dirPrefix;
	vtkFileName << "hex8.vtk";

	albaString outPrefix = GET_TEST_DATA_DIR();

	albaString fileName = outPrefix;
	fileName << "/testMeshExporter.lis";

	albaString nodesFileName = outPrefix;
	nodesFileName << "/testMeshExporter_NODES.lis";

	albaString elementsFileName = outPrefix;
	elementsFileName << "/testMeshExporter_ELEMENTS.lis";

	albaString materialsFileName = outPrefix;
	materialsFileName << "/testMeshExporter_MATERIALS.lis";

	// Import a hex8 VTK file which will be used for testing the Operation
	albaOpImporterVTK* importerVTK = new albaOpImporterVTK();
	importerVTK->TestModeOn();

	// Use a dummy vme as input
	DummyVme *dummyVme = new DummyVme();
	importerVTK->SetInput(dummyVme);
	importerVTK->SetFileName(vtkFileName.GetCStr());
	importerVTK->ImportFile();

	albaVMEMesh *vmeMesh = albaVMEMesh::SafeDownCast(importerVTK->GetOutput());
	vmeMesh->Update();


	// Call the exporter
	albaOpExporterMesh *meshExporter = new albaOpExporterMesh("mesh exporter");
	meshExporter->TestModeOn();

	meshExporter->SetFileName(fileName.GetCStr());
	meshExporter->SetNodesFileName(nodesFileName.GetCStr());
	meshExporter->SetElementsFileName(elementsFileName.GetCStr());
	meshExporter->SetMaterialsFileName(materialsFileName.GetCStr());
	meshExporter->SetEnableBackCalculation(true);
	
	//Change configuration values 
	BonematConfiguration conf = meshExporter->GetConfiguration();
	conf.a_OneInterval++;
	conf.b_OneInterval++;
	conf.c_OneInterval++;
	meshExporter->SetConfiguration(conf);

	meshExporter->SetInput(vmeMesh);
	meshExporter->Write();


	// Test MATERIAL file
	wxString line;
	wxString file_m;
	file_m.append(materialsFileName.GetCStr());
	wxFileInputStream inputFile_m(file_m);
	wxTextInputStream text_m(inputFile_m);

	line = text_m.ReadLine();
	albaString header = line;
	CPPUNIT_ASSERT(header.Compare("MAT_N\tEx\tNUxy\tDens") == 0);

	line = text_m.ReadLine();
	albaString mat = line;
	CPPUNIT_ASSERT(mat.Compare("1\t200000\t0.33\t316.227") == 0);

	albaDEL(meshExporter);
	albaDEL(importerVTK);
	delete dummyVme;
}

//----------------------------------------------------------------------------
void albaOpExporterMeshTest::TestBackCalculationTripleInterval()
{

	// Generate output files
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/pipemesh/";

	albaString vtkFileName = dirPrefix;
	vtkFileName << "hex8.vtk";

	albaString outPrefix = GET_TEST_DATA_DIR();

	albaString fileName = outPrefix;
	fileName << "/testMeshExporter.lis";

	albaString nodesFileName = outPrefix;
	nodesFileName << "/testMeshExporter_NODES.lis";

	albaString elementsFileName = outPrefix;
	elementsFileName << "/testMeshExporter_ELEMENTS.lis";

	albaString materialsFileName = outPrefix;
	materialsFileName << "/testMeshExporter_MATERIALS.lis";

	// Import a hex8 VTK file which will be used for testing the Operation
	albaOpImporterVTK* importerVTK = new albaOpImporterVTK();
	importerVTK->TestModeOn();

	// Use a dummy vme as input
	DummyVme *dummyVme = new DummyVme();
	importerVTK->SetInput(dummyVme);
	importerVTK->SetFileName(vtkFileName.GetCStr());
	importerVTK->ImportFile();

	albaVMEMesh *vmeMesh = albaVMEMesh::SafeDownCast(importerVTK->GetOutput());
	vmeMesh->Update();


	// Call the exporter
	albaOpExporterMesh *meshExporter = new albaOpExporterMesh("mesh exporter");
	meshExporter->TestModeOn();

	meshExporter->SetFileName(fileName.GetCStr());
	meshExporter->SetNodesFileName(nodesFileName.GetCStr());
	meshExporter->SetElementsFileName(elementsFileName.GetCStr());
	meshExporter->SetMaterialsFileName(materialsFileName.GetCStr());
	meshExporter->SetEnableBackCalculation(true);

	//Change configuration values 
	BonematConfiguration conf = meshExporter->GetConfiguration();
	conf.densityIntervalsNumber = THREE_INTERVALS;
	conf.a_RhoBiggerThanRho2 = 1;
	conf.b_RhoBiggerThanRho2 = 3;
	conf.c_RhoBiggerThanRho2 = 4;
	meshExporter->SetConfiguration(conf);

	meshExporter->SetInput(vmeMesh);
	meshExporter->Write();


	// Test MATERIAL file
	wxString line;
	wxString file_m;
	file_m.append(materialsFileName.GetCStr());
	wxFileInputStream inputFile_m(file_m);
	wxTextInputStream text_m(inputFile_m);

	line = text_m.ReadLine();
	albaString header = line;
	CPPUNIT_ASSERT(header.Compare("MAT_N\tEx\tNUxy\tDens") == 0);

	line = text_m.ReadLine();
	albaString mat = line;
	CPPUNIT_ASSERT(mat.Compare("1\t200000\t0.33\t16.0685") == 0);

	albaDEL(meshExporter);
	albaDEL(importerVTK);
	delete dummyVme;
}

void albaOpExporterMeshTest::TestConstructor()
{

  albaOpExporterMesh *meshExporter= new albaOpExporterMesh("mesh exporter");
  meshExporter->TestModeOn();
  CPPUNIT_ASSERT(meshExporter->GetOutput() == NULL);

  albaDEL(meshExporter);
}
