/*=========================================================================

 Program: MAF2
 Module: mafLODActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafLODActortest_H__
#define __CPP_UNIT_mafLODActortest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class mafLODActor;

//------------------------------------------------------------------------------
// Test class for mafLODActor
//------------------------------------------------------------------------------
class mafLODActorTest : public mafTest
{
  public:
    enum ID_TEST_LIST
    {
      ID_FLAG_DIMENSION = 0,
	    ID_PIXEL_THRESHOLD,
			ID_ENABLE_FADING,
			ID_ENABLE_HIGH_THRESHOLD,
    };

    CPPUNIT_TEST_SUITE( mafLODActorTest );
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
    void RenderData(mafLODActor *actor);
    
    int m_TestNumber;
};



#endif
