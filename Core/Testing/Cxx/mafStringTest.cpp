#include "mafString.h"
#include <iostream>

#define ASSERT(a) if (!(a)) \
{ \
  std::cerr << "Test failed at line " \
  << __LINE__ << " : " << #a << std::endl; \
  return MAF_ERROR; \
}

int main()
{
  mafString hello = "Hello World";
  mafString number(150.2);
  
  std::cout<<(const char *)hello<<std::endl;
  std::cout<<(const char *)number<<std::endl;

  ASSERT(hello.Length()==11);
  
  mafString italy = " and Italy";
  mafString tmp = hello + italy;
  
  ASSERT(tmp=="Hello World and Italy");
  ASSERT(tmp.EndsWith("Italy"));
  ASSERT(tmp.StartsWith("Hello"));
  ASSERT(tmp.FindFirst("and")==12);
  
  mafString copy=tmp;

  ASSERT(tmp==copy);
  
  tmp.Erase(6,15);
  ASSERT(tmp=="Hello Italy");

  tmp.Erase(5);
  ASSERT(tmp=="Hello");

  tmp<<" everybody";
  tmp<<italy;
  tmp<<". Hello!";
  
  ASSERT(tmp.FindLast("Hello")==(tmp.Length()-mafString::Length("Hello!")));
  ASSERT(tmp[tmp.FindLast("Hello")]=='H');
  
  return 0;
}
