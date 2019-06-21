/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeGrayTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMEVOLUMEGRAYTEST_H__
#define __CPP_UNIT_ALBAVMEVOLUMEGRAYTEST_H__

#include "albaTest.h"

class albaVMEVolumeGrayTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaVMEVolumeGrayTest );
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestSetData();
};

#endif
