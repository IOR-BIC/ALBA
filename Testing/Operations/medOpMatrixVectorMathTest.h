/*=========================================================================

 Program: MAF2Medical
 Module: medOpMatrixVectorMathTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_medOpMatrixVectorMathTest_H
#define CPP_UNIT_medOpMatrixVectorMathTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medOpMatrixVectorMathTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( medOpMatrixVectorMathTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	//CPPUNIT_TEST(TestSetHomogeneous);
	CPPUNIT_TEST(TestDivideVectorByHomoCoord);  
	CPPUNIT_TEST(TestSetVectorToZero);
	CPPUNIT_TEST(TestMagnitudeOfVector);
	CPPUNIT_TEST(TestNormalizeVector1);                 
	CPPUNIT_TEST(TestNormalizeVector2); 
	CPPUNIT_TEST(TestInvertVector1);                 
	CPPUNIT_TEST(TestInvertVector2);                 
	CPPUNIT_TEST(TestMultiplyVectorByScalar);   
	CPPUNIT_TEST(TestDivideVectorByScalar);   
	CPPUNIT_TEST(TestAddVectors);   
	CPPUNIT_TEST(TestSubtractVectors);   
	CPPUNIT_TEST(TestAddMultipleOfVector);   
	CPPUNIT_TEST(TestSubtractMultipleOfVector);   
	CPPUNIT_TEST(TestDotProduct);   
	CPPUNIT_TEST(TestVectorProduct);   
	CPPUNIT_TEST(TestCopyVector);   
	CPPUNIT_TEST(TestEquals);   
	CPPUNIT_TEST(TestCalculateNormalsToU);   
	CPPUNIT_TEST(TestCalculateNormalsToV);   
	CPPUNIT_TEST(TestCalculateNormalsToW);   
	CPPUNIT_TEST(TestDistance);   
	CPPUNIT_TEST(TestDistanceSquared);
	CPPUNIT_TEST(TestPrintVector);    
	CPPUNIT_TEST(TestDivideMatrixByHomoCoord);  
	CPPUNIT_TEST(TestSetMatrixToZero);
	CPPUNIT_TEST(TestSetMatrixToIdentity);
	CPPUNIT_TEST(TestMultiplyMatrixByScalar);   
	CPPUNIT_TEST(TestDivideMatrixByScalar);     
	CPPUNIT_TEST(TestMultiplyColumnsByScalars);
	CPPUNIT_TEST(TestMultiplyRowsByScalars);
	CPPUNIT_TEST(TestMultiplyMatrixByVector);
	CPPUNIT_TEST(TestMultiplyMatrixByMatrix);
	CPPUNIT_TEST(TestTranspose);
	CPPUNIT_TEST(TestGetColumn);
	CPPUNIT_TEST(TestPrintMatrix);     
	CPPUNIT_TEST(TestTranspose2DArray);
	CPPUNIT_TEST(TestCopy2DArrayToMatrix3x3);
	CPPUNIT_TEST(TestCopy2DArrayToMatrix4x4);
	CPPUNIT_TEST(TestCopyMatrixTo2DArray3x3);
	CPPUNIT_TEST(TestCopyMatrixTo2DArray4x4);
  CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	//void TestSetHomogeneous();
	void TestDivideVectorByHomoCoord();  
	void TestSetVectorToZero();
	void TestMagnitudeOfVector();
	void TestNormalizeVector1();                 
	void TestNormalizeVector2(); 
	void TestInvertVector1();                 
	void TestInvertVector2();                 
	void TestMultiplyVectorByScalar();   
	void TestDivideVectorByScalar();   
	void TestAddVectors();   
	void TestSubtractVectors();   
	void TestAddMultipleOfVector();   
	void TestSubtractMultipleOfVector();   
	void TestDotProduct();   
	void TestVectorProduct();   
	void TestCopyVector();   
	void TestEquals();   
	void TestCalculateNormalsToU();   
	void TestCalculateNormalsToV();   
	void TestCalculateNormalsToW();   
	void TestDistance();   
	void TestDistanceSquared();
	void TestPrintVector();    
	void TestDivideMatrixByHomoCoord();  
	void TestSetMatrixToZero();
	void TestSetMatrixToIdentity();
	void TestMultiplyMatrixByScalar();   
	void TestDivideMatrixByScalar();     
	void TestMultiplyColumnsByScalars();
	void TestMultiplyRowsByScalars();
	void TestMultiplyMatrixByVector();
	void TestMultiplyMatrixByMatrix();
	void TestTranspose();
	void TestGetColumn();
	void TestPrintMatrix();     
	void TestTranspose2DArray();
	void TestCopy2DArrayToMatrix3x3();
	void TestCopy2DArrayToMatrix4x4();
	void TestCopyMatrixTo2DArray3x3();
	void TestCopyMatrixTo2DArray4x4();
};


int 
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( medOpMatrixVectorMathTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
