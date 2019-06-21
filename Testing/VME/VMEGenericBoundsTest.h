/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: VMEGenericBoundsTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VMEGenericBoundsTest_H__
#define __CPP_UNIT_VMEGenericBoundsTest_H__

#include "albaTest.h"

class VMEGenericBoundsTest : public albaTest
{
  CPPUNIT_TEST_SUITE( VMEGenericBoundsTest );
  CPPUNIT_TEST( VMEGenericBoundsMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void VMEGenericBoundsMainTest();
};

#endif