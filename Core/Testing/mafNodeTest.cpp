#include "mafNode.h"
#include "mafNodeIterator.h"
#include <iostream>

class mafTestNode: public mafNode
{
public:
  mafTypeMacro(mafTestNode,mafNode);
  mafTestNode():Flag(NULL),m_ID(-1) {}
  ~mafTestNode() {if (Flag) *Flag=false;}
  bool *Flag;
  int m_ID;
};

mafCxxTypeMacro(mafTestNode);

class mafNodeB: public mafNode
{
public:
  mafTypeMacro(mafNodeB,mafNode);
  bool CanReparentTo(mafNode *parent) {return Superclass::CanReparentTo(parent)&&parent->IsA(mafNodeB::GetTypeId());}
};

mafCxxTypeMacro(mafNodeB);

class mafNodeA: public mafNode
{
public:
  mafTypeMacro(mafNodeA,mafNode);
  bool CanReparentTo(mafNode *parent) {return Superclass::CanReparentTo(parent)&&!parent->IsA(mafNodeB::GetTypeId());}
};

mafCxxTypeMacro(mafNodeA);

int main()
{
  mafSmartPointer<mafTestNode> root;
  mafSmartPointer<mafTestNode> first_node;
  mafTestNode *first_ptr=first_node;
  mafSmartPointer<mafTestNode> second_node;
  mafTestNode *second_ptr=second_node;
  mafSmartPointer<mafTestNode> third_node;
  mafTestNode *third_ptr=third_node;

  //
  // test tree reference counting
  //
  bool Flags[4]={true,true,true,true};
  root->Flag=&Flags[0];
  first_node->Flag=&Flags[1];
  second_node->Flag=&Flags[2];
  third_node->Flag=&Flags[3];

  root->AddChild(first_node);
  root->AddChild(second_node);
  first_node->AddChild(third_node);

  // unregister subnodes
  first_node=NULL;
  second_node=NULL;
  third_node=NULL;
    
  // test if they are still alive
  MAF_TEST(Flags[1]);
  MAF_TEST(Flags[2]);
  MAF_TEST(Flags[3]);

  // check reference count is still 1
  MAF_TEST(first_ptr->GetReferenceCount()==1);
  MAF_TEST(second_ptr->GetReferenceCount()==1);
  MAF_TEST(third_ptr->GetReferenceCount()==1);
  
  root->CleanTree();
  // test if they are dead
  MAF_TEST(!Flags[1]);
  MAF_TEST(!Flags[2]);
  MAF_TEST(!Flags[3]);

  //
  // test CanReparentTo
  //
  mafSmartPointer<mafNodeB> rootB;
  mafSmartPointer<mafNodeA> nodeA1;
  mafSmartPointer<mafNodeA> nodeA2;
  mafSmartPointer<mafNodeB> nodeB;
  
  MAF_TEST(root->AddChild(nodeA1)==MAF_OK);
  MAF_TEST(nodeA1->AddChild(nodeA2)==MAF_OK);
  MAF_TEST(root->AddChild(nodeB)==MAF_ERROR);
  MAF_TEST(rootB->AddChild(nodeB)==MAF_OK);
  MAF_TEST(nodeA2->ReparentTo(root)==MAF_OK);
  MAF_TEST(nodeA2->ReparentTo(rootB)==MAF_ERROR);
  
  root->CleanTree();

  //
  // test iterators
  //
  int seq[10]={4,9,2,8,7,3,1,5,0,6};

  mafTestNode *node=NULL;

  //
  // create a binary tree with ordered numbers
  //
  for (int i=0;i<10;i++)
  {
    for (node=(mafTestNode *)root;node;)
    {
      if (node->m_ID < 0)
      {
        node->m_ID = seq[i];
        break;
      }      else
      {
        if (node->GetNumberOfChildren()==0)
        {
            // add two children
            node->AddChild(mafTestNode::New());
            node->AddChild(mafTestNode::New());
        }

        int value=node->m_ID;
        if (value>=seq[i])
        {          
          node=(mafTestNode *)node->GetFirstChild();
          continue;
        }
        else
        {
          node=(mafTestNode *)node->GetLastChild();
          continue;
        }
      }
    }
  }
  
  MAF_TEST(root!=NULL);
  mafNodeIterator *iter=root->NewIterator();

  //
  // PreOrder Traverse
  //
  cerr<<"Testing mafNodeIterator PreOrder Traverse\n";  
  iter->SetTraversalModeToPreOrder();

  int preorder[10]={4,2,1,0,3,9,8,7,5,6};
  int vect[10];

  int j=0;
  for (node=(mafTestNode *)iter->GetFirstNode();node;node=(mafTestNode *)iter->GetNextNode())
  {
    if (node->m_ID>=0)
    {
      vect[j]=node->m_ID;
      MAF_TEST(vect[j]==preorder[j]);
      j++;
    }
  }

  //
  // PreOrder Traverse
  //
  cerr<<"Testing mafNodeIterator PreOrder Traverse in Reverse\n";
  iter->SetTraversalModeToPreOrder();

  int preorderrev[10]={6,5,7,8,9,3,0,1,2,4};

  j=0;
  for (node=(mafTestNode *)iter->GetLastNode();node;node=(mafTestNode *)iter->GetPreviousNode())
  {
    if (node->m_ID>=0)
    {
      vect[j]=node->m_ID;
      MAF_TEST(vect[j]==preorderrev[j]);
      j++;
    }
  }

  //
  // PostOrder Traverse Reverse
  //
  cerr<<"Testing mafNodeIterator PostOrder Traverse in Reverse\n";
  iter->SetTraversalModeToPostOrder();

  int postorderrev[10]={4,9,8,7,5,6,2,3,1,0};

  j=0;
  for (node=(mafTestNode *)iter->GetLastNode();node;node=(mafTestNode *)iter->GetPreviousNode())
  {
    if (node->m_ID>=0)
    {
      vect[j]=node->m_ID;
      MAF_TEST(vect[j]==postorderrev[j]);
      j++;
    }
  }

  //
  // PostOrder Traverse
  //
  cerr<<"Testing mafNodeIterator PostOrder Traverse\n";
  iter->SetTraversalModeToPostOrder();

  int postorder[10]={0,1,3,2,6,5,7,8,9,4};

  j=0;
  for (node=(mafTestNode *)iter->GetFirstNode();node;node=(mafTestNode *)iter->GetNextNode())
  {
    if (node->m_ID>=0)
    {
      vect[j]=node->m_ID;
      MAF_TEST(vect[j]==postorder[j]);
      j++;
    }
  }
  
  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
