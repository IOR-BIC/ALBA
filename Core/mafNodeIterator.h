/*=========================================================================

 Program: MAF2
 Module: mafNodeIterator
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafNodeIterator_h
#define __mafNodeIterator_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafNode.h"
#include "mafVector.h"
#include "mafEventSource.h"

template class MAF_EXPORT mafVector<mafID>;

/** mafNodeIterator - an m-way tree data structure iterator
  mafNodeIterator is a class to traverse a tree data structure. It allows 
  to set the traverse modality. The iterator allows to set some callback to the 
  traverse by means of the MAF Subject/Observer mechanism.
  Issued events are: "PreTraversal","PostTraversal","Deeper","Upper","FirstNode",
  "LastNode", "Done".
  These can be overidden by adding an observer
  
  @sa mafNode
  */

class MAF_EXPORT mafNodeIterator : public mafObject
{
public:
  mafTypeMacro(mafNodeIterator,mafObject);
  
  /** Retrieve the current node pointer data from the iterator. */
  mafNode * GetCurrentNode() {return m_CurrentNode;}

  /**  Shortcut to traverse the tree*/
  mafNode * GetFirstNode() {this->GoToFirstNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}
  /**  Shortcut to traverse the tree*/
  mafNode * GetLastNode() {this->GoToLastNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}
  /**  Shortcut to traverse the tree*/
  mafNode * GetNextNode() {this->GoToNextNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}
  /**  Shortcut to traverse the tree*/
  mafNode * GetPreviousNode() {this->GoToPreviousNode(); return (m_TraversalDone)?NULL:this->GetCurrentNode();}

  /**
    Return true if the VME is visible. This function can be overridden to implement
    different visibility rules.*/
  virtual bool IsVisible(mafNode *node) { return node->IsVisible();}

  /** Allow to ignore m_VisibleToTraverse flag for the iterator. */
  virtual void IgnoreVisibleToTraverse(bool ignore) {m_IgnoreVisibleToTraverse = ignore;};

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

  enum events_ID {ID_PreTraversal=0,ID_PostTraversal,ID_Deeper, ID_Upper, ID_FirstNode, ID_LastNode, ID_Done};
protected:

  /** Find the left most leaf of the tree*/
  mafNode *FindLeftMostLeaf(mafNode *node);

  /** Find the right most leaf of the tree*/
  mafNode *FindRightMostLeaf(mafNode *node);

  mafEventSource &GetEventSource() {return m_EventSource;}

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void PreExecute(); 

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void PostExecute();  

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void DeeperExecute(mafNode *); 

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void UpperExecute(mafNode *);  

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void FirstExecute(); 

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void LastExecute();  

  /** Callback function. By default send an event through the m_EventSource source. */
  virtual void DoneExecute();  

  mafNodeIterator(mafNode *root=NULL);
  virtual ~mafNodeIterator();

  mafNode         *m_RootNode;
  mafNode         *m_CurrentNode;
  int             m_TraversalMode;
  int             m_TraversalDone;
  bool            m_IgnoreVisibleToTraverse;

  mafVector<mafID> m_CurrentIdx;

  mafEventSource  m_EventSource; ///< Source of events issued during traverse

private:
  mafNodeIterator(const mafNodeIterator&) {} // Not implemented
  void operator=(const mafNodeIterator&) {} // Not implemented
};

#endif
