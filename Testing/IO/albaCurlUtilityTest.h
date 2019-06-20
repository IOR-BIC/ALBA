/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCurlUtilityTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaCurlUtilityTest_H__
#define __CPP_UNIT_albaCurlUtilityTest_H__

#include "albaTest.h"

class albaCurlUtilityTest : public albaTest
{
public:

	CPPUNIT_TEST_SUITE( albaCurlUtilityTest );
	CPPUNIT_TEST( TestWriteMemoryCallback );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestFixture();
	void TestWriteMemoryCallback();

};

#endif
