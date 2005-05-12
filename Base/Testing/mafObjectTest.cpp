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
  std::cout<<" = "<<foo.GetTypeName()<<std::endl;

  dummy.Print(std::cerr);
  std::cout<<" = "<<dummy.GetTypeName()<<std::endl;
  
  // test functions to query the type
  MAF_TEST(foo.IsA("mafFooObject")); // test the real object type with string type
  MAF_TEST(foo.IsA(mafObject::GetStaticTypeId())); // test the real object type with typeid: IsType == IsA(typeid())
  MAF_TEST(foo.IsA("mafObject"));
  MAF_TEST(foo.IsA(mafObject::GetStaticTypeId()));
  MAF_TEST(!foo.IsA(dummy.GetTypeId()));
  MAF_TEST(!foo.IsA(dummy.GetTypeName()));
  MAF_TEST(!dummy.IsA(mafFooObject::GetStaticTypeId()));
  MAF_TEST(!dummy.IsA(mafFooObject::GetStaticTypeId()));
  MAF_TEST(dummy.GetStaticTypeId()==dummy.GetTypeId());
  MAF_TEST(dummy.GetStaticTypeId()==dummy.GetTypeId());

  // use NewInstance to test new objects of the same type. NewInstance create
  // objects of the same "real" type.
  mafObject *new_dummy=dummy.NewInstance();
  mafObject *new_foo=foo.NewInstance();

  MAF_TEST(new_dummy);
  MAF_TEST(new_foo);
  MAF_TEST(new_foo->IsA(foo.GetStaticTypeId()));
  MAF_TEST(new_foo->IsA(foo.GetStaticTypeName()));
  MAF_TEST(new_foo->IsA(typeid(mafFooObject)));
  MAF_TEST(new_foo->IsA(typeid(mafFooObject)));
  MAF_TEST(!new_foo->IsA(new_dummy->GetTypeId()));
  MAF_TEST(!new_foo->IsA(new_dummy->GetTypeName()));
  MAF_TEST(!new_dummy->IsA(new_foo->GetTypeId()));
  MAF_TEST(!new_dummy->IsA(new_foo->GetTypeName()));
  MAF_TEST(new_dummy->IsA(new_foo->GetStaticTypeId())); // they are both mafObject * variables
  MAF_TEST(new_dummy->IsA(new_foo->GetStaticTypeName()));

  // test casting function
  MAF_TEST(mafFooObject::SafeDownCast(new_dummy)==NULL); // this cast should be failed

  mafDummyObject* tmp_dummy = mafDummyObject::SafeDownCast(new_dummy);
  MAF_TEST(tmp_dummy!=NULL); // this should have been casted successfully 

  new_foo->Delete(); // clean memory
  new_dummy->Delete(); // clean memory

  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
