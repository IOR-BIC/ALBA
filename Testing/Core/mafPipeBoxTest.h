/*=========================================================================

 Program: MAF2
 Module: mafPipeBoxTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeBoxTest_H__
#define __CPP_UNIT_mafPipeBoxTest_H__

#include "mafTest.h"

class vtkPolyData;

class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeBoxTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafPipeBoxTest );
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

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

};

#endif
