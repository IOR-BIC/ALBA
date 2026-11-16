/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPolylineTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterPolylineTest_H
#define CPP_UNIT_albaOpExporterPolylineTest_H

#include "albaTest.h"

class albaOpExporterPolylineTest : public albaTest
{
	CPPUNIT_TEST_SUITE(albaOpExporterPolylineTest);
	CPPUNIT_TEST(TestAccept);
	CPPUNIT_TEST(TestExport);
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestAccept();
	void TestExport();
};

#endif
