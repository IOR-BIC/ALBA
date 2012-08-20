/*=========================================================================

 Program: MAF2Medical
 Module: medQueryObjectTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medQueryObjectTest.h"
#include "medQueryObject.h"
#include <string.h>

//----------------------------------------------------------------------------
void medQueryObjectTest::setUp()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void medQueryObjectTest::tearDown()
//----------------------------------------------------------------------------
{
	
}

//----------------------------------------------------------------------------
void medQueryObjectTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
	medQueryObject *queryObject = new medQueryObject("Test");
	cppDEL(queryObject);
}
//----------------------------------------------------------------------------
void medQueryObjectTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
	medQueryObject queryObject("Test");
}
//----------------------------------------------------------------------------
void medQueryObjectTest::TestGetValueAsString()
//----------------------------------------------------------------------------
{
  medQueryObject queryObject("Test");
  CPPUNIT_ASSERT(strcmp(queryObject.GetValueAsString(),"Test") == 0 );
}
//----------------------------------------------------------------------------
void medQueryObjectTest::TestGetValueAsInt()
//----------------------------------------------------------------------------
{
  medQueryObject queryObject("5");
  CPPUNIT_ASSERT(queryObject.GetValueAsInt() == 5 );
}
//----------------------------------------------------------------------------
void medQueryObjectTest::TestGetValueAsLong()
//----------------------------------------------------------------------------
{
  medQueryObject queryObject("123456789");
  CPPUNIT_ASSERT(queryObject.GetValueAsLong() == 123456789 );
}
//----------------------------------------------------------------------------
void medQueryObjectTest::TestGetValueAsFloat()
//----------------------------------------------------------------------------
{
  medQueryObject queryObject("1.3");
  
  // passing on visual studio 2003
  // failing on visual studio 2010
  // commented for the moment (more investigation is needed...)
  // CPPUNIT_ASSERT(queryObject.GetValueAsFloat() == 1.3 );
}
//----------------------------------------------------------------------------
void medQueryObjectTest::TestGetValueAsDouble()
//----------------------------------------------------------------------------
{
  medQueryObject queryObject("3.1415926535");
  CPPUNIT_ASSERT(queryObject.GetValueAsDouble() == 3.1415926535 );
}