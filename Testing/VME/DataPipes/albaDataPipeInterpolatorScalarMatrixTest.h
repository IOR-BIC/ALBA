/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorScalarMatrixTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeInterpolatorScalarMatrixTest_H__
#define __CPP_UNIT_albaDataPipeInterpolatorScalarMatrixTest_H__

#include "albaTest.h"

#include <vnl/vnl_matrix.h>

/** 
   Class Name: albaDataPipeInterpolatorScalarMatrixTest.
   Test for albaScalarMatrixInterpolator; 
   Use this suite to trace memory problems 
 */
class albaDataPipeInterpolatorScalarMatrixTest : public albaTest
{
public: 
  /** Executed before each test */ 
  void BeforeTest();

  /* CPPUNIT test suite START*/
  CPPUNIT_TEST_SUITE( albaDataPipeInterpolatorScalarMatrixTest );
  /** Test to control Creation/Destruction of the object */
  CPPUNIT_TEST(TestDynamicAllocation);
  /** Test to control if the datapipe accept correctly vme types */
  CPPUNIT_TEST(TestAccept);
  /** Test to control if the events chain is correct */
  CPPUNIT_TEST(TestOnEvent);
  /** Test to control GetScalarData Retrieve the correct vnl matrix data */
  CPPUNIT_TEST(TestGetScalarData);
  /** Test to control if CurrentItem is of the correct type and contain correct data */
  CPPUNIT_TEST(TestGetCurrentItem);
  
  /* CPPUNIT test suite END*/
  CPPUNIT_TEST_SUITE_END();

private:
  /** Creation/Destruction of the object*/
  void TestDynamicAllocation();
  /** Call Accept function of Datapipe */
  void TestAccept();
  /** Call OnEvent and send event in order to test the events chain */
  void TestOnEvent();
  /** Retrieve directly vnl matrix and control if it is correct*/
  void TestGetScalarData();
  /** Retrieve Current item and control if data is correct*/
  void TestGetCurrentItem();
  
  bool m_Result;
  vnl_matrix<double> m_DataMatrix;
};

#endif

