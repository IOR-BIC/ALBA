#include "mafString.h"
#include <iostream>

int main()
{
  mafString hello = "Hello World";
  mafString number(150.2);
  
  std::cout<<(const char *)hello<<std::endl;
  std::cout<<(const char *)number<<std::endl;

  MAF_TEST(hello.Length()==11);
  
  mafString italy = " and Italy";
  
  mafString tmp;
  tmp << hello << italy;

  MAF_TEST(tmp=="Hello World and Italy");
  MAF_TEST(tmp.EndsWith("Italy"));
  MAF_TEST(tmp.StartsWith("Hello"));
  MAF_TEST(tmp.FindFirst("and")==12);

  mafString tmp2 = "Ciao";
  tmp2 << " Bel" << " Mondo!";

  MAF_TEST(tmp2 == "Ciao Bel Mondo!");
  
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
