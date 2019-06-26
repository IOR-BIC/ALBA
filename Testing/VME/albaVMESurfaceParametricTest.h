/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceParametricTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMESurfaceParametricTEST_H__
#define __CPP_UNIT_albaVMESurfaceParametricTEST_H__

#include "albaTest.h"

class albaVMESurfaceParametricTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaVMESurfaceParametricTest );
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestSetData();
};

#endif
