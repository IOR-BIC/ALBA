/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeIterator.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-30 18:18:21 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafNodeIterator_cxx
#define __mafNodeIterator_cxx

#include "mafNodeIterator.h"
#include "mafStack.txx"


//----------------------------------------------------------------------------
mafNodeIterator::mafNodeIterator(mafNode *root)
//----------------------------------------------------------------------------
{ 
  m_CurrentNode=NULL;
  m_RootNode=NULL; // initialize
  SetRootNode(root);
  m_CurrentIdx=mafStack<mafID>::New();
  m_TraversalMode=0;
  m_TraversalDone=0;
}

//----------------------------------------------------------------------------
mafNodeIterator::~mafNodeIterator()
//----------------------------------------------------------------------------
{
  m_CurrentNode=NULL; // do not unregister since it was not registered!
  SetRootNode(NULL);
  if (m_CurrentIdx)
  {
    m_CurrentIdx->UnRegister(this);
    m_CurrentIdx=NULL;
  }
}


//----------------------------------------------------------------------------
void mafNodeIterator::InitTraversal()
//----------------------------------------------------------------------------
{
  GoToFirstNode();
}

//----------------------------------------------------------------------------
void mafNodeIterator::SetTraversalMode(int mode)
//----------------------------------------------------------------------------
{
  if (mode!=PreOrder && mode!=PostOrder)
  {
    mafErrorMacro("Unsupported Traversal Mode");
  }
  else
  {
    m_TraversalMode=mode;
    InitTraversal();
  }
}


//----------------------------------------------------------------------------
int mafNodeIterator::GoToNextNode()
//----------------------------------------------------------------------------
{
  switch (m_TraversalMode)
  {
    case PreOrder:
      {
        if (m_CurrentNode)
        {
          if (m_CurrentNode->GetNumberOfChildren()>0)
          {
            m_TraversalDone=0; //reset the traversal flag
            // before changing node call the post-execute
            PostExecute();

            // go to root of first subtree
            m_CurrentNode=m_CurrentNode->GetChild(0);
            if (m_CurrentNode)
            {
              m_CurrentIdx->Push(0);
              DeeperExecute(m_CurrentNode); //call the deeper-callback

              // before doing anything else call the pre-execute
              PreExecute();
            }
            else
            {
              return VTK_ERROR;
            }
          }
          else
          { 
						if (m_CurrentNode!=m_RootNode)
            {
							int idx=0;
							mafNode *parent=m_CurrentNode->GetParent();
							if (parent) 
							{ 
								idx=m_CurrentIdx->Pop();
								UpperExecute(parent); //call the upper-callback

								while (parent&&parent!=m_RootNode&&idx>=(parent->GetNumberOfChildren()-1))
								{
									parent=parent->GetParent();
									idx=m_CurrentIdx->Pop();
									UpperExecute(parent); //call the upper-callback
								}
							}

							if (parent&&idx<(parent->GetNumberOfChildren()-1))
							{
								// reset the traversal flag
								m_TraversalDone=0;

								// before changing node call the post-execute
								PostExecute();

								// go to root of next brother subtree
								idx++;
								m_CurrentNode=parent->GetChild(idx);
								m_CurrentIdx->Push(idx);
								DeeperExecute(m_CurrentNode); //call the deeper-callback

								// before doing anything else call the pre-execute
								PreExecute();
							}
							else
							{
								// Tuch Down!!!
								m_TraversalDone=1; // set the traveral flag
								DoneExecute(); // call the Done-execute
							}            
						}
						else
						{
							// Tuch Down!!!
								m_TraversalDone=1; // set the traveral flag
								DoneExecute(); // call the Done-execute
						}
          }
        }
        break;
      }
    case PostOrder:
      {
        if ((m_CurrentNode)&&(m_CurrentNode!=m_RootNode))
        {
          mafNode *parent=m_CurrentNode->GetParent();

          if (parent)
          {
            int idx;
            if (m_CurrentIdx->Pop(idx)!=VTK_OK)
            {
              mafErrorMacro("Stack Underflow");
              m_TraversalDone=1; //set the traversal flag
              m_CurrentNode=NULL;
              DoneExecute();
            }
            else if (idx<(parent->GetNumberOfChildren()-1))
            {
              m_TraversalDone=0; //reset the traversal flag
              UpperExecute(parent); //call the upper-callback

              PostExecute(); // call the post-execute

              idx++;
              m_CurrentIdx->Push(idx);
              m_CurrentNode=FindLeftMostLeaf(parent->GetChild(idx));
              // the call to the deeper-callback is inside FindLeftMostLeaf

              // before doing anything else call the pre-execute
              PreExecute();
            }
            else
            {
              m_TraversalDone=0; //reset the traversal flag
              UpperExecute(parent); //call the upper-callback
              // before changing node call the post-execute
              PostExecute();

              m_CurrentNode=parent;

              // before doing anything else call the pre-execute
              PreExecute();

            }
          }
          else
          {
            mafErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
            m_TraversalDone=1; //set the traversal flag
            m_CurrentNode=NULL;
            DoneExecute();
          }
        }
        else
        {
          // Got to the last node (or Current==NULL)
          m_TraversalDone=1; //set the traversal flag
          DoneExecute();
        }
        break;
      }
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
      m_TraversalDone=1;
  }

  return (!m_TraversalDone)?VTK_OK:VTK_ERROR;

}

//----------------------------------------------------------------------------
int mafNodeIterator::GoToPreviousNode()
//----------------------------------------------------------------------------
{
  switch (m_TraversalMode)
  {
    case PreOrder:
      {
        if ((m_CurrentNode)&&(m_CurrentNode!=m_RootNode))
        {
          mafNode *parent=m_CurrentNode->GetParent();

          if (parent)
          {
            int idx;
            if (m_CurrentIdx->Pop(idx)!=VTK_OK)
            {
              mafErrorMacro("Stack Underflow");
              m_TraversalDone=1;
              m_CurrentNode=NULL;
              DoneExecute();
            }
            else if (idx>0)
            {
              m_TraversalDone=0;
              UpperExecute(parent); //call the upper-execute
              PostExecute(); // call the post-execute

              idx--;
              m_CurrentIdx->Push(idx);
              m_CurrentNode=FindRightMostLeaf(parent->GetChild(idx));
              //the call to the deeper-callback is inside the FindRightMostLeaf
              
              PreExecute(); // call the pre-execute
            }
            else
            {
              m_TraversalDone=0;
              UpperExecute(parent); //call the upper-callback
              PostExecute(); // call the post-execute

              m_CurrentNode=parent;

              PreExecute(); // call the pre-execute
            }
          }
          else
          {
            mafErrorMacro("Find an orphan node: stopping traversing immediately!");
            m_TraversalDone=1;
            m_CurrentNode=NULL;
            DoneExecute();
          }
        }
      }
      break;
    case PostOrder:
      {
        if (m_CurrentNode)
        {
          if (m_CurrentNode->GetNumberOfChildren()>=0)
          {
            m_TraversalDone=0;
            PostExecute(); 

            // go to root of last subtree
            int idx=m_CurrentNode->GetNumberOfChildren()-1;
            m_CurrentIdx->Push(idx);
            m_CurrentNode=m_CurrentNode->GetChild(idx);

            DeeperExecute(m_CurrentNode);
            PreExecute();
          }
          else
          {
            mafNode *parent=m_CurrentNode->GetParent();

            int idx;
            if (CurrentIdx->Pop(idx)==VTK_OK)
            {
              UpperExecute(parent); 

              while (parent && parent!=m_RootNode && idx>0)
              {
                parent=parent->GetParent();

                if (m_CurrentIdx->Pop(idx)!=VTK_OK)
                {
                  mafErrorMacro("Stack Underflow");
                  m_TraversalDone=1;
                  m_CurrentNode=NULL;
                  DoneExecute();
                  return VTK_ERROR;
                }

                UpperExecute(parent); //call the upper-callback
              }
            }
            else
            {
              mafErrorMacro("Stack Underflow");
              m_CurrentNode=NULL;
              m_TraversalDone=1;
              return VTK_ERROR;
            }

            if (parent)
            {
              if (idx>0)
              {
                m_TraversalDone=0;
                PostExecute();

                idx--;
                m_CurrentIdx->Push(idx);
                // go to root of prevoius brother subtree
                CurrentNode=parent->GetChild(idx);
                DeeperExecute(m_CurrentNode); //call the deeper-callback

                // before doing anything else call the pre-execute
                PreExecute();
              }
              else
              {
                // touch down!
                m_TraversalDone=1;
                DoneExecute();
              }
            }
            else
            {
              m_TraversalDone=1;
              mafErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
              CurrentNode=NULL;
              DoneExecute();
            }            
          }
        }
        break;
      }
      break;
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_TraversalDone=1;
  }
  return (!m_TraversalDone)?VTK_OK:VTK_ERROR;
}

//----------------------------------------------------------------------------
mafNode *mafNodeIterator::FindLeftMostLeaf(mafNode *node)
//----------------------------------------------------------------------------
{
  if (node)
    DeeperExecute(node);
  while (node&&node->GetNumberOfChildren()>0) 
  {
    node=node->GetChild(0);
    m_CurrentIdx->Push(0);
    if (node)
      DeeperExecute(node);
  }

  return node;
}

//----------------------------------------------------------------------------
mafNode *mafNodeIterator::FindRightMostLeaf(mafNode *node)
//----------------------------------------------------------------------------
{
  if (node)
    DeeperExecute(node);
  while (node&&node->GetNumberOfChildren()>0) 
  {
    node=node->GetChild(node->GetNumberOfChildren()-1);
    m_CurrentIdx->Push(node->GetNumberOfChildren()-1);
    if (node)
      DeeperExecute(node);
  }

  return node;
}

//----------------------------------------------------------------------------
int mafNodeIterator::GoToFirstNode()
//----------------------------------------------------------------------------
{
  m_CurrentIdx->RemoveAllItems();
  switch (m_TraversalMode)
  {
    case PreOrder:
      m_CurrentNode=m_RootNode;
      DeeperExecute(m_CurrentNode);
      break;
    case PostOrder:
      m_CurrentNode=FindLeftMostLeaf(m_RootNode);
      break;
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
  }

  FirstExecute(); // Call the FirstNode-Callback

  if (m_CurrentNode)
  {
    m_TraversalDone=0; // reset the traversal flag
    PreExecute(); // Call the pre-callback
    return VTK_OK;
  }
  else
  {
    // the tree is empty traverse is already complete

    m_TraversalDone=1; // set traversal flag
    DoneExecute();  // Call the Done-callback   
    return VTK_ERROR;
  }

  
}

//----------------------------------------------------------------------------
int mafNodeIterator::GoToLastNode()
//----------------------------------------------------------------------------
{
  m_CurrentIdx->RemoveAllItems();
  switch (m_TraversalMode)
  {
    case PreOrder:
      m_CurrentNode=FindRightMostLeaf(m_RootNode);
      break;
    case PostOrder:
      m_CurrentNode=m_RootNode;
      DeeperExecute(m_CurrentNode);
      break;
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
  }

  LastExecute(); // Call the LastNode-callbacks

  if (m_CurrentNode)
  {
    m_TraversalDone=0; // reset the traversal flag
    PreExecute(); // Call the Pre-callbacks
    return VTK_OK;
  }
  else
  {
    m_TraversalDone=1; // set traversal flag
    DoneExecute(); // Call the Done-callback
    return VTK_ERROR;
  }
}

//----------------------------------------------------------------------------
void mafNodeIterator::SetRootNode(mafNode *root)
//----------------------------------------------------------------------------
{
  if (m_RootNode==root)
    return;

  if (m_RootNode)
  {
    m_RootNode->UnRegister(this);
  }

  m_RootNode=root;

  if (m_RootNode)
    m_RootNode->Register(this);
}

//----------------------------------------------------------------------------
// executed before traversing a node
mafNodeIterator::PreExecute()
//----------------------------------------------------------------------------
{
  if (HasObserver(PreTraversal))
  {
    InvokeEvent(PreTraversal,m_CurrentNode);
  }
}
//----------------------------------------------------------------------------
// executed after traversing a node
mafNodeIterator::PostExecute()
//----------------------------------------------------------------------------
{
  if (HasObserver(PostTraversal))
  {
    InvokeEvent(PostTraversal,m_CurrentNode);
  }
}
//----------------------------------------------------------------------------
// executed when going down in the tree
mafNodeIterator::DeeperExecute(mafNode *node)
//----------------------------------------------------------------------------
{
  if (HasObserver(Deeper))
  {
    InvokeEvent(Deeper,node);
  }
}
//----------------------------------------------------------------------------
// executed when going up in the tree
mafNodeIterator::UpperExecute(mafNode *node)
//----------------------------------------------------------------------------
{
  if (HasObserver(Upper))
  {
    InvokeEvent(Upper,node);
  }
}
//----------------------------------------------------------------------------
// executed when GoToFirstNode is executed
mafNodeIterator::FirstExecute()
//----------------------------------------------------------------------------
{
  if (HasObserver(FirstNode))
  {
    InvokeEvent(FirstNode,this);
  }
}
//----------------------------------------------------------------------------
// executed when last node is traversed
mafNodeIterator::LastExecute()
//----------------------------------------------------------------------------
{
  if (HasObserver(LastNode))
  {
    InvokeEvent(LastNode,this);
  }
}
//----------------------------------------------------------------------------
// executed when IsDoneWithTraversal return "true"
mafNodeIterator::DoneExecute()
//----------------------------------------------------------------------------
{
  if (HasObserver(Done))
  {
    InvokeEvent(Done,this);
  }
}
#endif
