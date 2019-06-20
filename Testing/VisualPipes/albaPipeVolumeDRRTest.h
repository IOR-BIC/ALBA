/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeDRRTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeVolumeDRRTest_H__
#define __CPP_UNIT_albaPipeVolumeDRRTest_H__

#include "albaTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeVolumeDRRTest : public albaTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( albaPipeVolumeDRRTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestPipeExecution );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeExecution();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
