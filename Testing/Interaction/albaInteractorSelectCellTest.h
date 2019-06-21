/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSelectCellTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractorSelectCellTest_H__
#define __CPP_UNIT_albaInteractorSelectCellTest_H__

#include "albaTest.h"

class albaInteractorSelectCellTest : public albaTest
{
  public:
  
    CPPUNIT_TEST_SUITE( albaInteractorSelectCellTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConstructor );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestConstructor();
      
};

#endif
