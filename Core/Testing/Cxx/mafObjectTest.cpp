#include "mafObject.h"
#include <iostream>

class mafFooObject: public mafObject
{
public:
  mafTypeMacro(mafFooObject,mafObject);

  void Print(std::ostream out) {out<<"Foo";}
};

mafCxxTypeMacro(mafFooObject);

class mafDummyObject: public mafObject
{
public:
  mafTypeMacro(mafDummyObject,mafObject);

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
  
  if (!foo.IsA("mafFooObject"))
    return MAF_ERROR;

  if (!foo.IsA(mafObject::GetTypeId()))
    return MAF_ERROR;

  if (!foo.IsA("mafObject"))
    return MAF_ERROR;

  if (!foo.IsA(mafObject::GetTypeId()))
    return MAF_ERROR;

  if (foo.IsA(dummy.GetClassTypeId()))
    return MAF_ERROR;

  if (foo.IsA(dummy.GetClassName()))
    return MAF_ERROR;

  if (dummy.IsA(mafFooObject::GetTypeId()))
    return MAF_ERROR;

  if (dummy.IsA(mafFooObject::GetTypeName()))
    return MAF_ERROR;

  if (dummy.GetTypeId()!=dummy.GetClassTypeId())
    return MAF_ERROR;

  if (dummy.GetTypeId()!=dummy.GetClassTypeId())
    return MAF_ERROR;

  if (mafObject::GetClassTypeId("mafDummyObject")!=dummy.GetClassTypeId())
    return MAF_ERROR;

  mafObject *new_dummy=dummy.NewInstance();
  mafObject *new_foo=foo.NewInstance();

  if (!new_foo->IsA(foo.GetTypeId()))
    return MAF_ERROR;

  if (!new_foo->IsA(foo.GetTypeName()))
    return MAF_ERROR;

  if (!new_foo->IsA(mafFooObject::GetTypeId()))
    return MAF_ERROR;

  if (!new_foo->IsA(mafFooObject::GetTypeName()))
    return MAF_ERROR;

  if (new_foo->IsA(new_dummy->GetClassTypeId()))
    return MAF_ERROR;

  if (new_foo->IsA(new_dummy->GetClassName()))
    return MAF_ERROR;

  if (!new_dummy->IsA(new_foo->GetTypeId()))
    return MAF_ERROR;

  if (!new_dummy->IsA(new_foo->GetTypeName()))
    return MAF_ERROR;

  if (mafFooObject::SafeDownCast(new_dummy))
    return MAF_ERROR;

  mafDummyObject* tmp_dummy = mafDummyObject::SafeDownCast(new_dummy);
  if (tmp_dummy==NULL)
    return MAF_ERROR;
  
  std::cout<<"Test completed successfully!"<<std::endl;
  return MAF_OK;
}
