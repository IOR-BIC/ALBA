/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_TimeSetTest_H__
#define __CPP_UNIT_TimeSetTest_H__

#include "albaTest.h"
#include "albaVME.h"


class TimeSetTest : public albaTest
{
  CPPUNIT_TEST_SUITE( TimeSetTest );
  CPPUNIT_TEST( TimeSetMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TimeSetMainTest();
};

#endif