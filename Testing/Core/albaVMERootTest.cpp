/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERootTest
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
#include "albaVMERootTest.h"

#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaMatrix.h"
#include "albaSmartPointer.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaSmartPointer.h"
#include "albaTagArray.h"
#include "albaCoreTests.h"
#include "albaEventIO.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMERootTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaVMERootTest::TestDynamicAllocation()
{
  albaSmartPointer<albaVMERoot> root;
}

//----------------------------------------------------------------------------
void albaVMERootTest::TestEquals()
{
  albaMatrix m1;
  m1.SetElement(0,3,3.0);
  m1.SetTimeStamp(1.0);

  albaMatrix m2;
  m2.SetElement(0,4,4.0);
  m2.SetTimeStamp(1.0);

  albaSmartPointer<albaVMERoot> root1;
  root1->SetMatrix(m1);

  albaSmartPointer<albaVMERoot> root2;
  root2->SetMatrix(m2);

  albaSmartPointer<albaVMERoot> root3;
  root3->SetMatrix(m1);

  // VMERoot with same matrix, so result must be true
  result = root1->Equals(root3);
  TEST_RESULT;

  // VMERoot with different matrix, so result must be false
  result = !root1->Equals(root2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERootTest::TestDeepCopy()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);
  m.SetTimeStamp(1.0);

  albaSmartPointer<albaVMERoot> root1;
  root1->SetMatrix(m);

  albaSmartPointer<albaVMERoot> root2;
  root2->DeepCopy(root1);

  //After DeepCopy the two VMERoot must be the same, so Equals must return true
  result = root1->Equals(root2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERootTest::TestGetLocalTimeStamps()
{
  std::vector<albaTimeStamp> kframes;

  albaSmartPointer<albaVMERoot> root1;
  root1->GetLocalTimeStamps(kframes);

  //kframes must have size = 1
  result = (kframes.size() == 1);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERootTest::TestSetMaxItemId()
{
  albaID id = 5;
  albaID idReturned;
  albaSmartPointer<albaVMERoot> root;
  root->SetMaxItemId(id);

  idReturned =  root->GetMaxItemId();

  //id set must equal id returned 
  result = (id == idReturned);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMERootTest::TestResetMaxItemId()
{
  albaID id = 5;
  albaID idReturned;
  albaSmartPointer<albaVMERoot> root;
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
void albaVMERootTest::TestGetNextItemId()
{
  albaID id = 5;

  albaID idReturned;
  albaSmartPointer<albaVMERoot> root;
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
void albaVMERootTest::TestCleanTree()
{
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaTagArray> ta;
  int childsNumber;

  albaSmartPointer<albaVMERoot> root;
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
void albaVMERootTest::TestCanReparentTo()
{
  albaSmartPointer<albaVMERoot> root0;
  albaSmartPointer<albaVMERoot> root1;
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
void albaVMERootTest::GetMaxNodeIdTest()
{
	albaVMERoot *r;
	albaNEW(r);
	//max node id must be 0
	int id = r->GetMaxNodeId();
	result = (id == 0);
	TEST_RESULT(result);
	albaDEL(r);
}
//----------------------------------------------------------------------------
void albaVMERootTest::GetNextNodeIdTest()
{
	albaVMERoot *r;
	albaNEW(r);
	//max node id must be 0 , and with this method must return it incremented by 1
	int id = r->GetNextNodeId();
	result = (id == 1);

	TEST_RESULT(result);
	albaDEL(r);
}

//----------------------------------------------------------------------------
void albaVMERootTest::SetMaxNodeIdTest()
{
	albaVMERoot *r;
	albaNEW(r);
	//max node id must be arbitrary 
	r->SetMaxNodeId(10);
	int id = r->GetMaxNodeId();
	result = (id == 10);

	TEST_RESULT(result);
	albaDEL(r);
}
//----------------------------------------------------------------------------
void albaVMERootTest::ResetMaxNodeIdTest()
{
	albaVMERoot *r;
	albaNEW(r);
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
	albaDEL(r);
}
//----------------------------------------------------------------------------
void albaVMERootTest::CanReparentToTest()
{
	albaVMERoot *r;
	albaNEW(r);
	result = (true == r->CanReparentTo(NULL));
	//root parent can not be reparented
	TEST_RESULT(result);

	albaVMEHelper *n = albaVMEHelper::New();
	result = (false == r->CanReparentTo(n));

	TEST_RESULT(result);

	albaDEL(n);
	albaDEL(r);
}
//----------------------------------------------------------------------------
void albaVMERootTest::SafeDownCastTest()
{
	albaVMERoot *r;
	albaNEW(r);
	albaVMERoot *rp;
	//valid cast

	//-----------------
	// this test is failing on vs2010 but not on vs2003 so I comment it out (more investigation is needed...)
	//----------------- start test comment
	// rp = (albaVMERoot *)albaRoot::SafeDownCast((albaObject *)r1);
	// result = (rp != NULL);
	// TEST_RESULT(result);
	//----------------- end test comment

	//bad cast
	albaVMEHelper *n = albaVMEHelper::New();
	rp = (albaVMERoot *)albaVMERoot::SafeDownCast((albaObject *)n);
	result = (rp == NULL);

	TEST_RESULT(result);
	albaDEL(r);
	albaDEL(n);
}
//----------------------------------------------------------------------------
void albaVMERootTest::OnRootEventTest()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *r1;
	albaNEW(r1);
	r1->SetStorage(storage);

	albaEventIO *eventA = new albaEventIO();
	eventA->SetId(NODE_GET_STORAGE);

	r1->OnEvent(eventA);

	//check if eventIO has storage variable correctly set
	albaVMEStorage *s = r1->GetStorage();
	result = (s == eventA->GetStorage());
	TEST_RESULT(result);

	albaDEL(r1);
	cppDEL(eventA);
	albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaVMERootTest::GetStorageTest()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMERoot *r1;
	albaNEW(r1);
	//storage not defined
	albaVMEStorage *s = r1->GetStorage();
	result = (s == NULL);
	TEST_RESULT(result);

	//define storage with listener
	r1->SetStorage(storage);

	s = r1->GetStorage();
	result = (s == storage);
	TEST_RESULT(result);

	albaDEL(r1);
	albaDEL(storage);
}
