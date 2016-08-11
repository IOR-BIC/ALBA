/*=========================================================================

 Program: MAF2
 Module: vtkMAFTubeFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFTubeFilterTEST_H__
#define __CPP_UNIT_vtkMAFTubeFilterTEST_H__

#include "mafTest.h"

class vtkMAFTubeFilterTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE(vtkMAFTubeFilterTest);
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
