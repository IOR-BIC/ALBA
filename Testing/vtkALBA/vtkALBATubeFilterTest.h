/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATubeFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBATubeFilterTEST_H__
#define __CPP_UNIT_vtkALBATubeFilterTEST_H__

#include "albaTest.h"

class vtkALBATubeFilterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE(vtkALBATubeFilterTest);
    CPPUNIT_TEST(TestInput);
    CPPUNIT_TEST(TestCapping);
    CPPUNIT_TEST(TestNormals);
		CPPUNIT_TEST(TestTCoords);
		CPPUNIT_TEST(TestRadius);
		CPPUNIT_TEST(TestSides);
		CPPUNIT_TEST(TestOldVersion);
		CPPUNIT_TEST_SUITE_END();

  protected:
    void TestInput();
    void TestCapping();
    void TestNormals();
		void TestTCoords();
		void TestRadius();
		void TestSides();
		void TestOldVersion();
};

#endif
