/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorableTest
 Authors: Eleonora Mambrini
 
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
#include "albaStorableTest.h"
#include "albaStorable.h"
#include "albaString.h"

//BEGIN DUMMY TEST CLASS//
class albaDummyStorable : public albaStorable
{
public:

  albaDummyStorable(){m_Log = "";};

  albaString GetLog(){return m_Log;};

protected:
  int InternalStore(albaStorageElement *node);
  int InternalRestore(albaStorageElement *node);

  albaString m_Log;
};

int albaDummyStorable::InternalStore(albaStorageElement *node)
{
  m_Log = "Stored";
  return m_Log.Length();
}

int albaDummyStorable::InternalRestore(albaStorageElement *node)
{
  m_Log = "Restored";
  return m_Log.Length();
}

//----------------------------------------------------------------------------
void albaStorableTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaDummyStorable *storable = new albaDummyStorable();
  cppDEL(storable);
}

//----------------------------------------------------------------------------
void albaStorableTest::StoreTest()
//----------------------------------------------------------------------------
{

  albaDummyStorable *storable = new albaDummyStorable();

  CPPUNIT_ASSERT(storable->Store(NULL));
  CPPUNIT_ASSERT(storable->GetLog().Equals("Stored"));

  cppDEL(storable);

}

//----------------------------------------------------------------------------
void albaStorableTest::RestoreTest()
//----------------------------------------------------------------------------
{
  albaDummyStorable *storable = new albaDummyStorable();

  CPPUNIT_ASSERT(storable->Restore(NULL));
  CPPUNIT_ASSERT(storable->GetLog().Equals("Restored"));

  cppDEL(storable);

}