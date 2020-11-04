/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterPointCloudTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterPointCloudTest_H
#define CPP_UNIT_albaOpImporterPointCloudTest_H

#include "albaTest.h"

class albaOpImporterPointCloudTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterPointCloudTest );
  CPPUNIT_TEST(TestImport);
	CPPUNIT_TEST(TestFirstCoordCol);
	CPPUNIT_TEST(TestNumberOfScalars);
	CPPUNIT_TEST(TestScalarColumn);
	CPPUNIT_TEST(TestScalarNames);
	CPPUNIT_TEST(TestCommentLine);
	CPPUNIT_TEST_SUITE_END();

  protected:
    void TestImport();
		void TestFirstCoordCol();
		void TestNumberOfScalars();
		void TestScalarColumn();
		void TestScalarNames();
		void TestCommentLine();
};

#endif
