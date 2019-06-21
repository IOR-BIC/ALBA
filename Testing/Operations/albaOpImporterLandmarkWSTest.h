/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkWSTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterLandmarkWSTest_H
#define CPP_UNIT_albaOpImporterLandmarkWSTest_H

#include "albaTest.h"

class albaOpImporterLandmarkWSTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpImporterLandmarkWSTest );
  CPPUNIT_TEST( TestWSImporter );
  CPPUNIT_TEST( TestVisibility );
  CPPUNIT_TEST( TestCoordinates );
 
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestWSImporter();
    void TestVisibility();
    void TestCoordinates();
};

#endif
