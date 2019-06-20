/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventIOTest
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
#include "albaEventIOTest.h"

#include "albaReferenceCounted.h"
#include "albaEventIO.h"
#include "vtkALBASmartPointer.h"
#include "albaEvent.h"
#include "albaVMEFactory.h"
#include "albaVMEStorage.h"
#include  "albaVMERoot.h"

#include <iostream>
#include <utility>

//creation of test object
class albaTestObject: public albaReferenceCounted
{
public:
  albaTypeMacro(albaTestObject,albaReferenceCounted);
};

albaCxxTypeMacro(albaTestObject);

//----------------------------------------------------------------------------
void albaEventIOTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaEventIOTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaEventIO eventA;
}
//----------------------------------------------------------------------------
void albaEventIOTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaEventIO *eventA = new albaEventIO();
  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventIOTest::TestSetItemId()
//----------------------------------------------------------------------------
{
  albaEventIO *eventA = new albaEventIO;

  albaID id = 5;

  eventA->SetItemId(id);

  albaID idReturned = eventA->GetItemId();
  
  //Check if albaId set and get are equals;
  CPPUNIT_ASSERT(idReturned = 5);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void albaEventIOTest::TestSetStorage()
//----------------------------------------------------------------------------
{
  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  albaStorage *storageSet = storage->GetRoot()->GetStorage();

  albaEventIO *eventA = new albaEventIO();
  eventA->SetStorage(storageSet);

  albaStorage *storageReturned = eventA->GetStorage();

  //Test if albaStorage set equal albaStorage get
  CPPUNIT_ASSERT(storageReturned == storageSet);

  cppDEL(eventA);
  cppDEL(storage);
}
//----------------------------------------------------------------------------
void albaEventIOTest::TestSetRoot()
//----------------------------------------------------------------------------
{
  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);
  albaVMERoot *rootSet = storage->GetRoot();

  albaEventIO *eventA = new albaEventIO();
  eventA->SetRoot(rootSet);

  albaVME *rootReturned = eventA->GetRoot();
 
  //Check if node returned is a albaVEMRoot
  CPPUNIT_ASSERT(rootReturned->IsA("albaVMERoot"));

  //Test if albaVMERoot set equal albaVMERoot get
  CPPUNIT_ASSERT(rootReturned == (albaVMERoot *)rootSet);

  cppDEL(eventA);
  cppDEL(storage);
}
