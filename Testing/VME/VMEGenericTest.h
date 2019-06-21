/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: VMEGenericTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VMEGenericTest_H__
#define __CPP_UNIT_VMEGenericTest_H__

#include "albaTest.h"

class VMEGenericTest : public albaTest
{
  CPPUNIT_TEST_SUITE( VMEGenericTest );
  CPPUNIT_TEST( VMEGenericMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void VMEGenericMainTest();
};

#endif