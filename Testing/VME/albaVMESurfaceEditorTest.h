/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceEditorTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMESurfaceEditorTest_H__
#define __CPP_UNIT_albaVMESurfaceEditorTest_H__

#include "albaTest.h"

class albaVMESurfaceEditorTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaVMESurfaceEditorTest );
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
