/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEMeshTest_H__
#define __CPP_UNIT_mafVMEMeshTest_H__

#include "mafTest.h"

class mafVMEMeshTest : public mafTest
{

public:

  CPPUNIT_TEST_SUITE( mafVMEMeshTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructo();
  void TestSetData();
};

#endif
