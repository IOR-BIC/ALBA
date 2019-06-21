/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEMeshTest_H__
#define __CPP_UNIT_albaVMEMeshTest_H__

#include "albaTest.h"

class albaVMEMeshTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaVMEMeshTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructo();
  void TestSetData();
};

#endif
