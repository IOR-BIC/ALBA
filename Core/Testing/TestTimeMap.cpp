#include "mmuTimeSet.cpp"
#include "mafReferenceCounted.h"
#include "mafTimeMap.txx"
#include "mafSmartPointer.h"
#include <iostream>

class mafTestTObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafTestTObject,mafReferenceCounted);
  mafTestTObject():Flag(NULL),m_TimeStamp(-1) {}
  ~mafTestTObject() {if (Flag) *Flag=false;}
  mafTimeStamp GetTimeStamp() {return m_TimeStamp;}
  void SetTimeStamp(mafTimeStamp t) {m_TimeStamp=t;}
  void SetFlag(bool &flag) {Flag=&flag;}
  bool GetFlag() {return *Flag;}
protected:
  bool *Flag;
  mafTimeStamp m_TimeStamp;
};

mafCxxTypeMacro(mafTestTObject);

class mafTestTVector: public mafTimeMap<mafTestTObject>
{
  mafTypeMacro(mafTestTVector,mafObject);
};

mafCxxTypeMacro(mafTestTVector);

int main()
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};

  mafTestTVector tvector;
  
  int i;
  for (i=0;i<6;i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }


  MAF_TEST(tvector.GetNumberOfItems()==6);

  // test ordering
  mafTimeStamp old_t;
  mafTestTVector::Iterator it;
  for (old_t=0,it=tvector.Begin();it!=tvector.End();it++)
  {
    MAF_TEST(it->second->GetTimeStamp()==it->first);
    MAF_TEST(old_t<it->second->GetTimeStamp());
    old_t=it->first;
    MAF_TEST(it->second->GetReferenceCount()==1);
  }
  
  // test find by index
  MAF_TEST(tvector.FindItemIndex(1.2)==2);
  MAF_TEST(tvector.FindItemIndex(.5)==0);
  MAF_TEST(tvector.FindItemIndex(3.6)==5);

  mafSmartPointer<mafTestTObject> t0;
  t0->SetTimeStamp(.3);
  // test insertion at begin
  tvector.PrependItem(t0); // inserted in constant time
  MAF_TEST(tvector.FindItemIndex(.3)==0);
  mafSmartPointer<mafTestTObject> t5;
  t5->SetTimeStamp(2);
  tvector.PrependItem(t5); // inserted in 2*log(N) time: worst case 
  MAF_TEST(tvector.FindItemIndex(2)==5); 

  // test insertion at end
  mafSmartPointer<mafTestTObject> t8;
  t8->SetTimeStamp(4);
  tvector.AppendItem(t8); // inserted in constant time
  MAF_TEST(tvector.FindItemIndex(4)==8);
  mafSmartPointer<mafTestTObject> t0b;
  t0b->SetTimeStamp(.2);
  tvector.AppendItem(t0b); // inserted in 2*log(N) time: worst case
  MAF_TEST(tvector.FindItemIndex(.2)==0); 
  
  // check we have the correct number of time stamps
  MAF_TEST(tvector.GetNumberOfItems()==10);
  
  mafTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  
  // check for set sort integrity
  for (i=0,it=tvector.Begin();it!=tvector.End();it++,i++)
  {
    MAF_TEST(final_tarray[i]==it->first);
  }
  
  // test the find algorithms
  MAF_TEST(tvector.GetItemBefore(2.4)->GetTimeStamp()==2.0);
  MAF_TEST(tvector.GetNearestItem(2.4)->GetTimeStamp()==2.5);
  
  // test search of an element out of bounds
  MAF_TEST(tvector.GetItemBefore(.1)->GetTimeStamp()==.2); 
  MAF_TEST(tvector.GetNearestItem(.1)->GetTimeStamp()==.2);

  MAF_TEST(tvector.GetItemBefore(5)->GetTimeStamp()==4); 
  MAF_TEST(tvector.GetNearestItem(5)->GetTimeStamp()==4);

  // test removal
  it=tvector.FindItem(2.0);
  MAF_TEST(it!=tvector.End());
  tvector.RemoveItem(it);
  MAF_TEST(tvector.GetNumberOfItems()==9);
  MAF_TEST(tvector.FindItem(2.0)==tvector.End());

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
