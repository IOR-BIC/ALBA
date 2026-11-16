/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterPolylineTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterPolylineTest_H
#define CPP_UNIT_albaOpImporterPolylineTest_H

#include "albaTest.h"

class albaOpImporterPolylineTest : public albaTest
{
	CPPUNIT_TEST_SUITE(albaOpImporterPolylineTest);
	CPPUNIT_TEST(TestImport);
	CPPUNIT_TEST(TestScalars);
	CPPUNIT_TEST(TestSetters);
	CPPUNIT_TEST(TestWrongColumns);
	CPPUNIT_TEST(TestCommentLine);
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestImport();
	void TestScalars();
	void TestSetters();
	void TestWrongColumns();
	void TestCommentLine();
};

#endif
