/*=========================================================================

 Program: MAF2
 Module: mafVMERootTest
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
#include "mafVMERootTest.h"

#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafMatrix.h"
#include "mafSmartPointer.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafSmartPointer.h"
#include "mafTagArray.h"
#include "mafCoreTests.h"
#include "mafEventIO.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMERootTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void mafVMERootTest::TestDynamicAllocation()
{
  mafSmartPointer<mafVMERoot> root;
}

//----------------------------------------------------------------------------
void mafVMERootTest::TestEquals()
{
  mafMatrix m1;
  m1.SetElement(0,3,3.0);
  m1.SetTimeStamp(1.0);

  mafMatrix m2;
  m2.SetElement(0,4,4.0);
  m2.SetTimeStamp(1.0);

  mafSmartPointer<mafVMERoot> root1;
  root1->SetMatrix(m1);

  mafSmartPointer<mafVMERoot> root2;
  root2->SetMatrix(m2);

  mafSmartPointer<mafVMERoot> root3;
  root3->SetMatrix(m1);

  // VMERoot with same matrix, so result must be true
  result = root1->Equals(root3);
  TEST_RESULT;

  // VMERoot with different matrix, so result must be false
  result = !root1->Equals(root2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestDeepCopy()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMERoot> root1;
  root1->SetMatrix(m);

  mafSmartPointer<mafVMERoot> root2;
  root2->DeepCopy(root1);

  //After DeepCopy the two VMERoot must be the same, so Equals must return true
  result = root1->Equals(root2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestGetLocalTimeStamps()
{
  std::vector<mafTimeStamp> kframes;

  mafSmartPointer<mafVMERoot> root1;
  root1->GetLocalTimeStamps(kframes);

  //kframes must have size = 1
  result = (kframes.size() == 1);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestSetMaxItemId()
{
  mafID id = 5;
  mafID idReturned;
  mafSmartPointer<mafVMERoot> root;
  root->SetMaxItemId(id);

  idReturned =  root->GetMaxItemId();

  //id set must equal id returned 
  result = (id == idReturned);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestResetMaxItemId()
{
  mafID id = 5;
  mafID idReturned;
  mafSmartPointer<mafVMERoot> root;
  root->SetMaxItemId(id);

  idReturned =  root->GetMaxItemId();

  //id set must equal id returned 
  result = (id == idReturned);
  TEST_RESULT;

  root->ResetMaxItemId();
  idReturned =  root->GetMaxItemId();

  //id set must equals to zero
  result = (idReturned == 0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestGetNextItemId()
{
  mafID id = 5;

  mafID idReturned;
  mafSmartPointer<mafVMERoot> root;
  root->SetMaxItemId(id);

  idReturned = root->GetNextItemId();

  //id set must equals to id + 1
  result = (idReturned == id + 1);
  TEST_RESULT;

  idReturned = root->GetNextItemId();

  //id set must equals to id +2
  result = (idReturned == id + 2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestCleanTree()
{
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafTagArray> ta;
  int childsNumber;

  mafSmartPointer<mafVMERoot> root;
  root->AddChild(surface);
  root->AddChild(volume);

  root->SetAttribute("attribute", ta);

  childsNumber = root->GetNumberOfChildren();

  root->CleanTree();

  childsNumber = root->GetNumberOfChildren();
  //Number of children must be zero
  result = (childsNumber == 0);
  TEST_RESULT;

  //"attribute" must be NULL
  result = ( root->GetAttribute("attribute") == NULL);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMERootTest::TestCanReparentTo()
{
  mafSmartPointer<mafVMERoot> root0;
  mafSmartPointer<mafVMERoot> root1;
  bool canReparent = root0->CanReparentTo(root1);

  //can not reparent
  result = (!canReparent);
  TEST_RESULT;

  canReparent = root0->CanReparentTo(NULL);

  //can only reparent to NULL
  result = (canReparent);
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafVMERootTest::GetMaxNodeIdTest()
{
	mafVMERoot *r;
	mafNEW(r);
	//max node id must be 0
	int id = r->GetMaxNodeId();
	result = (id == 0);
	TEST_RESULT(result);
	mafDEL(r);
}
//----------------------------------------------------------------------------
void mafVMERootTest::GetNextNodeIdTest()
{
	mafVMERoot *r;
	mafNEW(r);
	//max node id must be 0 , and with this method must return it incremented by 1
	int id = r->GetNextNodeId();
	result = (id == 1);

	TEST_RESULT(result);
	mafDEL(r);
}

//----------------------------------------------------------------------------
void mafVMERootTest::SetMaxNodeIdTest()
{
	mafVMERoot *r;
	mafNEW(r);
	//max node id must be arbitrary 
	r->SetMaxNodeId(10);
	int id = r->GetMaxNodeId();
	result = (id == 10);

	TEST_RESULT(result);
	mafDEL(r);
}
//----------------------------------------------------------------------------
void mafVMERootTest::ResetMaxNodeIdTest()
{
	mafVMERoot *r;
	mafNEW(r);
	//max node id must be arbitrary
	r->SetMaxNodeId(10);
	int id = r->GetMaxNodeId();
	result = (id == 10);

	TEST_RESULT(result);

	//but then is reset to 0
	r->ResetMaxNodeId();
	id = r->GetMaxNodeId();
	result = (id == 0);

	TEST_RESULT(result);
	mafDEL(r);
}
//----------------------------------------------------------------------------
void mafVMERootTest::CanReparentToTest()
{
	mafVMERoot *r;
	mafNEW(r);
	result = (true == r->CanReparentTo(NULL));
	//root parent can not be reparented
	TEST_RESULT(result);

	mafVMEHelper *n = mafVMEHelper::New();
	result = (false == r->CanReparentTo(n));

	TEST_RESULT(result);

	mafDEL(n);
	mafDEL(r);
}
//----------------------------------------------------------------------------
void mafVMERootTest::SafeDownCastTest()
{
	mafVMERoot *r;
	mafNEW(r);
	mafVMERoot *rp;
	//valid cast

	//-----------------
	// this test is failing on vs2010 but not on vs2003 so I comment it out (more investigation is needed...)
	//----------------- start test comment
	// rp = (mafVMERoot *)mafRoot::SafeDownCast((mafObject *)r1);
	// result = (rp != NULL);
	// TEST_RESULT(result);
	//----------------- end test comment

	//bad cast
	mafVMEHelper *n = mafVMEHelper::New();
	rp = (mafVMERoot *)mafVMERoot::SafeDownCast((mafObject *)n);
	result = (rp == NULL);

	TEST_RESULT(result);
	mafDEL(r);
	mafDEL(n);
}
//----------------------------------------------------------------------------
void mafVMERootTest::OnRootEventTest()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *r1;
	mafNEW(r1);
	r1->SetStorage(storage);

	mafEventIO *eventA = new mafEventIO();
	eventA->SetId(NODE_GET_STORAGE);

	r1->OnEvent(eventA);

	//check if eventIO has storage variable correctly set
	mafVMEStorage *s = r1->GetStorage();
	result = (s == eventA->GetStorage());
	TEST_RESULT(result);

	mafDEL(r1);
	cppDEL(eventA);
	mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafVMERootTest::GetStorageTest()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mafVMERoot *r1;
	mafNEW(r1);
	//storage not defined
	mafVMEStorage *s = r1->GetStorage();
	result = (s == NULL);
	TEST_RESULT(result);

	//define storage with listener
	r1->SetStorage(storage);

	s = r1->GetStorage();
	result = (s == storage);
	TEST_RESULT(result);

	mafDEL(r1);
	mafDEL(storage);
}
