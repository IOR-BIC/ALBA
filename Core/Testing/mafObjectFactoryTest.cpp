#include "mafObject.h"
#include "mafCoreFactory.h"
#include <iostream>

/** a mafObject to be placed in the factory. */
class mafFooObject: public mafObject
{
public:
  mafTypeMacro(mafFooObject,mafObject);
  mafFooObject() {}
  void Print(std::ostream &out) {out<<"Foo";}
};

mafCxxTypeMacro(mafFooObject);

/** a mafObject to be placed in the factory. */
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

  // register objects to the factories
  first_factory->RegisterOverride(mafFooObject::GetStaticTypeName(),mafFooObject::GetStaticTypeName(),"foo class",true,mafFooObject::NewObject);
  second_factory->RegisterOverride(mafDummyObject::GetStaticTypeName(),mafDummyObject::GetStaticTypeName(),"foo class",true,mafDummyObject::NewObject);

  // create objects from the factory
  mafFooObject *foo=mafFooObject::SafeDownCast(mafObjectFactory::CreateInstance("mafFooObject"));
  mafDummyObject *dummy=mafDummyObject::SafeDownCast(mafObjectFactory::CreateInstance("mafDummyObject"));

  MAF_TEST(foo!=NULL);
  MAF_TEST(dummy!=NULL);
  
  foo->Print(std::cerr);
  std::cout<<" = "<<foo->GetTypeName()<<std::endl;

  dummy->Print(std::cerr);
  std::cout<<" = "<<dummy->GetTypeName()<<std::endl;
  
  // test newly created objects types
  MAF_TEST(foo->IsA("mafFooObject"));
  MAF_TEST(foo->IsA(typeid(mafObject)));
  MAF_TEST(foo->IsA("mafObject"));
  MAF_TEST(foo->IsMAFType(mafObject));
  MAF_TEST(!foo->IsA(dummy->GetTypeId()));
  MAF_TEST(!foo->IsA(dummy->GetTypeName()));
  MAF_TEST(!dummy->IsMAFType(mafFooObject)); // test through static type id (equivalent to typeid())
  MAF_TEST(!dummy->IsA(mafFooObject::GetStaticTypeName())); // test through static string type 
  MAF_TEST(dummy->GetStaticTypeId()==dummy->GetTypeId()); 
  MAF_TEST(dummy->GetStaticTypeId()==dummy->GetTypeId());

  foo->Delete();
  dummy->Delete();
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
