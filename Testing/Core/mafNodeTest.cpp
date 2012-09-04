/*=========================================================================

 Program: MAF2
 Module: mafNodeTest
 Authors: Paolo Quadrani
 
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
#include "mafNodeTest.h"

#include "mafSmartPointer.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafNodeRoot.h"
#include "mafNode.h"
#include "mafNodeIterator.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafNodeB: public mafNode
  //-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafNodeB,mafNode);
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeB)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafNodeA: public mafNode
  //-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafNodeA,mafNode);
  void SetId(mafID id) {m_Id = id;};
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeA);
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafNodeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafNodeTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafNodeTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafNodeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;

  mafNodeA *na2;
  mafNEW(na2);
  na2->Delete();
}
//----------------------------------------------------------------------------
void mafNodeTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  result = na->GetReferenceCount() == 1; // mafSmartPointer increase the reference count
  TEST_RESULT;

  mafNodeA *na2 = NULL;
  na2 = mafNodeA::New();
  result = na2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  mafDEL(na2);

  mafNodeA *na3 = NULL;
  mafNEW(na3);
  result = na3->GetReferenceCount() == 1; // mafNEW macro increase the reference count
  TEST_RESULT;
  mafDEL(na3);
}
//----------------------------------------------------------------------------
void mafNodeTest::TestSetName()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node a");
  result = mafString::Equals(na->GetName(),"node a");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node");
  result = na->Equals(nb);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestCanCopy()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node a");

  mafSmartPointer<mafNodeA> na2;
  result = na2->CanCopy(na);
  TEST_RESULT;

  mafSmartPointer<mafNodeB> nb; // True because also mafNodeB is a mafNode!!
  result = nb->CanCopy(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node a");

  mafSmartPointer<mafNodeA> na2;
  na2->DeepCopy(na);

  result = na2->Equals(na);
  TEST_RESULT;

  mafSmartPointer<mafNodeA> nl;
  nl->SetName("node linked");
  nl->SetId(1); // Needed to be linked

  na2->SetLink("LinkName", nl);

  mafSmartPointer<mafNodeA> na3;
  na3->DeepCopy(na2);
  result = na3->GetNumberOfLinks() == 1;
  TEST_RESULT;

  result = na3->GetLink("LinkName")->Equals(nl);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestFindInTreeById()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> na;
  mafSmartPointer<mafNodeA> na2;
  mafSmartPointer<mafNodeA> na3;

  int i = na->GetMaxNodeId();

  na2->ReparentTo(na);
  na3->ReparentTo(na2);

  /*na2->UpdateId();
  na3->UpdateId();*/

  mafNode *n = na->FindInTreeById(2);
  result = n != NULL;
  TEST_RESULT;
  result = n->Equals(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestFindInTreeByName()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root node");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("child 1");
  mafSmartPointer<mafNodeA> na3;
  na3->SetName("child 2");
  mafSmartPointer<mafNodeA> na4;
  na4->SetName("child 3");

  na->AddChild(na2);
  na2->AddChild(na3);
  na3->AddChild(na4);

  // by default 'match case' and 'whole word' are active
  mafNode *n = na->FindInTreeByName("child 2");
  result = n != NULL;
  TEST_RESULT;
  result = n->Equals(na3);
  TEST_RESULT;

  n = na->FindInTreeByName("CHILD 2");
  result = n == NULL;
  TEST_RESULT;

  // deactivate the 'match case' option
  n = na->FindInTreeByName("CHILD 2", 0);
  result = n != NULL;
  TEST_RESULT;
  result = n->Equals(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestFindInTreeByTag()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;
  mafSmartPointer<mafNodeA> na3;
  
  mafTagItem ti;
  ti.SetName("test");
  mafTagArray *ta = na3->GetTagArray();
  ta->SetTag(ti);

  na->AddChild(na2);
  na->AddChild(na3);

  mafNode *n = na->FindInTreeByTag("test");
  result = n != NULL;
  TEST_RESULT;
  result = n->Equals(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestFindNodeIdx()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node nb");
  mafSmartPointer<mafNodeA> nc;
  nc->SetName("node nc");
  na->AddChild(nb);
  na->AddChild(nc);

  result = na->FindNodeIdx("node nb") == 0;
  TEST_RESULT;
  result = na->FindNodeIdx("node nc") == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetAttribute()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  result = na->GetAttribute("Not Exists") == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetChild()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node na");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node nb");
  mafSmartPointer<mafNodeA> nc;
  nc->SetName("node nc");

  na->AddChild(nb);
  na->AddChild(nc);
  result = na->GetChild(0)->Equals(nb);
  TEST_RESULT;

  result = na->GetChild(1)->Equals(nc);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetFirstChild()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node na");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node nb");
  mafSmartPointer<mafNodeA> nc;
  nc->SetName("node nc");

  na->AddChild(nb);
  na->AddChild(nc);
  mafNode *n = na->GetFirstChild();
  result = n->Equals(nb);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetLastChild()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node na");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node nb");
  mafSmartPointer<mafNodeA> nc;
  nc->SetName("node nc");

  na->AddChild(nb);
  na->AddChild(nc);
  mafNode *n = na->GetLastChild();
  result = n->Equals(nc);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetLink()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafNode *nl = na->GetLink("Not Existing Link");
  result = nl == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestSetLink()
//----------------------------------------------------------------------------
{
  // To link a node it have to be into a tree with Id != -1 (invalid Id)
  mafSmartPointer<mafNodeRoot> root;
  mafSmartPointer<mafNodeA> na;
  na->SetName("node na");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node nb");
  root->AddChild(na);
  root->AddChild(nb);
  na->UpdateId();
  nb->UpdateId();

  na->SetLink("test", nb);
  result = na->GetLink("test")->Equals(nb);
  TEST_RESULT;

  // Overwrite old link
  mafSmartPointer<mafNodeA> nc;
  nc->SetName("node nc");
  root->AddChild(nc);
  nc->UpdateId();

  na->SetLink("test", nc);
  result = na->GetLink("test")->Equals(nc);
  TEST_RESULT;

  result = !na->GetLink("test")->Equals(nb);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetLinkSubId()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> root;
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> nb;
  mafSmartPointer<mafNodeA> nc;
  root->AddChild(na);
  root->AddChild(nb);
  root->AddChild(nc);
  na->UpdateId();
  nb->UpdateId();
  nc->UpdateId();

  na->SetLink("test", nb);
  result = na->GetLinkSubId("test") == -1;
  TEST_RESULT;

  na->SetLink("test2", nc, 2);
  result = na->GetLinkSubId("test2") == 2;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetNumberOfLinks()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> root;
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> nb;
  mafSmartPointer<mafNodeA> nc;
  root->AddChild(na);
  root->AddChild(nb);
  root->AddChild(nc);
  na->UpdateId();
  nb->UpdateId();
  nc->UpdateId();

  na->SetLink("test", nb);
  na->SetLink("test2", nc, 2);
  result = na->GetNumberOfLinks() == 2;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestRemoveLink()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> root;
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> nb;
  mafSmartPointer<mafNodeA> nc;
  root->AddChild(na);
  root->AddChild(nb);
  root->AddChild(nc);
  na->UpdateId();
  nb->UpdateId();
  nc->UpdateId();

  na->SetLink("test", nb);
  na->SetLink("test2", nc, 2);
  na->RemoveLink("test2");
  
  result = na->GetNumberOfLinks() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestRemoveAllLinks()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> root;
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> nb;
  mafSmartPointer<mafNodeA> nc;
  root->AddChild(na);
  root->AddChild(nb);
  root->AddChild(nc);
  na->UpdateId();
  nb->UpdateId();
  nc->UpdateId();

  na->SetLink("test", nb);
  na->SetLink("test2", nc, 2);
  na->RemoveAllLinks();

  result = na->GetNumberOfLinks() == 0;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestDependsOnLinkedNode()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> root;
  mafSmartPointer<mafNodeA> na;
  na->SetName("node na");
  mafSmartPointer<mafNodeA> nb;
  nb->SetName("node nb");
  mafSmartPointer<mafNodeA> nc;
  nc->SetName("node nc");
  root->AddChild(na);
  root->AddChild(nb);
  root->AddChild(nc);
  na->UpdateId();
  nb->UpdateId();
  nc->UpdateId();

  na->SetLink("test", nb);
  na->SetLink("test2", nc, 2);
  
  // Node 'ns' depends on linked node modified time.
  na->DependsOnLinkedNodeOn();
  unsigned long mt0 = na->GetMTime();

  // modify MTime for linked node => 'na' MTime will be modified
  nb->SetName("Node nb");
  unsigned long mt1 = na->GetMTime();

  result = mt1 > mt0;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestSetAttribute()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;

  mafSmartPointer<mafTagArray> ta;
  ta->SetName("My Attribute");
  na->SetAttribute("My Attribute", ta);

  mafAttribute *a = na->GetAttribute("My Attribute");
  result = a != NULL;
  TEST_RESULT;
  result = a->Equals(ta);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestRemoveAttribute()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;

  mafSmartPointer<mafTagArray> ta;
  ta->SetName("My Attribute");
  na->SetAttribute("My Attribute", ta);

  na->RemoveAttribute("My Attribute");

  mafAttribute *a = na->GetAttribute("My Attribute");
  result = a == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestRemoveAllAttributes()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;

  mafSmartPointer<mafTagArray> ta;
  ta->SetName("My Attribute");
  na->SetAttribute("My Attribute", ta);

  na->RemoveAllAttributes();

  mafAttribute *a = na->GetAttribute("My Attribute");
  result = a == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestMakeCopy()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("node a");

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;

  mafNodeA *na2 = (mafNodeA *)na->MakeCopy();
  result = na2->Equals(na);
  TEST_RESULT;

  result = na2->GetReferenceCount() == 0;
  TEST_RESULT;
  mafDEL(na2);
}
//----------------------------------------------------------------------------
void mafNodeTest::TestReparentTo()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");

  result = na2->ReparentTo(na) == MAF_OK;
  TEST_RESULT;

  result = na2->ReparentTo(na) == MAF_OK;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetParent()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");
  na->AddChild(na2);

  result = na2->GetParent()->Equals(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetRoot()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");
  mafSmartPointer<mafNodeA> na3;
  na3->SetName("node na3");
  mafSmartPointer<mafNodeA> na4;
  na4->SetName("node na4");

  na->AddChild(na2);
  na2->AddChild(na3);
  na3->AddChild(na4);
  result = na4->GetRoot()->Equals(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetTagArray()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafTagArray *ta = na->GetTagArray();
  result = ta != NULL;
  TEST_RESULT;

  result = ta->IsA("mafTagArray");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestIsValid()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeRoot> root;
  mafNodeA *na = NULL;
  mafNEW(na);

  root->AddChild(na);
  na->UpdateId();
  
  result = na->IsValid();
  TEST_RESULT;

  // Destroy the node and so it will become invalid.
  root->RemoveChild(na);
  na->Delete();
  result = !na->IsValid();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestIsVisible()
//----------------------------------------------------------------------------
{
  // By default a node can be traversed by iterator, so it is called 'visible'
  mafSmartPointer<mafNodeA> na;
  result = na->IsVisible();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestIsInTree()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  result = !na->IsInTree(NULL);
  TEST_RESULT;

  mafSmartPointer<mafNodeA> na1;
  na1->SetName("node na1");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");

  na2->ReparentTo(na1);

  result = !na->IsInTree(na2);
  TEST_RESULT;

  result = na1->IsInTree(na2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestIsAChild()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");

  result = !na->IsAChild(na2);
  TEST_RESULT;

  na2->ReparentTo(na);
  result = na->IsAChild(na2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetNumberOfChildren()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");

  result = na->GetNumberOfChildren() == 0;
  TEST_RESULT;

  na2->ReparentTo(na);
  result = na->GetNumberOfChildren() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestIsEmpty()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");

  result = na->IsEmpty();
  TEST_RESULT;

  na2->ReparentTo(na);
  result = !na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestRemoveChild()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  na->SetName("root");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("node na2");
  mafSmartPointer<mafNodeA> na3;
  na3->SetName("node na3");

  na->AddChild(na2);
  na->AddChild(na3);

  na->RemoveChild(na2);
  result = na->GetNumberOfChildren() == 1;
  TEST_RESULT;

  mafID node_id = 0;
  na->RemoveChild(node_id);
  result = na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestRemoveAllChildren()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;

  na2->ReparentTo(na);
  na->RemoveAllChildren();
  result = na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestCleanTree()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;

  na2->ReparentTo(na);
  na->CleanTree();
  result = na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestNewIterator()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafNodeIterator *iter = NULL;
  iter = na->NewIterator();
  result = na->GetReferenceCount() == 2; // Iterator increase the reference count of the node!!
  TEST_RESULT;

  result = iter != NULL;
  TEST_RESULT;
  iter->Delete();

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestCanReparentTo()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  result = na->CanReparentTo(NULL);
  TEST_RESULT;

  mafSmartPointer<mafNodeA> na2;
  result = na2->CanReparentTo(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestGetChildren()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;

  result = na->GetChildren()->size() == 0;
  TEST_RESULT;

  na2->ReparentTo(na);
  result = na->GetChildren()->size() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestAddChild()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;

  na->AddChild(na2);
  result = na->GetChildren()->size() == 1;
  TEST_RESULT;

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;

  result = na2->GetReferenceCount() == 2; // Is referenced by 'na'
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestCompareTree()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeB> na;
  na->SetName("NodeB");
  mafSmartPointer<mafNodeA> na2;
  na2->SetName("NodeB_Child");

  mafSmartPointer<mafNodeB> na3;
  na3->SetName("NodeB");
  mafSmartPointer<mafNodeA> na4;
  na4->SetName("NodeB_Child");

  na->AddChild(na2);
  na3->AddChild(na4);

  result = na->CompareTree(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestCopyTree()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;
  na->AddChild(na2);

  mafNodeA *na3 = (mafNodeA *)na->CopyTree();
  result = na->CompareTree(na3);
  TEST_RESULT;
  mafDEL(na3);
}
//----------------------------------------------------------------------------
void mafNodeTest::TestImport()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> na;
  mafSmartPointer<mafNodeA> na2;
  na->AddChild(na2);

  mafSmartPointer<mafNodeA> na3;
  na3->Import(na);

  result = na3->GetChild(0)->Equals(na2);
  TEST_RESULT;

  result = na3->GetChildren()->size() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafNodeTest::TestBuildAndDestroyATree()
//----------------------------------------------------------------------------
{
  mafNodeA *na = NULL;
  mafNEW(na);

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;

  mafNodeA *na2 = NULL;
  mafNEW(na2);

  result = na2->GetReferenceCount() == 1;
  TEST_RESULT;

  na->AddChild(na2);
  result = na2->GetReferenceCount() == 2;
  TEST_RESULT;

  na->RemoveChild(na2);

  result = na2->GetReferenceCount() == 1;
  TEST_RESULT;

  mafDEL(na);
  mafDEL(na2);
}

//----------------------------------------------------------------------------
void mafNodeTest::TestGetByPath()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafNodeA> root ;
  mafSmartPointer<mafNodeA> sideA ;
  mafSmartPointer<mafNodeA> sideB ;
  mafSmartPointer<mafNodeA> a1 ;
  mafSmartPointer<mafNodeA> a2 ;
  mafSmartPointer<mafNodeA> a3 ;
  mafSmartPointer<mafNodeA> b1 ;
  mafSmartPointer<mafNodeA> b2 ;
  mafSmartPointer<mafNodeA> b3 ;


  //setting names
  root->SetName("root");
  sideA->SetName("sideA");
  sideB->SetName("sideB");
  a1->SetName("a1");
  a2->SetName("a2");
  a3->SetName("a3");
  b1->SetName("b1");
  b2->SetName("b2");
  b3->SetName("b3");


  //generating structure
  //             Root
  //            /    \
  //       sideA      sideB
  //      /  |  \    /  |  \
  //     a1  a2 a3  b1  b2  b3 

  root->AddChild(sideA);
  root->AddChild(sideB);
  sideA->AddChild(a1);
  sideA->AddChild(a2);
  sideA->AddChild(a3);
  sideB->AddChild(b1);
  sideB->AddChild(b2);
  sideB->AddChild(b3);

  //Tresting wrong keyword
  result= (root->GetByPath("wrong") == NULL);
  TEST_RESULT;


  //Testing Next
  result= (root->GetByPath("next") == NULL);
  TEST_RESULT;
  result= (sideB->GetByPath("next") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("next") == (mafNodeA *)(sideB));
  TEST_RESULT;

  //Testing Prev
  result= (root->GetByPath("prev") == NULL);
  TEST_RESULT;
  result= (sideA->GetByPath("prev") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideB->GetByPath("prev") == (mafNodeA *)(sideA));
  TEST_RESULT;

  //Testing FirstPair
  result= (root->GetByPath("firstPair") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a2->GetByPath("firstPair") == (mafNodeA *)(a1));
  TEST_RESULT;

  //Testing LastChild
  result= (root->GetByPath("lastPair") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a2->GetByPath("lastPair") == (mafNodeA *)(a3));
  TEST_RESULT;


  //Testing FirstChild
  result= (a1->GetByPath("firstChild") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("firstChild") == (mafNodeA *)(a1));
  TEST_RESULT;

  //Testing LastChild
  result= (a1->GetByPath("lastChild") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("lastChild") == (mafNodeA *)(a3));
  TEST_RESULT;

  //Testing Pair[]
  result= ((mafNodeA *)root->GetByPath("pair[2]") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair[2") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair[two]") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair[3]") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair[2]") == (mafNodeA *)(a3));
  TEST_RESULT;

  //Testing Pair{}
  result= ((mafNodeA *)root->GetByPath("pair{a2}") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair{a2") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair{a4}") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("pair{a2}") == (mafNodeA *)(a2));
  TEST_RESULT;


  //Testing Child[]
  result= ((mafNodeA *)sideA->GetByPath("child[2") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("child[two]") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("child[3]") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("child[2]") == (mafNodeA *)(a3));
  TEST_RESULT;

  //Testing Child{}
  result= ((mafNodeA *)sideA->GetByPath("child{a2") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("child{a4}") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)sideA->GetByPath("child{a2}") == (mafNodeA *)(a2));
  TEST_RESULT;


  //Testing ".."
  result= ((mafNodeA *)root->GetByPath("..") == NULL);
  TEST_RESULT;
  result= ((mafNodeA *)a1->GetByPath("..") == (mafNodeA *)(sideA));
  TEST_RESULT;

  //Testing "."
  result= ((mafNodeA *)a1->GetByPath(".") == (mafNodeA *)(a1));
  TEST_RESULT;

  //Testing complex paths

  //a1->b3
  result= ((mafNodeA *)a1->GetByPath("../../child{sideB}/child[2]") == (mafNodeA *)(b3));
  TEST_RESULT;

  //sideB->a2
  result= ((mafNodeA *)sideB->GetByPath("prev/child{a2}") == (mafNodeA *)(a2));
  TEST_RESULT;

  //sideA->b1
  result= ((mafNodeA *)sideA->GetByPath("lastPair/firstChild") == (mafNodeA *)(b1));
  TEST_RESULT;
  
  //wrong
  result= ((mafNodeA *)sideA->GetByPath("../../lastChild") == NULL);
  TEST_RESULT;
}
