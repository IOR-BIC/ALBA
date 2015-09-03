/*=========================================================================

 Program: MAF2
 Module: mafOpImporterLandmarkWSTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterLandmarkWSTest_H
#define CPP_UNIT_mafOpImporterLandmarkWSTest_H

#include "mafTest.h"

class mafOpImporterLandmarkWSTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterLandmarkWSTest );
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
