/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterDicomTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterDicomTest_H
#define CPP_UNIT_albaOpImporterDicomTest_H

#include "albaTest.h"
using namespace std;
 
class albaOpImporterDicomTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaOpImporterDicomTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestAccept );
    CPPUNIT_TEST( TestCreateVolume );
    CPPUNIT_TEST( TestCompareDicomImage );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAccept();
	void TestCompareDicomImage();
  void TestCreateVolume();

};

#endif
