/*=========================================================================

 Program: MAF2
 Module: mafPipeFactoryTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeFactoryTEST_H__
#define __CPP_UNIT_mafPipeFactoryTEST_H__

#include "mafTest.h"

class mafPipeFactoryTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafPipeFactoryTest );
  CPPUNIT_TEST( TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames );
	CPPUNIT_TEST( TestGetMAFSourceVersion );
	CPPUNIT_TEST( TestGetDescription );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestGetInstance_Initialize_CreatePipeInstance_RegisterNewPipe_GetPipeNames();
		void TestGetMAFSourceVersion();
		void TestGetDescription();
};



#endif
