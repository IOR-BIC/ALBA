/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeIterator.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:15:02 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __vtkTreeIterator_cxx
#define __vtkTreeIterator_cxx

#include "vtkTreeIterator.h"
#include "vtkStack.txx"


//----------------------------------------------------------------------------

vtkTreeIterator *vtkTreeIterator::New()
{ 
#ifdef VTK_DEBUG_LEAKS
  vtkDebugLeaks::ConstructClass("vtkTreeIterator");
#endif
  return new vtkTreeIterator(); 
}

vtkTreeIterator::vtkTreeIterator()
{ 
  this->CurrentNode=NULL;
  this->RootNode=NULL;
  this->CurrentIdx=vtkStack<vtkIdType>::New();
  this->TraversalMode=0;
  this->TraversalDone=0;
}

vtkTreeIterator::~vtkTreeIterator()
{
  this->CurrentNode=NULL; // do not unregister since it was not registered!
  this->SetRootNode(NULL);
  if (this->CurrentIdx)
  {
    this->CurrentIdx->UnRegister(this);
    this->CurrentIdx=NULL;
  }
}


//----------------------------------------------------------------------------

void vtkTreeIterator::InitTraversal()
{
  this->GoToFirstNode();
}

//----------------------------------------------------------------------------

void vtkTreeIterator::SetTraversalMode(int mode)
{
  if (mode!=PreOrder && mode!=PostOrder)
  {
    vtkErrorMacro("Unsupported Traversal Mode");
  }
  else
  {
    this->TraversalMode=mode;
    this->InitTraversal();
  }
}


//----------------------------------------------------------------------------

int vtkTreeIterator::GoToNextNode()
{
  switch (this->TraversalMode)
  {
    case PreOrder:
      {
        if (this->CurrentNode)
        {
          if (this->CurrentNode->GetNumberOfChildren()>0)
          {
            this->TraversalDone=0; //reset the traversal flag
            // before changing node call the post-execute
            this->PostExecute();

            // go to root of first subtree
            this->CurrentNode=this->CurrentNode->GetChild(0);
            if (this->CurrentNode)
            {
              this->CurrentIdx->Push(0);
              this->DeeperExecute(this->CurrentNode); //call the deeper-callback

              // before doing anything else call the pre-execute
              this->PreExecute();
            }
            else
            {
              return VTK_ERROR;
            }
          }
          else
          { 
						if (this->CurrentNode!=this->RootNode)
            {
							int idx=0;
							vtkTree *parent=this->CurrentNode->GetParent();
							if (parent) 
							{ 
								idx=this->CurrentIdx->Pop();
								this->UpperExecute(parent); //call the upper-callback

								while (parent&&parent!=this->RootNode&&idx>=(parent->GetNumberOfChildren()-1))
								{
									parent=parent->GetParent();
									idx=this->CurrentIdx->Pop();
									this->UpperExecute(parent); //call the upper-callback
								}
							}

							if (parent&&idx<(parent->GetNumberOfChildren()-1))
							{
								// reset the traversal flag
								this->TraversalDone=0;

								// before changing node call the post-execute
								this->PostExecute();

								// go to root of next brother subtree
								idx++;
								this->CurrentNode=parent->GetChild(idx);
								this->CurrentIdx->Push(idx);
								this->DeeperExecute(this->CurrentNode); //call the deeper-callback

								// before doing anything else call the pre-execute
								this->PreExecute();
							}
							else
							{
								// Tuch Down!!!
								this->TraversalDone=1; // set the traveral flag
								this->DoneExecute(); // call the Done-execute
							}            
						}
						else
						{
							// Tuch Down!!!
								this->TraversalDone=1; // set the traveral flag
								this->DoneExecute(); // call the Done-execute
						}
          }
        }
        break;
      }
    case PostOrder:
      {
        if ((this->CurrentNode)&&(this->CurrentNode!=this->RootNode))
        {
          vtkTree *parent=this->CurrentNode->GetParent();

          if (parent)
          {
            int idx;
            if (this->CurrentIdx->Pop(idx)!=VTK_OK)
            {
              vtkErrorMacro("Stack Underflow");
              this->TraversalDone=1; //set the traversal flag
              this->CurrentNode=NULL;
              this->DoneExecute();
            }
            else if (idx<(parent->GetNumberOfChildren()-1))
            {
              this->TraversalDone=0; //reset the traversal flag
              this->UpperExecute(parent); //call the upper-callback

              this->PostExecute(); // call the post-execute

              idx++;
              this->CurrentIdx->Push(idx);
              this->CurrentNode=this->FindLeftMostLeaf(parent->GetChild(idx));
              // the call to the deeper-callback is inside FindLeftMostLeaf

              // before doing anything else call the pre-execute
              this->PreExecute();
            }
            else
            {
              this->TraversalDone=0; //reset the traversal flag
              this->UpperExecute(parent); //call the upper-callback
              // before changing node call the post-execute
              this->PostExecute();

              this->CurrentNode=parent;

              // before doing anything else call the pre-execute
              this->PreExecute();

            }
          }
          else
          {
            vtkErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
            this->TraversalDone=1; //set the traversal flag
            this->CurrentNode=NULL;
            this->DoneExecute();
          }
        }
        else
        {
          // Got to the last node (or Current==NULL)
          this->TraversalDone=1; //set the traversal flag
          this->DoneExecute();
        }
        break;
      }
    default:
      vtkErrorMacro("Unsupported Traversal Mode");
      this->CurrentNode=NULL;
      this->TraversalDone=1;
  }

  return (!this->TraversalDone)?VTK_OK:VTK_ERROR;

}

//----------------------------------------------------------------------------

int vtkTreeIterator::GoToPreviousNode()
{
  switch (this->TraversalMode)
  {
    case PreOrder:
      {
        if ((this->CurrentNode)&&(this->CurrentNode!=this->RootNode))
        {
          vtkTree *parent=this->CurrentNode->GetParent();

          if (parent)
          {
            int idx;
            if (this->CurrentIdx->Pop(idx)!=VTK_OK)
            {
              vtkErrorMacro("Stack Underflow");
              this->TraversalDone=1;
              this->CurrentNode=NULL;
              this->DoneExecute();
            }
            else if (idx>0)
            {
              this->TraversalDone=0;
              this->UpperExecute(parent); //call the upper-execute
              this->PostExecute(); // call the post-execute

              idx--;
              this->CurrentIdx->Push(idx);
              this->CurrentNode=this->FindRightMostLeaf(parent->GetChild(idx));
              //the call to the deeper-callback is inside the FindRightMostLeaf
              
              this->PreExecute(); // call the pre-execute
            }
            else
            {
              this->TraversalDone=0;
              this->UpperExecute(parent); //call the upper-callback
              this->PostExecute(); // call the post-execute

              this->CurrentNode=parent;

              this->PreExecute(); // call the pre-execute
            }
          }
          else
          {
            vtkErrorMacro("Find an orphan node: stopping traversing immediately!");
            this->TraversalDone=1;
            this->CurrentNode=NULL;
            this->DoneExecute();
          }
        }
      }
      break;
    case PostOrder:
      {
        if (this->CurrentNode)
        {
          if (this->CurrentNode->GetNumberOfChildren()>=0)
          {
            this->TraversalDone=0;
            this->PostExecute(); 

            // go to root of last subtree
            int idx=this->CurrentNode->GetNumberOfChildren()-1;
            this->CurrentIdx->Push(idx);
            this->CurrentNode=this->CurrentNode->GetChild(idx);

            this->DeeperExecute(this->CurrentNode);
            this->PreExecute();
          }
          else
          {
            vtkTree *parent=this->CurrentNode->GetParent();

            int idx;
            if (this->CurrentIdx->Pop(idx)==VTK_OK)
            {
              this->UpperExecute(parent); 

              while (parent && parent!=this->RootNode && idx>0)
              {
                parent=parent->GetParent();

                if (this->CurrentIdx->Pop(idx)!=VTK_OK)
                {
                  vtkErrorMacro("Stack Underflow");
                  this->TraversalDone=1;
                  this->CurrentNode=NULL;
                  this->DoneExecute();
                  return VTK_ERROR;
                }

                this->UpperExecute(parent); //call the upper-callback
              }
            }
            else
            {
              vtkErrorMacro("Stack Underflow");
              this->CurrentNode=NULL;
              this->TraversalDone=1;
              return VTK_ERROR;
            }

            if (parent)
            {
              if (idx>0)
              {
                this->TraversalDone=0;
                this->PostExecute();

                idx--;
                this->CurrentIdx->Push(idx);
                // go to root of prevoius brother subtree
                this->CurrentNode=parent->GetChild(idx);
                this->DeeperExecute(this->CurrentNode); //call the deeper-callback

                // before doing anything else call the pre-execute
                this->PreExecute();
              }
              else
              {
                // touch down!
                this->TraversalDone=1;
                this->DoneExecute();
              }
            }
            else
            {
              this->TraversalDone=1;
              vtkErrorMacro("Troubles: found an orphan node: stopping traversing immediately!");
              this->CurrentNode=NULL;
              this->DoneExecute();
            }            
          }
        }
        break;
      }
      break;
    default:
      vtkErrorMacro("Unsupported Traversal Mode");
      this->TraversalDone=1;
  }
  return (!this->TraversalDone)?VTK_OK:VTK_ERROR;
}

//----------------------------------------------------------------------------

vtkTree *vtkTreeIterator::FindLeftMostLeaf(vtkTree *node)
{
  if (node)
    this->DeeperExecute(node);
  while (node&&node->GetNumberOfChildren()>0) 
  {
    node=node->GetChild(0);
    this->CurrentIdx->Push(0);
    if (node)
      this->DeeperExecute(node);
  }

  return node;
}

//----------------------------------------------------------------------------

vtkTree *vtkTreeIterator::FindRightMostLeaf(vtkTree *node)
{
  if (node)
    this->DeeperExecute(node);
  while (node&&node->GetNumberOfChildren()>0) 
  {
    node=node->GetChild(node->GetNumberOfChildren()-1);
    this->CurrentIdx->Push(node->GetNumberOfChildren()-1);
    if (node)
      this->DeeperExecute(node);
  }

  return node;
}

//----------------------------------------------------------------------------

int vtkTreeIterator::GoToFirstNode()
{
  this->CurrentIdx->RemoveAllItems();
  switch (this->TraversalMode)
  {
    case PreOrder:
      this->CurrentNode=this->RootNode;
      this->DeeperExecute(this->CurrentNode);
      break;
    case PostOrder:
      this->CurrentNode=this->FindLeftMostLeaf(this->RootNode);
      break;
    default:
      vtkErrorMacro("Unsupported Traversal Mode");
      this->CurrentNode=NULL;
  }

  this->FirstExecute(); // Call the FirstNode-Callback

  if (this->CurrentNode)
  {
    this->TraversalDone=0; // reset the traversal flag
    this->PreExecute(); // Call the pre-callback
    return VTK_OK;
  }
  else
  {
    // the tree is empty traverse is already complete

    this->TraversalDone=1; // set traversal flag
    this->DoneExecute();  // Call the Done-callback   
    return VTK_ERROR;
  }

  
}

//----------------------------------------------------------------------------

int vtkTreeIterator::GoToLastNode()
{
  this->CurrentIdx->RemoveAllItems();
  switch (this->TraversalMode)
  {
    case PreOrder:
      this->CurrentNode=this->FindRightMostLeaf(this->RootNode);
      break;
    case PostOrder:
      this->CurrentNode=this->RootNode;
      this->DeeperExecute(this->CurrentNode);
      break;
    default:
      vtkErrorMacro("Unsupported Traversal Mode");
      this->CurrentNode=NULL;
  }

  this->LastExecute(); // Call the LastNode-callbacks

  if (this->CurrentNode)
  {
    this->TraversalDone=0; // reset the traversal flag
    this->PreExecute(); // Call the Pre-callbacks
    return VTK_OK;
  }
  else
  {
    this->TraversalDone=1; // set traversal flag
    this->DoneExecute(); // Call the Done-callback
    return VTK_ERROR;
  }
}

//----------------------------------------------------------------------------
void vtkTreeIterator::SetRootNode(vtkTree *root)
{
  if (this->RootNode==root)
    return;

  if (this->RootNode)
  {
    this->RootNode->UnRegister(this);
  }

  this->RootNode=root;

  if (this->RootNode)
    this->RootNode->Register(this);
}

//----------------------------------------------------------------------------
// executed before traversing a node
vtkTreeIterator::PreExecute()
{
  if (this->HasObserver(PreTraversal))
  {
    this->InvokeEvent(PreTraversal,this->CurrentNode);
  }
}
//----------------------------------------------------------------------------
// executed after traversing a node
vtkTreeIterator::PostExecute()
{
  if (this->HasObserver(PostTraversal))
  {
    this->InvokeEvent(PostTraversal,this->CurrentNode);
  }
}
//----------------------------------------------------------------------------
// executed when going down in the tree
vtkTreeIterator::DeeperExecute(vtkTree *node)
{
  if (this->HasObserver(Deeper))
  {
    this->InvokeEvent(Deeper,node);
  }
}
//----------------------------------------------------------------------------
// executed when going up in the tree
vtkTreeIterator::UpperExecute(vtkTree *node)
{
  if (this->HasObserver(Upper))
  {
    this->InvokeEvent(Upper,node);
  }
}
//----------------------------------------------------------------------------
// executed when GoToFirstNode is executed
vtkTreeIterator::FirstExecute()
{
  if (this->HasObserver(FirstNode))
  {
    this->InvokeEvent(FirstNode,this);
  }
}
//----------------------------------------------------------------------------
// executed when last node is traversed
vtkTreeIterator::LastExecute()
{
  if (this->HasObserver(LastNode))
  {
    this->InvokeEvent(LastNode,this);
  }
}
//----------------------------------------------------------------------------
// executed when IsDoneWithTraversal return "true"
vtkTreeIterator::DoneExecute()
{
  if (this->HasObserver(Done))
  {
    this->InvokeEvent(Done,this);
  }
}
#endif
