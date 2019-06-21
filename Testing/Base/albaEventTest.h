/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaEventTest_H__
#define __CPP_UNIT_albaEventTest_H__

#include "albaTest.h"
#include "albaVME.h"
#include "albaVMEGenericAbstract.h"


class albaEventTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaEventTest );
  CPPUNIT_TEST( MafEventTestMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void MafEventTestMainTest();
};

#endif