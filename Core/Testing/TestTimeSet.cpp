#include "mmuTimeSet.cpp"
#include "mafDefines.h"
#include <iostream>

int main()
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};

  mmuTimeSet tset;
  int i;
  for (i=0;i<6;i++)
    tset.Insert(tarray[i]);

  MAF_TEST(tset.GetNumberOfTimeStamps()==6);

  // test ordering
  mafTimeStamp old_t;
  mmuTimeSet::Iterator it;
  for (old_t=0,it=tset.Begin();it!=tset.End();it++)
  {
    MAF_TEST(old_t<*it);
    old_t=*it;
  }
  
  // test find by index
  MAF_TEST(tset.FindTimeStampIndex(1.2)==2);
  MAF_TEST(tset.FindTimeStampIndex(.5)==0);
  MAF_TEST(tset.FindTimeStampIndex(3.6)==5);

  // test insertion at begin
  tset.Prepend(.3); // inserted in constant time
  MAF_TEST(tset.FindTimeStampIndex(.3)==0); 
  tset.Prepend(2); // inserted in 2*log(N) time
  MAF_TEST(tset.FindTimeStampIndex(2)==5); 

  // test insertion at end
  tset.Append(4); // inserted in constant time
  MAF_TEST(tset.FindTimeStampIndex(4)==8); 
  tset.Append(.2); // inserted in 2*log(N) time
  MAF_TEST(tset.FindTimeStampIndex(.2)==0); 

  // check we have the correct number of time stamps
  MAF_TEST(tset.GetNumberOfTimeStamps()==10);

  mafTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};


  // check for set sort integrity
  for (i=0,it=tset.Begin();it!=tset.End();it++,i++)
  {
    MAF_TEST(final_tarray[i]==*it);
  }

  // test the find algorithms
  MAF_TEST(tset.GetTimeStampBefore(2.4)==2.0);
  MAF_TEST(tset.GetNearestTimeStamp(2.4)==2.5);

  // test search of an element out of bounds
  MAF_TEST(tset.GetTimeStampBefore(.1)==.2); 
  MAF_TEST(tset.GetNearestTimeStamp(.1)==.2);

  MAF_TEST(tset.GetTimeStampBefore(5)==4); 
  MAF_TEST(tset.GetNearestTimeStamp(5)==4);

  // test merge
  mmuTimeSet tset2;
  tset2.Append(1.5); 
  tset2.Append(1.8);
  tset2.Append(2.3);
  tset2.Append(2.5);

  mmuTimeSet tset_result;
  mmuTimeSet::Merge(tset,tset2,tset_result);

  MAF_TEST(tset_result.GetNumberOfTimeStamps()==12);

  for (old_t=0,it=tset_result.Begin();it!=tset_result.End();it++)
  {
    MAF_TEST(old_t<*it);
    old_t=*it;
    // test the time to be in one of the two original sets
    MAF_TEST((tset.FindTimeStamp(*it)!=tset.End())||(tset2.FindTimeStamp(*it)!=tset2.End()))
  }

  for (it=tset.Begin();it!=tset.End();it++)
  {
    MAF_TEST(tset_result.FindTimeStamp(*it)!=tset_result.End());
  }

  for (it=tset2.Begin();it!=tset2.End();it++)
  {
    MAF_TEST(tset_result.FindTimeStamp(*it)!=tset_result.End());
  }
  
  // test copy
  mmuTimeSet tset_copia=tset;
  MAF_TEST(tset==tset_copia);

  // test second Merge syntax
  tset_copia.Merge(tset2);
  MAF_TEST(tset_result==tset_copia);

  // test third Merge syntax
  mmuTimeSet::Merge(tset,tset2,tset2);
  MAF_TEST(tset_result==tset2);

  // test removal
  it=tset_result.FindTimeStamp(2.0);
  MAF_TEST(it!=tset_result.End());
  tset_result.Remove(it);
  MAF_TEST(tset_result.GetNumberOfTimeStamps()==11);
  MAF_TEST(tset_result.FindTimeStamp(2.0)==tset_result.End());

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
