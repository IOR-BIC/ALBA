/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEFactoryTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEFactoryTEST_H__
#define __CPP_UNIT_albaVMEFactoryTEST_H__

#include "albaTest.h"

class albaVMEFactoryTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaVMEFactoryTest );
  CPPUNIT_TEST( TestGetInstance );
  CPPUNIT_TEST( Initialize_CreateVMEInstance );
	CPPUNIT_TEST( TestGetDescription );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestGetInstance();
    void Initialize_CreateVMEInstance();
		void TestGetALBASourceVersion();
		void TestGetDescription();
};

#endif
