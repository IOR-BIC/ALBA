/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoRotateTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIGizmoRotateTest_H__
#define __CPP_UNIT_mafGUIGizmoRotateTest_H__

#include "mafTest.h"
#include "mafEventBase.h"
#include "mafObserver.h"

class vtkCubeSource;
class mafVMESurface;

/** mafGUIGizmoRotateTest */
class mafGUIGizmoRotateTest : public mafTest
{
public: 
  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIGizmoRotateTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetAbsOrientation);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();
  void TestConstructorDestructor();
  void TestSetAbsOrientation();
  void TestOnEvent();
  
  mafVMESurface *m_VMECube;
  vtkCubeSource *m_VTKCube;
};

#endif
