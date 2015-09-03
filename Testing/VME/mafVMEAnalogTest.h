/*=========================================================================

 Program: MAF2
 Module: mafVMEAnalogTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEAnalogTEST_H__
#define __CPP_UNIT_mafVMEAnalogTEST_H__

#include "mafTest.h"

class mafVMEAnalogTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafVMEAnalogTest );
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
