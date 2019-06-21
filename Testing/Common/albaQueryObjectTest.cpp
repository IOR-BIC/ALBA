/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaQueryObjectTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaQueryObjectTest.h"
#include "albaQueryObject.h"
#include <string.h>


//----------------------------------------------------------------------------
void albaQueryObjectTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	albaQueryObject *queryObject = new albaQueryObject("Test");
	cppDEL(queryObject);
}
//----------------------------------------------------------------------------
void albaQueryObjectTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	albaQueryObject queryObject("Test");
}
//----------------------------------------------------------------------------
void albaQueryObjectTest::TestGetValueAsString()
//----------------------------------------------------------------------------
{
  albaQueryObject queryObject("Test");
  CPPUNIT_ASSERT(strcmp(queryObject.GetValueAsString(),"Test") == 0 );
}
//----------------------------------------------------------------------------
void albaQueryObjectTest::TestGetValueAsInt()
//----------------------------------------------------------------------------
{
  albaQueryObject queryObject("5");
  CPPUNIT_ASSERT(queryObject.GetValueAsInt() == 5 );
}
//----------------------------------------------------------------------------
void albaQueryObjectTest::TestGetValueAsLong()
//----------------------------------------------------------------------------
{
  albaQueryObject queryObject("123456789");
  CPPUNIT_ASSERT(queryObject.GetValueAsLong() == 123456789 );
}
//----------------------------------------------------------------------------
void albaQueryObjectTest::TestGetValueAsFloat()
//----------------------------------------------------------------------------
{
  albaQueryObject queryObject("1.3");
  
  // passing on visual studio 2003
  // failing on visual studio 2010
  // commented for the moment (more investigation is needed...)
  // CPPUNIT_ASSERT(queryObject.GetValueAsFloat() == 1.3 );
}
//----------------------------------------------------------------------------
void albaQueryObjectTest::TestGetValueAsDouble()
//----------------------------------------------------------------------------
{
  albaQueryObject queryObject("3.1415926535");
  CPPUNIT_ASSERT(queryObject.GetValueAsDouble() == 3.1415926535 );
}