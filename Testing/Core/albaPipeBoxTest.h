/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeBoxTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeBoxTest_H__
#define __CPP_UNIT_albaPipeBoxTest_H__

#include "albaTest.h"

class vtkPolyData;

class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeBoxTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeBoxTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipe3DExecution );
  CPPUNIT_TEST( TestPipe4DExecution );
  CPPUNIT_TEST( TestPipe3DSubtreeExecution );
  CPPUNIT_TEST( TestPipe4DSubtreeExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestPipe3DExecution();
  void TestPipe4DExecution();
  void TestPipe3DSubtreeExecution();
  void TestPipe4DSubtreeExecution();

};

#endif
