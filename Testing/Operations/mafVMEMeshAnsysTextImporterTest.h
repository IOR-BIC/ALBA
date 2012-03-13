/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextImporterTest.h,v $
Language:  C++
Date:      $Date: 2008-02-14 14:24:18 $
Version:   $Revision: 1.6 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMEMeshAnsysTextImporterTest_H__
#define __CPP_UNIT_mafVMEMeshAnsysTextImporterTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafString.h"
#include "mafVMEMeshAnsysTextImporter.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class mafVMEMeshAnsysTextImporterTest : public CPPUNIT_NS::TestFixture
{

public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();


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
  runner.addTest( mafVMEMeshAnsysTextImporterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
