#include "mafString.h"
#include <iostream>

int main()
{
  mafString hello = "Hello World";
  mafString number(150.2567890123456); // 16 digits precision
  
  std::cout<<(const char *)hello<<std::endl;
  std::cout<<(const char *)number<<std::endl;

  MAF_TEST(number=="150.2567890123456")
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
  MAF_TEST(tmp.GetCStr()!=copy.GetCStr());
  
  tmp.Erase(6,15);
  MAF_TEST(tmp=="Hello Italy");

  tmp.Erase(5);
  MAF_TEST(tmp=="Hello");

  tmp<<" everybody";
  tmp<<italy;
  tmp<<". Hello!";
  
  MAF_TEST(tmp.FindLast("Hello")==(tmp.Length()-mafString::Length("Hello!")));
  MAF_TEST(tmp[tmp.FindLast("Hello")]=='H');

  mafString num5=5;
  mafString num1=1;
  mafString num10=10;

  MAF_TEST(num1<num5);
  MAF_TEST(num5>num1);
  MAF_TEST(num5>num10); // alphabetical order, not numerical!
  
  mafString achar="a";
  MAF_TEST(achar<="a");
  MAF_TEST(achar>="a");

  const char *char_str="const string";
  mafString test_str(char_str);

  MAF_TEST(char_str==test_str.GetCStr());

  test_str << " not overwritten!";

  MAF_TEST(char_str!=test_str.GetCStr());
  MAF_TEST(test_str=="const string not overwritten!");

  mafString const_str=char_str;
  const_str.Erase(6,7);
  MAF_TEST(const_str.GetCStr()!=char_str);
  MAF_TEST(const_str=="const ring");

  std::cout<<"Test completed successfully!"<<std::endl;
  
  return 0;
}
