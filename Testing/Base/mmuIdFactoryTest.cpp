/*=========================================================================

 Program: MAF2
 Module: mmuIdFactoryTest
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
#include "mmuIdFactoryTest.h"

#include "mmuIdFactory.h"
#include "mafDecl.h"


//----------------------------------------------------------------------------
void mmuIdFactoryTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::StaticAllocationTest()
//----------------------------------------------------------------------------
{
  mmuIdFactory m;
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mmuIdFactory *m = new mmuIdFactory();
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::GetNextIdTest()
//----------------------------------------------------------------------------
{
  mmuIdFactory *m = new mmuIdFactory();
  int idCounter = m->m_IdCounter;
  mafString idName1 = "eventNew1";
  mafString idName2 = "eventNew2";
  mafID id;

  //Create a new event
  id = m->GetNextId(idName1);
  CPPUNIT_ASSERT(id == idCounter);

  //Create a new event and check if it is the next one
  id = m->GetNextId(idName2);
  CPPUNIT_ASSERT(id == idCounter+1);

  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::GetIdTest()
//----------------------------------------------------------------------------
{
  mmuIdFactory *m = new mmuIdFactory();
  mafString idName1 = "eventNew1";
  mafString idName2 = "eventNew2";
  mafID id;

  //Create a new event
  id = m->GetNextId(idName1);

  //Check if the new event has the right Id
  mafID idReturn1 = m->GetId(idName1);
  CPPUNIT_ASSERT(idReturn1 == id);

  //Create a new event and check if it is the next one
  id = m->GetNextId(idName2);

  //Check if the new event has the right Id
  mafID idReturn2 = m->GetId(idName2);
  CPPUNIT_ASSERT(idReturn2 == idReturn1+1);

  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::GetIdNameTest()
//----------------------------------------------------------------------------
{
  mmuIdFactory *m = new mmuIdFactory();
  mafString idName1 = "eventNew1";
  mafString idName2 = "eventNew2";
  mafID id;

  //Create a new event
  id = m->GetNextId(idName1);

  //Check if the new event has the right name
  mafString name1 = m->GetIdName(id);
  CPPUNIT_ASSERT(name1 == idName1);

  //Create a new event and check if it is the next one
  id = m->GetNextId(idName2);

  //Check if the new event has the right name
  mafString name2 = m->GetIdName(id);
  CPPUNIT_ASSERT(name2 == idName2);

  cppDEL(m);
}
