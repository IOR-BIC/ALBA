/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoTranslateTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNITmafGUIGizmoTranslateTest_H__
#define __CPP_UNITmafGUIGizmoTranslateTest_H__

#include "mafTest.h"
#include "mafEventBase.h"
#include "mafObserver.h"

class vtkSphereSource;
class mafVMESurface;

/** mafGUIGizmoTranslateTest */
class mafGUIGizmoTranslateTest : public mafTest
{
public: 
  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIGizmoTranslateTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetAbsPosition);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

  
private:

  void TestFixture();
  void TestConstructorDestructor();
  void TestSetAbsPosition();
  void TestOnEvent();
  
  mafVMESurface *m_VMESphere;
  vtkSphereSource *m_VTKSphere;
};

#endif
