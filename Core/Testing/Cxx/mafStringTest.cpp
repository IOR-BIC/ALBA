#include "mafString.h"
#include <iostream>
int main()
{
  mafString hello = "Hello World";
  mafString number(150.2);
  
  std::cout<<(const char *)hello<<std::endl;
  std::cout<<(const char *)number<<std::endl;
  
  return 0;
}
