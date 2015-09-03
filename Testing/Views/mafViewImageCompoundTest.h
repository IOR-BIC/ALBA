/*=========================================================================

 Program: MAF2
 Module: mafViewImageCompoundTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafViewImageCompoundTEST_H__
#define __CPP_UNIT_mafViewImageCompoundTEST_H__

#include "mafTest.h"

class mafViewImageCompoundTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafViewImageCompoundTest );
  CPPUNIT_TEST( CreateDestroyTest );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void CreateDestroyTest();
};

#endif