/*=========================================================================

 Program: MAF2
 Module: vtkMAFImageFillHolesRemoveIslandsTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFImageFillHolesRemoveIslandsTest_H__
#define __CPP_UNIT_vtkMAFImageFillHolesRemoveIslandsTest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkActor;

//------------------------------------------------------------------------------
// Test class for vtkMAFImageFillHolesRemoveIslands
//------------------------------------------------------------------------------
class vtkMAFImageFillHolesRemoveIslandsTest : public mafTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    CPPUNIT_TEST_SUITE( vtkMAFImageFillHolesRemoveIslandsTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestFillHoles );
    CPPUNIT_TEST( TestRemoveIslands );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestFillHoles();
    void TestRemoveIslands();
    
    //accessories
    void RenderData(vtkActor *actor );
    void TestAlgorithm();

    int m_Algorithm;
		char* m_TestName;
};

#endif
