#include "mafEventSource.h"
#include "mafObserver.h"
#include "mafEventBase.h"

#include <iostream>

int main()
{
  

  MAF_TEST(hello.Length()==11);
  
  mafString italy = " and Italy";
  mafString tmp = hello + italy;
  
  MAF_TEST(tmp=="Hello World and Italy");
  MAF_TEST(tmp.EndsWith("Italy"));
  MAF_TEST(tmp.StartsWith("Hello"));
  MAF_TEST(tmp.FindFirst("and")==12);
  
  mafString copy=tmp;

  MAF_TEST(tmp==copy);
  
  tmp.Erase(6,15);
  MAF_TEST(tmp=="Hello Italy");

  tmp.Erase(5);
  MAF_TEST(tmp=="Hello");

  tmp<<" everybody";
  tmp<<italy;
  tmp<<". Hello!";
  
  MAF_TEST(tmp.FindLast("Hello")==(tmp.Length()-mafString::Length("Hello!")));
  MAF_TEST(tmp[tmp.FindLast("Hello")]=='H');
  
  std::cout<<"Test completed successfully!"<<std::endl;
  
  return 0;
}
