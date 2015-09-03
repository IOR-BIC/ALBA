/*=========================================================================

 Program: MAF2
 Module: mafView3DTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafView3DTEST_H__
#define __CPP_UNIT_mafView3DTEST_H__

#include "mafTest.h"

class mafView3DTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafView3DTest );
  CPPUNIT_TEST( CreateDestroyTest );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void CreateDestroyTest();
};

#endif