/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorFieldGlyphsTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeVectorFieldGlyphsTest_H__
#define __CPP_UNIT_mafPipeVectorFieldGlyphsTest_H__

#include "mafTest.h"

class vtkRenderWindow;

class mafPipeVectorFieldGlyphsTest : public mafTest
{
public: 

	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

  CPPUNIT_TEST_SUITE( mafPipeVectorFieldGlyphsTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestCreate );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestCreate();

	vtkRenderWindowInteractor *m_RenderWindowInteractor;
};

#endif