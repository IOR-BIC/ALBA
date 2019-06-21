/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterRAWTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterRAWTest_H
#define CPP_UNIT_albaOpExporterRAWTest_H

#include "albaTest.h"

class albaOpExporterRAWTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpExporterRAWTest );
  CPPUNIT_TEST( Test );
	CPPUNIT_TEST( TestRG );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
		void TestRG();
};

#endif
