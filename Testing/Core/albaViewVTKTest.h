/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_ALBAVIEWVTKTEST_H
#define CPP_UNIT_ALBAVIEWVTKTEST_H

#include "albaTest.h"


class albaViewVTKTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaViewVTKTest );
  CPPUNIT_TEST( CreateDestroyTest );
	CPPUNIT_TEST( Surface );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void CreateDestroyTest();
		void Surface();
};


#endif