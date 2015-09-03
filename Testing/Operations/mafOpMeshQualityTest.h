/*=========================================================================

 Program: MAF2
 Module: mafOpMeshQualityTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpMeshQualityTest_H
#define CPP_UNIT_mafOpMeshQualityTest_H

#include "mafTest.h"

class mafOpMeshQualityTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpMeshQualityTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOpRun();
};

#endif
