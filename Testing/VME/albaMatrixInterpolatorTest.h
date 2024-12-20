/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixInterpolatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaMatrixInterpolatorTest_H__
#define __CPP_UNIT_albaMatrixInterpolatorTest_H__

#include "albaTest.h"

/** 
Class Name: albaMatrixInterpolatorTest.
Test for albaMatrixInterpolatorTest; 
Use this suite to trace memory problems 
*/

class albaMatrixInterpolatorTest : public albaTest
{
public: 

  /** CPPUNIT test suite START*/
  CPPUNIT_TEST_SUITE( albaMatrixInterpolatorTest );
  
  /** Test to control Creation/Destruction of the object */
  CPPUNIT_TEST(TestDynamicAllocation);
  /** Test to control if the datapipe accept correctly vme types */
  CPPUNIT_TEST(TestAccept);
  /** Test to if current item is corrected and if changing timestamp and then update are successful */
  CPPUNIT_TEST(TestGetCurrentItem_SetTimeStamp_Update);
  /** Test if modified time changes after an update*/
  CPPUNIT_TEST(TestGetMTime);

  /** CPPUNIT test suite END */
  CPPUNIT_TEST_SUITE_END();

private:
  /** Creation/Destruction of the object*/
  void TestDynamicAllocation();
  /** Call Accept function of Datapipe */
  void TestAccept();
  /** Retrieve Current item and control if data is correct, use SetTimeStamp and Update the pipe */
  void TestGetCurrentItem_SetTimeStamp_Update();
  /** Check the Modified time of the pipe*/
  void TestGetMTime();
	

  bool m_Result;
};

#endif

