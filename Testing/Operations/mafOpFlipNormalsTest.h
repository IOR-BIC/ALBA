/*=========================================================================

 Program: MAF2
 Module: mafOpFlipNormalsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpFlipNormalsTEST_H
#define CPP_UNIT_mafOpFlipNormalsTEST_H

#include "mafTest.h"

class mafOpFlipNormalsTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpFlipNormalsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestSetSeed );
  CPPUNIT_TEST( TestSetDiameter );
  CPPUNIT_TEST( TestFlipNormals );
  CPPUNIT_TEST( TestModifyAllNormal );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestAccept();
  void TestOpRun();
  void TestSetSeed();
  void TestSetDiameter();
  void TestFlipNormals();
  void TestModifyAllNormal();
};

#endif
