#include "mafString.h"
#include <iostream>
int main()
{
  mafString hello = "Hello World";
  mafString number(150.2);
  
  std::cout<<hello<<std::endl;
  std::cout<<number<<std::endl;
  
  return 0;
}
