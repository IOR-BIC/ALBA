#include "mafMatrixVector.h"
#include "mafMatrix.h"
#include <iostream>


int main()
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};

  mafMatrixVector tvector;
  
  int i;
  for (i=0;i<6;i++)
  {
    mafSmartPointer<mafMatrix> mat;
    mat->SetTimeStamp(tarray[i]);
    mat->SetElement(0,4,0.1*i);
    MAF_TEST(mat->GetElement(0,4)==.1*i);
    tvector.SetMatrix(mat);
  }
  
  MAF_TEST(tvector.GetNumberOfItems()==6);
  
  double results[6]={0.5,0.0,0.2,0.3,0.1,0.4};

  i=0;
  for (mafMatrixVector::Iterator it=tvector.Begin();it!=tvector.End();it++,i++)
  {
    mafMatrix *mat=it->second;
    mat->GetElement(0,4);
    MAF_TEST(mat->GetElement(0,4)-results[i]<.01);
  }

  for (i=0;i<6;i++)
  { 
    mafMatrix *mat= tvector.GetKeyMatrix(i);
    MAF_TEST(mat->GetReferenceCount()==1);
    MAF_TEST(mat->GetElement(0,4)-results[i]<.01);
  }

  mafMatrix *mat=tvector.GetMatrix(1.8);
  MAF_TEST(mat!=NULL && mat->GetTimeStamp()==1.8);

  mat=tvector.GetMatrix(2);
  MAF_TEST(mat==NULL);

  mat=tvector.GetMatrixBefore(2.4);
  MAF_TEST(mat!=NULL && mat->GetTimeStamp()==1.8);

  mat=tvector.GetNearestMatrix(2.4);
  MAF_TEST(mat!=NULL && mat->GetTimeStamp()==2.5);

  std::vector<mafMatrix*> mvect;
  tvector.GetKeyMatrixVector(mvect);

  MAF_TEST(mvect.size()==6);

  mafTimeStamp result_tarray[6]={0.5, 1, 1.2, 1.8, 2.5, 3.6};

  for (i=0;i<6;i++)
  {
    MAF_TEST(mvect[i]->GetTimeStamp()==result_tarray[i]);
    MAF_TEST(mvect[i]->GetElement(0,4)-results[i]<.01);
  }

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
