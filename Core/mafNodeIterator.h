/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeIterator.h,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:15:02 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __vtkTreeIterator_h
#define __vtkTreeIterator_h

#include "vtkObject.h"
#include "mflCoreWin32Header.h"
#include "vtkTree.h"
//#include "vtkStack.h"

/** vtkTreeIterator - an m-way tree data structure iterator
  vtkTreeIterator is a class to traverse a tree data structure. It allows 
  to set the traverse modality. The iterator allows to set some callback to the 
  traverse by means of the Subject/Observer mechanism of the vtkObject. Recognized
  observers' events are: "PreTraversal","PostTraversal","Deeper","Upper","FirstNode",
  "LastNode", "Done".
  These can be overidden by adding an observer
  
  @sa vtkTree vtkObject vtkCommand vtkCallbackCommand 
  */
template <class DType>
class vtkStack;

class MFL_CORE_EXPORT vtkTreeIterator : public vtkObject
{
public:
  vtkTypeMacro(vtkTreeIterator,vtkObject);
  static vtkTreeIterator *New(); 
  
  /**
  Retrieve the current node pointer data from the iterator. */
  vtkTree * GetCurrentNode() {return this->CurrentNode;}

  /**
  Shortcuts to traverse the tree*/
  vtkTree * GetFirstNode() {this->GoToFirstNode(); return (this->TraversalDone)?NULL:this->GetCurrentNode();}
  vtkTree * GetLastNode() {this->GoToLastNode(); return (this->TraversalDone)?NULL:this->GetCurrentNode();}
  vtkTree * GetNextNode() {this->GoToNextNode(); return (this->TraversalDone)?NULL:this->GetCurrentNode();}
  vtkTree * GetPreviousNode() {this->GoToPreviousNode(); return (this->TraversalDone)?NULL:this->GetCurrentNode();}

  /**
  Set the root node of the (sub)tree to be traversed. Used to set the start 
  point. */
  void SetRootNode(vtkTree *root);
  
  /**
  Initialize the traversal of the container. 
  Set the iterator to the "beginning" of the container.*/
  void InitTraversal();
  
  /**
  Check if the iterator is at the end of the container. Returns 1
  for yes and 0 for no.*/
  int IsDoneWithTraversal() {return this->TraversalDone;}
  
  /**
  Increment the iterator to the next location.*/
  virtual int GoToNextNode();
  
  /**
  Decrement the iterator to the next location.*/
  virtual int GoToPreviousNode();
  
  /**
  Go to the first item of the list.*/
  virtual int GoToFirstNode();
  
  /**
  Go to the last item of the list.*/
  virtual int GoToLastNode();

  /**
  Set/Get the traverse modality. Default to PreOrder, i.e. process 
  first the root an then the sub trees (left to right).
  Change the traverse mode restart the traversing!*/
  void SetTraversalMode(int mode);
  int GetTraversalMode() { return this->TraversalMode;}

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
  vtkTree *FindLeftMostLeaf(vtkTree *node);

  /**
  Find the right most leaf of the tree*/
  vtkTree *FindRightMostLeaf(vtkTree *node);

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command.
  Executed before traversing a node. This function pass the current
  node pointer to the callback function.*/
  virtual PreExecute(); 

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command.
  Executed after traversing a node. This function pass the current
   node pointer to the callback function.*/
  virtual PostExecute();  

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command.
  Executed when going down in the tree.. This function pass to the
  callback function the pointer to the node is being processed
  while changing the depth in the tree. Not necessary it is the
  current node or next node to be traversed.*/
  virtual DeeperExecute(vtkTree *); 

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command. 
  Executed when going up in the tree. This function pass to the
  callback function the pointer to the node is being processed
  while changing the depth in the tree. Not necessary it is the
  current node or next node to be traversed.*/
  virtual UpperExecute(vtkTree *);  

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command. 
  executed when GoToFirstNode is executed. This function pass the
  the pointer to this iterator to the callback function.*/
  virtual FirstExecute(); 

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command.
  executed when last node is traversed. This function pass the
  the pointer to this iterator to the callback function.*/
  virtual LastExecute();  

  /**
  Callback function. By deafult these functions look for the corresponding
  observer to be present and execute the associated command.
  executed when the container is exhausted, i.e. when IsDoneWithTraversal
  return "true". This function pass the the pointer to this iterator to 
  the callback function.*/
  virtual DoneExecute();  

  //vtkSetObjectMacro(CurrentNode,vtkTree);
  //vtkSetObjectMacro(CurrentIdx,vtkStack<vtkIdType>);

  vtkTreeIterator();

  virtual ~vtkTreeIterator();

  vtkTree *RootNode;
  vtkTree *CurrentNode;
  int TraversalMode;
  int TraversalDone;
  vtkStack<vtkIdType> *CurrentIdx;

private:
  vtkTreeIterator(const vtkTreeIterator&) {} // Not implemented
  void operator=(const vtkTreeIterator&) {} // Not implemented
};

#endif
