/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaReferenceCountedTest
 Authors: Matteo Giacomoni
 
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
#include "albaBaseTests.h"
#include "albaReferenceCountedTest.h"

#include "albaSmartPointer.h"
#include "albaReferenceCounted.h"

#include <iostream>
#define TEST_RESULT CPPUNIT_ASSERT(result);

/**
This object set an external flag when allocated and reset it when deallocated,
this way I can test when it's really deallocated.
*/


/** 
this creates an object, set its internal flag, increase the reference
counter by 1 and returns the object pointer */
albaReferenceCounted *CreateSmartObject(int &flag)
{
  albaFooObject *foo= albaFooObject::New();
  foo->Flag=&flag;
  flag=true;
  foo->Register(0); // keep it alive
  return foo;
}
//----------------------------------------------------------------------------
void albaReferenceCountedTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaReferenceCountedTest::TestRegister()
//----------------------------------------------------------------------------
{
  int flag; // the flag use to know when the object is deallocated

  // create an object on the Heap with reference count set to 1 and set
  // its flag reference to the "flag" variable.
  albaReferenceCounted *obj=CreateSmartObject(flag);
  CPPUNIT_ASSERT(flag==true);
  obj->UnRegister(0);
  CPPUNIT_ASSERT(flag==false);
}
//----------------------------------------------------------------------------
void albaReferenceCountedTest::TestGetReferenceCount()
//----------------------------------------------------------------------------
{
  int flag; // the flag use to know when the object is deallocated
  albaReferenceCounted *obj=CreateSmartObject(flag);
  CPPUNIT_ASSERT(obj->GetReferenceCount()==1);
  obj->SetReferenceCount(10);
  CPPUNIT_ASSERT(obj->GetReferenceCount()==10);
  obj->SetReferenceCount(1);
  obj->Delete();
  CPPUNIT_ASSERT(flag==false);
}
//----------------------------------------------------------------------------
void albaReferenceCountedTest::TestDelete()
//----------------------------------------------------------------------------
{
  int flag; // the flag use to know when the object is deallocated
  albaReferenceCounted *obj=CreateSmartObject(flag);
  obj->Register(0);

  obj->Delete();
  CPPUNIT_ASSERT(obj->GetReferenceCount()==1);
  obj->Delete();
  CPPUNIT_ASSERT(flag==false);

}
