/*=========================================================================

 Program: MAF2
 Module: mafASCIIImporterUtilityTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafASCIIImporterUtilityTest_H__
#define __CPP_UNIT_mafASCIIImporterUtilityTest_H__

#include "mafTest.h"

#include <iostream>


class mafASCIIImporterUtilityTest : public mafTest
{
public:
  CPPUNIT_TEST_SUITE( mafASCIIImporterUtilityTest );
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

#endif // #ifndef __CPP_UNIT_mafASCIIImporterUtilityTest_H__
