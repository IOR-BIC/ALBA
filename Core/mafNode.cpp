/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNode.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-18 22:07:43 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNode_cxx
#define __mafNode_cxx

#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVector.txx"
#include <sstream>
#include <assert.h>

mafCxxAbstractTypeMacro(mafNode)

//-------------------------------------------------------------------------
mafNode::mafNode()
//-------------------------------------------------------------------------
{
  m_Parent              = NULL;
  m_Initialized         = false;
  m_VisibleToTraverse   = true;
  m_Crypting            = NO_CRYPTING;
}

//-------------------------------------------------------------------------
mafNode::~mafNode()
//-------------------------------------------------------------------------
{
  // remove all the children
  RemoveAllChildren();

  SetParent(NULL);
}

//------------------------------------------------------------------------------
int mafNode::Initialize()
//------------------------------------------------------------------------------
{
  if (m_Initialized)
    return -1;

  if (this->InternalInitialize() == 0)
  {
    m_Initialized=1;
    return 0;
  }

  return -1;

}

//------------------------------------------------------------------------------
void mafNode::Shutdown()
//------------------------------------------------------------------------------
{
  if (m_Initialized)
  {
    InternalShutdown();
    m_Initialized = 0;
  }
}

//-------------------------------------------------------------------------
const char *mafNode::GetName()
//-------------------------------------------------------------------------
{
  return m_Name;
}

//-------------------------------------------------------------------------
void mafNode::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=mafString(name); // force string copy
  Modified();
} 

//-------------------------------------------------------------------------
void mafNode::SetName(mafString name)
//-------------------------------------------------------------------------
{
  m_Name=name;Modified();
}

//-------------------------------------------------------------------------
mafNodeIterator *mafNode::NewIterator()
//-------------------------------------------------------------------------
{
  mafNodeIterator *iter= mafNodeIterator::New();
  iter->SetRootNode(this);
  return iter;
}

//-------------------------------------------------------------------------
int mafNode::GetNumberOfChildren()
//-------------------------------------------------------------------------
{
  return m_Children.GetNumberOfItems();
}

//-------------------------------------------------------------------------
bool mafNode::IsAChild(mafNode *a)
//-------------------------------------------------------------------------
{
  return (a->GetParent()==this);
}

//-------------------------------------------------------------------------
mafNode *mafNode::GetFirstChild()
//-------------------------------------------------------------------------
{
  return this->GetChild(0);
}

//-------------------------------------------------------------------------
mafNode *mafNode::GetLastChild()
//-------------------------------------------------------------------------
{
  return this->GetChild(this->GetNumberOfChildren()-1);
}

//-------------------------------------------------------------------------
mafNode *mafNode::GetChild(mafID idx)
//-------------------------------------------------------------------------
{
  return m_Children[idx].GetPointer();
}
  
//-------------------------------------------------------------------------
int mafNode::FindNodeIdx(mafNode *a)
//-------------------------------------------------------------------------
{
  mafID idx;
  mafAutoPointer<mafNode> tmp(a);
  return m_Children.FindItem(a,idx)?idx:-1;
}

//-------------------------------------------------------------------------
int mafNode::AddChild(mafNode *node)
//-------------------------------------------------------------------------
{
  if (node->SetParent(this)==MAF_OK)
  {
    m_Children.AppendItem(node);
    Modified();
    return MAF_OK;
  }
  return MAF_ERROR;
}  

//-------------------------------------------------------------------------
void mafNode::RemoveChild(const mafID idx)
//-------------------------------------------------------------------------
{  
  mafNode *oldnode=this->GetChild(idx);
  if (oldnode)
  {
    // when called by ReparentTo the parent is already changed
    if (oldnode->GetParent()==this)
      oldnode->SetParent(NULL); 
    m_Children.RemoveItem(idx);
  }
}

//-------------------------------------------------------------------------
int mafNode::ReparentTo(mafNode *newparent)
//-------------------------------------------------------------------------
{
  // We cannot reparent to a subnode!!!
  if (!this->IsInTree(newparent))
  {
    // Add this node to the new parent children list and
    // remove it from old parent children list.
    // We first add it to the new parent, thus it is registered
    // from the new parent, the we remove it from the list of the old parent.
    // We must keep the oldparent pointer somewhere since it is overwritten
    // by AddChild.
    mafNode *oldparent=m_Parent;
    
    if (oldparent!=newparent)
    {
      if (newparent)
      {
        if (newparent->AddChild(this)==MAF_ERROR)
          return MAF_ERROR;
      }
      else
      {
        this->SetParent(NULL);
      }

      if (oldparent)
      {
        oldparent->RemoveChild(this);
      }
      Modified();
    }
    
    return MAF_OK;
  }
  else
  {
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
mafNode *mafNode::GetRoot()
//-------------------------------------------------------------------------
{
  mafNode *node;
  for (node=this;node->GetParent();node=node->GetParent()) ;
  return node;
}

//-------------------------------------------------------------------------
bool mafNode::IsInTree(mafNode *a)
//-------------------------------------------------------------------------
{
  for (mafNode* node=a;node;node=node->GetParent())
  {
    if (this==node)
      return true;
  }

  return false;
}

//-------------------------------------------------------------------------
void mafNode::UnRegister(void *o)
//-------------------------------------------------------------------------
{
  if (this->ReferenceCount<=1)
  {
    if (m_Parent)
    {
      if (o!=&(m_Parent->m_Children))
      {
        m_Parent->RemoveChild(this);

        return;
      }
    }    
  }
  
  Superclass::UnRegister(o);
}

//-------------------------------------------------------------------------
void mafNode::CleanTree()
//-------------------------------------------------------------------------
{
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mafNode *curr=this->GetChild(i);
    if (curr)
      curr->CleanTree();
  }
  
  this->RemoveAllChildren();
  
}

//-------------------------------------------------------------------------
void mafNode::RemoveAllChildren()
//-------------------------------------------------------------------------
{
  
  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    mafNode *curr=this->GetChild(i);
    if (curr)
      curr->SetParent(NULL);
  }
  
  m_Children.RemoveAllItems();
}

//------------------------------------------------------------------------------
int mafNode::SetParent(mafNode *parent)
//-------------------------------------------------------------------------
{
  if (mafNode *parent_node=parent)
  {
    if (this->CanReparentTo(parent_node))
    {  
      m_Parent=parent_node;

      // TODO: reimplement with new events
      /*if (parent_node==NULL)
      {
        this->InvokeEvent(mafNode::DetachFromTreeEvent,this);
      }
      else
      {
        this->InvokeEvent(mafNode::AttachToTreeEvent,this);
      }*/

      return MAF_OK;
    }

    
    // modified by Stefano 27-10-2004: Changed the error macro to give feedback about node names 
    mafErrorMacro("Cannot reparent the VME: " << GetName() << " under the " << parent->GetClassName() \
      << " named " << parent_node->GetName());
  }
  else
  {
    // reparenting to NULL is admitted in any case
    if (parent==NULL)
    {
      m_Parent=parent;
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}



//-------------------------------------------------------------------------
mafNode *mafNode::MakeCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  mafNode* newnode=a->NewInstance();
  if (newnode)
    newnode->DeepCopy(a);
  return newnode;
}

//-------------------------------------------------------------------------
int mafNode::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{  
  if (this->CanCopy(a))
  {
    // Copy arrays.
    /*
		if (this->TagArray)
			this->TagArray->DeepCopy(a->GetTagArray());

    this->DataArray->DeepCopy(a->GetDataArray());

    this->MatrixVector->DeepCopy(a->MatrixVector);

    this->SetMatrixPipe(a->GetMatrixPipe()?a->GetMatrixPipe()->MakeACopy():NULL);
    this->SetDataPipe(a->GetDataPipe()?a->GetDataPipe()->MakeACopy():NULL);
    
    */
    // attributes
    SetName(a->GetName());

    return MAF_OK;
  }
  else
  {
    mafErrorMacro("Cannot copy Node of type "<<a->GetClassName()<<" into a Node of type " \
      << GetClassName());

    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
bool mafNode::CanCopy(mafNode *node)
//-------------------------------------------------------------------------
{
  if (!node)
    return false;

  if (node->IsA(GetTypeId()))
  {
    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
bool mafNode::Equals(mafNode *vme)
//-------------------------------------------------------------------------
{
  return m_Name == vme->m_Name;
}
//-------------------------------------------------------------------------
bool mafNode::CompareTree(mafNode *vme)
//-------------------------------------------------------------------------
{
  if (!this->Equals(vme))
    return false;

  if (vme->GetNumberOfChildren()!=this->GetNumberOfChildren())
  {
    return false;
  }

  for (int i=0;i<this->GetNumberOfChildren();i++)
  {
    if (!this->GetChild(i)->CompareTree(vme->GetChild(i)))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
mafNode *mafNode::CopyTree(mafNode *vme, mafNode *parent)
//-------------------------------------------------------------------------
{
  
  mafNode* v = vme->MakeCopy();

  v->ReparentTo(parent);

  if (parent)
  {
    v->Delete();
  }

  for(int i=0; i<vme->GetNumberOfChildren(); i++)
  {
    if (mafNode *child=vme->GetChild(i))
      if (child->IsVisible())
        mafNode::CopyTree(child,v);
  }

  return v;
}


/*//-------------------------------------------------------------------------
void mafNode::PrintSelf(ostream& os, vtkIndent indent)
{
	os << indent << "Number of Children: "<<this->GetNumberOfChildren()<<endl;
}*/
  
#endif