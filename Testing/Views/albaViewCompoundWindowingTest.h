/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewCompoundWindowingTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaViewCompoundWindowingTEST_H__
#define __CPP_UNIT_albaViewCompoundWindowingTEST_H__

#include "albaTest.h"

class albaViewCompoundWindowingTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaViewCompoundWindowingTest );
  CPPUNIT_TEST( CreateDestroyTest );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void CreateDestroyTest();
};

#endif