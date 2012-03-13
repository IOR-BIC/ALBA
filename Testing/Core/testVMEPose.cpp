#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputVTK.h"
#include "mafMatrixPipe.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafIndent.h"
#include <iostream>
#include <vector>

// this test does not perform any test on serialization, it tests matrix pipe and
// abs matrix pipe.

//-------------------------------------------------------------------------
// VMETest BEGIN
//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafVMETest : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMETest, mafVMEGenericAbstract);

protected:
  mafVMETest();
  virtual ~mafVMETest();

private:
  mafVMETest(const mafVMETest&); // Not implemented
  void operator=(const mafVMETest&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETest);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMETest::mafVMETest()
//-------------------------------------------------------------------------
{
  SetOutput(mafVMEOutputVTK::New());
}
//-------------------------------------------------------------------------
mafVMETest::~mafVMETest()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
// VMETest END
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
int main()
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

    MAF_TEST(root_pose==root_pose_result.GetMatrix());
    MAF_TEST(vme1_pose==vme1_pose_test.GetMatrix());
    MAF_TEST(vme2_pose==vme2_pose_test.GetMatrix())
    MAF_TEST(vme3_pose==vme3_pose_test.GetMatrix())
    MAF_TEST(vme1_abspose==vme1_abspose_test.GetMatrix());
    MAF_TEST(vme2_abspose==vme2_abspose_test.GetMatrix())
    MAF_TEST(vme3_abspose==vme3_abspose_test.GetMatrix())
  }

   std::vector<mafTimeStamp> tvector;
  root->GetTimeStamps(tvector);

  for (int n=0;n<10;n++)
  {
    MAF_TEST(tvector[n]==n);
  }

  mafTimeStamp tree_tbounds[2];
  root->GetOutput()->GetTimeBounds(tree_tbounds);
  MAF_TEST(mafEquals(tree_tbounds[0],0.0));
  MAF_TEST(mafEquals(tree_tbounds[1],9.0));

  std::cerr<<"Test completed successfully!"<<std::endl;

  mafDEL(vme1);
  mafDEL(vme2);
  mafDEL(vme3);

  return MAF_OK;
}
