/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkTXTTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterLandmarkTXTTest_H
#define CPP_UNIT_albaOpImporterLandmarkTXTTest_H

#include "albaTest.h"

class albaOpImporterLandmarkTXTTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterLandmarkTXTTest );
  CPPUNIT_TEST( TestTXTImporter );
  CPPUNIT_TEST( TestVisibility );
  CPPUNIT_TEST( TestCoordinates );
 
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestTXTImporter();
    void TestVisibility();
    void TestCoordinates();
};

#endif
