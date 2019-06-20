/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterSurfaceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpFilterSurfaceTest_H__
#define __CPP_UNIT_albaOpFilterSurfaceTest_H__

#include "albaTest.h"

class vtkPolyData;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpFilterSurfaceTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpFilterSurfaceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOnClean );
  CPPUNIT_TEST( TestOnSmooth );
  CPPUNIT_TEST( TestOnDecimate );
  CPPUNIT_TEST( TestOnTriangulate );
  CPPUNIT_TEST( TestOnVtkConnect );
  CPPUNIT_TEST( TestOnStripper );
  CPPUNIT_TEST( TestOnGenerateNormals );
  CPPUNIT_TEST( TestUndo );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnClean();
  void TestOnSmooth();
  void TestOnDecimate();
  void TestOnTriangulate();
  void TestOnVtkConnect();
  void TestOnStripper();
  void TestOnGenerateNormals();
  void TestUndo();

  void CheckPolydata(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter);
  void CheckNormals(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter);
  bool m_Result;
};

#endif
