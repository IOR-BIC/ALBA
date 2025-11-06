/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeFactoryTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeFactoryTEST_H__
#define __CPP_UNIT_albaPipeFactoryTEST_H__

#include "albaTest.h"

class albaPipeFactoryTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaPipeFactoryTest );
  CPPUNIT_TEST( TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames );
	CPPUNIT_TEST( TestGetDescription );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames();
		void TestGetDescription();
};



#endif
