/*=========================================================================

 Program: MAF2
 Module: mafOpCropDeformableROITest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpCropDeformableROITest_H__
#define __CPP_UNIT_mafOpCropDeformableROITest_H__

#include "mafTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/**
class name : mafOpCropDeformableROITest
Test class for mafOpCropDeformableROI
*/
class mafOpCropDeformableROITest : public mafTest
{
public:

  /** Start test suite macro */
  CPPUNIT_TEST_SUITE( mafOpCropDeformableROITest );
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
