/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEIterator
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "albaVMEIterator.h"
#include "albaVector.h"

albaCxxTypeMacro(albaVMEIterator)

//----------------------------------------------------------------------------
albaVMEIterator::albaVMEIterator(albaVME *root)
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
albaVMEIterator::~albaVMEIterator()
//----------------------------------------------------------------------------
{
  m_CurrentNode=NULL; // do not unregister since it was not registered!
  SetRootNode(NULL);
}

//----------------------------------------------------------------------------
void albaVMEIterator::InitTraversal()
//----------------------------------------------------------------------------
{
  GoToFirstNode();
}

//----------------------------------------------------------------------------
void albaVMEIterator::SetTraversalMode(int mode)
//----------------------------------------------------------------------------
{
  if (mode!=PreOrder && mode!=PostOrder)
  {
    albaErrorMacro("Unsupported Traversal Mode");
  }
  else
  {
    m_TraversalMode=mode;
    InitTraversal();
  }
}


//----------------------------------------------------------------------------
int albaVMEIterator::GoToNextNode()
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
              return ALBA_ERROR;
            }
          }
          else
          { 
						if (m_CurrentNode!=m_RootNode)
            {
							albaID idx=0;
							albaVME *parent=m_CurrentNode->GetParent();
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
          albaVME *parent=m_CurrentNode->GetParent();

          if (parent)
          {
            albaID idx;
            if (!m_CurrentIdx.Pop(idx))
            {
              albaErrorMacro("Stack Underflow");
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
            albaErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
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
      albaErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
      m_TraversalDone=1;
  }

  if (!m_TraversalDone)
  {
    //return (IsVisible(GetCurrentNode()))?ALBA_OK:GoToNextNode();
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse ? ALBA_OK : GoToNextNode();
  }

  return ALBA_ERROR;
}

//----------------------------------------------------------------------------
int albaVMEIterator::GoToPreviousNode()
//----------------------------------------------------------------------------
{
  switch (m_TraversalMode)
  {
    case PreOrder:
      {
        if ((m_CurrentNode)&&(m_CurrentNode!=m_RootNode))
        {
          albaVME *parent=m_CurrentNode->GetParent();

          if (parent)
          {
            albaID idx;
            if (!m_CurrentIdx.Pop(idx))
            {
              albaErrorMacro("Stack Underflow");
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
            albaErrorMacro("Find an orphan node: stopping traversing immediately!");
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
            albaID idx=m_CurrentNode->GetNumberOfChildren()-1;
            m_CurrentIdx.Push(idx);
            m_CurrentNode=m_CurrentNode->GetChild(idx);
          }
          else
          {
            albaVME *parent=m_CurrentNode->GetParent();

            albaID idx;
            if (m_CurrentIdx.Pop(idx))
            {
              while (parent && parent!=m_RootNode && idx<=0) // search for the first root where we still have children to be visited
              {
                parent=parent->GetParent();

                if (!m_CurrentIdx.Pop(idx))
                {
                  albaErrorMacro("Stack Underflow");
                  m_TraversalDone=1;
                  m_CurrentNode=NULL;
                  return ALBA_ERROR;
                }
              }
            }
            else
            {
              albaErrorMacro("Stack Underflow");
              m_CurrentNode=NULL;
              m_TraversalDone=1;
              return ALBA_ERROR;
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
              albaErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
              m_CurrentNode=NULL;
            }            
          }
        }
        break;
      }
      break;
    default:
      albaErrorMacro("Unsupported Traversal Mode");
      m_TraversalDone=1;
  }

  if (!m_TraversalDone)
  {
    //return (IsVisible(GetCurrentNode()))?ALBA_OK:GoToPreviousNode();
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse ? ALBA_OK : GoToPreviousNode();
  }

  return ALBA_ERROR;
}

//----------------------------------------------------------------------------
albaVME *albaVMEIterator::FindLeftMostLeaf(albaVME *node)
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
albaVME *albaVMEIterator::FindRightMostLeaf(albaVME *node)
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
int albaVMEIterator::GoToFirstNode()
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
      albaErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
  }

  if (m_CurrentNode)
  {
    m_TraversalDone=0; // reset the traversal flag
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse?ALBA_OK:GoToNextNode();
  }
  else
  {
    // the tree is empty traverse is already complete

    m_TraversalDone=1; // set traversal flag
    return ALBA_ERROR;
  }

  
}

//----------------------------------------------------------------------------
int albaVMEIterator::GoToLastNode()
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
      albaErrorMacro("Unsupported Traversal Mode");
      m_CurrentNode=NULL;
  }

  if (m_CurrentNode)
  {
    m_TraversalDone=0; // reset the traversal flag
    //return IsVisible(m_CurrentNode)?ALBA_OK:GoToPreviousNode();
    return IsVisible(m_CurrentNode)||m_IgnoreVisibleToTraverse ? ALBA_OK : GoToPreviousNode();
  }
  else
  {
    m_TraversalDone=1; // set traversal flag
    return ALBA_ERROR;
  }
}

//----------------------------------------------------------------------------
void albaVMEIterator::SetRootNode(albaVME *root)
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
