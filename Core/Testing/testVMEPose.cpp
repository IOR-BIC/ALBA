#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEOutput.h"
#include "mafMatrixPipe.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafIndent.h"
#include <iostream>
#include <vector>

// this test does not perform any test on serialization, it tests matrix pipe and
// abs matrix pipe.

//-------------------------------------------------------------------------
// VMETestOutput BEGIN
//-------------------------------------------------------------------------
/** Simple VME output concrete class for test purposes.
  */
class mafVMETestOutput : public mafVMEOutput
{
public:
  mafTypeMacro(mafVMETestOutput,mafVMEOutput)

  //virtual void Update();
    
protected:
  mafVMETestOutput(){}; // to be allocated with New()
  virtual ~mafVMETestOutput(){}; // to be deleted with Delete()

private:
  mafVMETestOutput(const mafVMETestOutput&); // Not implemented
  void operator=(const mafVMETestOutput&); // Not implemented
};

mafCxxTypeMacro(mafVMETestOutput)

//-------------------------------------------------------------------------
// VMETest BEGIN
//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafVMETest : public mafVME
{
public:
  mafTypeMacro(mafVMETest,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(mafVME *vme);
 
  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const mafMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  std::vector<mafSmartPointer<mafMatrix> > &GetMatrixVector() {return m_MatrixVector;}

protected:
  mafVMETest();
  virtual ~mafVMETest();

  std::vector<mafSmartPointer<mafMatrix> > m_MatrixVector;
  //std::vector<long>      m_DataVector;
private:
  mafVMETest(const mafVMETest&); // Not implemented
  void operator=(const mafVMETest&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETest);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// VMETestOutput END
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// MatrixPipeTest BEGIN
//-------------------------------------------------------------------------
/** simple matrix pipe to produce output data! */
class MAF_EXPORT mafMatrixPipeTest:public mafMatrixPipe
{
public:
  mafTypeMacro(mafMatrixPipeTest,mafMatrixPipe);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(mafVME *vme) {return Superclass::Accept(vme)&&vme->IsMAFType(mafVMETest);}

protected:
  mafMatrixPipeTest() {};
  virtual ~mafMatrixPipeTest() {};

  /** update the output matrix */
  virtual void InternalUpdate();

private:
  mafMatrixPipeTest(const mafMatrixPipeTest&); // Not implemented
  void operator=(const mafMatrixPipeTest&); // Not implemented
  
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafMatrixPipeTest);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafMatrixPipeTest::InternalUpdate()
//-------------------------------------------------------------------------
{
  mafVMETest *vme=(mafVMETest *)m_VME;
  unsigned long index=(GetCurrentTime()<vme->GetMatrixVector().size())?GetCurrentTime():vme->GetMatrixVector().size()-1;
  m_Matrix->DeepCopy(vme->GetMatrixVector()[index]);
}

//-------------------------------------------------------------------------
// MatrixPipeTest END
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafVMETest::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os << indent << "Matrix vector: [";
  for (int i=0;i<m_MatrixVector.size();i++)
  {
    if (i!=0)
      os << " , ";

    os << m_MatrixVector[i]->GetTimeStamp();
  }
  os << std::endl;

  /*os << indent << "Data vector: [";
  for (int i=0;i<m_DataVector.size();i++)
  {
    if (i!=0)
    os << " , ";
    os << m_DataVector[i];
  }
  os << std::endl;*/
}

//-------------------------------------------------------------------------
bool mafVMETest::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMETest *vme_test=mafVMETest::SafeDownCast(vme);
    assert(vme);
    int i;
    for (i=0;i<m_MatrixVector.size();i++)
    {
      if (!(m_MatrixVector[i]->Equals(vme_test->m_MatrixVector[i])))
        return false;
    }

    //for (i=0;i<m_DataVector.size();i++)
    //{
    //  if (m_DataVector[i]!=vme_test->m_DataVector[i])
    //    return false; 
    //}
    
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
void mafVMETest::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  if (m_MatrixVector.size()<=mat.GetTimeStamp()) // make the vector fit the requested size
    m_MatrixVector.resize(mat.GetTimeStamp()+1);

  m_MatrixVector[mat.GetTimeStamp()]->DeepCopy(&mat); // simple integer timestamps!
  Modified();
}

//-------------------------------------------------------------------------
void mafVMETest::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
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
mafVMETest::mafVMETest()
//-------------------------------------------------------------------------
{
  m_Output = mafVMETestOutput::New();
  m_Output->SetVME(this);
  SetMatrixPipe(mafMatrixPipeTest::New());
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
  MAF_TEST(tree_tbounds[0]==0);
  MAF_TEST(tree_tbounds[1]==9);

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
