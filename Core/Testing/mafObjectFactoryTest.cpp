#include "mafObject.h"
#include "mafCoreFactory.h"
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
  mafCoreFactory *first_factory = mafCoreFactory::New();
  mafCoreFactory *second_factory = mafCoreFactory::New();

  MAF_TEST(first_factory!=NULL);
  MAF_TEST(second_factory!=NULL);

  // register factories
  mafObjectFactory::RegisterFactory(first_factory);
  mafObjectFactory::RegisterFactory(second_factory);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  MAF_TEST(list.size()==2);

  first_factory->RegisterOverride(mafFooObject::GetStaticTypeName(),mafFooObject::GetStaticTypeName(),"foo class",true,mafFooObject::NewObject);
  second_factory->RegisterOverride(mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName(),"foo class",true,mafDummyObject::NewObject);

  mafFooObject *foo=mafFooObject::SafeDownCast(mafObjectFactory::CreateInstance("mafFooObject"));
  mafDummyObject *dummy=mafDummyObject::SafeDownCast(mafObjectFactory::CreateInstance("mafDummyObject"));

  MAF_TEST(foo!=NULL);
  MAF_TEST(dummy!=NULL);
  
  foo->Print(std::cerr);
  std::cout<<" = "<<foo->GetTypeName()<<std::endl;

  dummy->Print(std::cerr);
  std::cout<<" = "<<dummy->GetTypeName()<<std::endl;
  
  MAF_TEST(foo->IsA("mafFooObject"));
  MAF_TEST(foo->IsA(mafObject::GetStaticTypeId()));
  MAF_TEST(foo->IsA("mafObject"));
  MAF_TEST(foo->IsA(mafObject::GetStaticTypeId()));
  MAF_TEST(!foo->IsA(dummy->GetTypeId()));
  MAF_TEST(!foo->IsA(dummy->GetTypeName()));
  MAF_TEST(!dummy->IsA(mafFooObject::GetStaticTypeId()));
  MAF_TEST(!dummy->IsA(mafFooObject::GetStaticTypeName()));
  MAF_TEST(dummy->GetStaticTypeId()==dummy->GetTypeId());
  MAF_TEST(dummy->GetStaticTypeId()==dummy->GetTypeId());
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
