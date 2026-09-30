/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAExtendedGlyph3DTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAExtendedGlyph3DTest_H__
#define __CPP_UNIT_vtkALBAExtendedGlyph3DTest_H__

#include "albaTest.h"

class vtkPolyData;

class vtkALBAExtendedGlyph3DTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( vtkALBAExtendedGlyph3DTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetNumberOfSources );
  CPPUNIT_TEST( TestSetSource );
  CPPUNIT_TEST( TestSetScaling );
  CPPUNIT_TEST( TestSetScaleFactor );
  CPPUNIT_TEST( TestSetRange );
  CPPUNIT_TEST( TestSetOrient );
  CPPUNIT_TEST( TestSetClamping );
  CPPUNIT_TEST( TestSetGeneratePointIds );
  CPPUNIT_TEST( TestSetPointIdsName );
  CPPUNIT_TEST( TestSetScalarVisibility );
  CPPUNIT_TEST_SUITE_END();

protected:

  void CreatePointsSet();

	void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestSetNumberOfSources();
  void TestSetSource();
  void TestSetScaling();
  void TestSetScaleFactor();
  void TestSetRange();
  void TestSetOrient();
  void TestSetClamping();
  void TestSetGeneratePointIds();
  void TestSetPointIdsName();
  void TestSetScalarVisibility();

  vtkPolyData *m_Points;
  int m_TestNumber;
};

#endif
