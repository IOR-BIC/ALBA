#include "mafObject.h"
#include "mafObjectFactory.h"
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

  // a couple of factories
  mafObjectFactory *first_factory = mafObjectFactory::New();
  mafObjectFactory *second_factory = mafObjectFactory::New();

  MAF_TEST(first_factory!=NULL);
  MAF_TEST(second_factory!=NULL);

  // register factories
  mafObjectFactory::RegisterFactory(first_factory);
  mafObjectFactory::RegisterFactory(second_factory);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  MAF_TEST(list.size()==2);

  first_factory->RegisterOverride(mafFooObject::GetTypeName(),mafFooObject::GetTypeName(),"foo class",true,mafFooObject::NewObject);
  second_factory->RegisterOverride(mafDummyObject::GetTypeName(),mafDummyObject::GetTypeName(),"foo class",true,mafDummyObject::NewObject);

  mafFooObject *foo=mafFooObject::SafeDownCast(mafObjectFactory::CreateInstance("mafFooObject"));
  mafDummyObject *dummy=mafDummyObject::SafeDownCast(mafObjectFactory::CreateInstance("mafDummyObject"));

  MAF_TEST(foo!=NULL);
  MAF_TEST(dummy!=NULL);
  
  foo->Print(std::cerr);
  std::cout<<" = "<<foo->GetClassName()<<std::endl;

  dummy->Print(std::cerr);
  std::cout<<" = "<<dummy->GetClassName()<<std::endl;
  
  MAF_TEST(foo->IsA("mafFooObject"));
  MAF_TEST(foo->IsA(mafObject::GetTypeId()));
  MAF_TEST(foo->IsA("mafObject"));
  MAF_TEST(foo->IsA(mafObject::GetTypeId()));
  MAF_TEST(!foo->IsA(dummy->GetClassId()));
  MAF_TEST(!foo->IsA(dummy->GetClassName()));
  MAF_TEST(!dummy->IsA(mafFooObject::GetTypeId()));
  MAF_TEST(!dummy->IsA(mafFooObject::GetTypeName()));
  MAF_TEST(dummy->GetTypeId()==dummy->GetClassId());
  MAF_TEST(dummy->GetTypeId()==dummy->GetClassId());
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
