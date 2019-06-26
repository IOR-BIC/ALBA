/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterASCIITest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpImporterASCIITest_H__
#define __CPP_UNIT_albaOpImporterASCIITest_H__

#include "albaTest.h"

class albaOpImporterASCIITest : public albaTest
{
public:
  
  CPPUNIT_TEST_SUITE( albaOpImporterASCIITest );
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
