/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecimateSurfaceTest
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpDecimateSurfaceTEST_H__
#define __CPP_UNIT_albaOpDecimateSurfaceTEST_H__

#include "albaTest.h"

class albaOpDecimateSurfaceTest : public albaTest
{
	CPPUNIT_TEST_SUITE( albaOpDecimateSurfaceTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Test();
};

#endif
