/*=========================================================================

 Program: MAF2
 Module: mafExpirationDateTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafExpirationDateTest.h"
#include "mafExpirationDate.h"

#include <wx/app.h>


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafExpirationDateTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafExpirationDateTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafExpirationDateTest::tearDown()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void mafExpirationDateTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafExpirationDate e1("30");
  mafExpirationDate e2("2009-06-30");
  mafExpirationDate e3(30);
  mafExpirationDate e4(2009,06,30);
}
//----------------------------------------------------------------------------
void mafExpirationDateTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafExpirationDate *e1 = new mafExpirationDate("30");
  mafExpirationDate *e2 = new mafExpirationDate("2009-06-30");
  mafExpirationDate *e3 = new mafExpirationDate(30);
  mafExpirationDate *e4 = new mafExpirationDate(2009,06,30);
  mafExpirationDate *e5 = new mafExpirationDate("NEVER");

  cppDEL(e1);
  cppDEL(e2);
  cppDEL(e3);
  cppDEL(e4);
  cppDEL(e5);
}
//----------------------------------------------------------------------------
void mafExpirationDateTest::TestHasExpired()
//----------------------------------------------------------------------------
{
  //used a expiration date already passed.
  mafExpirationDate e1 = mafExpirationDate("2008-06-10"); 
  result = e1.HasExpired();
  CPPUNIT_ASSERT(result == true);

  mafExpirationDate e2 = mafExpirationDate("2035-06-10"); 
  result = e2.HasExpired();
  CPPUNIT_ASSERT(result == false);

  mafExpirationDate e3 = mafExpirationDate("2005-06-10"); 
  result = e3.HasExpired();
  CPPUNIT_ASSERT(result == true);

  mafExpirationDate e4 = mafExpirationDate("2035-10-10"); 
  result = e4.HasExpired();
  CPPUNIT_ASSERT(result == false);

  mafExpirationDate e5 = mafExpirationDate("2036-10-10"); 
  result = e5.HasExpired();
  CPPUNIT_ASSERT(result == false);

  mafExpirationDate e6 = mafExpirationDate("NEVER"); 
  result = e6.HasExpired();
  CPPUNIT_ASSERT(result == false);

}
//----------------------------------------------------------------------------
void mafExpirationDateTest::TestGetInformation()
//----------------------------------------------------------------------------
{
  //used a expiration date already passed.
  mafExpirationDate e1 = mafExpirationDate("2008-06-10");
  result = e1.HasExpired();
  printf(">>>>>  %s   <<<<<", e1.GetInformation());
}