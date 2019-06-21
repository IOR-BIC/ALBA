/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSlicerTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaViewSlicerTEST_H__
#define __CPP_UNIT_albaViewSlicerTEST_H__

#include "albaTest.h"

class albaViewSlicerTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaViewSlicerTest );
  CPPUNIT_TEST( CreateDestroyTest );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void CreateDestroyTest();
};

#endif