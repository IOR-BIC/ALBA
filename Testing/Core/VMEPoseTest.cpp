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
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "VMEPoseTest.h"
#include "albaVMERoot.h"
#include "albaVMEOutputVTK.h"
#include "albaMatrixPipe.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaIndent.h"
#include <iostream>
#include <vector>

// this test does not perform any test on serialization, it tests matrix pipe and
// abs matrix pipe.

//-------------------------------------------------------------------------
// VMETest BEGIN
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMETestHelper);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMETestHelper::albaVMETestHelper()
//-------------------------------------------------------------------------
{
  SetOutput(albaVMEOutputVTK::New());
}
//-------------------------------------------------------------------------
albaVMETestHelper::~albaVMETestHelper()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
// VMETest END
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
void VMEPoseTest::VmePoseMainTest()
//-------------------------------------------------------------------------
{
  // create a small tree
  albaVMETestHelper *vme1=albaVMETestHelper::New();
  albaVMETestHelper *vme2=albaVMETestHelper::New();
  albaVMETestHelper *vme3=albaVMETestHelper::New();
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
    CPPUNIT_ASSERT(vme2_pose==vme2_pose_test.GetMatrix());
    CPPUNIT_ASSERT(vme3_pose==vme3_pose_test.GetMatrix());
    CPPUNIT_ASSERT(vme1_abspose==vme1_abspose_test.GetMatrix());
    CPPUNIT_ASSERT(vme2_abspose==vme2_abspose_test.GetMatrix());
    CPPUNIT_ASSERT(vme3_abspose==vme3_abspose_test.GetMatrix());
  }

   std::vector<albaTimeStamp> tvector;
  root->GetTimeStamps(tvector);

  for (int n=0;n<10;n++)
  {
    CPPUNIT_ASSERT(tvector[n]==n);
  }

  albaTimeStamp tree_tbounds[2];
  root->GetOutput()->GetTimeBounds(tree_tbounds);
  CPPUNIT_ASSERT(albaEquals(tree_tbounds[0],0.0));
  CPPUNIT_ASSERT(albaEquals(tree_tbounds[1],9.0));

  std::cerr<<"Test completed successfully!"<<std::endl;

  albaDEL(vme1);
  albaDEL(vme2);
  albaDEL(vme3);
}

