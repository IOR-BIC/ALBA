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
  importerVTK->ImportVTK();

  // Check that the vme has been correctly generated
  CPPUNIT_ASSERT(dummyVme->GetChild(0) && dummyVme->GetChild(0)->IsA("albaVMEMesh"));
  albaVMEMesh *vmeMesh = albaVMEMesh::SafeDownCast(importerVTK->GetOutput());
	vmeMesh->Update();
	((vtkUnstructuredGrid *)(vmeMesh->GetOutput()->GetVTKData()))->UpdateData();
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
  CPPUNIT_ASSERT( val_scalar[0][2] == 1);
  CPPUNIT_ASSERT( val_scalar[1][2] == 1);
  CPPUNIT_ASSERT( val_scalar[0][6] == 2);
  CPPUNIT_ASSERT( val_scalar[1][6] == 1);
  CPPUNIT_ASSERT( val_scalar[0][10] == 12);
  CPPUNIT_ASSERT( val_scalar[1][10] == 3);
  CPPUNIT_ASSERT( val_scalar[0][13] == 5);
  CPPUNIT_ASSERT( val_scalar[1][13] == 11);
  // -------------------

  // Test MATERIAL file
  wxString file_m;
  file_m.append(materialsFileName.GetCStr());
  wxFileInputStream inputFile_m( file_m );
  wxTextInputStream text_m( inputFile_m );

  line = text_m.ReadLine();
  wxStringTokenizer tkz_m(line,wxT(' '),wxTOKEN_DEFAULT);
  albaString title = tkz_m.GetNextToken();
  CPPUNIT_ASSERT( title.Compare("MATERIAL")==0);
  // -------------------
	
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
