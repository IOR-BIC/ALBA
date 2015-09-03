/*=========================================================================

 Program: MAF2
 Module: mafOpImporterLandmarkTXTTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterLandmarkTXTTest_H
#define CPP_UNIT_mafOpImporterLandmarkTXTTest_H

#include "mafTest.h"

class mafOpImporterLandmarkTXTTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterLandmarkTXTTest );
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
