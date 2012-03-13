/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEDataSetAttributesImporterTest.h,v $
Language:  C++
Date:      $Date: 2008-04-30 16:57:46 $
Version:   $Revision: 1.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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


int
  main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafVMEDataSetAttributesImporterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
