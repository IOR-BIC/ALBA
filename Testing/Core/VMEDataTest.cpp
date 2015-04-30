/*=========================================================================

 Program: MAF2
 Module: mafViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "VMEDataTest.h"
#include <cppunit/config/SourcePrefix.h>
#include "mafVMERoot.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafIndent.h"
#include <iostream>
#include <vector>

// this test does not perform any test on serialization, it tests matrix pipe and
// abs matrix pipe.

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETestOutputHelper)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETestHelper);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafMatrixPipeTestHelper);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafMatrixPipeTestHelper::InternalUpdate()
//-------------------------------------------------------------------------
{
  mafVMETestHelper *vme=(mafVMETestHelper *)m_VME;
  unsigned long index=(GetTimeStamp()<vme->GetMatrixVector().size())?GetTimeStamp():vme->GetMatrixVector().size()-1;
  m_Matrix->DeepCopy(vme->GetMatrixVector()[index]);
}

//-------------------------------------------------------------------------
// MatrixPipeTest END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafVMETestHelper::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os << indent << "Matrix vector: [";
  int i;
  for (i=0;i<m_MatrixVector.size();i++)
  {
    if (i!=0)
      os << " , ";

    os << m_MatrixVector[i]->GetTimeStamp();
  }
  os << "]" << std::endl;

  os << indent << "Data vector: [";
  for (i=0;i<m_DataVector.size();i++)
  {
    if (i!=0)
      os << " , ";
    os << m_DataVector[i];
  }
  os << "]" << std::endl;
}

//-------------------------------------------------------------------------
bool mafVMETestHelper::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMETestHelper *vme_test=mafVMETestHelper::SafeDownCast(vme);
    assert(vme);
    int i;
    for (i=0;i<m_MatrixVector.size();i++)
    {
      if (!(m_MatrixVector[i]->Equals(vme_test->m_MatrixVector[i])))
        return false;
    }

    for (i=0;i<m_DataVector.size();i++)
    {
      if (m_DataVector[i]!=vme_test->m_DataVector[i])
        return false; 
    }
    
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
void mafVMETestHelper::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  if (m_MatrixVector.size()<=mat.GetTimeStamp()) // make the vector fit the requested size
    m_MatrixVector.resize(mat.GetTimeStamp()+1);

  m_MatrixVector[mat.GetTimeStamp()]->DeepCopy(&mat); // simple integer timestamps!
  Modified();
}

//-------------------------------------------------------------------------
void mafVMETestHelper::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
  kframes.resize(m_MatrixVector.size());
  for (int i=0;i<m_MatrixVector.size();i++)
  {
    kframes[i]=m_MatrixVector[i]->GetTimeStamp();
  }
}

//-------------------------------------------------------------------------
mafVMETestHelper::mafVMETestHelper()
//-------------------------------------------------------------------------
{
  m_Output = mafVMETestOutputHelper::New();
  m_Output->SetVME(this);
  SetMatrixPipe(mafMatrixPipeTestHelper::New());
  //
}

//-------------------------------------------------------------------------
// VMETest END
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
void VMEDataTest::VmeDataMainTest()
//-------------------------------------------------------------------------
{
  // create a small tree
  mafVMETest *vme1=mafVMETest::New();
  mafVMETest *vme2=mafVMETest::New();
  mafVMETest *vme3=mafVMETest::New();
  mafSmartPointer<mafVMERoot> root;
  
  root->AddChild(vme1);
  root->AddChild(vme2);
  vme1->AddChild(vme3);

  root->SetPose(-10,-10,-10,0,0,0,0);

  mafTransform trans;
  for (int i=0;i<10;i++)
  {
    
    vme1->SetPose(trans.GetMatrix(),i);     // test the SetPose(matrix,t) function
    trans.Translate(10,10,10,PRE_MULTIPLY);
    vme2->SetPose(i,i,i,0,0,0,i);           // test the SetPose(x,y,z,rx,ry,rz,t) function
    vme3->SetPose(0,0,0,i*10,i*10,i*10,i);  // test the SetPose(x,y,z,rx,ry,rz,t) function
  }
  
  mafTransform root_pose_result;
  root_pose_result.SetPosition(-10,-10,-10);

  // test output values for different times
  for (int j=0;j<10;j++)
  {
    root->SetTreeTime(j);
    mafMatrix root_pose = *(root->GetOutput()->GetMatrix());
    mafMatrix vme1_pose = *(vme1->GetOutput()->GetMatrix());
    mafMatrix vme2_pose = *(vme2->GetOutput()->GetMatrix());
    mafMatrix vme3_pose = *(vme3->GetOutput()->GetMatrix());
    mafMatrix vme1_abspose = *(vme1->GetOutput()->GetAbsMatrix());
    mafMatrix vme2_abspose = *(vme2->GetOutput()->GetAbsMatrix());
    mafMatrix vme3_abspose = *(vme3->GetOutput()->GetAbsMatrix());
    
    mafTransform vme1_abspose_test;
    vme1_abspose_test.SetPosition(10*j-10,10*j-10,10*j-10);
    vme1_abspose_test.SetTimeStamp(j);

    mafTransform vme2_abspose_test;
    vme2_abspose_test.SetPosition(j-10,j-10,j-10);
    vme2_abspose_test.SetTimeStamp(j);

    mafTransform vme3_abspose_test;
    vme3_abspose_test.SetPosition(10*j-10,10*j-10,10*j-10);
    vme3_abspose_test.SetOrientation(j*10,j*10,j*10);
    vme3_abspose_test.SetTimeStamp(j);

    mafTransform vme1_pose_test;
    vme1_pose_test.SetPosition(10*j,10*j,10*j);
    vme1_pose_test.SetTimeStamp(j);

    mafTransform vme2_pose_test;
    vme2_pose_test.SetPosition(j,j,j);
    vme2_pose_test.SetTimeStamp(j);

    mafTransform vme3_pose_test;
    vme3_pose_test.SetOrientation(j*10,j*10,j*10);
    vme3_pose_test.SetTimeStamp(j);

    CPPUNIT_ASSERT(root_pose==root_pose_result.GetMatrix());
    CPPUNIT_ASSERT(vme1_pose==vme1_pose_test.GetMatrix());
    CPPUNIT_ASSERT(vme2_pose==vme2_pose_test.GetMatrix())
    CPPUNIT_ASSERT(vme3_pose==vme3_pose_test.GetMatrix())
    CPPUNIT_ASSERT(vme1_abspose==vme1_abspose_test.GetMatrix());
    CPPUNIT_ASSERT(vme2_abspose==vme2_abspose_test.GetMatrix())
    CPPUNIT_ASSERT(vme3_abspose==vme3_abspose_test.GetMatrix())
  }

}

