/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorableTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaStorableTest_H__
#define __CPP_UNIT_albaStorableTest_H__

#include "albaTest.h"

#include <iostream>


class albaStorableTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaStorableTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( StoreTest );
  CPPUNIT_TEST( RestoreTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void DynamicAllocationTest();
  void StoreTest();
  void RestoreTest();
};


#endif // #ifndef __CPP_UNIT_albaStorableTest_H__
