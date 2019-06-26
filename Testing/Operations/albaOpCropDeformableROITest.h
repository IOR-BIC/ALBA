/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCropDeformableROITest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpCropDeformableROITest_H__
#define __CPP_UNIT_albaOpCropDeformableROITest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/**
class name : albaOpCropDeformableROITest
Test class for albaOpCropDeformableROI
*/
class albaOpCropDeformableROITest : public albaTest
{
public:

  /** Start test suite macro */
  CPPUNIT_TEST_SUITE( albaOpCropDeformableROITest );
  /** macro for test TestDynamicAllocation */
  CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestStaticAllocation );
  /** macro for test TestOpExecute */
  CPPUNIT_TEST( TestOpExecute );
  /** End test suite macro */
  CPPUNIT_TEST_SUITE_END();

protected:
  /** Dynamic allocation test */
  void TestDynamicAllocation();
  /** Static allocation test */
  void TestStaticAllocation();
  /** Algorithm execution test */
  void TestOpExecute();
};

#endif
