/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeArbSliceTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeVolumeArbSliceTest_H__
#define __CPP_UNIT_albaPipeVolumeArbSliceTest_H__

#include "albaTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeVolumeArbSliceTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeVolumeArbSliceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST( TestPipeExecution_SetSliceOpacity );
  CPPUNIT_TEST( TestPipeExecution_SetLutRange );
  CPPUNIT_TEST( TestPipeExecution_SetColorLookupTable );
  CPPUNIT_TEST( TestPipeExecution_TicksOnOff );
	CPPUNIT_TEST( TestPipeExecution_Arbitrary);

  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestPipeExecution();
  void TestPipeExecution_SetSliceOpacity();
  void TestPipeExecution_SetLutRange();
  void TestPipeExecution_SetColorLookupTable();
  void TestPipeExecution_TicksOnOff();
	void TestPipeExecution_Arbitrary();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;
};

#endif
