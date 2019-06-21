/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILandmarkTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUILandmarkTest_H__
#define __CPP_UNIT_albaGUILandmarkTest_H__

#include "albaTest.h"

class albaGUILandmarkTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaGUILandmarkTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestVmeAccept );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestVmeAccept();

  bool m_Result;

};

#endif
