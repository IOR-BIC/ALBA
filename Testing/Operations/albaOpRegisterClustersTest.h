/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRegisterClustersTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpRegisterClustersTest_H__
#define __CPP_UNIT_albaOpRegisterClustersTest_H__

#include "albaTest.h"

class albaOpRegisterClustersTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaOpRegisterClustersTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( CopyTest );
  CPPUNIT_TEST( AcceptTest );   
  CPPUNIT_TEST( OpRunTest );
  CPPUNIT_TEST( OpDoUndoTest );
  CPPUNIT_TEST( ClosedCloudAcceptTest );
  CPPUNIT_TEST( SurfaceAcceptTest );
	CPPUNIT_TEST( OutputTest );
	CPPUNIT_TEST( Output2Test );
  CPPUNIT_TEST_SUITE_END();

protected:

    void DynamicAllocationTest();
    void CopyTest();
    void AcceptTest();   
    void OpRunTest();
    void OpDoUndoTest();
    void ClosedCloudAcceptTest();
    void SurfaceAcceptTest();
		void OutputTest();
		void Output2Test();

		bool AboutEqual(double a, double b, double epsilon = 0.001);
};

#endif
