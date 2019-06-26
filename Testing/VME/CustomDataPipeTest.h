/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: CustomDataPipeTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CustomDataPipeTest_H__
#define __CPP_UNIT_CustomDataPipeTest_H__

#include "albaTest.h"
#include "albaVME.h"

class CustomDataPipeTest : public albaTest
{
  CPPUNIT_TEST_SUITE( CustomDataPipeTest );
  CPPUNIT_TEST( CustomDataPipeMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void CustomDataPipeMainTest();
};

#endif