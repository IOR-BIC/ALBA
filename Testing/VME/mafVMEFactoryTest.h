/*=========================================================================

 Program: MAF2
 Module: mafVMEFactoryTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEFactoryTEST_H__
#define __CPP_UNIT_mafVMEFactoryTEST_H__

#include "mafTest.h"

class mafVMEFactoryTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafVMEFactoryTest );
  CPPUNIT_TEST( TestGetInstance );
  CPPUNIT_TEST( Initialize_CreateVMEInstance );
	CPPUNIT_TEST( TestGetMAFSourceVersion );
	CPPUNIT_TEST( TestGetDescription );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestGetInstance();
    void Initialize_CreateVMEInstance();
		void TestGetMAFSourceVersion();
		void TestGetDescription();
};

#endif
