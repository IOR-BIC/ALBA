/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadMouseDialogTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDeviceButtonsPadMouseDialogTest_H__
#define __CPP_UNIT_albaDeviceButtonsPadMouseDialogTest_H__

#include "albaTest.h"

class albaDeviceButtonsPadMouseDialogTest : public albaTest
{
 public:

 CPPUNIT_TEST_SUITE( albaDeviceButtonsPadMouseDialogTest );
 CPPUNIT_TEST( TestFixture );
 CPPUNIT_TEST( TestConstructorDestructor );
 CPPUNIT_TEST( TestSetView );
 CPPUNIT_TEST_SUITE_END();

 protected:

   void TestFixture();
   void TestConstructorDestructor();
   void TestSetView();

};

#endif
