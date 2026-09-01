/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADataPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBADataPipeTest_H__
#define __CPP_UNIT_vtkALBADataPipeTest_H__

#include "albaTest.h"

/** Test for vtkALBADataPipe; Use this suite to trace memory problems */
class vtkALBADataPipeTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( vtkALBADataPipeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetNthInput);
  CPPUNIT_TEST(TestGetOutput);
  CPPUNIT_TEST(TestGetMTime);
  CPPUNIT_TEST(TestGetInformationTime);
  CPPUNIT_TEST(TestSetGetDataPipe);
  CPPUNIT_TEST(TestUpdateInformation);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
	void TestSetNthInput();
	void TestGetOutput();
	void TestGetMTime();
	void TestGetInformationTime();
	void TestSetGetDataPipe();
	void TestUpdateInformation();
};

#endif

