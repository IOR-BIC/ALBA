#include "mafVector.txx"
#include "mafString.h"

#include <iostream>

int main()
{ 
  // Test the Vector with strings
  mafVector<mafString> sarray;
  sarray.Push("Ciao");
  sarray.Push("Hello");
  sarray.Push("Holà");
  
  MAF_TEST(sarray.GetNumberOfItems()==3);
  MAF_TEST(sarray[1]=="Hello");
  sarray[1]="Dasvidania";
  MAF_TEST(sarray[1]=="Dasvidania");
  
  mafString popped;
  MAF_TEST(sarray.Pop(popped));
  MAF_TEST(popped=="Holà");
  MAF_TEST(sarray.GetNumberOfItems()==2);

  // Test with an array of smart pointers

  std::cout<<"Test completed successfully!"<<std::endl;
  
  return 0;
}
