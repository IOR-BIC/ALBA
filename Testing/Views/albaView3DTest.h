/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaView3DTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaView3DTEST_H__
#define __CPP_UNIT_albaView3DTEST_H__

#include "albaTest.h"

class albaView3DTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaView3DTest );
  CPPUNIT_TEST( CreateDestroyTest );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void CreateDestroyTest();
};

#endif