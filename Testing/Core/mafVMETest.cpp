/*=========================================================================

 Program: MAF2
 Module: mafVMETest
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
#include "mafVMETest.h"

#include "mafInteractorCameraMove.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEOutputNULL.h"

#include "mafCoreTests.h"

#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMERoot.h"
#include "mafVMEIterator.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMETest::TestFixture()
{
}

//----------------------------------------------------------------------------
void mafVMETest::TestDynamicAllocation()
{
  mafSmartPointer<mafVMEHelper> vme;

  mafVMEHelper *vme2;
  mafNEW(vme2);
  vme2->Delete();
}
//----------------------------------------------------------------------------
void mafVMETest::TestAllConstructor()
{
  mafSmartPointer<mafVMEHelper> vme;
  result = vme->GetReferenceCount() == 1; // mafSmartPointer increase the reference count
  TEST_RESULT;

  mafVMEHelper *vme2 = NULL;
  vme2 = mafVMEHelper::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  mafDEL(vme2);

  mafVMEHelper *vme3 = NULL;
  mafNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // mafNEW macro increase the reference count
  TEST_RESULT;
  mafDEL(vme3);
}
//----------------------------------------------------------------------------
void mafVMETest::TestCanReparentTo()
{
  mafSmartPointer<mafVMEHelper> vme;
  result = vme->CanReparentTo(NULL);
  TEST_RESULT;

  mafSmartPointer<mafVMEHelper> vme2;
  result = vme2->CanReparentTo(vme);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestEquals()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMEHelper> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);

  mafSmartPointer<mafVMEHelper> vme2;
  
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
void mafVMETest::TestDeepCopy()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMEHelper> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);

  mafSmartPointer<mafVMEHelper> vme2;
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;

	//

	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node a");

	mafSmartPointer<mafVMEHelper> na2;
	na2->DeepCopy(na);

	result = na2->Equals(na);
	TEST_RESULT;

	mafSmartPointer<mafVMEHelper> nl;
	nl->SetName("node linked");
	nl->SetId(1); // Needed to be linked

	na2->SetLink("LinkName", nl);

	mafSmartPointer<mafVMEHelper> na3;
	na3->DeepCopy(na2);
	result = na3->GetNumberOfLinks() == 1;
	TEST_RESULT;

	result = na3->GetLink("LinkName")->Equals(nl);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestShallowCopy()
{
  // For basic VMEs ShallowCopy is the same of DeepCopy
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsAnimated()
{
  mafSmartPointer<mafVMEHelper> vme1;
  result = !vme1->IsAnimated();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsDataAvailable()
{
  mafSmartPointer<mafVMEHelper> vme1;
  result = vme1->IsDataAvailable();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetAbsMatrix()
{
  // matrix at timestamp = 0.0
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEHelper> vme1;
  vme1->SetAbsMatrix(m);
  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;

  mafMatrix mi;
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
void mafVMETest::TestSetAbsPose()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEHelper> vme1;
  vme1->SetAbsPose(3.0,0.0,0.0,0.0,0.0,0.0);

  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestApplyAbsMatrix()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  m.SetElement(0,2,1.0);

  mafMatrix m_pre;
  m_pre.SetElement(0,3,6.0);
  m_pre.SetElement(0,2,1.0);
  m_pre.SetElement(1,3,1.0);
  m_pre.SetElement(2,3,2.0);

  mafMatrix m_post;
  m_post.SetElement(0,3,8.0);
  m_post.SetElement(0,2,1.0);
  m_post.SetElement(1,3,1.0);
  m_post.SetElement(2,3,2.0);

  mafSmartPointer<mafVMEHelper> vme1;
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
void mafVMETest::TestSetBehavior()
{
  mafSmartPointer<mafVMEHelper> vme1;

  // default behavior
  result = vme1->GetBehavior() == NULL;

  mafSmartPointer<mafInteractorCameraMove> bh;
  vme1->SetBehavior(bh);
  result = vme1->GetBehavior()->IsMAFType(mafInteractorCameraMove);
  TEST_RESULT;

  result = bh.GetPointer() == vme1->GetBehavior();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetCrypting()
{
  mafSmartPointer<mafVMEHelper> vme1;
  
  // default encrypting value:
  result = vme1->GetCrypting() == 0;
  TEST_RESULT;

  vme1->SetCrypting(1);
  result = vme1->GetCrypting() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetParent()
{
  mafSmartPointer<mafVMEHelper> vme1;
  mafSmartPointer<mafVMEHelper> vme2;

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
void mafVMETest::TestSetMatrix()
{
  // matrix at timestamp = 0.0
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEHelper> vme1;
  vme1->SetMatrix(m);
  result = vme1->GetOutput()->GetMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetPose()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEHelper> vme1;
  vme1->SetPose(3.0,0.0,0.0,0.0,0.0,0.0,0.0);

  result = vme1->GetOutput()->GetMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestApplyMatrix()
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  m.SetElement(0,2,1.0);

  mafMatrix m_pre;
  m_pre.SetElement(0,3,6.0);
  m_pre.SetElement(0,2,1.0);
  m_pre.SetElement(1,3,1.0);
  m_pre.SetElement(2,3,2.0);

  mafMatrix m_post;
  m_post.SetElement(0,3,8.0);
  m_post.SetElement(0,2,1.0);
  m_post.SetElement(1,3,1.0);
  m_post.SetElement(2,3,2.0);

  mafSmartPointer<mafVMEHelper> vme1;
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
void mafVMETest::TestSetTimeStamp()
{
  mafSmartPointer<mafVMEHelper> vme1;
  // default timestamp
  result = mafEquals(0.0, vme1->GetTimeStamp());
  TEST_RESULT;

  vme1->SetTimeStamp(1.0);
  result = mafEquals(1.0, vme1->GetTimeStamp());
  TEST_RESULT;

  // timestamp < 0 => timestamp = 0 in SetTimeStamp
  vme1->SetTimeStamp(-1.0);
  result = mafEquals(0.0, vme1->GetTimeStamp());
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetTreeTime()
{
  mafSmartPointer<mafVMEHelper> vme1;
  mafSmartPointer<mafVMEHelper> vme2;
  vme2->ReparentTo(vme1);

  result = mafEquals(vme1->GetTimeStamp(), 0.0);
  TEST_RESULT;

  result = mafEquals(vme2->GetTimeStamp(), 0.0);
  TEST_RESULT;

  vme1->SetTreeTime(1.0);
  
  result = mafEquals(vme1->GetTimeStamp(), 1.0);
  TEST_RESULT;

  result = mafEquals(vme2->GetTimeStamp(), 1.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetVisualMode()
{
  mafSmartPointer<mafVMEHelper> vme1;
  // default value for visual mode variable
  result = vme1->GetVisualMode() == mafVME::DEFAULT_VISUAL_MODE;
  TEST_RESULT;

  vme1->SetVisualMode(mafVME::NO_DATA_VISUAL_MODE);
  result = vme1->GetVisualMode() == mafVME::NO_DATA_VISUAL_MODE;
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafVMETest::TestSetName()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node a");
	result = mafString::Equals(na->GetName(), "node a");
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestCanCopy()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node a");

	mafSmartPointer<mafVMEHelper> na2;
	result = na2->CanCopy(na);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestFindInTreeById()
{
	mafSmartPointer<mafVMERoot> na;
	mafSmartPointer<mafVMEHelper> na2;
	mafSmartPointer<mafVMEHelper> na3;

	int i = na->GetMaxNodeId();

	na2->ReparentTo(na);
	na3->ReparentTo(na2);

	mafVME *n = na->FindInTreeById(2);
	result = n != NULL;
	TEST_RESULT;
	result = n->Equals(na3);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestFindInTreeByName()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root node");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("child 1");
	mafSmartPointer<mafVMEHelper> na3;
	na3->SetName("child 2");
	mafSmartPointer<mafVMEHelper> na4;
	na4->SetName("child 3");

	na->AddChild(na2);
	na2->AddChild(na3);
	na3->AddChild(na4);

	// by default 'match case' and 'whole word' are active
	mafVME *n = na->FindInTreeByName("child 2");
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
void mafVMETest::TestFindInTreeByTag()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;
	mafSmartPointer<mafVMEHelper> na3;

	mafTagItem ti;
	ti.SetName("test");
	mafTagArray *ta = na3->GetTagArray();
	ta->SetTag(ti);

	na->AddChild(na2);
	na->AddChild(na3);

	mafVME *n = na->FindInTreeByTag("test");
	result = n != NULL;
	TEST_RESULT;
	result = n->Equals(na3);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestFindNodeIdx()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	mafSmartPointer<mafVMEHelper> nc;
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
void mafVMETest::TestGetAttribute()
{
	mafSmartPointer<mafVMEHelper> na;
	result = na->GetAttribute("Not Exists") == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestGetChild()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	mafSmartPointer<mafVMEHelper> nc;
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
void mafVMETest::TestGetFirstChild()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	mafSmartPointer<mafVMEHelper> nc;
	nc->SetName("node nc");

	na->AddChild(nb);
	na->AddChild(nc);
	mafVME *n = na->GetFirstChild();
	result = n->Equals(nb);
	TEST_RESULT;

	//testing by visibility
	nb->SetVisibleToTraverse(false);
	mafVME *nInv = na->GetFirstChild(true);
	result = nInv->Equals(nc);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestGetLastChild()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	mafSmartPointer<mafVMEHelper> nc;
	nc->SetName("node nc");

	na->AddChild(nb);
	na->AddChild(nc);
	mafVME *n = na->GetLastChild();
	result = n->Equals(nc);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetAttribute()
{
	mafSmartPointer<mafVMEHelper> na;

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
void mafVMETest::TestRemoveAttribute()
{
	mafSmartPointer<mafVMEHelper> na;

	mafSmartPointer<mafTagArray> ta;
	ta->SetName("My Attribute");
	na->SetAttribute("My Attribute", ta);

	na->RemoveAttribute("My Attribute");

	mafAttribute *a = na->GetAttribute("My Attribute");
	result = a == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestRemoveAllAttributes()
{
	mafSmartPointer<mafVMEHelper> na;

	mafSmartPointer<mafTagArray> ta;
	ta->SetName("My Attribute");
	na->SetAttribute("My Attribute", ta);

	na->RemoveAllAttributes();

	mafAttribute *a = na->GetAttribute("My Attribute");
	result = a == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestMakeCopy()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node a");

	result = na->GetReferenceCount() == 1;
	TEST_RESULT;

	mafVMEHelper *na2 = (mafVMEHelper *)na->MakeCopy();
	result = na2->Equals(na);
	TEST_RESULT;

	result = na2->GetReferenceCount() == 0;
	TEST_RESULT;
	mafDEL(na2);
}
//----------------------------------------------------------------------------
void mafVMETest::TestReparentTo()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");

	result = na2->ReparentTo(na) == MAF_OK;
	TEST_RESULT;

	result = na2->ReparentTo(na) == MAF_OK;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestGetParent()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");
	na->AddChild(na2);

	result = na2->GetParent()->Equals(na);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestGetRoot()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");
	mafSmartPointer<mafVMEHelper> na3;
	na3->SetName("node na3");
	mafSmartPointer<mafVMEHelper> na4;
	na4->SetName("node na4");

	na->AddChild(na2);
	na2->AddChild(na3);
	na3->AddChild(na4);
	result = na4->GetRoot()->Equals(na);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestGetTagArray()
{
	mafSmartPointer<mafVMEHelper> na;
	mafTagArray *ta = na->GetTagArray();
	result = ta != NULL;
	TEST_RESULT;

	result = ta->IsA("mafTagArray");
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsValid()
{
	mafSmartPointer<mafVMERoot> root;
	mafVMEHelper *na = NULL;
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
void mafVMETest::TestIsVisible()
{
	// By default a node can be traversed by iterator, so it is called 'visible'
	mafSmartPointer<mafVMEHelper> na;
	result = na->IsVisible();
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsInTree()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	result = !na->IsInTree(NULL);
	TEST_RESULT;

	mafSmartPointer<mafVMEHelper> na1;
	na1->SetName("node na1");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");

	na2->ReparentTo(na1);

	result = !na->IsInTree(na2);
	TEST_RESULT;

	result = na1->IsInTree(na2);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsAChild()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");

	result = !na->IsAChild(na2);
	TEST_RESULT;

	na2->ReparentTo(na);
	result = na->IsAChild(na2);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestGetNumberOfChildren()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
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
void mafVMETest::TestIsEmpty()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");

	result = na->IsEmpty();
	TEST_RESULT;

	na2->ReparentTo(na);
	result = !na->IsEmpty();
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestRemoveChild()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("root");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");
	mafSmartPointer<mafVMEHelper> na3;
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
void mafVMETest::TestRemoveAllChildren()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;

	na2->ReparentTo(na);
	na->RemoveAllChildren();
	result = na->IsEmpty();
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestCleanTree()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;

	na2->ReparentTo(na);
	na->CleanTree();
	result = na->IsEmpty();
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestNewIterator()
{
	mafSmartPointer<mafVMEHelper> na;
	mafVMEIterator *iter = NULL;
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
void mafVMETest::TestGetChildren()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;

	result = na->GetChildren()->size() == 0;
	TEST_RESULT;

	na2->ReparentTo(na);
	result = na->GetChildren()->size() == 1;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestAddChild()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;

	na->AddChild(na2);
	result = na->GetChildren()->size() == 1;
	TEST_RESULT;

	result = na->GetReferenceCount() == 1;
	TEST_RESULT;

	result = na2->GetReferenceCount() == 2; // Is referenced by 'na'
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestCompareTree()
{
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("NodeB");
	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("NodeB_Child");

	mafSmartPointer<mafVMEHelper> na3;
	na3->SetName("NodeB");
	mafSmartPointer<mafVMEHelper> na4;
	na4->SetName("NodeB_Child");

	na->AddChild(na2);
	na3->AddChild(na4);

	result = na->CompareTree(na3);
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestCopyTree()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;
	na->AddChild(na2);

	mafVMEHelper *na3 = (mafVMEHelper *)na->CopyTree();
	result = na->CompareTree(na3);
	TEST_RESULT;
	mafDEL(na3);
}
//----------------------------------------------------------------------------
void mafVMETest::TestImport()
{
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> na2;
	na->AddChild(na2);

	mafSmartPointer<mafVMEHelper> na3;
	na3->Import(na);

	result = na3->GetChild(0)->Equals(na2);
	TEST_RESULT;

	result = na3->GetChildren()->size() == 1;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestBuildAndDestroyATree()
{
	mafVMEHelper *na = NULL;
	mafNEW(na);

	result = na->GetReferenceCount() == 1;
	TEST_RESULT;

	mafVMEHelper *na2 = NULL;
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
void mafVMETest::TestGetByPath()
{
	mafSmartPointer<mafVMEHelper> root;
	mafSmartPointer<mafVMEHelper> sideA;
	mafSmartPointer<mafVMEHelper> sideB;
	mafSmartPointer<mafVMEHelper> a1;
	mafSmartPointer<mafVMEHelper> a2;
	mafSmartPointer<mafVMEHelper> a3;
	mafSmartPointer<mafVMEHelper> b1;
	mafSmartPointer<mafVMEHelper> b2;
	mafSmartPointer<mafVMEHelper> b3;
	
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
	result = ((mafVMEHelper *)sideA->GetByPath("next") == (mafVMEHelper *)(sideB));
	TEST_RESULT;

	//Testing Prev
	result = (root->GetByPath("prev") == NULL);
	TEST_RESULT;
	result = (sideA->GetByPath("prev") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideB->GetByPath("prev") == (mafVMEHelper *)(sideA));
	TEST_RESULT;

	//Testing FirstPair
	result = (root->GetByPath("firstPair") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a2->GetByPath("firstPair") == (mafVMEHelper *)(a1));
	TEST_RESULT;

	//Testing LastChild
	result = (root->GetByPath("lastPair") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a2->GetByPath("lastPair") == (mafVMEHelper *)(a3));
	TEST_RESULT;
	
	//Testing FirstChild
	result = (a1->GetByPath("firstChild") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("firstChild") == (mafVMEHelper *)(a1));
	TEST_RESULT;

	//Testing LastChild
	result = (a1->GetByPath("lastChild") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("lastChild") == (mafVMEHelper *)(a3));
	TEST_RESULT;

	//Testing Pair[]
	result = ((mafVMEHelper *)root->GetByPath("pair[2]") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair[2") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair[two]") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair[3]") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair[2]") == (mafVMEHelper *)(a3));
	TEST_RESULT;

	//Testing Pair{}
	result = ((mafVMEHelper *)root->GetByPath("pair{a2}") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair{a2") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair{a4}") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("pair{a2}") == (mafVMEHelper *)(a2));
	TEST_RESULT;
	
	//Testing Child[]
	result = ((mafVMEHelper *)sideA->GetByPath("child[2") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("child[two]") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("child[3]") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("child[2]") == (mafVMEHelper *)(a3));
	TEST_RESULT;

	//Testing Child{}
	result = ((mafVMEHelper *)sideA->GetByPath("child{a2") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("child{a4}") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)sideA->GetByPath("child{a2}") == (mafVMEHelper *)(a2));
	TEST_RESULT;


	//Testing ".."
	result = ((mafVMEHelper *)root->GetByPath("..") == NULL);
	TEST_RESULT;
	result = ((mafVMEHelper *)a1->GetByPath("..") == (mafVMEHelper *)(sideA));
	TEST_RESULT;

	//Testing "."
	result = ((mafVMEHelper *)a1->GetByPath(".") == (mafVMEHelper *)(a1));
	TEST_RESULT;

	//Testing complex paths

	//a1->b3 (b2 is not visible)
	result = ((mafVMEHelper *)a1->GetByPath("../../child{sideB}/child[1]") == (mafVMEHelper *)(b3));
	TEST_RESULT;

	//a1->b2 (b2 is not visible)
	result = ((mafVMEHelper *)a1->GetByPath("../../child{sideB}/child[1]", false) == (mafVMEHelper *)(b2));
	TEST_RESULT;
	
	//sideB->a2
	result = ((mafVMEHelper *)sideB->GetByPath("prev/child{a2}") == (mafVMEHelper *)(a2));
	TEST_RESULT;

	//sideA->b1
	result = ((mafVMEHelper *)sideA->GetByPath("lastPair/firstChild") == (mafVMEHelper *)(b1));
	TEST_RESULT;

	//wrong
	result = ((mafVMEHelper *)sideA->GetByPath("../../lastChild") == NULL);
	TEST_RESULT;
}

// Links

//----------------------------------------------------------------------------
void mafVMETest::TestGetLink()
{
	mafSmartPointer<mafVMEHelper> na;
	mafVME *nl = na->GetLink("Not Existing Link");
	result = nl == NULL;
	TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetLink()
{
	// To link a node it have to be into a tree with Id != -1 (invalid Id)
	mafSmartPointer<mafVMERoot> root;

	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	root->AddChild(na);
	na->UpdateId();

	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	root->AddChild(nb);
	nb->UpdateId();

	na->SetLink("test", nb);
	result = na->GetLink("test")->Equals(nb);
	TEST_RESULT;

	// Overwrite old link
	mafSmartPointer<mafVMEHelper> nc;
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
void mafVMETest::TestMandatoryLink()
{
	mafSmartPointer<mafVMERoot> root;

	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	root->AddChild(na);
	na->UpdateId();

	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	root->AddChild(nb);
	nb->UpdateId();

	mafSmartPointer<mafVMEHelper> nc;
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
	mafString name = na->GetLink("linkAB")->GetName();
	CPPUNIT_ASSERT(name.Equals("new node nb"));
}

//----------------------------------------------------------------------------
void mafVMETest::TestDependencies()
{
	mafSmartPointer<mafVMERoot> root;

	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	root->AddChild(na);
	na->UpdateId();

	mafSmartPointer<mafVMEHelper> na1;
	na1->SetName("node na1");
	na->AddChild(na1);
	na1->UpdateId();

	mafSmartPointer<mafVMEHelper> na2;
	na2->SetName("node na2");
	na->AddChild(na2);
	na2->UpdateId();

	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	root->AddChild(nb);
	nb->UpdateId();

	mafSmartPointer<mafVMEHelper> nb1;
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
void mafVMETest::TestGetNumberOfLinks()
{
	mafSmartPointer<mafVMERoot> root;
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> nb;
	mafSmartPointer<mafVMEHelper> nc;
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
void mafVMETest::TestRemoveLink()
{
	mafSmartPointer<mafVMERoot> root;
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> nb;
	mafSmartPointer<mafVMEHelper> nc;
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
void mafVMETest::TestRemoveAllLinks()
{
	mafSmartPointer<mafVMERoot> root;
	mafSmartPointer<mafVMEHelper> na;
	mafSmartPointer<mafVMEHelper> nb;
	mafSmartPointer<mafVMEHelper> nc;
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
void mafVMETest::TestDependsOnLinkedNode()
{
	mafSmartPointer<mafVMERoot> root;
	mafSmartPointer<mafVMEHelper> na;
	na->SetName("node na");
	mafSmartPointer<mafVMEHelper> nb;
	nb->SetName("node nb");
	mafSmartPointer<mafVMEHelper> nc;
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

