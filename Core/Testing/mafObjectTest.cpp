#include "mafObject.h"
#include <iostream>

class mafFooObject: public mafObject
{
public:
  mafTypeMacro(mafFooObject,mafObject);
  mafFooObject() {}
  void Print(std::ostream &out) {out<<"Foo";}
};

mafCxxTypeMacro(mafFooObject);

class mafDummyObject: public mafObject
{
public:
  mafTypeMacro(mafDummyObject,mafObject);
  mafDummyObject() {}
  void Print(std::ostream &out) {out<<"Dummy";}
};

mafCxxTypeMacro(mafDummyObject);

int main()
{

  mafFooObject foo;
  mafDummyObject dummy;
  
  foo.Print(std::cerr);
  std::cout<<" = "<<foo.GetClassName()<<std::endl;

  dummy.Print(std::cerr);
  std::cout<<" = "<<dummy.GetClassName()<<std::endl;
  
  MAF_TEST(foo.IsA("mafFooObject"));
  MAF_TEST(foo.IsA(mafObject::GetTypeId()));
  MAF_TEST(foo.IsA("mafObject"));
  MAF_TEST(foo.IsA(mafObject::GetTypeId()));
  MAF_TEST(!foo.IsA(dummy.GetClassId()));
  MAF_TEST(!foo.IsA(dummy.GetClassName()));
  MAF_TEST(!dummy.IsA(mafFooObject::GetTypeId()));
  MAF_TEST(!dummy.IsA(mafFooObject::GetTypeName()));
  MAF_TEST(dummy.GetTypeId()==dummy.GetClassId());
  MAF_TEST(dummy.GetTypeId()==dummy.GetClassId());
  MAF_TEST(mafObject::GetTypeId("mafDummyObject")==dummy.GetClassId());

  mafObject *new_dummy=dummy.NewInstance();
  mafObject *new_foo=foo.NewInstance();

  MAF_TEST(new_dummy);
  MAF_TEST(new_foo);
  MAF_TEST(new_foo->IsA(foo.GetTypeId()));
  MAF_TEST(new_foo->IsA(foo.GetTypeName()));
  MAF_TEST(new_foo->IsA(mafFooObject::GetTypeId()));
  MAF_TEST(new_foo->IsA(mafFooObject::GetTypeName()));
  MAF_TEST(!new_foo->IsA(new_dummy->GetClassId()));
  MAF_TEST(!new_foo->IsA(new_dummy->GetClassName()));
  MAF_TEST(!new_dummy->IsA(new_foo->GetClassId()));
  MAF_TEST(!new_dummy->IsA(new_foo->GetClassName()));
  MAF_TEST(new_dummy->IsA(new_foo->GetTypeId())); // they are both mafObject * variables
  MAF_TEST(new_dummy->IsA(new_foo->GetTypeName()));

  MAF_TEST(mafFooObject::SafeDownCast(new_dummy)==NULL);

  mafDummyObject* tmp_dummy = mafDummyObject::SafeDownCast(new_dummy);

  MAF_TEST(tmp_dummy!=NULL);

  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
