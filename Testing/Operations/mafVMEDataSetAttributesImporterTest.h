/*=========================================================================

 Program: MAF2
 Module: mafVMEDataSetAttributesImporterTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEDataSetAttributesImporterTest_H__
#define __CPP_UNIT_mafVMEDataSetAttributesImporterTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafString.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class mafVMEMeshAnsysTextImporter;
class mafVMEMesh;

class mafVMEDataSetAttributesImporterTest : public CPPUNIT_NS::TestFixture
{

public:

  mafVMEDataSetAttributesImporterTest();
  ~mafVMEDataSetAttributesImporterTest();

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafVMEDataSetAttributesImporterTest );
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

  void SaveToDiskAndDisplay(mafVMEMesh *vmeMesh,mafString &dirPrefix, int dataType = 0, const char *attributeLabel = "UNDEFINED", bool writeToDisk = false, const char *outputVtkDataName = "defaultFileName.vtk", bool render = true, bool linearize = false);
  void RenderData( vtkUnstructuredGrid *data, int dataType = 0, bool linearize = false);
	
  void ReadMesh(mafString &dirPrefix, bool readMaterials);
	void SaveToDiskForTimeVarying( mafVMEMesh *vmeMesh,mafString &dirPrefix, int dataType /*= 0*/, const char *attributeLabel /*= "UNDEFINED"*/, bool writeToDisk /*= false*/, const char *outputDataName /*= "defaultFileName.vtk"*/ );
  	
private:

  mafVMEMeshAnsysTextImporter* m_AnsysTextReader;
};

#endif
