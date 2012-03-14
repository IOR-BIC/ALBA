/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafStorableTest.cpp,v $
Language:  C++
Date:      $Date: 2010-01-14 11:58:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafStorableTest.h"
#include "mafStorable.h"
#include "mafString.h"

//BEGIN DUMMY TEST CLASS//
class mafDummyStorable : public mafStorable
{
public:

  mafDummyStorable(){m_Log = "";};

  mafString GetLog(){return m_Log;};

protected:
  int InternalStore(mafStorageElement *node);
  int InternalRestore(mafStorageElement *node);

  mafString m_Log;
};

int mafDummyStorable::InternalStore(mafStorageElement *node)
{
  m_Log = "Stored";
  return m_Log.Length();
}

int mafDummyStorable::InternalRestore(mafStorageElement *node)
{
  m_Log = "Restored";
  return m_Log.Length();
}

//END DUMMY TEST CLASS//

//----------------------------------------------------------------------------
void mafStorableTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafStorableTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafStorableTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafDummyStorable *storable = new mafDummyStorable();
  cppDEL(storable);
}

//----------------------------------------------------------------------------
void mafStorableTest::StoreTest()
//----------------------------------------------------------------------------
{

  mafDummyStorable *storable = new mafDummyStorable();

  CPPUNIT_ASSERT(storable->Store(NULL));
  CPPUNIT_ASSERT(storable->GetLog().Equals("Stored"));

  cppDEL(storable);

}

//----------------------------------------------------------------------------
void mafStorableTest::RestoreTest()
//----------------------------------------------------------------------------
{
  mafDummyStorable *storable = new mafDummyStorable();

  CPPUNIT_ASSERT(storable->Restore(NULL));
  CPPUNIT_ASSERT(storable->GetLog().Equals("Restored"));

  cppDEL(storable);

}