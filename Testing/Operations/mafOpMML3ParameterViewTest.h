/*=========================================================================

 Program: MAF2
 Module: mafOpMML3ParameterViewTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpMML3ParameterViewTest_H__
#define __CPP_UNIT_mafOpMML3ParameterViewTest_H__

#include "mafTest.h"

#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

class mafOpMML3ParameterViewTest : public mafTest
{
public:

  void BeforeTest();
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafOpMML3ParameterViewTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetValue );
  CPPUNIT_TEST( TestGetNumberOfDataPoints );
  CPPUNIT_TEST( TestGetMaxY );
  CPPUNIT_TEST( TestGetMinY );
  CPPUNIT_TEST( TestSetLineActorX );
  CPPUNIT_TEST( TestGetPointsActor );
  CPPUNIT_TEST( TestGetSplineActor );
  // CPPUNIT_TEST( TestSetRangeX ); Cannot test
  // CPPUNIT_TEST( TestSetRangeY ); Already tested in TestGetMaxY, TestGetMinY
  CPPUNIT_TEST( TestRemovePoint );
  CPPUNIT_TEST( TestAddPoint );
  CPPUNIT_TEST( TestRender );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetValue();
  void TestGetNumberOfDataPoints();
  void TestGetMaxY();
  void TestGetMinY();
  void TestSetLineActorX();
  void TestGetPointsActor();
  void TestGetSplineActor();
  // void TestSetRangeX();
  // void TestSetRangeY();
  void TestRemovePoint();
  void TestAddPoint();
  void TestRender();
};

#endif
