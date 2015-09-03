/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceEditorTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMESurfaceEditorTest_H__
#define __CPP_UNIT_mafVMESurfaceEditorTest_H__

#include "mafTest.h"

class mafVMESurfaceEditorTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafVMESurfaceEditorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST( TestSetGetVisualPipe );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestSetData();
    void TestSetGetVisualPipe();
    bool m_Result;
};

#endif
