#include "mafNode.h"
#include "mafNodeRoot.h"
#include "mafNodeIterator.h"
#include "mafTagArray.h"
#include "mafXMLStorage.h"
#include "mafCoreFactory.h"
#include <iostream>

//-------------------------------------------------------------------------
class mafTestNode: public mafNode
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafTestNode,mafNode);
  mafTestNode():Flag(NULL),m_Value(-1) {}
  ~mafTestNode() {if (Flag) *Flag=false;}
  bool *Flag;
  int m_Value;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafTestNode)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** class for testing root mafNodeRoot. */
class mafTestRootNode: public mafNodeRoot, public mafTestNode
//-------------------------------------------------------------------------
{
public:
  mafTestRootNode() {SetId(0);}
  mafTypeMacro(mafTestRootNode,mafTestNode);
protected:
  inline int InternalStore(mafStorageElement *parent);
  inline int InternalRestore(mafStorageElement *element);
};

//-------------------------------------------------------------------------
inline int mafTestRootNode::InternalRestore(mafStorageElement *element)
//-------------------------------------------------------------------------
{
  if (mafNodeRoot::RestoreRoot(element)==MAF_OK && \
      Superclass::InternalRestore(element)==MAF_OK)
    return MAF_OK;

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
inline int mafTestRootNode::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  mafNodeRoot::StoreRoot(parent);
  Superclass::InternalStore(parent);
  return MAF_OK;
}

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafTestRootNode)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** class for testing reparenting. */
class mafNodeB: public mafNode
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafNodeB,mafNode);
  bool CanReparentTo(mafNode *parent) {return Superclass::CanReparentTo(parent)&&parent->IsA(mafNodeB::GetStaticTypeId());}
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeB)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** class for testing reparenting. */
class mafNodeA: public mafNode
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafNodeA,mafNode);
  bool CanReparentTo(mafNode *parent) {return Superclass::CanReparentTo(parent)&&!parent->IsA(mafNodeB::GetStaticTypeId());}
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeA);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafTestRootNode> root;
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
      if (node->m_Value < 0)
      {
        node->m_Value = seq[i];
        node->SetName(mafString(node->m_Value));
        break;
      }
      else
      {
        if (node->GetNumberOfChildren()==0)
        {
            // add two children
            node->AddChild(mafTestNode::New());
            node->AddChild(mafTestNode::New());
        }

        int value=node->m_Value;
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
  std::cerr<<"Testing mafNodeIterator PreOrder Traverse\n";  
  iter->SetTraversalModeToPreOrder();

  int preorder[10]={4,2,1,0,3,9,8,7,5,6};
  int vect[10];

  int j=0;
  for (node=(mafTestNode *)iter->GetFirstNode();node;node=(mafTestNode *)iter->GetNextNode())
  {
    if (node->m_Value>=0)
    {
      vect[j]=node->m_Value;
      MAF_TEST(vect[j]==preorder[j]);
      j++;
    }
  }

  //
  // PreOrder Reverse Traverse
  //
  std::cerr<<"Testing mafNodeIterator PreOrder Traverse in Reverse\n";
  iter->SetTraversalModeToPreOrder();

  int preorderrev[10]={6,5,7,8,9,3,0,1,2,4};

  j=0;
  for (node=(mafTestNode *)iter->GetLastNode();node;node=(mafTestNode *)iter->GetPreviousNode())
  {
    if (node->m_Value>=0)
    {
      vect[j]=node->m_Value;
      MAF_TEST(vect[j]==preorderrev[j]);
      j++;
    }
  }
  
  //
  // PostOrder Reverse Traverse
  //
  std::cerr<<"Testing mafNodeIterator PostOrder Traverse in Reverse\n";
  iter->SetTraversalModeToPostOrder();

  int postorderrev[10]={4,9,8,7,5,6,2,3,1,0};

  j=0;
  for (node=(mafTestNode *)iter->GetLastNode();node;node=(mafTestNode *)iter->GetPreviousNode())
  {
    if (node->m_Value>=0)
    {
      vect[j]=node->m_Value;
      MAF_TEST(vect[j]==postorderrev[j]);
      j++;
    }
  }

  //
  // PostOrder Traverse
  //
  std::cerr<<"Testing mafNodeIterator PostOrder Traverse\n";
  iter->SetTraversalModeToPostOrder();

  int postorder[10]={0,1,3,2,6,5,7,8,9,4};

  j=0;
  for (node=(mafTestNode *)iter->GetFirstNode();node;node=(mafTestNode *)iter->GetNextNode())
  {
    if (node->m_Value>=0)
    {
      vect[j]=node->m_Value;
      MAF_TEST(vect[j]==postorder[j]);
      j++;
    }
  }

  // find nodes by name
  mafNode *node1=root->FindInTreeByName("5");
  mafNode *node2=root->FindInTreeByName("7");
  mafNode *node3=root->FindInTreeByName("2");

  MAF_TEST(node1);
  MAF_TEST(node2);
  MAF_TEST(node3);

  // a node to test detach from tree
  mafSmartPointer<mafTestNode> test_node;
  test_node->SetName("test_node");
  node2->AddChild(test_node);

  // set some links in the tree
  node1->SetLink("link1",node2);
  node1->SetLink("link2",node3);
  node1->SetLink("link3",test_node);
  
  MAF_TEST(node1->GetLink("link1")==node2);
  MAF_TEST(node1->GetLink("link2")==node3);
  MAF_TEST(node1->GetLink("link3")==test_node.GetPointer());

  // test detaching from tree
  test_node->ReparentTo(NULL);
  MAF_TEST(node1->GetLink("link3")==NULL);
  
  // test attaching to another tree
  test_node->ReparentTo(rootB);
  MAF_TEST(node1->GetLink("link3")==NULL);

  // test attaching to tree
  test_node->ReparentTo(node3);
  MAF_TEST(node1->GetLink("link3")==test_node.GetPointer());
  
  // add some tags
  root->GetTagArray()->SetTag(mmuTagItem("NumericTag",10.5));
  root->GetTagArray()->SetTag(mmuTagItem("StringTag","Donald"));
  node3->GetTagArray()->SetTag(mmuTagItem("TestTag","test value"));
  node2->GetTagArray()->SetTag(mmuTagItem("TestTag","second value"));

  // dump root node with tags
  std::cout<<"Root node with tags:\n";
  root->Print(std::cout);

  // test finding by TAG
  mafNode *tagged_node=root->FindInTreeByTag("TestTag","test value");
  MAF_TEST(tagged_node==node3);

  // test tree copy
  mafNode *root_copy=root->MakeCopy();
  root_copy->IsA(mafTestRootNode::GetStaticTypeId());

  MAF_TEST(root->Equals(root_copy));

  // plug nodes to factory for being able to restore
  mafCoreFactory::Initialize();
  mafPlugObject<mafTestNode>("Test Node");
  mafPlugObject<mafTagArray>("Test Node");

  // test storing/restoring...
  mafXMLStorage storage;
  storage.SetURL("testNode.xml");
  storage.SetFileType("MSF");
  storage.SetVersion("2.0");
  
  storage.SetRoot(root);
  int ret=storage.Store();
  
  MAF_TEST(ret==MAF_OK);

  mafXMLStorage restore;
  restore.SetURL("testNode.xml");
  restore.SetFileType("MSF");
  restore.SetVersion("2.0");
  
  mafSmartPointer<mafTestRootNode> new_root;
  restore.SetRoot(new_root);
  ret=restore.Restore();

  MAF_TEST(ret==MAF_OK);

  // initialized the tree to resolve links
  ret=new_root->Initialize();

  MAF_TEST(ret==MAF_OK);

  MAF_TEST(new_root->CompareTree(root));

  // test restored links
  mafNode *new_node1=new_root->FindInTreeByName("5");
  MAF_TEST(new_node1);

  // display a dump of restored node
  std::cout << "Restored node with Links:\n";
  new_node1->Print(std::cout);

  mafNode *new_node2=new_node1->GetLink("link1");
  mafNode *new_node3=new_node1->GetLink("link2");
  mafNode *new_test_node=new_node1->GetLink("link3");
  MAF_TEST(mafCString("7")==new_node2->GetName());
  MAF_TEST(mafCString("2")==new_node3->GetName());
  MAF_TEST(mafCString("test_node")==new_test_node->GetName())

  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
