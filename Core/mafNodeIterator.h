/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeIterator.h,v $
  Language:  C++
  Date:      $Date: 2004-11-30 18:18:21 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafNodeIterator_h
#define __mafNodeIterator_h

#include "mafObject.h"
#include "mafNode.h"
#include "mafStack.h"

/** mafNodeIterator - an m-way tree data structure iterator
  mafNodeIterator is a class to traverse a tree data structure. It allows 
  to set the traverse modality. The iterator allows to set some callback to the 
  traverse by means of the Subject/Observer mechanism of the mafObject. Recognized
  observers' events are: "PreTraversal","PostTraversal","Deeper","Upper","FirstNode",
  "LastNode", "Done".
  These can be overidden by adding an observer
  
  @sa mafNode mafObject
  */

class MAF_EXPORT mafNodeIterator : public mafObject
{
public:
  mafTypeMacro(mafNodeIterator,mafObject);
  
  /** Retrieve the current node pointer data from the iterator. */
  mafNode * GetCurrentNode() {return m_CurrentNode;}

  /**
  Shortcuts to traverse the tree*/
  mafNode * GetFirstNode() {this->GoToFirstNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}
  mafNode * GetLastNode() {this->GoToLastNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}
  mafNode * GetNextNode() {this->GoToNextNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}
  mafNode * GetPreviousNode() {this->GoToPreviousNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}

  /**
  Set the root node of the (sub)tree to be traversed. Used to set the start 
  point. */
  void SetRootNode(mafNode *root);
  
  /**
    Initialize the traversal of the container. 
    Set the iterator to the "beginning" of the container.*/
  void InitTraversal();
  
  /**
    Check if the iterator is at the end of the container. Returns 1
    for yes and 0 for no.*/
  int IsDoneWithTraversal() {return m_TraversalDone;}
  
  /** Increment the iterator to the next location.*/
  virtual int GoToNextNode();
  
  /** Decrement the iterator to the next location.*/
  virtual int GoToPreviousNode();
  
  /** Go to the first item of the list.*/
  virtual int GoToFirstNode();
  
  /** Go to the last item of the list.*/
  virtual int GoToLastNode();

  /**
    Set/Get the traverse modality. Default to PreOrder, i.e. process 
    first the root an then the sub trees (left to right).
    Change the traverse mode restart the traversing!*/
  void SetTraversalMode(int mode);
  int GetTraversalMode() { return m_TraversalMode;}

  /**
    Set the traverse modality to PreOrder: first the root then the subtrees
    left to right*/
  void SetTraversalModeToPreOrder() {this->SetTraversalMode(PreOrder);}
  /**
    Set the traverse modality to PostOrder: first the the subtrees
    left to right, then the root.*/
  void SetTraversalModeToPostOrder() {this->SetTraversalMode(PostOrder);}

  /**
    Set the traverse modality to InOrder: first the the left subtrees
    (left to right), then right subtrees (left to right), and finally the root.
    The partioning of the sub-trees is dicotomic. This mode is useful for
    B-Tree's traversing*/
  //void SetTraversalModeToInOrder() {this->SetTraversalMode(InOrder);}
  
  enum traversalMode {PreOrder=0,PostOrder};
  enum callbacks {PreTraversal=100,PostTraversal,Deeper, Upper,FirstNode,LastNode, Done};
protected:

  /**
  Find the left most leaf of the tree*/
  mafNode *FindLeftMostLeaf(mafNode *node);

  /**
  Find the right most leaf of the tree*/
  mafNode *FindRightMostLeaf(mafNode *node);

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command.
    Executed before traversing a node. This function pass the current
    node pointer to the callback function.*/
  virtual PreExecute(); 

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command.
    Executed after traversing a node. This function pass the current
     node pointer to the callback function.*/
  virtual PostExecute();  

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command.
    Executed when going down in the tree.. This function pass to the
    callback function the pointer to the node is being processed
    while changing the depth in the tree. Not necessary it is the
    current node or next node to be traversed.*/
  virtual DeeperExecute(mafNode *); 

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command. 
    Executed when going up in the tree. This function pass to the
    callback function the pointer to the node is being processed
    while changing the depth in the tree. Not necessary it is the
    current node or next node to be traversed.*/
  virtual UpperExecute(mafNode *);  

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command. 
    executed when GoToFirstNode is executed. This function pass the
    the pointer to this iterator to the callback function.*/
  virtual FirstExecute(); 

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command.
    executed when last node is traversed. This function pass the
    the pointer to this iterator to the callback function.*/
  virtual LastExecute();  

  /**
    Callback function. By default these functions look for the corresponding
    observer to be present and execute the associated command.
    executed when the container is exhausted, i.e. when IsDoneWithTraversal
    return "true". This function pass the the pointer to this iterator to 
    the callback function.*/
  virtual DoneExecute();  

  //mafSetObjectMacro(CurrentNode,mafNode);
  //mafSetObjectMacro(CurrentIdx,mafStack<mafIdType>);

  mafNodeIterator(mafNode *root=NULL);
  virtual ~mafNodeIterator();

  mafNode         *m_RootNode;
  mafNode         *m_CurrentNode;
  int             m_TraversalMode;
  int             m_TraversalDone;

  mafTemplatedVector<mafID> m_CurrentIdx;

private:
  mafNodeIterator(const mafNodeIterator&) {} // Not implemented
  void operator=(const mafNodeIterator&) {} // Not implemented
};

#endif
