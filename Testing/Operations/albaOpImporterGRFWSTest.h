/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterGRFWSTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterLandmarkWSTEST_H
#define CPP_UNIT_albaOpImporterLandmarkWSTEST_H

#include "albaTest.h"

class albaOpImporterGRFWSTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterGRFWSTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestRead );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestRead();
};

#endif
