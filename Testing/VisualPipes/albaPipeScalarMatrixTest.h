/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeScalarMatrixTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeScalarMatrixTest_H__
#define __CPP_UNIT_albaPipeScalarMatrixTest_H__

#include "albaTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeScalarMatrixTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeScalarMatrixTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  /* Just to test that the fixture has no leaks **/
  void TestFixture();

  /** Test the pipe execution */
  void TestPipeExecution();
};

#endif
