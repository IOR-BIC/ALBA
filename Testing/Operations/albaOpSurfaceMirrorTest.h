/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSurfaceMirrorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpSurfaceMirrorTEST_H
#define CPP_UNIT_albaOpSurfaceMirrorTEST_H

#include "albaTest.h"

class albaOpSurfaceMirrorTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpSurfaceMirrorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestDynamicAllocation();
    void TestOpRun();
};

#endif
