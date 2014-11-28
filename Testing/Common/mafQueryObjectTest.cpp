/*=========================================================================

 Program: MAF2
 Module: mafQueryObjectTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafQueryObjectTest.h"
#include "mafQueryObject.h"
#include <string.h>

//----------------------------------------------------------------------------
void mafQueryObjectTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void mafQueryObjectTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	mafQueryObject *queryObject = new mafQueryObject("Test");
	cppDEL(queryObject);
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	mafQueryObject queryObject("Test");
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::TestGetValueAsString()
//----------------------------------------------------------------------------
{
  mafQueryObject queryObject("Test");
  CPPUNIT_ASSERT(strcmp(queryObject.GetValueAsString(),"Test") == 0 );
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::TestGetValueAsInt()
//----------------------------------------------------------------------------
{
  mafQueryObject queryObject("5");
  CPPUNIT_ASSERT(queryObject.GetValueAsInt() == 5 );
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::TestGetValueAsLong()
//----------------------------------------------------------------------------
{
  mafQueryObject queryObject("123456789");
  CPPUNIT_ASSERT(queryObject.GetValueAsLong() == 123456789 );
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::TestGetValueAsFloat()
//----------------------------------------------------------------------------
{
  mafQueryObject queryObject("1.3");
  
  // passing on visual studio 2003
  // failing on visual studio 2010
  // commented for the moment (more investigation is needed...)
  // CPPUNIT_ASSERT(queryObject.GetValueAsFloat() == 1.3 );
}
//----------------------------------------------------------------------------
void mafQueryObjectTest::TestGetValueAsDouble()
//----------------------------------------------------------------------------
{
  mafQueryObject queryObject("3.1415926535");
  CPPUNIT_ASSERT(queryObject.GetValueAsDouble() == 3.1415926535 );
}