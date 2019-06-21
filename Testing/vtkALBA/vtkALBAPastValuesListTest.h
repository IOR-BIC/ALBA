/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPastValuesListTest
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAPastValuesListtest_H__
#define __CPP_UNIT_vtkALBAPastValuesListtest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Test class for vtkALBAPastValuesList
//------------------------------------------------------------------------------
class vtkALBAPastValuesListTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBAPastValuesListTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestPastValues );
    CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
    void TestFixture();
    void TestPastValues();
};

#endif
