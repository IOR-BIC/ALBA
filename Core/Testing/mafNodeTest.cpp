#include "mafNode.h"
#include <iostream>

class mafTestNode: public mafSmartObject
{
public:
  mafTypeMacro(mafTestNode,mafSmartObject);
  mafTestNode():Flag(NULL) {}
  ~mafTestNode() {if (Flag) *Flag=false;}
  bool *Flag;
};

mafCxxTypeMacro(mafTestNode);

int main()
{
  
  mafSmartPointer<mafTestNode> root;
  mafSmartPointer<mafTestNode> first_node;
  mafSmartPointer<mafTestNode> second_node;
  mafSmartPointer<mafTestNode> third_node;



  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
