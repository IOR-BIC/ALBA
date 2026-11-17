/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateGUITest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoRotateGUITest_H__
#define __CPP_UNIT_albaGizmoRotateGUITest_H__

#include "albaTest.h"
#include "albaEventBase.h"
#include "albaObserver.h"

class vtkCubeSource;
class albaVMESurface;

/** albaGizmoRotateGUITest */
class albaGizmoRotateGUITest : public albaTest
{
public: 
  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoRotateGUITest );
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
  
  albaVMESurface *m_VMECube;
  vtkCubeSource *m_VTKCube;
};

#endif
