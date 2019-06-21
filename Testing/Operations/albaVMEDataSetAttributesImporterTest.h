/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEDataSetAttributesImporterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEDataSetAttributesImporterTest_H__
#define __CPP_UNIT_albaVMEDataSetAttributesImporterTest_H__

#include "albaTest.h"
#include "albaString.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class albaVMEMeshAnsysTextImporter;
class albaVMEMesh;

class albaVMEDataSetAttributesImporterTest : public albaTest
{

public:

  albaVMEDataSetAttributesImporterTest();
  ~albaVMEDataSetAttributesImporterTest();

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaVMEDataSetAttributesImporterTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestTetra4 );
  CPPUNIT_TEST( TestTetra10SingleTime3CellAttributes );
  CPPUNIT_TEST( TestTetra10SingleTime1CellAttributes );
  CPPUNIT_TEST( TestTetra10SingleTime1PointsAttributes );
  CPPUNIT_TEST( TestTetra10MultiTime1CellAttributes );
  CPPUNIT_TEST( TestTetra10MultiTime3CellAttributes );
  CPPUNIT_TEST( TestTetra10MultiTime1PointAttributes );
  CPPUNIT_TEST( TestHexa8SingleTime3CellAttributes );
  CPPUNIT_TEST( TestHexa8SingleTime1CellAttributes );
  CPPUNIT_TEST( TestHexa8SingleTime1PointsAttributes );
  CPPUNIT_TEST( TestHexa20SingleTime3CellAttributes );
  CPPUNIT_TEST( TestHexa20SingleTime1CellAttributes );
  CPPUNIT_TEST( TestHexa20SingleTime1PointsAttributes );
  CPPUNIT_TEST( TestHexa8 );
  CPPUNIT_TEST( TestHexa20 );
  CPPUNIT_TEST( TestTetra10ANSYS_ELEMENT_IDJumpingSingleTime1CellAttributes );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestConstructor();
  void TestTetra4();
  void TestTetra10SingleTime3CellAttributes();
  void TestTetra10SingleTime1CellAttributes();
  void TestTetra10SingleTime1PointsAttributes();
  void TestTetra10MultiTime1CellAttributes();
  void TestTetra10MultiTime3CellAttributes();
  void TestTetra10MultiTime1PointAttributes();
  void TestHexa8();
  void TestHexa8SingleTime3CellAttributes();
  void TestHexa8SingleTime1CellAttributes();
  void TestHexa8SingleTime1PointsAttributes();
  void TestHexa20SingleTime3CellAttributes();
  void TestHexa20SingleTime1CellAttributes();
  void TestHexa20SingleTime1PointsAttributes();
  void TestHexa20();

  void TestTetra10ANSYS_ELEMENT_IDJumpingSingleTime1CellAttributes();

  // dataType
  enum
  {
    POINT_DATA = 0,
    CELL_DATA,
  };

  void SaveToDiskAndDisplay(albaVMEMesh *vmeMesh,albaString &dirPrefix, int dataType = 0, const char *attributeLabel = "UNDEFINED", bool writeToDisk = false, const char *outputVtkDataName = "defaultFileName.vtk", bool render = true, bool linearize = false);
  void RenderData( vtkUnstructuredGrid *data, int dataType = 0, bool linearize = false);
	
  void ReadMesh(albaString &dirPrefix, bool readMaterials);
	void SaveToDiskForTimeVarying( albaVMEMesh *vmeMesh,albaString &dirPrefix, int dataType /*= 0*/, const char *attributeLabel /*= "UNDEFINED"*/, bool writeToDisk /*= false*/, const char *outputDataName /*= "defaultFileName.vtk"*/ );
  	
private:

  albaVMEMeshAnsysTextImporter* m_AnsysTextReader;
};

#endif
