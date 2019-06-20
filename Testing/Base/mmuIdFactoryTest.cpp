/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuIdFactoryTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mmuIdFactoryTest.h"

#include "mmuIdFactory.h"
#include "albaDecl.h"


//----------------------------------------------------------------------------
void mmuIdFactoryTest::TestFixture()
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
  albaString idName1 = "eventNew1";
  albaString idName2 = "eventNew2";
  albaID id;

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
  albaString idName1 = "eventNew1";
  albaString idName2 = "eventNew2";
  albaID id;

  //Create a new event
  id = m->GetNextId(idName1);

  //Check if the new event has the right Id
  albaID idReturn1 = m->GetId(idName1);
  CPPUNIT_ASSERT(idReturn1 == id);

  //Create a new event and check if it is the next one
  id = m->GetNextId(idName2);

  //Check if the new event has the right Id
  albaID idReturn2 = m->GetId(idName2);
  CPPUNIT_ASSERT(idReturn2 == idReturn1+1);

  cppDEL(m);
}
//----------------------------------------------------------------------------
void mmuIdFactoryTest::GetIdNameTest()
//----------------------------------------------------------------------------
{
  mmuIdFactory *m = new mmuIdFactory();
  albaString idName1 = "eventNew1";
  albaString idName2 = "eventNew2";
  albaID id;

  //Create a new event
  id = m->GetNextId(idName1);

  //Check if the new event has the right name
  albaString name1 = m->GetIdName(id);
  CPPUNIT_ASSERT(name1 == idName1);

  //Create a new event and check if it is the next one
  id = m->GetNextId(idName2);

  //Check if the new event has the right name
  albaString name2 = m->GetIdName(id);
  CPPUNIT_ASSERT(name2 == idName2);

  cppDEL(m);
}
