#include "mafString.h"
#include <iostream>

int main()
{
  // create a text string and a numerical string
  mafString hello = "Hello World";
  mafString number(150.2567890123456); // 16 digits precision
  
  // streaming towards standard output
  std::cout<<hello.GetCStr()<<std::endl;
  std::cout<<number.GetCStr()<<std::endl;

  // test the contents and the length of the numerical string
  MAF_TEST(number=="150.2567890123456")
  MAF_TEST(hello.Length()==11);
  
  // try the concatenation of mafString's
  mafString italy = " and Italy";
  
  // this concatenates two strings to the 'tmp' string
  mafString tmp;
  tmp << hello << italy;

  MAF_TEST(tmp=="Hello World and Italy");

  // test query functions
  MAF_TEST(tmp.EndsWith("Italy"));
  MAF_TEST(tmp.StartsWith("Hello"));
  MAF_TEST(tmp.FindFirst("and")==12);

  // test concatenation of (const char *)
  mafString tmp2 = "Ciao";
  tmp2 << " Bel" << " Mondo!";

  MAF_TEST(tmp2 == "Ciao Bel Mondo!");
  
  // test copy  feature
  mafString copy=tmp;

  MAF_TEST(tmp==copy);
  MAF_TEST(tmp.GetCStr()!=copy.GetCStr());
  
  // test erase feature
  tmp.Erase(6,15);
  MAF_TEST(tmp=="Hello Italy");

  tmp.Erase(5);
  MAF_TEST(tmp=="Hello");

  // test the FindLast function to find last occurance of the "Hello" string.
  tmp<<" everybody";
  tmp<<italy;
  tmp<<". Hello!";
  
  MAF_TEST(tmp.FindLast("Hello")==(tmp.Length()-mafString::Length("Hello!")));
  
  // test single character accessor 
  MAF_TEST(tmp[tmp.FindLast("Hello")]=='H');

  // test alphabetical comparison
  mafString num5=5;
  mafString num1=1;
  mafString num10=10;

  MAF_TEST(num1<num5);
  MAF_TEST(num5>num1);
  MAF_TEST(num5>num10); // alphabetical order, not numerical!
  
  // some extra comparison. Notice the "a" string is not copied!
  mafCString achar="a";
  MAF_TEST(achar<="a");
  MAF_TEST(achar>="a");

  // test the referencing mechansim of mafString. Notice mafCString is
  // equivalent to mafString::Set() 
  const char *char_str="const string";
  mafCString test_str(char_str);

  MAF_TEST(char_str==test_str.GetCStr()); // No memory copy!

  test_str << " not overwritten!"; 
  
  // trying to modify a const string makes the string to be automatically
  // copied and the final string memory is different.
  MAF_TEST(char_str!=test_str.GetCStr());
  MAF_TEST(test_str=="const string not overwritten!");

  // test with another kind of modification
  mafCString const_str(char_str);
  const_str.Erase(6,7);
  MAF_TEST(const_str.GetCStr()!=char_str);
  MAF_TEST(const_str=="const ring"); // test contents

  std::cout<<"Test completed successfully!"<<std::endl;
  
  return 0;
}
