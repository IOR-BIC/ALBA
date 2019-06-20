/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaASCIIImporterUtilityTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaASCIIImporterUtilityTest_H__
#define __CPP_UNIT_albaASCIIImporterUtilityTest_H__

#include "albaTest.h"

#include <iostream>


class albaASCIIImporterUtilityTest : public albaTest
{
public:
  CPPUNIT_TEST_SUITE( albaASCIIImporterUtilityTest );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( GetNumberOfRowsTest );
  CPPUNIT_TEST( GetNumberOfColsTest );
  CPPUNIT_TEST( GetNumberOfScalarsTest );
  CPPUNIT_TEST( GetScalarTest );
  CPPUNIT_TEST( GetMatrixTest );
  CPPUNIT_TEST( ExtractRowTest );
  CPPUNIT_TEST( ExtractColumnTest );
  CPPUNIT_TEST_SUITE_END();

protected:
    void TestConstructor();
    void GetNumberOfRowsTest();
    void GetNumberOfColsTest();
    void GetNumberOfScalarsTest();
    void GetScalarTest();
    void GetMatrixTest();
    void ExtractRowTest();
    void ExtractColumnTest();
};

#endif // #ifndef __CPP_UNIT_albaASCIIImporterUtilityTest_H__
