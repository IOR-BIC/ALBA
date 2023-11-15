/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMETest
 Authors: Paolo Quadrani
 
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
#include "albaVMETest.h"

#include "albaInteractorCameraMove.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaVMEOutputNULL.h"

#include "albaCoreTests.h"

#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaVMERoot.h"
#include "albaVMEIterator.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMETest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaVMETest::TestDynamicAllocation()
{
  albaSmartPointer<albaVMEHelper> vme;

  albaVMEHelper *vme2;
  albaNEW(vme2);
  vme2->Delete();
}
//----------------------------------------------------------------------------
void albaVMETest::TestAllConstructor()
{
  albaSmartPointer<albaVMEHelper> vme;
  result = vme->GetReferenceCount() == 1; // albaSmartPointer increase the reference count
  TEST_RESULT;

  albaVMEHelper *vme2 = NULL;
  vme2 = albaVMEHelper::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  albaDEL(vme2);

  albaVMEHelper *vme3 = NULL;
  albaNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // albaNEW macro increase the reference count
  TEST_RESULT;
  albaDEL(vme3);
}
//----------------------------------------------------------------------------
void albaVMETest::TestCanReparentTo()
{
  albaSmartPointer<albaVMEHelper> vme;
  result = vme->CanReparentTo(NULL);
  TEST_RESULT;

  albaSmartPointer<albaVMEHelper> vme2;
  result = vme2->CanReparentTo(vme);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestEquals()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);

  albaSmartPointer<albaVMEHelper> vme2;
  
  // different name, matrix and timestamp
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // same name but different matrix and timestamp
  vme2->SetName("vme test");
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // same name and timestamp but different matrix
  vme2->SetTimeStamp(1.0);
  result = !vme1->Equals(vme2);
  TEST_RESULT;

  // vmes equals
  vme2->SetAbsMatrix(m);
  result = vme1->Equals(vme2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestDeepCopy()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);

  albaSmartPointer<albaVMEHelper> vme2;
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;

	//

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
void albaVMETest::TestShallowCopy()
{
  // For basic VMEs ShallowCopy is the same of DeepCopy
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsAnimated()
{
  albaSmartPointer<albaVMEHelper> vme1;
  result = !vme1->IsAnimated();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsDataAvailable()
{
  albaSmartPointer<albaVMEHelper> vme1;
  result = vme1->IsDataAvailable();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetAbsMatrix()
{
  // matrix at timestamp = 0.0
  albaMatrix m;
  m.SetElement(0,3,3.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetAbsMatrix(m);
  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;

  albaMatrix mi;
  mi.Identity();
  // reset the matrix
  vme1->SetAbsMatrix(mi);

  // set the matrix for different timestamp
  vme1->SetAbsMatrix(m, 1.0);
  
  // Basic VME is NOT time varying so matrix is always the same
  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetAbsPose()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetAbsPose(3.0,0.0,0.0,0.0,0.0,0.0);

  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestApplyAbsMatrix()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);
  m.SetElement(0,2,1.0);

  albaMatrix m_pre;
  m_pre.SetElement(0,3,6.0);
  m_pre.SetElement(0,2,1.0);
  m_pre.SetElement(1,3,1.0);
  m_pre.SetElement(2,3,2.0);

  albaMatrix m_post;
  m_post.SetElement(0,3,8.0);
  m_post.SetElement(0,2,1.0);
  m_post.SetElement(1,3,1.0);
  m_post.SetElement(2,3,2.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetAbsPose(3.0,1.0,2.0,0.0,0.0,0.0);
  // Premultiply flag == 1 => VME ABS matrix Premultiply the matrix 'm'
  vme1->ApplyAbsMatrix(m,1);

  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m_pre);
  TEST_RESULT;

  // reset the abs matrix
  vme1->SetAbsPose(3.0,1.0,2.0,0.0,0.0,0.0);
  // Postmultiply
  vme1->ApplyAbsMatrix(m,0);
  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m_post);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetBehavior()
{
  albaSmartPointer<albaVMEHelper> vme1;

  // default behavior
  result = vme1->GetBehavior() == NULL;

  albaSmartPointer<albaInteractorCameraMove> bh;
  vme1->SetBehavior(bh);
  result = vme1->GetBehavior()->IsALBAType(albaInteractorCameraMove);
  TEST_RESULT;

  result = bh.GetPointer() == vme1->GetBehavior();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetCrypting()
{
  albaSmartPointer<albaVMEHelper> vme1;
  
  // default encrypting value:
  result = vme1->GetCrypting() == 0;
  TEST_RESULT;

  vme1->SetCrypting(1);
  result = vme1->GetCrypting() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetParent()
{
  albaSmartPointer<albaVMEHelper> vme1;
  albaSmartPointer<albaVMEHelper> vme2;

  result = vme2->GetParent() == NULL;
  TEST_RESULT;

  vme2->ReparentTo(vme1);
  result = vme2->GetParent() == vme1.GetPointer();
  TEST_RESULT;

  // SetParent doesn't manage the reference count of VMEs.
  // ReparenTo doesn't need the line of code below.
  vme2->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetMatrix()
{
  // matrix at timestamp = 0.0
  albaMatrix m;
  m.SetElement(0,3,3.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetMatrix(m);
  result = vme1->GetOutput()->GetMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetPose()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetPose(3.0,0.0,0.0,0.0,0.0,0.0,0.0);

  result = vme1->GetOutput()->GetMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestApplyMatrix()
{
  albaMatrix m;
  m.SetElement(0,3,3.0);
  m.SetElement(0,2,1.0);

  albaMatrix m_pre;
  m_pre.SetElement(0,3,6.0);
  m_pre.SetElement(0,2,1.0);
  m_pre.SetElement(1,3,1.0);
  m_pre.SetElement(2,3,2.0);

  albaMatrix m_post;
  m_post.SetElement(0,3,8.0);
  m_post.SetElement(0,2,1.0);
  m_post.SetElement(1,3,1.0);
  m_post.SetElement(2,3,2.0);

  albaSmartPointer<albaVMEHelper> vme1;
  vme1->SetPose(3.0,1.0,2.0,0.0,0.0,0.0,0.0);
  // Premultiply flag == 1 => VME ABS matrix Premultiply the matrix 'm'
  vme1->ApplyMatrix(m,1);

  result = vme1->GetOutput()->GetMatrix()->Equals(&m_pre);
  TEST_RESULT;

  // reset the abs matrix
  vme1->SetPose(3.0,1.0,2.0,0.0,0.0,0.0,0.0);
  // Postmultiply
  vme1->ApplyMatrix(m,0);
  result = vme1->GetOutput()->GetMatrix()->Equals(&m_post);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetTimeStamp()
{
  albaSmartPointer<albaVMEHelper> vme1;
  // default timestamp
  result = albaEquals(0.0, vme1->GetTimeStamp());
  TEST_RESULT;

  vme1->SetTimeStamp(1.0);
  result = albaEquals(1.0, vme1->GetTimeStamp());
  TEST_RESULT;

  // timestamp < 0 => timestamp = 0 in SetTimeStamp
  vme1->SetTimeStamp(-1.0);
  result = albaEquals(0.0, vme1->GetTimeStamp());
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetTreeTime()
{
  albaSmartPointer<albaVMEHelper> vme1;
  albaSmartPointer<albaVMEHelper> vme2;
  vme2->ReparentTo(vme1);

  result = albaEquals(vme1->GetTimeStamp(), 0.0);
  TEST_RESULT;

  result = albaEquals(vme2->GetTimeStamp(), 0.0);
  TEST_RESULT;

  vme1->SetTreeTime(1.0);
  
  result = albaEquals(vme1->GetTimeStamp(), 1.0);
  TEST_RESULT;

  result = albaEquals(vme2->GetTimeStamp(), 1.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetVisualMode()
{
  albaSmartPointer<albaVMEHelper> vme1;
  // default value for visual mode variable
  result = vme1->GetVisualMode() == albaVME::DEFAULT_VISUAL_MODE;
  TEST_RESULT;

  vme1->SetVisualMode(albaVME::NO_DATA_VISUAL_MODE);
  result = vme1->GetVisualMode() == albaVME::NO_DATA_VISUAL_MODE;
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVMETest::TestSetName()
{
	albaSmartPointer<albaVMEHelper> na;
	na->SetName("node a");
	result = albaString::Equals(na->GetName(), "node a");
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestCanCopy()
{
	albaSmartPointer<albaVMEHelper> na;
	na->SetName("node a");

	albaSmartPointer<albaVMEHelper> na2;
	result = na2->CanCopy(na);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestFindInTreeById()
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
	result = na->FindNodeIdx("node nb", true) == -1;
	TEST_RESULT;
	result = na->FindNodeIdx("node nc", true) == 0;
	TEST_RESULT;

	////////////////////////////
	//Testing overloaded function

	result = na->FindNodeIdx(nb) == 0;
	TEST_RESULT;
	result = na->FindNodeIdx(nc) == 1;
	TEST_RESULT;

	//Testing find by visibility
	result = na->FindNodeIdx(nb, true) == -1;
	TEST_RESULT;
	result = na->FindNodeIdx(nc, true) == 0;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetAttribute()
{
	albaSmartPointer<albaVMEHelper> na;
	result = na->GetAttribute("Not Exists") == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetChild()
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
	result = na->GetChild(0, true)->Equals(nc);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestGetFirstChild()
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
void albaVMETest::TestSetAttribute()
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
	result = !na->IsValid();
	na->Delete();

	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsVisible()
{
	// By default a node can be traversed by iterator, so it is called 'visible'
	albaSmartPointer<albaVMEHelper> na;
	result = na->IsVisible();
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestIsInTree()
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
void albaVMETest::TestGetChildren()
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
{
	albaSmartPointer<albaVMEHelper> na;
	na->SetName("NodeB");
	albaSmartPointer<albaVMEHelper> na2;
	na2->SetName("NodeB_Child");

	albaSmartPointer<albaVMEHelper> na3;
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
{
	albaSmartPointer<albaVMEHelper> na;
	albaSmartPointer<albaVMEHelper> na2;

	na->SetName("na");
	na2->SetName("na2");

	na->AddChild(na2);

	albaVMEHelper *na3 = (albaVMEHelper *)na->CopyTree();
	result = na->CompareTree(na3);
	TEST_RESULT;
	albaDEL(na3);
}
//----------------------------------------------------------------------------
void albaVMETest::TestImport()
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
{
	albaSmartPointer<albaVMEHelper> root;
	albaSmartPointer<albaVMEHelper> sideA;
	albaSmartPointer<albaVMEHelper> sideB;
	albaSmartPointer<albaVMEHelper> a1;
	albaSmartPointer<albaVMEHelper> a2;
	albaSmartPointer<albaVMEHelper> a3;
	albaSmartPointer<albaVMEHelper> b1;
	albaSmartPointer<albaVMEHelper> b2;
	albaSmartPointer<albaVMEHelper> b3;
	
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

	//Testing wrong keyword
	result = (root->GetByPath("wrong") == NULL);
	TEST_RESULT;
	
	//Testing Next
	result = (root->GetByPath("next") == NULL);
	TEST_RESULT;
	result = (sideB->GetByPath("next") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("next") == (albaVMEHelper *)(sideB));
	TEST_RESULT;

	//Testing Prev
	result = (root->GetByPath("prev") == NULL);
	TEST_RESULT;
	result = (sideA->GetByPath("prev") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideB->GetByPath("prev") == (albaVMEHelper *)(sideA));
	TEST_RESULT;

	//Testing FirstPair
	result = (root->GetByPath("firstPair") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a2->GetByPath("firstPair") == (albaVMEHelper *)(a1));
	TEST_RESULT;

	//Testing LastChild
	result = (root->GetByPath("lastPair") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a2->GetByPath("lastPair") == (albaVMEHelper *)(a3));
	TEST_RESULT;
	
	//Testing FirstChild
	result = (a1->GetByPath("firstChild") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("firstChild") == (albaVMEHelper *)(a1));
	TEST_RESULT;

	//Testing LastChild
	result = (a1->GetByPath("lastChild") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("lastChild") == (albaVMEHelper *)(a3));
	TEST_RESULT;

	//Testing Pair[]
	result = ((albaVMEHelper *)root->GetByPath("pair[2]") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair[2") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair[two]") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair[3]") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair[2]") == (albaVMEHelper *)(a3));
	TEST_RESULT;

	//Testing Pair{}
	result = ((albaVMEHelper *)root->GetByPath("pair{a2}") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair{a2") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair{a4}") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("pair{a2}") == (albaVMEHelper *)(a2));
	TEST_RESULT;
	
	//Testing Child[]
	result = ((albaVMEHelper *)sideA->GetByPath("child[2") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("child[two]") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("child[3]") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("child[2]") == (albaVMEHelper *)(a3));
	TEST_RESULT;

	//Testing Child{}
	result = ((albaVMEHelper *)sideA->GetByPath("child{a2") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("child{a4}") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)sideA->GetByPath("child{a2}") == (albaVMEHelper *)(a2));
	TEST_RESULT;


	//Testing ".."
	result = ((albaVMEHelper *)root->GetByPath("..") == NULL);
	TEST_RESULT;
	result = ((albaVMEHelper *)a1->GetByPath("..") == (albaVMEHelper *)(sideA));
	TEST_RESULT;

	//Testing "."
	result = ((albaVMEHelper *)a1->GetByPath(".") == (albaVMEHelper *)(a1));
	TEST_RESULT;

	//Testing complex paths

	//a1->b3 (b2 is not visible)
	result = ((albaVMEHelper *)a1->GetByPath("../../child{sideB}/child[1]") == (albaVMEHelper *)(b3));
	TEST_RESULT;

	//a1->b2 (b2 is not visible)
	result = ((albaVMEHelper *)a1->GetByPath("../../child{sideB}/child[1]", false) == (albaVMEHelper *)(b2));
	TEST_RESULT;
	
	//sideB->a2
	result = ((albaVMEHelper *)sideB->GetByPath("prev/child{a2}") == (albaVMEHelper *)(a2));
	TEST_RESULT;

	//sideA->b1
	result = ((albaVMEHelper *)sideA->GetByPath("lastPair/firstChild") == (albaVMEHelper *)(b1));
	TEST_RESULT;

	//wrong
	result = ((albaVMEHelper *)sideA->GetByPath("../../lastChild") == NULL);
	TEST_RESULT;
}

// Links

//----------------------------------------------------------------------------
void albaVMETest::TestGetLink()
{
	albaSmartPointer<albaVMEHelper> na;
	albaVME *nl = na->GetLink("Not Existing Link");
	result = nl == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestSetLink()
{
	// To link a node it have to be into a tree with Id != -1 (invalid Id)
	albaSmartPointer<albaVMERoot> root;

	albaSmartPointer<albaVMEHelper> na;
	na->SetName("node na");
	root->AddChild(na);
	na->UpdateId();

	albaSmartPointer<albaVMEHelper> nb;
	nb->SetName("node nb");
	root->AddChild(nb);
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
void albaVMETest::TestMandatoryLink()
{
	albaSmartPointer<albaVMERoot> root;

	albaSmartPointer<albaVMEHelper> na;
	na->SetName("node na");
	root->AddChild(na);
	na->UpdateId();

	albaSmartPointer<albaVMEHelper> nb;
	nb->SetName("node nb");
	root->AddChild(nb);
	nb->UpdateId();

	albaSmartPointer<albaVMEHelper> nc;
	nc->SetName("node nc");
	root->AddChild(nc);
	nc->UpdateId();

	na->SetLink("linkAB", nb, NORMAL_LINK);
	int backLinks = nb->GetNumberOfBackLinks();
	CPPUNIT_ASSERT(nb->GetNumberOfBackLinks() == 0);

	nb->SetLink("linkBC", nc, MANDATORY_LINK);	
	backLinks = nc->GetNumberOfBackLinks();
	CPPUNIT_ASSERT(nc->GetNumberOfBackLinks() == 1);

	// Overwrite old link
	na->SetLink("linkAB", nb, MANDATORY_LINK);
	backLinks = nb->GetNumberOfBackLinks();
	CPPUNIT_ASSERT(nb->GetNumberOfBackLinks() == 1);

	// Remove Link
	int nlinks = nb->GetNumberOfLinks();
	backLinks = nc->GetNumberOfBackLinks();

	nb->RemoveLink("linkBC");

	nlinks = nb->GetNumberOfLinks();
	backLinks = nc->GetNumberOfBackLinks();
	CPPUNIT_ASSERT(nb->GetNumberOfLinks() == 0 && nc->GetNumberOfBackLinks() == 0);
	
	// Remove node
	nb->SetLink("linkBC", nc, MANDATORY_LINK);
	root->RemoveChild(nb);
	
	CPPUNIT_ASSERT(nb->GetNumberOfLinks() == 1 && nb->GetNumberOfLinks() == 1);
	CPPUNIT_ASSERT(na->GetNumberOfLinks() == 1 && nb->GetNumberOfLinks() == 1 && nc->GetNumberOfBackLinks() == 1);

	
	// Change name
	nb->SetName("new node nb");
	root->AddChild(nb);
	nb->UpdateId();

	CPPUNIT_ASSERT(na->GetNumberOfLinks() == 1);
	albaString name = na->GetLink("linkAB")->GetName();
	CPPUNIT_ASSERT(name.Equals("new node nb"));
}

//----------------------------------------------------------------------------
void albaVMETest::TestDependencies()
{
	albaSmartPointer<albaVMERoot> root;

	albaSmartPointer<albaVMEHelper> na;
	na->SetName("node na");
	root->AddChild(na);
	na->UpdateId();

	albaSmartPointer<albaVMEHelper> na1;
	na1->SetName("node na1");
	na->AddChild(na1);
	na1->UpdateId();

	albaSmartPointer<albaVMEHelper> na2;
	na2->SetName("node na2");
	na->AddChild(na2);
	na2->UpdateId();

	albaSmartPointer<albaVMEHelper> nb;
	nb->SetName("node nb");
	root->AddChild(nb);
	nb->UpdateId();

	albaSmartPointer<albaVMEHelper> nb1;
	nb1->SetName("node nb1");
	nb->AddChild(nb1);
	nb1->UpdateId();

	// Case 1
	na->SetLink("link A-A2", na2, MANDATORY_LINK);
	
	CPPUNIT_ASSERT(na->GetDependenciesVMEs().size() == 1);

	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(na1));
	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(na));
	CPPUNIT_ASSERT(!na->WillBeRemovedWithDependencies(nb));

	// Case 2
	nb1->SetLink("link B1-A", na, MANDATORY_LINK);
	
	CPPUNIT_ASSERT(na2->GetDependenciesVMEs().size() == 2);

	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(na1));
	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(na2));
	CPPUNIT_ASSERT(!na->WillBeRemovedWithDependencies(nb));
	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(nb1));

	CPPUNIT_ASSERT(!nb1->WillBeRemovedWithDependencies(na));
	CPPUNIT_ASSERT(!nb1->WillBeRemovedWithDependencies(na1));
	CPPUNIT_ASSERT(!nb1->WillBeRemovedWithDependencies(na2));

	// Case 3
	na2->SetLink("link A2-B", nb, MANDATORY_LINK);

	CPPUNIT_ASSERT(na->GetDependenciesVMEs().size() == 2);
	CPPUNIT_ASSERT(na2->GetDependenciesVMEs().size() == 2);
	CPPUNIT_ASSERT(nb->GetDependenciesVMEs().size() == 3);

	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(na1));
	CPPUNIT_ASSERT(na->WillBeRemovedWithDependencies(na2));
	CPPUNIT_ASSERT(!na->WillBeRemovedWithDependencies(nb));

	CPPUNIT_ASSERT(nb->WillBeRemovedWithDependencies(na));
	CPPUNIT_ASSERT(nb->WillBeRemovedWithDependencies(na1));
	CPPUNIT_ASSERT(nb->WillBeRemovedWithDependencies(na2));
	CPPUNIT_ASSERT(!nb->WillBeRemovedWithDependencies(nb));
}

//----------------------------------------------------------------------------
void albaVMETest::TestGetNumberOfLinks()
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
	na->SetLink("test2", nc);
	result = na->GetNumberOfLinks() == 2;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestRemoveLink()
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
	na->SetLink("test2", nc);
	na->RemoveLink("test2");

	result = na->GetNumberOfLinks() == 1;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestRemoveAllLinks()
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
	na->SetLink("test2", nc);
	na->RemoveAllLinks();

	result = na->GetNumberOfLinks() == 0;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMETest::TestDependsOnLinkedNode()
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
	na->SetLink("test2", nc);

	// Node 'ns' depends on linked node modified time.
	na->DependsOnLinkedNodeOn();
	unsigned long mt0 = na->GetMTime();

	// modify MTime for linked node => 'na' MTime will be modified
	nb->SetName("Node nb");
	unsigned long mt1 = na->GetMTime();

	result = mt1 > mt0;
	TEST_RESULT;
}

