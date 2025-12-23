/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkXRayVolumeMapperTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkXRayVolumeMapperTest_H__
#define __CPP_UNIT_vtkXRayVolumeMapperTest_H__

#include "albaTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class vtkXRayVolumeMapperTest : public albaTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( vtkXRayVolumeMapperTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestPipeExecution );
	CPPUNIT_TEST( Test_SetInput_GetInput );
	CPPUNIT_TEST( TestReduceColorReduction );
	CPPUNIT_TEST( TestExposureCorrection );
	CPPUNIT_TEST( TestGamma );
	CPPUNIT_TEST( TestPerspectiveCorrection );
	CPPUNIT_TEST( TestColor );
	CPPUNIT_TEST( TestEnableAutoLOD );
	CPPUNIT_TEST( TestDataValid );
	CPPUNIT_TEST( TestTextureMemoryAndPercentage );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeExecution();
	void Test_SetInput_GetInput();
	void TestReduceColorReduction();
  void TestExposureCorrection();
	void TestGamma();
	void TestPerspectiveCorrection();
	void TestColor();
	void TestEnableAutoLOD();
	void TestDataValid();
	void TestTextureMemoryAndPercentage();

	enum {
    ID_TEST_PIPE_EXECUTION = 0,
		ID_TEST_REDUCE_COLOR_REDUCTION,
		ID_TEST_EXPOSURE_CORRECTION,
		ID_TEST_GAMMA,
		ID_TEST_PERSPECTIVE_CORRECTION,
		ID_TEST_COLOR,
		ID_TEST_ENABLE_AUTOLOAD,
	};

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};


#endif
