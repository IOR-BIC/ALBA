#include "mafObject.h"
#include <iostream>

#define ASSERT(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return MAF_ERROR; \
}

class mafFooObject: public mafObject
{
public:
  mafTypeMacro(mafFooObject,mafObject);
  mafFooObject() {}
  void Print(std::ostream out) {out<<"Foo";}
};

mafCxxTypeMacro(mafFooObject);

class mafDummyObject: public mafObject
{
public:
  mafTypeMacro(mafDummyObject,mafObject);
  mafDummyObject() {}
  void Print(std::ostream out) {out<<"Dummy";}
};

mafCxxTypeMacro(mafDummyObject);

int main()
{

  mafFooObject foo;
  mafDummyObject dummy;
  
  foo.Print(std::cout);
  std::cout<<" = "<<foo.GetClassName()<<std::endl;

  dummy.Print(std::cout);
  std::cout<<" = "<<dummy.GetClassName()<<std::endl;
  
  ASSERT(foo.IsA("mafFooObject"));
  ASSERT(foo.IsA(mafObject::GetTypeId()));
  ASSERT(foo.IsA("mafObject"));
  ASSERT(foo.IsA(mafObject::GetTypeId()));
  ASSERT(!foo.IsA(dummy.GetClassId()));
  ASSERT(!foo.IsA(dummy.GetClassName()));
  ASSERT(!dummy.IsA(mafFooObject::GetTypeId()));
  ASSERT(!dummy.IsA(mafFooObject::GetTypeName()));
  ASSERT(dummy.GetTypeId()==dummy.GetClassId());
  ASSERT(dummy.GetTypeId()==dummy.GetClassId());
  ASSERT(mafObject::GetTypeId("mafDummyObject")==dummy.GetClassId());

  mafObject *new_dummy=dummy.NewInstance();
  mafObject *new_foo=foo.NewInstance();

  ASSERT(new_dummy);
  ASSERT(new_foo);
  ASSERT(new_foo->IsA(foo.GetTypeId()));
  ASSERT(new_foo->IsA(foo.GetTypeName()));
  ASSERT(new_foo->IsA(mafFooObject::GetTypeId()));
  ASSERT(new_foo->IsA(mafFooObject::GetTypeName()));
  ASSERT(!new_foo->IsA(new_dummy->GetClassId()));
  ASSERT(!new_foo->IsA(new_dummy->GetClassName()));
  ASSERT(!new_dummy->IsA(new_foo->GetClassId()));
  ASSERT(!new_dummy->IsA(new_foo->GetClassName()));
  ASSERT(new_dummy->IsA(new_foo->GetTypeId())); // they are both mafObject * variables
  ASSERT(new_dummy->IsA(new_foo->GetTypeName()));

  ASSERT(mafFooObject::SafeDownCast(new_dummy)==NULL);

  mafDummyObject* tmp_dummy = mafDummyObject::SafeDownCast(new_dummy);

  ASSERT(tmp_dummy!=NULL);

  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
