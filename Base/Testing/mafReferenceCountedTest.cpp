#include "mafReferenceCounted.h"
#include "mafSmartPointer.h"
#include <iostream>
#include <map>

/**
 This object set an external flag when allocated and reset it when deallocated,
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

/** 
  this creates an object, set its internal flag, increase the reference
  counter by 1 and returns the object pointer */
mafReferenceCounted *CreateSmartObject(int &flag)
{
  mafFooObject *foo= mafFooObject::New();
  foo->Flag=&flag;
  flag=true;
  foo->Register(0); // keep it alive
  return foo;
}

/** 
  this creates an object by means of mafSmartPointer, which self unregister when
  exiting the scope, sets the object's flag reference to the argument and then
  pass the object pointer to the auto-pointer passed as argument to keep the 
  object alive when exiting the function */
void CreateSmartObject2(mafAutoPointer<mafFooObject> &out,int &flag)
{
  // smart objects auto-dereference internal objects when exiting the scope
  mafSmartPointer<mafFooObject> smart_foo;
  smart_foo->Flag=&flag;
  flag=true;

  // set the pointer in the autopointer argument, this way the object
  // should stay alive also after exiting the function.
  out = smart_foo; 
}

int main()
{
  int flag; // the flag use to know when the object is deallocated

  // create an object on the Heap with reference count set to 1 and set
  // its flag reference to the "flag" variable.
  mafReferenceCounted *obj=CreateSmartObject(flag);

  MAF_TEST(flag);
  obj->UnRegister(0);
  MAF_TEST(!flag); // test if really deallocated

  // test if the auto-pointer keeps referenced the object  
  mafAutoPointer<mafFooObject> auto_foo;
  CreateSmartObject2(auto_foo,flag);
  MAF_TEST(flag); // the object is still alive

  // force releasing object.
  // Notice: calling the mafSmartPointer UnRegister() not the mafReferenceCounted one!!!
  auto_foo.UnRegister(NULL); 
  MAF_TEST(!flag);

  // test pointers exchanging between the smart pointers
  mafSmartPointer<mafFooObject> a,b,c;
  a=b;
  b=c;
  MAF_TEST(a->GetReferenceCount()==1);
  MAF_TEST(b->GetReferenceCount()==2);
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
