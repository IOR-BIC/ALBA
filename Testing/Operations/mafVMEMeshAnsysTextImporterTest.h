/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshAnsysTextImporterTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEMeshAnsysTextImporterTest_H__
#define __CPP_UNIT_mafVMEMeshAnsysTextImporterTest_H__

#include "mafTest.h"
#include "mafString.h"
#include "mafVMEMeshAnsysTextImporter.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class mafVMEMeshAnsysTextImporterTest : public mafTest
{

public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();


  CPPUNIT_TEST_SUITE( mafVMEMeshAnsysTextImporterTest );
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
  
  void ReadAndDisplay(mafString &dirPrefix, int dataType = 0, bool readMaterials = false, bool writeToDisk = false);
  void SaveUnstructuredGridToFile(mafString &dirPrefix, mafString &fileName, vtkUnstructuredGrid *data);

  void RenderData( vtkUnstructuredGrid *data, int dataType = 0);
  void Read( mafVMEMeshAnsysTextImporter *reader, mafString &dirPrefix, bool readMaterials /*= false*/, bool writeToDisk /*= false*/, mafString &outputFileName );

  int  GetRowsNumber(vtkUnstructuredGrid *inputUGrid);

  vtkFileOutputWindow               *m_Log;

};

#endif
