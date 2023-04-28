/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEInfoTextTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEInfoTextTEST_H__
#define __CPP_UNIT_albaVMEInfoTextTEST_H__

#include "albaTest.h"

class vtkPolyData;

class albaVMEInfoTextTest : public albaTest
{
public :

	CPPUNIT_TEST_SUITE( albaVMEInfoTextTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestGetVTKOutput );
  CPPUNIT_TEST( TestGetOutput );
  CPPUNIT_TEST( TestGetSetLabel );
  CPPUNIT_TEST( TestGetIcon );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestEquals );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
	void TestGetVTKOutput();
  void TestGetOutput();
  void TestGetSetLabel();
  void TestGetIcon();
  void TestDeepCopy();
  void TestEquals();

  bool result;
};

#endif
