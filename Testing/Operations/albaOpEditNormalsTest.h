/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditNormalsTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpEditNormalsTest_H
#define CPP_UNIT_albaOpEditNormalsTest_H

#include "albaTest.h"

class albaOpEditNormalsTest : public albaTest
{
	CPPUNIT_TEST_SUITE( albaOpEditNormalsTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST( TestUndo1 );
	CPPUNIT_TEST( TestUndo2 );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Test();
	void TestUndo1();
	void TestUndo2();
};

#endif
