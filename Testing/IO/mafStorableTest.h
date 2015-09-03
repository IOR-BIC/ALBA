/*=========================================================================

 Program: MAF2
 Module: mafStorableTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafStorableTest_H__
#define __CPP_UNIT_mafStorableTest_H__

#include "mafTest.h"

#include <iostream>


class mafStorableTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( mafStorableTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( StoreTest );
  CPPUNIT_TEST( RestoreTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void DynamicAllocationTest();
  void StoreTest();
  void RestoreTest();
};


#endif // #ifndef __CPP_UNIT_mafStorableTest_H__
