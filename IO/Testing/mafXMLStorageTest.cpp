#include "mafObject.h"
#include "mafStorable.h"
#include "mafXMLStorage.h"

#include <iostream>

class mafStorableTestObject: public mafObject, public mafStorable
{
public:
  mafTypeMacro(mafStorableTestObject,mafObject);
  mafStorableTestObject() {}
  void Print(std::ostream &out) {out<<"Foo";}
};

mafCxxTypeMacro(mafStorableTestObject);

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

  mafStorableTestObject foo;
  mafDummyObject dummy;
  
  foo.Print(std::cerr);
  std::cout<<" = "<<foo.GetTypeName()<<std::endl;

  dummy.Print(std::cerr);
  std::cout<<" = "<<dummy.GetTypeName()<<std::endl;
  
  MAF_TEST(foo.IsA("mafStorableTestObject"));
  MAF_TEST(foo.IsA(mafObject::GetStaticTypeId()));
  MAF_TEST(foo.IsA("mafObject"));
  MAF_TEST(foo.IsA(mafObject::GetStaticTypeId()));
  MAF_TEST(!foo.IsA(dummy.GetTypeId()));
  MAF_TEST(!foo.IsA(dummy.GetTypeName()));
  MAF_TEST(!dummy.IsA(mafStorableTestObject::GetStaticTypeId()));
  MAF_TEST(!dummy.IsA(mafStorableTestObject::GetStaticTypeName()));
  MAF_TEST(dummy.GetStaticTypeId()==dummy.GetTypeId());
  MAF_TEST(dummy.GetStaticTypeId()==dummy.GetTypeId());

  mafObject *new_dummy=dummy.NewInstance();
  mafObject *new_foo=foo.NewInstance();

  MAF_TEST(new_dummy);
  MAF_TEST(new_foo);
  MAF_TEST(new_foo->IsA(foo.GetStaticTypeId()));
  MAF_TEST(new_foo->IsA(foo.GetStaticTypeName()));
  MAF_TEST(new_foo->IsA(mafStorableTestObject::GetStaticTypeId()));
  MAF_TEST(new_foo->IsA(mafStorableTestObject::GetStaticTypeName()));
  MAF_TEST(!new_foo->IsA(new_dummy->GetTypeId()));
  MAF_TEST(!new_foo->IsA(new_dummy->GetTypeName()));
  MAF_TEST(!new_dummy->IsA(new_foo->GetTypeId()));
  MAF_TEST(!new_dummy->IsA(new_foo->GetTypeName()));
  MAF_TEST(new_dummy->IsA(new_foo->GetStaticTypeId())); // they are both mafObject * variables
  MAF_TEST(new_dummy->IsA(new_foo->GetStaticTypeName()));

  MAF_TEST(mafStorableTestObject::SafeDownCast(new_dummy)==NULL);

  mafDummyObject* tmp_dummy = mafDummyObject::SafeDownCast(new_dummy);

  MAF_TEST(tmp_dummy!=NULL);

  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
