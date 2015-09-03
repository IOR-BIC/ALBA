/*=========================================================================

 Program: MAF2
 Module: mafLUTLibraryTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

using namespace std;

#ifndef __CPP_UNIT_mafLUTLibraryTest_H__
#define __CPP_UNIT_mafLUTLibraryTest_H__

#include "mafTest.h"

#include "mafLUTLibrary.h"
#include "mafString.h"
#include "vtkMAFSmartPointer.h"
#include "vtkLookupTable.h"

class mafLUTLibraryTest : public mafTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( mafLUTLibraryTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestSetGetDir );
  CPPUNIT_TEST(TestAdd);
  CPPUNIT_TEST(TestRemove);
  CPPUNIT_TEST(TestLoadSave);
  CPPUNIT_TEST(TestGetNumberOfLuts);
  CPPUNIT_TEST(TestGetLutNames);
  CPPUNIT_TEST(TestGetLutByName);
  CPPUNIT_TEST(TestHasLut);
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
	void TestSetGetDir();
  void TestAdd();
  void TestRemove();
  void TestLoadSave();
  void TestGetNumberOfLuts();
  void TestGetLutNames();
  void TestGetLutByName();
  void TestHasLut();
 
  vtkLookupTable *m_LutDefault;
  vtkLookupTable *m_LutEField;
  vtkLookupTable *m_LutGlow;
};


#endif
