/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "VMEDataTest.h"
#include <cppunit/config/SourcePrefix.h>
#include "albaVMERoot.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaIndent.h"
#include <iostream>
#include <vector>

// this test does not perform any test on serialization, it tests matrix pipe and
// abs matrix pipe.

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMETestOutputHelper)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMETestHelper);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaMatrixPipeTestHelper);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void albaMatrixPipeTestHelper::InternalUpdate()
//-------------------------------------------------------------------------
{
  albaVMETestHelper *vme=(albaVMETestHelper *)m_VME;
  unsigned long index=(GetTimeStamp()<vme->GetMatrixVector().size())?GetTimeStamp():vme->GetMatrixVector().size()-1;
  m_Matrix->DeepCopy(vme->GetMatrixVector()[index]);
}

//-------------------------------------------------------------------------
// MatrixPipeTest END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void albaVMETestHelper::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);
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
bool albaVMETestHelper::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    albaVMETestHelper *vme_test=albaVMETestHelper::SafeDownCast(vme);
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
void albaVMETestHelper::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  if (m_MatrixVector.size()<=mat.GetTimeStamp()) // make the vector fit the requested size
    m_MatrixVector.resize(mat.GetTimeStamp()+1);

  m_MatrixVector[mat.GetTimeStamp()]->DeepCopy(&mat); // simple integer timestamps!
  Modified();
}

//-------------------------------------------------------------------------
void albaVMETestHelper::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
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
albaVMETestHelper::albaVMETestHelper()
//-------------------------------------------------------------------------
{
  m_Output = albaVMETestOutputHelper::New();
  m_Output->SetVME(this);
  SetMatrixPipe(albaMatrixPipeTestHelper::New());
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
  albaVMETest *vme1=albaVMETest::New();
  albaVMETest *vme2=albaVMETest::New();
  albaVMETest *vme3=albaVMETest::New();
  albaSmartPointer<albaVMERoot> root;
  
  root->AddChild(vme1);
  root->AddChild(vme2);
  vme1->AddChild(vme3);

  root->SetPose(-10,-10,-10,0,0,0,0);

  albaTransform trans;
  for (int i=0;i<10;i++)
  {
    
    vme1->SetPose(trans.GetMatrix(),i);     // test the SetPose(matrix,t) function
    trans.Translate(10,10,10,PRE_MULTIPLY);
    vme2->SetPose(i,i,i,0,0,0,i);           // test the SetPose(x,y,z,rx,ry,rz,t) function
    vme3->SetPose(0,0,0,i*10,i*10,i*10,i);  // test the SetPose(x,y,z,rx,ry,rz,t) function
  }
  
  albaTransform root_pose_result;
  root_pose_result.SetPosition(-10,-10,-10);

  // test output values for different times
  for (int j=0;j<10;j++)
  {
    root->SetTreeTime(j);
    albaMatrix root_pose = *(root->GetOutput()->GetMatrix());
    albaMatrix vme1_pose = *(vme1->GetOutput()->GetMatrix());
    albaMatrix vme2_pose = *(vme2->GetOutput()->GetMatrix());
    albaMatrix vme3_pose = *(vme3->GetOutput()->GetMatrix());
    albaMatrix vme1_abspose = *(vme1->GetOutput()->GetAbsMatrix());
    albaMatrix vme2_abspose = *(vme2->GetOutput()->GetAbsMatrix());
    albaMatrix vme3_abspose = *(vme3->GetOutput()->GetAbsMatrix());
    
    albaTransform vme1_abspose_test;
    vme1_abspose_test.SetPosition(10*j-10,10*j-10,10*j-10);
    vme1_abspose_test.SetTimeStamp(j);

    albaTransform vme2_abspose_test;
    vme2_abspose_test.SetPosition(j-10,j-10,j-10);
    vme2_abspose_test.SetTimeStamp(j);

    albaTransform vme3_abspose_test;
    vme3_abspose_test.SetPosition(10*j-10,10*j-10,10*j-10);
    vme3_abspose_test.SetOrientation(j*10,j*10,j*10);
    vme3_abspose_test.SetTimeStamp(j);

    albaTransform vme1_pose_test;
    vme1_pose_test.SetPosition(10*j,10*j,10*j);
    vme1_pose_test.SetTimeStamp(j);

    albaTransform vme2_pose_test;
    vme2_pose_test.SetPosition(j,j,j);
    vme2_pose_test.SetTimeStamp(j);

    albaTransform vme3_pose_test;
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

