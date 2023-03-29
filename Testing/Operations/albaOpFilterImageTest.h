/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterImageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpFilterImageTest_H
#define CPP_UNIT_albaOpFilterImageTest_H
#include "albaTest.h"

class albaVMEImage;
class albaOpFilterImage;


class albaOpFilterImageTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaOpFilterImageTest );
	CPPUNIT_TEST(TestMedian);
	CPPUNIT_TEST(TestUndo);

	CPPUNIT_TEST(TestGaussianDiscrete);
	CPPUNIT_TEST(TestThreshold);
	CPPUNIT_TEST(TestThresholdBinary);
	CPPUNIT_TEST(TestGradientMagnitude);
	CPPUNIT_TEST(TestSobel);
	CPPUNIT_TEST(TestCanny);
	CPPUNIT_TEST(TestZeroCrossing);
	CPPUNIT_TEST(TestLaplacianRecursive);
	CPPUNIT_TEST_SUITE_END();


	/** Test specific stuff executed before each test */
	virtual void BeforeTest();

	/** Test specific stuff executed after each test */
	virtual void AfterTest();

  protected:

		albaOpFilterImage *m_TestOp;
		albaVMEImage *m_TestImage;

		void TestGaussianDiscrete();
		void TestThreshold();
		void TestThresholdBinary();
		void TestGradientMagnitude();
		void TestSobel();
		void TestCanny();
		void TestZeroCrossing();
		void TestLaplacianRecursive();
    void TestMedian();
		void TestUndo();

};

#endif
