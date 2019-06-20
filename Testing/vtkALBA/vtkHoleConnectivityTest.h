/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkHoleConnectivityTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkHoleConnectivityTEST_H__
#define __CPP_UNIT_vtkHoleConnectivityTEST_H__

#include "albaTest.h"

class vtkHoleConnectivityTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkHoleConnectivityTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestPrintSelf );
  CPPUNIT_TEST( TestGetClassName );
  CPPUNIT_TEST( TestSetGetPointID );
  CPPUNIT_TEST( TestSetGetPoint );
  CPPUNIT_TEST( TestExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestPrintSelf();
  void TestGetClassName();
  void TestSetGetPointID();
  void TestSetGetPoint();
  void TestExecution();
  
};

#endif