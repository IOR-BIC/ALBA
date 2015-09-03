/*=========================================================================

 Program: MAF2
 Module: mafInteractorSelectCellTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorSelectCellTest_H__
#define __CPP_UNIT_mafInteractorSelectCellTest_H__

#include "mafTest.h"

class mafInteractorSelectCellTest : public mafTest
{
  public:
  
    CPPUNIT_TEST_SUITE( mafInteractorSelectCellTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConstructor );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestConstructor();
      
};

#endif
