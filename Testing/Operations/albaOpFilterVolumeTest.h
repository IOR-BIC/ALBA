/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpFilterVolumeTest_H__
#define __CPP_UNIT_albaOpFilterVolumeTest_H__

#include "albaTest.h"

class vtkImageData;
class vtkRectilinearGrid;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpFilterVolumeTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpFilterVolumeTest );
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
