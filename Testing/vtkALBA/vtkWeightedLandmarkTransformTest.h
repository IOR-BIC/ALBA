/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkWeightedLandmarkTransformTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkWeightedLandmarkTransformTest_H__
#define __CPP_UNIT_vtkWeightedLandmarkTransformTest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class albaVMELandmarkCloud;

//------------------------------------------------------------------------------
// Test class for vtkWeightedLandmarkTransform
//------------------------------------------------------------------------------
class vtkWeightedLandmarkTransformTest : public albaTest
{
  public:
  
  CPPUNIT_TEST_SUITE( vtkWeightedLandmarkTransformTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestUpdate );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestUpdate();
};

#endif
