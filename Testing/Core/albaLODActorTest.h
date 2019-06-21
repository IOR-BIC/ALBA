/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLODActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaLODActortest_H__
#define __CPP_UNIT_albaLODActortest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class albaLODActor;

//------------------------------------------------------------------------------
// Test class for albaLODActor
//------------------------------------------------------------------------------
class albaLODActorTest : public albaTest
{
  public:
    enum ID_TEST_LIST
    {
      ID_FLAG_DIMENSION = 0,
	    ID_PIXEL_THRESHOLD,
			ID_ENABLE_FADING,
			ID_ENABLE_HIGH_THRESHOLD,
    };

    CPPUNIT_TEST_SUITE( albaLODActorTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
		CPPUNIT_TEST( TestPixelThreshold);
		CPPUNIT_TEST( TestFlagDimension );
		CPPUNIT_TEST( TestEnableFading );
		CPPUNIT_TEST( TestEnableHighThreshold);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFixture();
    void TestDynamicAllocation();
		void TestPixelThreshold();
		void TestFlagDimension();
    void TestEnableFading();
		void TestEnableHighThreshold();

    //accessories
    void RenderData(albaLODActor *actor);
    
    int m_TestNumber;
};



#endif
