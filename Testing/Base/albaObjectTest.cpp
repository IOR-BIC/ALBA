/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObjectTest
 Authors: Roberto Mucci
 
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
#include "albaObject.h"
#include "albaObjectTest.h"
#include "vtkALBASmartPointer.h"
#include "albaString.h"


#include <iostream>
#include <utility>



//----------------------------------------------------------------------------
void albaObjectTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaObjectTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaTestObject obj;
}
//----------------------------------------------------------------------------
void albaObjectTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestGetStaticTypeName()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  //Return the name of this type (static function)
  albaString static_name = obj->GetStaticTypeName();
  bool ok = static_name.Equals("albaTestObject");
  CPPUNIT_ASSERT(ok);

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestGetTypeName()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  //Return the class name of this instance
  albaString static_name = obj->GetStaticTypeName();
  bool ok = static_name.Equals("albaTestObject");
  CPPUNIT_ASSERT(ok);

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestIsStaticType()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  const char *type = "albaObject";

  bool ok = obj->IsStaticType(type);
  CPPUNIT_ASSERT(ok);

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestIsStaticType2()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  //Return TypeId for this object instance (the real type)
  const albaTypeID &type = obj->GetTypeId();

  bool ok = obj->IsStaticType(type);
  CPPUNIT_ASSERT(ok);

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestIsA()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  const char *type = "albaObject";

  bool ok = obj->IsA(type);
  CPPUNIT_ASSERT(ok);

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestIsA2()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();

  //Return TypeId for this object instance (the real type)
  const albaTypeID &type = obj->GetTypeId();

  bool ok = obj->IsA(type);
  CPPUNIT_ASSERT(ok);

  albaDEL(obj);
}
//----------------------------------------------------------------------------
void albaObjectTest::TestSafeDownCast()
//----------------------------------------------------------------------------
{
  albaTestObject *obj = albaTestObject::New();
  
  //used for casting from (albaObject *)
  albaTestObject* tmp = albaTestObject::SafeDownCast(obj);

  CPPUNIT_ASSERT(tmp != NULL);

  albaDEL(obj);
}
