#include "mafReferenceCounted.h"
#include "mafSmartPointer.h"
#include <iostream>
#include <map>

/**
 This object set a flag when allocated and reset it when deallocated,
 this way I can test when it's really deallocated.
*/
class mafFooObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafFooObject,mafReferenceCounted);
  mafFooObject():Flag(NULL) {}
  ~mafFooObject() {if (Flag) *Flag=false;}
  int *Flag;
};

mafCxxTypeMacro(mafFooObject);

mafReferenceCounted *CreateSmartObject(int &flag)
{
  mafFooObject *foo= mafFooObject::New();
  foo->Flag=&flag;
  flag=true;
  foo->Register(0); // keep it alive
  return foo;
}

void CreateSmartObject2(mafAutoPointer<mafFooObject> &out,int &flag)
{
  mafSmartPointer<mafFooObject> smart_foo;
  smart_foo->Flag=&flag;
  flag=true;
  out = smart_foo;
}

int main()
{
  int flag;
  mafReferenceCounted *obj=CreateSmartObject(flag);

  MAF_TEST(flag);
  obj->UnRegister(0);
  MAF_TEST(!flag);

  
  mafAutoPointer<mafFooObject> auto_foo;
  CreateSmartObject2(auto_foo,flag);
  MAF_TEST(flag);
  // force releasing object.
  // Notice: calling the mafSmartPointer UnRegister() not the mafReferenceCounted one!!!
  auto_foo.UnRegister(NULL); 
  MAF_TEST(!flag);

  mafSmartPointer<mafFooObject> a,b,c;
  a=b;
  b=c;
  MAF_TEST(a->GetReferenceCount()==1);
  

  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
