/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMETest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.4.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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

#define TEST_RESULT CPPUNIT_ASSERT(result);

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafVMEA: public mafVME
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafVMEA,mafVME);

  void SetMatrix(const mafMatrix &mat) {m_Transform->SetMatrix(mat); Modified();};
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) {kframes.clear();};
protected:
  mafVMEA();
  virtual ~mafVMEA() {mafDEL(m_Transform);};

  mafTransform *m_Transform;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEA);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEA::mafVMEA()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputNULL *output = mafVMEOutputNULL::New();
  output->SetTransform(m_Transform);
  SetOutput(output);
}

//----------------------------------------------------------------------------
void mafVMETest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMETest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafVMETest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMETest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme;

  mafVMEA *vme2;
  mafNEW(vme2);
  vme2->Delete();
}
//----------------------------------------------------------------------------
void mafVMETest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme;
  result = vme->GetReferenceCount() == 1; // mafSmartPointer increase the reference count
  TEST_RESULT;

  mafVMEA *vme2 = NULL;
  vme2 = mafVMEA::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  mafDEL(vme2);

  mafVMEA *vme3 = NULL;
  mafNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // mafNEW macro increase the reference count
  TEST_RESULT;
  mafDEL(vme3);
}
//----------------------------------------------------------------------------
void mafVMETest::TestCanReparentTo()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme;
  result = vme->CanReparentTo(NULL);
  TEST_RESULT;

  mafSmartPointer<mafVMEA> vme2;
  result = vme2->CanReparentTo(vme);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestEquals()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMEA> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);

  mafSmartPointer<mafVMEA> vme2;
  
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
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3,3.0);
  // Matrix pose for VME at the same time of the VME timestamp
  m.SetTimeStamp(1.0);

  mafSmartPointer<mafVMEA> vme1;
  vme1->SetName("vme test");
  vme1->SetTimeStamp(1.0);
  vme1->SetAbsMatrix(m);

  mafSmartPointer<mafVMEA> vme2;
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  // For basic VMEs ShallowCopy is the same of DeepCopy
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsAnimated()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
  result = !vme1->IsAnimated();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestIsDataAvailable()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
  result = vme1->IsDataAvailable();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetAbsMatrix()
//----------------------------------------------------------------------------
{
  // matrix at timestamp = 0.0
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEA> vme1;
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
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEA> vme1;
  vme1->SetAbsPose(3.0,0.0,0.0,0.0,0.0,0.0);

  result = vme1->GetOutput()->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestApplyAbsMatrix()
//----------------------------------------------------------------------------
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

  mafSmartPointer<mafVMEA> vme1;
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
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;

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
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
  
  // default encrypting value:
  result = vme1->GetCrypting() == 0;
  TEST_RESULT;

  vme1->SetCrypting(1);
  result = vme1->GetCrypting() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetParent()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
  mafSmartPointer<mafVMEA> vme2;

  result = vme2->GetParent() == NULL;
  TEST_RESULT;

  vme2->SetParent(vme1);
  result = vme2->GetParent() == vme1.GetPointer();
  TEST_RESULT;

  // SetParent doesn't manage the reference count of VMEs.
  // ReparenTo doesn't need the line of code below.
  vme2->SetParent(NULL);
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetMatrix()
//----------------------------------------------------------------------------
{
  // matrix at timestamp = 0.0
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEA> vme1;
  vme1->SetMatrix(m);
  result = vme1->GetOutput()->GetMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestSetPose()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3,3.0);

  mafSmartPointer<mafVMEA> vme1;
  vme1->SetPose(3.0,0.0,0.0,0.0,0.0,0.0,0.0);

  result = vme1->GetOutput()->GetMatrix()->Equals(&m);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMETest::TestApplyMatrix()
//----------------------------------------------------------------------------
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

  mafSmartPointer<mafVMEA> vme1;
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
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
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
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
  mafSmartPointer<mafVMEA> vme2;
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
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEA> vme1;
  // default value for visual mode variable
  result = vme1->GetVisualMode() == mafVME::DEFAULT_VISUAL_MODE;
  TEST_RESULT;

  vme1->SetVisualMode(mafVME::NO_DATA_VISUAL_MODE);
  result = vme1->GetVisualMode() == mafVME::NO_DATA_VISUAL_MODE;
  TEST_RESULT;
}
