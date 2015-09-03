/*=========================================================================

 Program: MAF2
 Module: mafCurlUtilityTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafCurlUtilityTest_H__
#define __CPP_UNIT_mafCurlUtilityTest_H__

#include "mafTest.h"

class mafCurlUtilityTest : public mafTest
{
public:

	CPPUNIT_TEST_SUITE( mafCurlUtilityTest );
	CPPUNIT_TEST( TestWriteMemoryCallback );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestFixture();
	void TestWriteMemoryCallback();

};

#endif
