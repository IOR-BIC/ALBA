/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNode.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-13 09:10:36 $
  Version:   $Revision: 1.10 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNode_cxx
#define __mafNode_cxx

#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafIndent.h"
#include "mmaTagArray.h"
#include <sstream>
#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_DEC(mafNode::MAF_CH_DOWNTREE)
//MAF_ID_DEC(mafNode::MAF_CH_UPTREE)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafNode)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafNode::mafNode()
//-------------------------------------------------------------------------
{
  m_Parent              = NULL;
  m_Initialized         = false;
  m_VisibleToTraverse   = true;
  m_Id                  = 0; // root ID
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
  return m_Name.c_str();
}

//-------------------------------------------------------------------------
void mafNode::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name; // force string copy
  Modified();
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
unsigned long mafNode::GetNumberOfChildren()
//-------------------------------------------------------------------------
{
  return m_Children.size();
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
  return (idx>=0&&idx<m_Children.size())?m_Children[idx].GetPointer():NULL;
}
  
//-------------------------------------------------------------------------
int mafNode::FindNodeIdx(mafNode *a)
//-------------------------------------------------------------------------
{
  for (mafID i=0;i<m_Children.size();i++)
  {
    if (m_Children[i].GetPointer()==a)
	  {
	    return i;
	  }
  }
  return -1;
}

//-------------------------------------------------------------------------
int mafNode::AddChild(mafNode *node)
//-------------------------------------------------------------------------
{
  if (node->SetParent(this)==MAF_OK)
  {
    m_Children.push_back(node);
    Modified();
    return MAF_OK;
  }
  return MAF_ERROR;
}  

//-------------------------------------------------------------------------
void mafNode::RemoveChild(const mafID idx)
//-------------------------------------------------------------------------
{  
  mafNode *oldnode=GetChild(idx);
  if (oldnode)
  {
    // when called by ReparentTo the parent is already changed
    if (oldnode->GetParent()==this)
    {
      oldnode->SetParent(NULL); 
    }
    else
    {
      mafErrorMacro("Wrong Parent pointer found in child node while removing it: should point to \""<<(m_Parent?m_Parent->GetName():"(NULL)")<<"\", instead points to "<<(oldnode->GetParent()?oldnode->GetParent()->GetName():"(NULL)")<<"\"");
    }
    m_Children.erase(m_Children.begin()+idx);
    Modified();
  }
  else
  {
    mafWarningMacro("Trying to remove a child node with wrong index: "<<idx);
  }
}

//-------------------------------------------------------------------------
int mafNode::ReparentTo(mafNode *newparent)
//-------------------------------------------------------------------------
{
  // We cannot reparent to a subnode!!!
  if (!IsInTree(newparent))
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
      // self register to preserve from distruction
      Register(this);

      if (oldparent)
      {
        oldparent->RemoveChild(this);
      }

      if (newparent)
      {
        if (newparent->AddChild(this)==MAF_ERROR)
          return MAF_ERROR;
      }
      else
      {
        this->SetParent(NULL);
      }

      // remove self registration
      UnRegister(this);
      
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
  if (this->m_ReferenceCount<=1)
  {
    // m_Parent should already be set to NULL when deallocating memory
    if (m_Parent)
    {
      mafWarningMacro("Deallocating a node still attached to the tree, detaching it immediatelly");
      m_Parent->RemoveChild(this);
      return;
    }    
  }
  
  Superclass::UnRegister(o);
}

//-------------------------------------------------------------------------
void mafNode::CleanTree()
//-------------------------------------------------------------------------
{
  for (unsigned long i=0;i<this->GetNumberOfChildren();i++)
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
  
  for (unsigned long i=0;i<this->GetNumberOfChildren();i++)
  {
    mafNode *curr=this->GetChild(i);
    if (curr)
      curr->SetParent(NULL);
  }
  
  m_Children.clear();
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
      Modified();
      return MAF_OK;
    }

    // modified by Stefano 27-10-2004: Changed the error macro to give feedback about node names 
    mafErrorMacro("Cannot reparent the VME: " << GetName() << " under the " << parent->GetTypeName() \
      << " named " << parent_node->GetName());
  }
  else
  {
    // reparenting to NULL is admitted in any case
    if (parent==NULL)
    {
      m_Parent=parent;
      Modified();
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
    mafErrorMacro("Cannot copy Node of type "<<a->GetTypeName()<<" into a Node of type " \
      << GetTypeName());

    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
bool mafNode::CanCopy(mafNode *node)
//-------------------------------------------------------------------------
{
  if (!node)
    return false;

  if (node->IsA(GetStaticTypeId()))
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

  for (unsigned long i=0;i<this->GetNumberOfChildren();i++)
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

  for(unsigned long i=0; i<vme->GetNumberOfChildren(); i++)
  {
    if (mafNode *child=vme->GetChild(i))
      if (child->IsVisible())
        mafNode::CopyTree(child,v);
  }

  return v;
}

//-------------------------------------------------------------------------
void mafNode::SetAttribute(const char *name,mafAttribute *a)
//-------------------------------------------------------------------------
{
  m_Attributes[name]=a;
}

//-------------------------------------------------------------------------
mafAttribute *mafNode::GetAttribute(const char *name)
//-------------------------------------------------------------------------
{
  mafAttributesMap::iterator it=m_Attributes.find(name);
  return (it!=m_Attributes.end())?(*it).second.GetPointer():NULL;
}

//-------------------------------------------------------------------------
mmaTagArray  *mafNode::GetTagArray()
//-------------------------------------------------------------------------
{
  mmaTagArray *tarray=mmaTagArray::SafeDownCast(GetAttribute("TagArray"));
  
  if (!tarray)
  {
    tarray=mmaTagArray::New();
    SetAttribute("TagArray",tarray);
  }

  return tarray;
}

//-------------------------------------------------------------------------
void mafNode::OnEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
  // default behavior is to send event to parent or to children depending on 
}

//-------------------------------------------------------------------------
void mafNode::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  Superclass::Print(os,indent);
  os << indent << "Name: \"" << m_Name << "\"" << std::endl;
  os << indent << "Initialized: " << m_Initialized << std::endl;
  os << indent << "VisibleToTraverse: " << m_VisibleToTraverse << std::endl;
  os << indent << "Parent: \"" << (m_Parent?m_Parent->m_Name:"NULL") << "\"" << std::endl; 
  os << indent << "Number of Children: " << GetNumberOfChildren() << std::endl;
}
  
#endif
