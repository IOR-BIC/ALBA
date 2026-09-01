/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImageFillHolesRemoveIslandsTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAImageFillHolesRemoveIslandsTest_H__
#define __CPP_UNIT_vtkALBAImageFillHolesRemoveIslandsTest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkActor;

//------------------------------------------------------------------------------
// Test class for vtkALBAImageFillHolesRemoveIslands
//------------------------------------------------------------------------------
class vtkALBAImageFillHolesRemoveIslandsTest : public albaTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    CPPUNIT_TEST_SUITE( vtkALBAImageFillHolesRemoveIslandsTest );
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
