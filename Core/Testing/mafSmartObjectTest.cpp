#include "mafSmartObject.h"
#include "mafSmartPointer.h"
#include <iostream>

/**
 This object set a flag when allocated and reset it when deallocated,
 this way I can test when it's really deallocated.
*/
class mafFooObject: public mafSmartObject
{
public:
  mafTypeMacro(mafFooObject,mafSmartObject);
  mafFooObject():Flag(NULL) {}
  ~mafFooObject() {if (Flag) *Flag=false;}
  int *Flag;
};

mafCxxTypeMacro(mafFooObject);

/*class mafDummyObject: public mafObject
{
public:
  mafTypeMacro(mafDummyObject,mafObject);
  mafDummyObject() {}
  void Print(std::ostream &out) {out<<"Dummy";}
};

mafCxxTypeMacro(mafDummyObject);
*/

mafSmartObject *CreateSmartObject(int &flag)
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
  mafSmartObject *obj=CreateSmartObject(flag);

  MAF_TEST(flag);
  obj->UnRegister(0);
  MAF_TEST(!flag);

  
  mafAutoPointer<mafFooObject> auto_foo;
  CreateSmartObject2(auto_foo,flag);
  MAF_TEST(flag);
  // force releasing object.
  // Notice: calling the mafSmartPointer UnRegister() not the mafSmartObject one!!!
  auto_foo.UnRegister(NULL); 
  MAF_TEST(!flag);

  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
