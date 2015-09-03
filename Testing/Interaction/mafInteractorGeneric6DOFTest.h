/*=========================================================================

 Program: MAF2
 Module: mafInteractorGeneric6DOFTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorGeneric6DOFTest_H__
#define __CPP_UNIT_mafInteractorGeneric6DOFTest_H__

#include "mafTest.h"

class mafInteractorGeneric6DOFTest : public mafTest
{
public:

	CPPUNIT_TEST_SUITE( mafInteractorGeneric6DOFTest );
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
