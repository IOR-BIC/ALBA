/*=========================================================================

 Program: MAF2
 Module: mafOpFilterVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpFilterVolumeTest_H__
#define __CPP_UNIT_mafOpFilterVolumeTest_H__

#include "mafTest.h"

class vtkImageData;
class vtkRectilinearGrid;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpFilterVolumeTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpFilterVolumeTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOnSmooth );
  CPPUNIT_TEST( TestOnMedian );
  CPPUNIT_TEST( TestOnClear );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestUndo );
  CPPUNIT_TEST( TestApplyFiltersToInputData );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestOnSmooth();
  void TestOnMedian();
  void TestOnClear();
  void TestAccept();
  void TestUndo();
  void TestApplyFiltersToInputData();

  void CreateDataTest();
  vtkImageData *m_InputIM;
  vtkRectilinearGrid *m_InputRG;
  bool m_Result;
};

#endif
