/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGenericTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMEGENERICTEST_H__
#define __CPP_UNIT_ALBAVMEGENERICTEST_H__

#include "albaTest.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"


class albaVMEGenericTest : public albaTest
{

  public:


    CPPUNIT_TEST_SUITE( albaVMEGenericTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestSetData );
    CPPUNIT_TEST( TestGetVisualPipe );
    CPPUNIT_TEST_SUITE_END();

protected:
    void TestDynamicAllocation();
    void TestSetData();
    void TestGetVisualPipe();

};

#endif
