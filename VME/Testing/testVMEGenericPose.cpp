#include "mafVMEGeneric.h"
#include "mafVMERoot.h"
#include "mafVMEOutput.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafIndent.h"
#include <iostream>


// this test does not perform any test on serialization and data, it tests matrix pipe and
// abs matrix pipe.
//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  // create a small tree
  mafVMEGeneric *vme1=mafVMEGeneric::New();
  mafVMEGeneric *vme2=mafVMEGeneric::New();
  mafVMEGeneric *vme3=mafVMEGeneric::New();
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

    root_pose_result.SetTimeStamp(j);
    root_pose_result.Modified();

    MAF_TEST(root_pose==root_pose_result.GetMatrix());
    MAF_TEST(vme1_pose==vme1_pose_test.GetMatrix());
    MAF_TEST(vme2_pose==vme2_pose_test.GetMatrix())
    MAF_TEST(vme3_pose==vme3_pose_test.GetMatrix())
    MAF_TEST(vme1_abspose==vme1_abspose_test.GetMatrix());
    MAF_TEST(vme2_abspose==vme2_abspose_test.GetMatrix())
    MAF_TEST(vme3_abspose==vme3_abspose_test.GetMatrix())
  }
  
  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
