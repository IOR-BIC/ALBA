/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCurvilinearAbscissaOnSkeletonHelperTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaCurvilinearAbscissaOnSkeletonHelperTest_H__
#define __CPP_UNIT_albaCurvilinearAbscissaOnSkeletonHelperTest_H__

#include "albaTest.h"


/** 
class name: albaCurvilinearAbscissaOnSkeletonHelperTest
  Test class for albaCurvilinearAbscissaOnSkeletonHelper
*/
class albaCurvilinearAbscissaOnSkeletonHelperTest : public albaTest
{
public:


	CPPUNIT_TEST_SUITE( albaCurvilinearAbscissaOnSkeletonHelperTest );
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