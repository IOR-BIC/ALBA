/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEAnalogTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEAnalogTEST_H__
#define __CPP_UNIT_albaVMEAnalogTEST_H__

#include "albaTest.h"

class albaVMEAnalogTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaVMEAnalogTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestPrint );
  CPPUNIT_TEST( TestGetLocalTimeBounds );
  CPPUNIT_TEST( TestIsAnimated );
  CPPUNIT_TEST( TestGetTimeBounds );
  CPPUNIT_TEST( TestGetLocalTimeStamps );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestPrint();
    void TestGetLocalTimeBounds();
    void TestIsAnimated();
    void TestGetTimeBounds();
    void TestGetLocalTimeStamps();

    bool m_Result;
};

#endif
