/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMETest
 Authors: Paolo Quadrani, Gianluigi Crimi
 
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
#include "albaCoreTests.h"
#include "albaVMETest.h"

#include "albaSmartPointer.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEIterator.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaVMETest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMETest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;

  albaVMEHelper *na2;
  albaNEW(na2);
  na2->Delete();
}
//----------------------------------------------------------------------------
void albaVMETest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  result = na->GetReferenceCount() == 1; // albaSmartPointer increase the reference count
  TEST_RESULT;

  albaVMEHelper *na2 = NULL;
  na2 = albaVMEHelper::New();
  result = na2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  albaDEL(na2);

  albaVMEHelper *na3 = NULL;
  albaNEW(na3);
  result = na3->GetReferenceCount() == 1; // albaNEW macro increase the reference count
  TEST_RESULT;
  albaDEL(na3);
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetName()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node a");
  result = albaString::Equals(na->GetName(),"node a");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestEquals()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node");
  result = na->Equals(nb);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestCanCopy()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node a");

  albaSmartPointer<albaVMEHelper> na2;
  result = na2->CanCopy(na);
  TEST_RESULT;

  albaSmartPointer<albaVMEBHelper> nb; // True because also albaVMEB is a albaVME!!
  result = nb->CanCopy(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node a");

  albaSmartPointer<albaVMEHelper> na2;
  na2->DeepCopy(na);

  result = na2->Equals(na);
  TEST_RESULT;

  albaSmartPointer<albaVMEHelper> nl;
  nl->SetName("node linked");
  nl->SetId(1); // Needed to be linked

  na2->SetLink("LinkName", nl);

  albaSmartPointer<albaVMEHelper> na3;
  na3->DeepCopy(na2);
  result = na3->GetNumberOfLinks() == 1;
  TEST_RESULT;

  result = na3->GetLink("LinkName")->Equals(nl);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestFindInTreeById()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> na;
  albaSmartPointer<albaVMEHelper> na2;
  albaSmartPointer<albaVMEHelper> na3;

  int i = na->GetMaxNodeId();

  na2->ReparentTo(na);
  na3->ReparentTo(na2);

  albaVME *n = na->FindInTreeById(2);
  result = n != NULL;
  TEST_RESULT;
  result = n->Equals(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestFindInTreeByName()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root node");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("child 1");
  albaSmartPointer<albaVMEHelper> na3;
  na3->SetName("child 2");
  albaSmartPointer<albaVMEHelper> na4;
  na4->SetName("child 3");

  na->AddChild(na2);
  na2->AddChild(na3);
  na3->AddChild(na4);

  // by default 'match case' and 'whole word' are active
  albaVME *n = na->FindInTreeByName("child 2");
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
void albaVMETest::TestFindInTreeByTag()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;
  albaSmartPointer<albaVMEHelper> na3;
  
  albaTagItem ti;
  ti.SetName("test");
  albaTagArray *ta = na3->GetTagArray();
  ta->SetTag(ti);

  na->AddChild(na2);
  na->AddChild(na3);

  albaVME *n = na->FindInTreeByTag("test");
  result = n != NULL;
  TEST_RESULT;
  result = n->Equals(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestFindNodeIdx()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node nb");
  albaSmartPointer<albaVMEHelper> nc;
  nc->SetName("node nc");
  na->AddChild(nb);
  na->AddChild(nc);
  
  nb->SetVisibleToTraverse(false);

  result = na->FindNodeIdx("node nb") == 0;
  TEST_RESULT;
  result = na->FindNodeIdx("node nc") == 1;
  TEST_RESULT;

  //Testing find by visibility
  result = na->FindNodeIdx("node nb",true) == -1;
  TEST_RESULT;
  result = na->FindNodeIdx("node nc",true) == 0;
  TEST_RESULT;

  ////////////////////////////
  //Testing overloaded function

  result = na->FindNodeIdx(nb) == 0;
  TEST_RESULT;
  result = na->FindNodeIdx(nc) == 1;
  TEST_RESULT;

  //Testing find by visibility
  result = na->FindNodeIdx(nb,true) == -1;
  TEST_RESULT;
  result = na->FindNodeIdx(nc,true) == 0;
  TEST_RESULT;


}
//----------------------------------------------------------------------------
void albaVMETest::TestGetAttribute()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  result = na->GetAttribute("Not Exists") == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetChild()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node na");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node nb");
  albaSmartPointer<albaVMEHelper> nc;
  nc->SetName("node nc");
  
  na->AddChild(nb);
  na->AddChild(nc);
  result = na->GetChild(0)->Equals(nb);
  TEST_RESULT;

  result = na->GetChild(1)->Equals(nc);
  TEST_RESULT;

  //Testing by visibility
  nb->SetVisibleToTraverse(false);
  result = na->GetChild(0,true)->Equals(nc);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaVMETest::TestGetFirstChild()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node na");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node nb");
  albaSmartPointer<albaVMEHelper> nc;
  nc->SetName("node nc");
  
  na->AddChild(nb);
  na->AddChild(nc);
  albaVME *n = na->GetFirstChild();
  result = n->Equals(nb);
  TEST_RESULT;

  //testing by visibility
  nb->SetVisibleToTraverse(false);
  albaVME *nInv = na->GetFirstChild(true);
  result = nInv->Equals(nc);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaVMETest::TestGetLastChild()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node na");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node nb");
  albaSmartPointer<albaVMEHelper> nc;
  nc->SetName("node nc");

  na->AddChild(nb);
  na->AddChild(nc);
  albaVME *n = na->GetLastChild();
  result = n->Equals(nc);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetLink()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaVME *nl = na->GetLink("Not Existing Link");
  result = nl == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetLink()
//----------------------------------------------------------------------------
{
  // To link a node it have to be into a tree with Id != -1 (invalid Id)
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node na");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node nb");
  root->AddChild(na);
  root->AddChild(nb);
  na->UpdateId();
  nb->UpdateId();

  na->SetLink("test", nb);
  result = na->GetLink("test")->Equals(nb);
  TEST_RESULT;

  // Overwrite old link
  albaSmartPointer<albaVMEHelper> nc;
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
void albaVMETest::TestGetLinkSubId()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> nb;
  albaSmartPointer<albaVMEHelper> nc;
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
void albaVMETest::TestGetNumberOfLinks()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> nb;
  albaSmartPointer<albaVMEHelper> nc;
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
void albaVMETest::TestRemoveLink()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> nb;
  albaSmartPointer<albaVMEHelper> nc;
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
void albaVMETest::TestRemoveAllLinks()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> nb;
  albaSmartPointer<albaVMEHelper> nc;
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
void albaVMETest::TestDependsOnLinkedNode()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node na");
  albaSmartPointer<albaVMEHelper> nb;
  nb->SetName("node nb");
  albaSmartPointer<albaVMEHelper> nc;
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
void albaVMETest::TestSetAttribute()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;

  albaSmartPointer<albaTagArray> ta;
  ta->SetName("My Attribute");
  na->SetAttribute("My Attribute", ta);

  albaAttribute *a = na->GetAttribute("My Attribute");
  result = a != NULL;
  TEST_RESULT;
  result = a->Equals(ta);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestRemoveAttribute()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;

  albaSmartPointer<albaTagArray> ta;
  ta->SetName("My Attribute");
  na->SetAttribute("My Attribute", ta);

  na->RemoveAttribute("My Attribute");

  albaAttribute *a = na->GetAttribute("My Attribute");
  result = a == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestRemoveAllAttributes()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;

  albaSmartPointer<albaTagArray> ta;
  ta->SetName("My Attribute");
  na->SetAttribute("My Attribute", ta);

  na->RemoveAllAttributes();

  albaAttribute *a = na->GetAttribute("My Attribute");
  result = a == NULL;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestMakeCopy()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("node a");

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;

  albaVMEHelper *na2 = (albaVMEHelper *)na->MakeCopy();
  result = na2->Equals(na);
  TEST_RESULT;

  result = na2->GetReferenceCount() == 0;
  TEST_RESULT;
  albaDEL(na2);
}
//----------------------------------------------------------------------------
void albaVMETest::TestReparentTo()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");

  result = na2->ReparentTo(na) == ALBA_OK;
  TEST_RESULT;

  result = na2->ReparentTo(na) == ALBA_OK;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetParent()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");
  na->AddChild(na2);

  result = na2->GetParent()->Equals(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetRoot()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");
  albaSmartPointer<albaVMEHelper> na3;
  na3->SetName("node na3");
  albaSmartPointer<albaVMEHelper> na4;
  na4->SetName("node na4");

  na->AddChild(na2);
  na2->AddChild(na3);
  na3->AddChild(na4);
  result = na4->GetRoot()->Equals(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetTagArray()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaTagArray *ta = na->GetTagArray();
  result = ta != NULL;
  TEST_RESULT;

  result = ta->IsA("albaTagArray");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsValid()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMERoot> root;
  albaVMEHelper *na = NULL;
  albaNEW(na);

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
void albaVMETest::TestIsVisible()
//----------------------------------------------------------------------------
{
  // By default a node can be traversed by iterator, so it is called 'visible'
  albaSmartPointer<albaVMEHelper> na;
  result = na->IsVisible();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsInTree()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  result = !na->IsInTree(NULL);
  TEST_RESULT;

  albaSmartPointer<albaVMEHelper> na1;
  na1->SetName("node na1");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");

  na2->ReparentTo(na1);

  result = !na->IsInTree(na2);
  TEST_RESULT;

  result = na1->IsInTree(na2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsAChild()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");

  result = !na->IsAChild(na2);
  TEST_RESULT;

  na2->ReparentTo(na);
  result = na->IsAChild(na2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetNumberOfChildren()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");
  
  result = na->GetNumberOfChildren() == 0;
  TEST_RESULT;

  na2->ReparentTo(na);
  result = na->GetNumberOfChildren() == 1;
  TEST_RESULT;

  //Testing by visibility
  na2->SetVisibleToTraverse(false);
  result = na->GetNumberOfChildren(true) == 0;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsEmpty()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");

  result = na->IsEmpty();
  TEST_RESULT;

  na2->ReparentTo(na);
  result = !na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestRemoveChild()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  na->SetName("root");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("node na2");
  albaSmartPointer<albaVMEHelper> na3;
  na3->SetName("node na3");

  na->AddChild(na2);
  na->AddChild(na3);

  na->RemoveChild(na2);
  result = na->GetNumberOfChildren() == 1;
  TEST_RESULT;

  albaID node_id = 0;
  na->RemoveChild(node_id);
  result = na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestRemoveAllChildren()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;

  na2->ReparentTo(na);
  na->RemoveAllChildren();
  result = na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestCleanTree()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;

  na2->ReparentTo(na);
  na->CleanTree();
  result = na->IsEmpty();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestNewIterator()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaVMEIterator *iter = NULL;
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
void albaVMETest::TestCanReparentTo()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  result = na->CanReparentTo(NULL);
  TEST_RESULT;

  albaSmartPointer<albaVMEHelper> na2;
  result = na2->CanReparentTo(na);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetChildren()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;

  result = na->GetChildren()->size() == 0;
  TEST_RESULT;

  na2->ReparentTo(na);
  result = na->GetChildren()->size() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestAddChild()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;

  na->AddChild(na2);
  result = na->GetChildren()->size() == 1;
  TEST_RESULT;

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;

  result = na2->GetReferenceCount() == 2; // Is referenced by 'na'
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestCompareTree()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEBHelper> na;
  na->SetName("NodeB");
  albaSmartPointer<albaVMEHelper> na2;
  na2->SetName("NodeB_Child");

  albaSmartPointer<albaVMEBHelper> na3;
  na3->SetName("NodeB");
  albaSmartPointer<albaVMEHelper> na4;
  na4->SetName("NodeB_Child");

  na->AddChild(na2);
  na3->AddChild(na4);

  result = na->CompareTree(na3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestCopyTree()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;
  na->AddChild(na2);

  albaVMEHelper *na3 = (albaVMEHelper *)na->CopyTree();
  result = na->CompareTree(na3);
  TEST_RESULT;
  albaDEL(na3);
}
//----------------------------------------------------------------------------
void albaVMETest::TestImport()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> na;
  albaSmartPointer<albaVMEHelper> na2;
  na->AddChild(na2);

  albaSmartPointer<albaVMEHelper> na3;
  na3->Import(na);

  result = na3->GetChild(0)->Equals(na2);
  TEST_RESULT;

  result = na3->GetChildren()->size() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestBuildAndDestroyATree()
//----------------------------------------------------------------------------
{
  albaVMEHelper *na = NULL;
  albaNEW(na);

  result = na->GetReferenceCount() == 1;
  TEST_RESULT;

  albaVMEHelper *na2 = NULL;
  albaNEW(na2);

  result = na2->GetReferenceCount() == 1;
  TEST_RESULT;

  na->AddChild(na2);
  result = na2->GetReferenceCount() == 2;
  TEST_RESULT;

  na->RemoveChild(na2);

  result = na2->GetReferenceCount() == 1;
  TEST_RESULT;

  albaDEL(na);
  albaDEL(na2);
}

//----------------------------------------------------------------------------
void albaVMETest::TestGetByPath()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEHelper> root ;
  albaSmartPointer<albaVMEHelper> sideA ;
  albaSmartPointer<albaVMEHelper> sideB ;
  albaSmartPointer<albaVMEHelper> a1 ;
  albaSmartPointer<albaVMEHelper> a2 ;
  albaSmartPointer<albaVMEHelper> a3 ;
  albaSmartPointer<albaVMEHelper> b1 ;
  albaSmartPointer<albaVMEHelper> b2 ;
  albaSmartPointer<albaVMEHelper> b3 ;


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

  b2->SetVisibleToTraverse(false);

  //generating structure
  //             Root
  //            /    \
  //       sideA      sideB
  //      /  |  \    /  |  \
  //     a1  a2 a3  b1  b2  b3 
  //                    ^
  //                 not visible
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
  result= ((albaVMEHelper *)sideA->GetByPath("next") == (albaVMEHelper *)(sideB));
  TEST_RESULT;

  //Testing Prev
  result= (root->GetByPath("prev") == NULL);
  TEST_RESULT;
  result= (sideA->GetByPath("prev") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideB->GetByPath("prev") == (albaVMEHelper *)(sideA));
  TEST_RESULT;

  //Testing FirstPair
  result= (root->GetByPath("firstPair") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a2->GetByPath("firstPair") == (albaVMEHelper *)(a1));
  TEST_RESULT;

  //Testing LastChild
  result= (root->GetByPath("lastPair") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a2->GetByPath("lastPair") == (albaVMEHelper *)(a3));
  TEST_RESULT;


  //Testing FirstChild
  result= (a1->GetByPath("firstChild") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("firstChild") == (albaVMEHelper *)(a1));
  TEST_RESULT;

  //Testing LastChild
  result= (a1->GetByPath("lastChild") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("lastChild") == (albaVMEHelper *)(a3));
  TEST_RESULT;

  //Testing Pair[]
  result= ((albaVMEHelper *)root->GetByPath("pair[2]") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair[2") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair[two]") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair[3]") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair[2]") == (albaVMEHelper *)(a3));
  TEST_RESULT;

  //Testing Pair{}
  result= ((albaVMEHelper *)root->GetByPath("pair{a2}") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair{a2") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair{a4}") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("pair{a2}") == (albaVMEHelper *)(a2));
  TEST_RESULT;


  //Testing Child[]
  result= ((albaVMEHelper *)sideA->GetByPath("child[2") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("child[two]") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("child[3]") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("child[2]") == (albaVMEHelper *)(a3));
  TEST_RESULT;

  //Testing Child{}
  result= ((albaVMEHelper *)sideA->GetByPath("child{a2") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("child{a4}") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)sideA->GetByPath("child{a2}") == (albaVMEHelper *)(a2));
  TEST_RESULT;


  //Testing ".."
  result= ((albaVMEHelper *)root->GetByPath("..") == NULL);
  TEST_RESULT;
  result= ((albaVMEHelper *)a1->GetByPath("..") == (albaVMEHelper *)(sideA));
  TEST_RESULT;

  //Testing "."
  result= ((albaVMEHelper *)a1->GetByPath(".") == (albaVMEHelper *)(a1));
  TEST_RESULT;

  //Testing complex paths

  //a1->b3 (b2 is not visible)
  result= ((albaVMEHelper *)a1->GetByPath("../../child{sideB}/child[1]") == (albaVMEHelper *)(b3));
  TEST_RESULT;

  //a1->b2 (b2 is not visible)
  result= ((albaVMEHelper *)a1->GetByPath("../../child{sideB}/child[1]",false) == (albaVMEHelper *)(b2));
  TEST_RESULT;


  //sideB->a2
  result= ((albaVMEHelper *)sideB->GetByPath("prev/child{a2}") == (albaVMEHelper *)(a2));
  TEST_RESULT;

  //sideA->b1
  result= ((albaVMEHelper *)sideA->GetByPath("lastPair/firstChild") == (albaVMEHelper *)(b1));
  TEST_RESULT;
  
  //wrong
  result= ((albaVMEHelper *)sideA->GetByPath("../../lastChild") == NULL);
  TEST_RESULT;
}
