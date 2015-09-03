/*=========================================================================

 Program: MAF2
 Module: mafEventIOTest
 Authors: Roberto Mucci
 
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
#include "mafEventIOTest.h"

#include "mafReferenceCounted.h"
#include "mafEventIO.h"
#include "vtkMAFSmartPointer.h"
#include "mafEvent.h"
#include "mafVMEFactory.h"
#include "mafVMEStorage.h"
#include  "mafVMERoot.h"

#include <iostream>
#include <utility>

//creation of test object
class mafTestObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafTestObject,mafReferenceCounted);
};

mafCxxTypeMacro(mafTestObject);

//----------------------------------------------------------------------------
void mafEventIOTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventIOTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafEventIO eventA;
}
//----------------------------------------------------------------------------
void mafEventIOTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafEventIO *eventA = new mafEventIO();
  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventIOTest::TestSetItemId()
//----------------------------------------------------------------------------
{
  mafEventIO *eventA = new mafEventIO;

  mafID id = 5;

  eventA->SetItemId(id);

  mafID idReturned = eventA->GetItemId();
  
  //Check if mafId set and get are equals;
  CPPUNIT_ASSERT(idReturned = 5);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventIOTest::TestSetStorage()
//----------------------------------------------------------------------------
{
  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  mafStorage *storageSet = storage->GetRoot()->GetStorage();

  mafEventIO *eventA = new mafEventIO();
  eventA->SetStorage(storageSet);

  mafStorage *storageReturned = eventA->GetStorage();

  //Test if mafStorage set equal mafStorage get
  CPPUNIT_ASSERT(storageReturned == storageSet);

  cppDEL(eventA);
  cppDEL(storage);
}
//----------------------------------------------------------------------------
void mafEventIOTest::TestSetRoot()
//----------------------------------------------------------------------------
{
  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);
  mafVMERoot *rootSet = storage->GetRoot();

  mafEventIO *eventA = new mafEventIO();
  eventA->SetRoot(rootSet);

  mafNode *rootReturned = eventA->GetRoot();
 
  //Check if node returned is a mafVEMRoot
  CPPUNIT_ASSERT(rootReturned->IsA("mafVMERoot"));

  //Test if mafVMERoot set equal mafVMERoot get
  CPPUNIT_ASSERT(rootReturned == (mafVMERoot *)rootSet);

  cppDEL(eventA);
  cppDEL(storage);
}
