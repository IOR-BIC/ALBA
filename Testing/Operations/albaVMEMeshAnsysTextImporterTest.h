/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshAnsysTextImporterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEMeshAnsysTextImporterTest_H__
#define __CPP_UNIT_albaVMEMeshAnsysTextImporterTest_H__

#include "albaTest.h"
#include "albaString.h"
#include "albaVMEMeshAnsysTextImporter.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class albaVMEMeshAnsysTextImporterTest : public albaTest
{

public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();


  CPPUNIT_TEST_SUITE( albaVMEMeshAnsysTextImporterTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestTetra4 );
  CPPUNIT_TEST( TestTetra10 );
  CPPUNIT_TEST( TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar );
  CPPUNIT_TEST( TestHexa8 );
  CPPUNIT_TEST( TestHexa20 );
  CPPUNIT_TEST_SUITE_END();


protected:

  void TestFixture();
  void TestConstructor();
  void TestTetra4();
  void TestTetra10();
  void TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar();
  void TestHexa8();
  void TestHexa20();

  // dataType
  enum
  {
    POINT_DATA = 0,
    CELL_DATA,
  };

private:
  
  void ReadAndDisplay(albaString &dirPrefix, int dataType = 0, bool readMaterials = false, bool writeToDisk = false);
  void SaveUnstructuredGridToFile(albaString &dirPrefix, albaString &fileName, vtkUnstructuredGrid *data);

  void RenderData( vtkUnstructuredGrid *data, int dataType = 0);
  void Read( albaVMEMeshAnsysTextImporter *reader, albaString &dirPrefix, bool readMaterials /*= false*/, bool writeToDisk /*= false*/, albaString &outputFileName );

  int  GetRowsNumber(vtkUnstructuredGrid *inputUGrid);
};

#endif
