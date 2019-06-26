/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: VMEGenericPoseTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VMEGenericPoseTest_H__
#define __CPP_UNIT_VMEGenericPoseTest_H__

#include "albaTest.h"

class VMEGenericPoseTest : public albaTest
{
  CPPUNIT_TEST_SUITE( VMEGenericPoseTest );
  CPPUNIT_TEST( VMEGenericPoseMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void VMEGenericPoseMainTest();
};

#endif