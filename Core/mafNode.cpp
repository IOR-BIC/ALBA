/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNode.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:15:02 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNode_cxx
#define __mafNode_cxx

#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafTemplatedVector.txx"

//-------------------------------------------------------------------------
mafNode::mafNode()
//-------------------------------------------------------------------------
{
  m_Children  = new mafTemplatedVector<mafNode>;
  m_Parent    = NULL;

  m_Initialized         = false;
  m_VisibleToTraverse   = true;
  m_Crypting            = -1;
}

//-------------------------------------------------------------------------
mafNode::~mafNode()
//-------------------------------------------------------------------------
{
  // remove all the children
  RemoveAllChildren();

  m_Children->UnRegister(this);
  SetParent(NULL);
  SetClientData(NULL);
}

//------------------------------------------------------------------------------
int mafNode::Initialize()
//------------------------------------------------------------------------------
{
  if (this->Initialized)
    return -1;

  if (this->InternalInitialize() == 0)
  {
    this->Initialized=1;
    return 0;
  }

  return -1;

}

//------------------------------------------------------------------------------
void mafNode::Shutdown()
//------------------------------------------------------------------------------
{
  if (Initialized)
  {
    InternalShutdown();
    Initialized = 0;
  }
}

//-------------------------------------------------------------------------
mafNodeIterator *mafNode::NewIterator()
//-------------------------------------------------------------------------
{
  mafNodeIterator *iter= new mafNodeIterator;
  iter->SetRootNode(this);
  return iter;
}

//-------------------------------------------------------------------------
int mafNode::GetNumberOfChildren()
//-------------------------------------------------------------------------
{
  return m_Children->GetNumberOfItems();
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
mafNode *mafNode::GetChild(vtkIdType idx)
//-------------------------------------------------------------------------
{
  mafNode *node;
  return m_Children->GetItem(idx,node)==MAF_OK?node:NULL;
}
  
//-------------------------------------------------------------------------
int mafNode::FindNodeIdx(mafNode *a)
//-------------------------------------------------------------------------
{
  int idx;
  return (m_Children->FindItem(a,idx)==MAF_OK?idx:-1);
}

//-------------------------------------------------------------------------
int mafNode::AddChild(mafNode *node)
//-------------------------------------------------------------------------
{
  if (node->SetParent(this)==MAF_OK)
  {
    if (m_Children->AppendItem(node)==MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}  

/*//-------------------------------------------------------------------------
void mafNode::ReplaceChild(int idx,mafNode *node)
//-------------------------------------------------------------------------
{
  if (idx>=0&&idx<m_GetNumberOfChildren())
  {
    mafNode *oldnode=m_GetChild(idx);
    if (oldnode)
    {
      oldnode->SetParent(NULL);
      m_Children->SetItem(idx,node);
      node->SetParent(this);
    }
  }
}
*/
//-------------------------------------------------------------------------
void mafNode::RemoveChild(int idx)
//-------------------------------------------------------------------------
{  
  mafNode *oldnode=this->GetChild(idx);
  if (oldnode)
  {
    assert(oldnode->GetParent()==this)
    oldnode->SetParent(NULL);
    m_Children->RemoveItem(idx);
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
    // We must keep the oldparent pointer somewhere since it is oeverwritten
    // by AddChild.
    mafNode *oldparent=m_Parent;

    if (newparent)
    {
      newparent->AddChild(this);
    }
    else
    {
      this->SetParent(NULL);
    }

    if (oldparent)
    {
      oldparent->RemoveChild(this);
    }

    return MAF_OK;
  }
  else
  {
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
mafRootNode *mafRootNode::GetRoot()
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
      if (o!=m_Parent->m_Children)
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
  
  m_Children->RemoveAllItems();
}

//-------------------------------------------------------------------------
int mafNode::SetParent(mafNode *parent)
//-------------------------------------------------------------------------
{
  m_Parent=parent;

  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafNode::SetParent(mafNode *parent)
//-------------------------------------------------------------------------
{
  if (mflVME *parent_node=mafNode::SafeDownCast(parent))
  {
    if (this->CanReparentTo(parent_node)==MAF_OK)
    {  
      m_Parent=parent_node;

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
bool mflVME::CompareTree(mflVME *vme)
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
mflVME *mflVME::CopyTree(mflVME *vme, mflVME *parent)
//-------------------------------------------------------------------------
{
  
  mflVME* v = vme->MakeCopy();

  v->ReparentTo(parent);

  if (parent)
  {
    v->Delete();
  }

  for(int i=0; i<vme->GetNumberOfChildren(); i++)
  {
    if (mflVME *child=vme->GetChild(i))
      if (child->IsVisible())
        mflVME::CopyTree(child,v);
  }

  return v;
}

//-------------------------------------------------------------------------
mflVME *mflVME::ReparentTo(mflVME *newparent)
//-------------------------------------------------------------------------
{
  // We cannot reparent to a subnode!!!
  if (!this->IsInTree(newparent))
  {
    // When we reparent to a different tree, or we simply
    // cut a tree, pre travers the sub tree to read data into memory
    // future release should read one item at once, write it
    // to disk and then release the data, or better simply copy the file
    // into the new place, this to be able to manage HUGE datasets.
    if (newparent==NULL||this->GetRoot()!=newparent->GetRoot())
    {
      mflVMEIterator *iter=this->NewIterator();
      for (mflVME *vme=iter->GetFirstNode();vme;vme=iter->GetNextNode())
      {
        for (int i=0;i<vme->GetNumberOfItems();i++)
        {
          mflVMEItem *item=vme->GetItem(i);
          if (item)
          {
            // read the data from disk and set the Id to -1
            // to advise the reader to write it again on disk.
            // Also remove the old file name...
            vtkDataSet *data=item->GetData();
            if (data)
            {
              item->SetId(-1);
              item->SetFileName("");
            }
          }
          else
          {
            vtkGenericWarningMacro("Debug: found NULL node");
          }
        }
      }

      iter->Delete();
    }

    // Add this node to the new parent children list and
    // remove it from old parent children list.
    // We first add it to the new parent, thus it is registered
    // from the new parent, the we remove it from the list of the old parent.
    // We must keep the oldparent pointer somewhere since it is oeverwritten
    // by AddChild.
    
    mflVME *oldparent=this->GetParent();

    this->Register(this);

    if (newparent)
    {
      if (newparent->AddChild(this)==VTK_ERROR)
      {
        vtkErrorMacro("Cannot Reparent node "<<this->GetName()<<" to node "<<newparent->GetName());
        return NULL;
      }

      this->SetCurrentTime(newparent->GetCurrentTime()); // update data & pose to parent CurrentTime
    }
    else
    {
      if (this->SetParent(NULL)==VTK_ERROR)
      {
        vtkErrorMacro("Cannot Reparent node "<<this->GetName()<<" to NULL");
        return NULL;
      }
    }

    if (this->AbsMatrixPipe)
    {
      this->AbsMatrixPipe->SetVME(this);
      this->AbsMatrixPipe->Update();
    }

    if (oldparent)
    {
      oldparent->RemoveChild(this);
    }

    mflVME *ret=(this->ReferenceCount==1)?NULL:this;
    
    this->UnRegister(this);

    return ret;
  }
  else
  {
    return NULL;
  }
}



/*//-------------------------------------------------------------------------
void mafNode::PrintSelf(ostream& os, vtkIndent indent)
{
	os << indent << "Number of Children: "<<this->GetNumberOfChildren()<<endl;
}*/
  
#endif