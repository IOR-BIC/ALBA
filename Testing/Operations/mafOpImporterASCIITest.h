/*=========================================================================

 Program: MAF2
 Module: mafOpImporterASCIITest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterASCIITest_H__
#define __CPP_UNIT_mafOpImporterASCIITest_H__

#include "mafTest.h"

class mafOpImporterASCIITest : public mafTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();
  
  CPPUNIT_TEST_SUITE( mafOpImporterASCIITest );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestImportSingleASCIIFile );
  CPPUNIT_TEST( TestImportMultipleASCIIFiles );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestConstructor();
  void TestCopy();
  void TestAccept();
  void TestImportSingleASCIIFile();
  void TestImportMultipleASCIIFiles();
  bool result;
};

#endif
