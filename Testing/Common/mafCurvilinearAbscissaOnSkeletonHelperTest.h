/*=========================================================================

 Program: MAF2
 Module: mafCurvilinearAbscissaOnSkeletonHelperTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafCurvilinearAbscissaOnSkeletonHelperTest_H__
#define __CPP_UNIT_mafCurvilinearAbscissaOnSkeletonHelperTest_H__

#include "mafTest.h"


/** 
class name: mafCurvilinearAbscissaOnSkeletonHelperTest
  Test class for mafCurvilinearAbscissaOnSkeletonHelper
*/
class mafCurvilinearAbscissaOnSkeletonHelperTest : public mafTest
{
public:


	CPPUNIT_TEST_SUITE( mafCurvilinearAbscissaOnSkeletonHelperTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSetGetConstraintPolylineGraph );
  CPPUNIT_TEST( SetGetCurvilinearAbscissa );
  CPPUNIT_TEST( SetGetActiveBranchId );
  CPPUNIT_TEST( TestMoveOnSkeleton );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestStaticAllocation();
  void TestSetGetConstraintPolylineGraph();
  void SetGetCurvilinearAbscissa();
  void SetGetActiveBranchId();
  void TestMoveOnSkeleton();
};


#endif