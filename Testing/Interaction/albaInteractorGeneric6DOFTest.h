/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorGeneric6DOFTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractorGeneric6DOFTest_H__
#define __CPP_UNIT_albaInteractorGeneric6DOFTest_H__

#include "albaTest.h"

class albaInteractorGeneric6DOFTest : public albaTest
{
public:

	CPPUNIT_TEST_SUITE( albaInteractorGeneric6DOFTest );
	CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestConstructorDestructor );
	CPPUNIT_TEST( TestOnEvent );
	CPPUNIT_TEST( TestOnMove );
	CPPUNIT_TEST( TestSetGetDifferentialMoving );
	CPPUNIT_TEST( TestDifferentialMovingOnOff );
 	CPPUNIT_TEST_SUITE_END();

protected:

	void TestFixture();
	void TestConstructorDestructor();
	void TestOnEvent();
    void TestOnMove();
    void TestSetGetDifferentialMoving();
    void TestDifferentialMovingOnOff();

};

#endif
