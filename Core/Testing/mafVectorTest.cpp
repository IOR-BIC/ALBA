#include "mafVector.txx"
#include "mafSmartPointer.h"
#include <iostream>

/**
 This object set a flag when allocated and reset it when deallocated,
 this way I can test when it's really deallocated.
*/
class mafTestObject: public mafSmartObject
{
public:
  mafTypeMacro(mafTestObject,mafSmartObject);
  mafTestObject():Flag(NULL) {}
  ~mafTestObject() {if (Flag) *Flag=false;}
  bool *Flag;
};

mafCxxTypeMacro(mafTestObject);

int main()
{ 
  bool flags[10];
  mafVector<mafAutoPointer<mafTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    flags[i]=true;
    mafSmartPointer<mafTestObject> obj;
    obj->Flag=&(flags[i]);
    oarray.SetItem(i,obj);
  }

  MAF_TEST(oarray.GetNumberOfItems()==10);

  MAF_TEST(oarray[10]==mafVector<mafAutoPointer<mafTestObject> >::NullItem);

  for (i=0;i<10;i++)
  {
    MAF_TEST(flags[i]);
    MAF_TEST(oarray[i]->GetReferenceCount()==1);
  }

  MAF_TEST(oarray[1].GetPointer()!=NULL);

  oarray.RemoveItem(3);

  MAF_TEST(oarray.GetNumberOfItems()==9);

  for (i=0;i<9;i++)
  {
    MAF_TEST(oarray[i]->GetReferenceCount()==1);
  }


  for (i=0;i<10;i++)
  {
    if (i==3)
    {
      MAF_TEST(!flags[i]);
    }
    else
    {
      MAF_TEST(flags[i]);
    }
  }
  
  // pop last object into a tmp container
  mafAutoPointer<mafTestObject> tmp;
  oarray.Pop(tmp);
  
  MAF_TEST(oarray.GetNumberOfItems()==8);

  MAF_TEST(tmp->GetReferenceCount()==1);

  // check the object is still alive
  MAF_TEST(flags[9]);
  tmp.UnRegister(NULL);
  MAF_TEST(!flags[9]); 

  // remove all objects
  oarray.RemoveAllItems();
  
  // check all the objects has been released
  for (i=0;i<10;i++)
  {
    MAF_TEST(!flags[i]);
  }

  MAF_TEST(!oarray.Pop(tmp));

  std::cout<<"Test completed successfully!"<<std::endl;
  
  return 0;
}
