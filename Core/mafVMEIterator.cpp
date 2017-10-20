/*=========================================================================

 Program: MAF2
 Module: mafVMEIterator
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafVMEIterator.h"
#include "mafVector.h"

mafCxxTypeMacro(mafVMEIterator)

//----------------------------------------------------------------------------
mafVMEIterator::mafVMEIterator(mafVME *root)
//----------------------------------------------------------------------------
{ 
  m_CurrentNode = NULL;
  m_RootNode    = NULL; // initialize
  
  SetRootNode(root);
  
  m_TraversalMode = 0;
  m_TraversalDone = 0;
  m_IgnoreVisibleToTraverse = false;
}

//----------------------------------------------------------------------------
mafVMEIterator::~mafVMEIterator()
//----------------------------------------------------------------------------
{
  m_CurrentNode=NULL; // do not unregister since it was not registered!
  SetRootNode(NULL);
}

//----------------------------------------------------------------------------
void mafVMEIterator::InitTraversal()
//----------------------------------------------------------------------------
{
  GoToFirstNode();
}

//----------------------------------------------------------------------------
void mafVMEIterator::SetTraversalMode(int mode)
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
int mafVMEIterator::GoToNextNode()
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

            // go to root of first subtree
            m_CurrentNode=m_CurrentNode->GetChild(0);
            if (m_CurrentNode)
            {
              m_CurrentIdx.Push(0);
            }
            else
            {
              return MAF_ERROR;
            }
          }
          else
          { 
						if (m_CurrentNode!=m_RootNode)
            {
							mafID idx=0;
							mafVME *parent=m_CurrentNode->GetParent();
							if (parent) 
							{ 
								m_CurrentIdx.Pop(idx);
								while (parent&&parent!=m_RootNode&&idx>=(parent->GetNumberOfChildren()-1))
								{
									parent=parent->GetParent();
									m_CurrentIdx.Pop(idx);
								}
							}

							if (parent&&idx<(parent->GetNumberOfChildren()-1))
							{
								// reset the traversal flag
								m_TraversalDone=0;

								// go to root of next brother subtree
								idx++;
								m_CurrentNode=parent->GetChild(idx);
								m_CurrentIdx.Push(idx);
							}
							else
							{
								// Tuch Down!!!
								m_TraversalDone=1; // set the traveral flag
							}            
						}
						else
						{
							// Tuch Down!!!
								m_TraversalDone=1; // set the traveral flag
						}
          }
        }
        break;
      }
    case PostOrder:
      {
        if ((m_CurrentNode)&&(m_CurrentNode!=m_RootNode))
        {
          mafVME *parent=m_CurrentNode->GetParent();

          if (parent)
          {
            mafID idx;
            if (!m_CurrentIdx.Pop(idx))
            {
              mafErrorMacro("Stack Underflow");
              m_TraversalDone=1; //set the traversal flag
              m_CurrentNode=NULL;
            }
            else if (idx<(parent->GetNumberOfChildren()-1))
            {
              m_TraversalDone=0; //reset the traversal flag

              idx++;
              m_CurrentIdx.Push(idx);
              m_CurrentNode=FindLeftMostLeaf(parent->GetChild(idx));
              // the call to the deeper-callback is inside FindLeftMostLeaf
            }
            else
            {
              m_TraversalDone=0; //reset the traversal flag
              m_CurrentNode=parent;
            }
          }
          else
          {
            mafErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
            m_TraversalDone=1; //set the traversal flag
            m_CurrentNode=NULL;
          }
        }
        else
        {
          // Got to the last node (or Current==NULL)
          m_TraversalDone=1; //set the traversal flag
        }
        break;
      }
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
      m_TraversalDone=1;
  }

  if (!m_TraversalDone)
  {
    //return (IsVisible(GetCurrentNode()))?MAF_OK:GoToNextNode();
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse ? MAF_OK : GoToNextNode();
  }

  return MAF_ERROR;
}

//----------------------------------------------------------------------------
int mafVMEIterator::GoToPreviousNode()
//----------------------------------------------------------------------------
{
  switch (m_TraversalMode)
  {
    case PreOrder:
      {
        if ((m_CurrentNode)&&(m_CurrentNode!=m_RootNode))
        {
          mafVME *parent=m_CurrentNode->GetParent();

          if (parent)
          {
            mafID idx;
            if (!m_CurrentIdx.Pop(idx))
            {
              mafErrorMacro("Stack Underflow");
              m_TraversalDone=1;
              m_CurrentNode=NULL;
            }
            else if (idx>0)
            {
              m_TraversalDone=0;
              idx--;
              m_CurrentIdx.Push(idx);
              m_CurrentNode=FindRightMostLeaf(parent->GetChild(idx));
              //the call to the deeper-callback is inside the FindRightMostLeaf
            }
            else
            {
              m_TraversalDone=0;

              m_CurrentNode=parent;
						}
          }
          else
          {
            mafErrorMacro("Find an orphan node: stopping traversing immediately!");
            m_TraversalDone=1;
            m_CurrentNode=NULL;
          }
        }
        else
        {
          m_TraversalDone=1;
        }
      }
      break;
    case PostOrder:
      {
        if (m_CurrentNode)
        {
          if (m_CurrentNode->GetNumberOfChildren()>0)
          {
            m_TraversalDone=0;
            // go to root of last subtree
            mafID idx=m_CurrentNode->GetNumberOfChildren()-1;
            m_CurrentIdx.Push(idx);
            m_CurrentNode=m_CurrentNode->GetChild(idx);
          }
          else
          {
            mafVME *parent=m_CurrentNode->GetParent();

            mafID idx;
            if (m_CurrentIdx.Pop(idx))
            {
              while (parent && parent!=m_RootNode && idx<=0) // search for the first root where we still have children to be visited
              {
                parent=parent->GetParent();

                if (!m_CurrentIdx.Pop(idx))
                {
                  mafErrorMacro("Stack Underflow");
                  m_TraversalDone=1;
                  m_CurrentNode=NULL;
                  return MAF_ERROR;
                }
              }
            }
            else
            {
              mafErrorMacro("Stack Underflow");
              m_CurrentNode=NULL;
              m_TraversalDone=1;
              return MAF_ERROR;
            }

            if (parent)
            {
              if (idx>0)
              {
                m_TraversalDone=0;
                idx--;
                m_CurrentIdx.Push(idx);
                // go to root of prevoius brother subtree
                m_CurrentNode=parent->GetChild(idx);
              }
              else
              {
                // touch down!
                m_TraversalDone=1;
              }
            }
            else
            {
              m_TraversalDone=1;
              mafErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
              m_CurrentNode=NULL;
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

  if (!m_TraversalDone)
  {
    //return (IsVisible(GetCurrentNode()))?MAF_OK:GoToPreviousNode();
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse ? MAF_OK : GoToPreviousNode();
  }

  return MAF_ERROR;
}

//----------------------------------------------------------------------------
mafVME *mafVMEIterator::FindLeftMostLeaf(mafVME *node)
//----------------------------------------------------------------------------
{
  while (node&&node->GetNumberOfChildren()>0) 
  {
    node=node->GetChild(0);
    m_CurrentIdx.Push(0);
  }

  return node;
}

//----------------------------------------------------------------------------
mafVME *mafVMEIterator::FindRightMostLeaf(mafVME *node)
//----------------------------------------------------------------------------
{
  while (node&&node->GetNumberOfChildren()>0) 
  {
    m_CurrentIdx.Push(node->GetNumberOfChildren()-1); // store index of the currently visited node
    node=node->GetChild(node->GetNumberOfChildren()-1);
  }

  return node;
}

//----------------------------------------------------------------------------
int mafVMEIterator::GoToFirstNode()
//----------------------------------------------------------------------------
{
  m_CurrentIdx.RemoveAllItems();
  switch (m_TraversalMode)
  {
    case PreOrder:
      m_CurrentNode=m_RootNode;
      break;
    case PostOrder:
      m_CurrentNode=FindLeftMostLeaf(m_RootNode);
      break;
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
  }

  if (m_CurrentNode)
  {
    m_TraversalDone=0; // reset the traversal flag
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse?MAF_OK:GoToNextNode();
  }
  else
  {
    // the tree is empty traverse is already complete

    m_TraversalDone=1; // set traversal flag
    return MAF_ERROR;
  }

  
}

//----------------------------------------------------------------------------
int mafVMEIterator::GoToLastNode()
//----------------------------------------------------------------------------
{
  m_CurrentIdx.RemoveAllItems();
  switch (m_TraversalMode)
  {
    case PreOrder:
      m_CurrentNode=FindRightMostLeaf(m_RootNode);
      break;
    case PostOrder:
      m_CurrentNode=m_RootNode;
      break;
    default:
      mafErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
  }

  if (m_CurrentNode)
  {
    m_TraversalDone=0; // reset the traversal flag
    //return IsVisible(m_CurrentNode)?MAF_OK:GoToPreviousNode();
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse ? MAF_OK : GoToPreviousNode();
  }
  else
  {
    m_TraversalDone=1; // set traversal flag
    return MAF_ERROR;
  }
}

//----------------------------------------------------------------------------
void mafVMEIterator::SetRootNode(mafVME *root)
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
