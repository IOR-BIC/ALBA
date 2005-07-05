/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNode.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-05 06:04:04 $
  Version:   $Revision: 1.36 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafRoot.h"
#include "mafEventIO.h"
#include "mafEvent.h"
#include "mafEventSource.h"
#include "mafDecl.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include "mafStorage.h"
#include "mafTagArray.h"
#include "mmgGui.h"
#include <assert.h>

#ifdef VTK_USE_ANSI_STDLIB
  #include <sstream>
#endif
#include <strstream>

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
  m_Id                  = -1; // invalid ID
  m_Gui                 = NULL;
  m_EventSource         = new mafEventSource;
  m_EventSource->SetChannel(MCH_NODE);
}

//-------------------------------------------------------------------------
mafNode::~mafNode()
//-------------------------------------------------------------------------
{
  RemoveAllLinks();
  
  // advise observers this is being destroyed
  m_EventSource->InvokeEvent(this,NODE_DESTROYED);

  // remove all the children
  RemoveAllChildren();

  SetParent(NULL);
  cppDEL(m_EventSource);
  cppDEL(m_Gui);

}

//------------------------------------------------------------------------------
void mafNode::SetId(mafID id)
//------------------------------------------------------------------------------
{
  m_Id=id;
  Modified();
}

//------------------------------------------------------------------------------
mafID mafNode::GetId() const
//------------------------------------------------------------------------------
{
  return m_Id;
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
int mafNode::InternalInitialize()
//-------------------------------------------------------------------------
{
  mafNode *root=GetRoot();
  for (mafLinksMap::iterator it=m_Links.begin();it!=m_Links.end();it++)
  {
    mmuNodeLink &link=it->second;
    if (link.m_Node==NULL&&link.m_NodeId>=0)
    {
      mafNode *node=root->FindInTreeById(link.m_NodeId);
      assert(node);
      if (node)
      {
        // attach linked node to this one
        link.m_Node=node;
        node->GetEventSource()->AddObserver(this);
      }
    }    
  }

  // initialize children
  for (int i=0;i<GetNumberOfChildren();i++)
  {
    mafNode *child=GetChild(i);
    if (child->Initialize()==MAF_ERROR)
      return MAF_ERROR;
  }
  return MAF_OK;
}
//-------------------------------------------------------------------------
void mafNode::ForwardUpEvent(mafEventBase &maf_event)
//-------------------------------------------------------------------------
{
  ForwardUpEvent(&maf_event);
}

//-------------------------------------------------------------------------
void mafNode::ForwardUpEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (m_Parent)
  {
    maf_event->SetChannel(MCH_UP);
    m_Parent->OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
void mafNode::ForwardDownEvent(mafEventBase &maf_event)
//-------------------------------------------------------------------------
{
  ForwardDownEvent(&maf_event);
}

//-------------------------------------------------------------------------
void mafNode::ForwardDownEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (GetNumberOfChildren()>0)
  {
    maf_event->SetChannel(MCH_DOWN);
    for (unsigned int i=0;i<GetNumberOfChildren();i++)
    {
      mafNode *child=m_Children[i];
      child->OnEvent(maf_event);
    }
  }
}
//-------------------------------------------------------------------------
void mafNode::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name; // force string copy
  Modified();
  mafEvent ev(this,VME_MODIFIED,this);
  ForwardUpEvent(ev);
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
unsigned long mafNode::GetNumberOfChildren() const
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
int mafNode::FindNodeIdx(const char *name)
//-------------------------------------------------------------------------
{
  for (mafID i=0;i<m_Children.size();i++)
  {
    if (mafString::Equals(m_Children[i]->GetName(),name))
	  {
	    return i;
	  }
  }
  return -1;
}
//-------------------------------------------------------------------------
mafNode *mafNode::FindInTreeByTag(const char *name,const char *value,int type)
//-------------------------------------------------------------------------
{
  mafTagItem *titem=GetTagArray()->GetTag(name);
  if (titem&&mafCString(titem->GetName())==name)
    return this;

  for (mafID i=0;i<m_Children.size();i++)
  {
    if (mafNode *node=m_Children[i]->FindInTreeByTag(name,value,type))
      return node;
  }
  return NULL;
}
//-------------------------------------------------------------------------
mafNode *mafNode::FindInTreeByName(const char *name)
//-------------------------------------------------------------------------
{
  if (mafCString(GetName())==name)
    return this;

  for (mafID i=0;i<m_Children.size();i++)
  {
    if (mafNode *node=m_Children[i]->FindInTreeByName(name))
      return node;
  }
  return NULL;
}
//-------------------------------------------------------------------------
mafNode *mafNode::FindInTreeById(const mafID id)
//-------------------------------------------------------------------------
{
  if (GetId()==id)
    return this;

  for (mafID i=0;i<m_Children.size();i++)
  {
    if (mafNode *node=m_Children[i]->FindInTreeById(id))
      return node;
  }
  return NULL;
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
void mafNode::RemoveChild(mafNode *node)
//-------------------------------------------------------------------------
{
  RemoveChild(FindNodeIdx(node));
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

//----------------------------------------------------------------------------
void mafNode::Import(mafNode *tree)
//-------------------------------------------------------------------------
{
  if (tree&&tree->GetNumberOfChildren()>0)
  {
    int num=tree->GetNumberOfChildren();
    for (int i=0;i<num;i++)
    {
      mafNode *vme=tree->GetFirstChild();
      vme->ReparentTo(this);
    }
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
  if (parent)
  {
    if (this->CanReparentTo(parent))
    {
      mafNode *old_root=(m_Parent?m_Parent->GetRoot():NULL);
      mafNode *new_root=parent->GetRoot();

      // if the Node was attached to another tree, first send detaching event
      if ( old_root && (new_root!=old_root) )
      {      
        ForwardUpEvent(&mafEventBase(this,NODE_DETACHED_FROM_TREE));
        m_EventSource->InvokeEvent(this,NODE_DETACHED_FROM_TREE);
      }

      m_Parent=parent;

      // if it's being attached to a new tree and this has 'mafRoot' root node, ask for a new Id
      mafRoot *root=mafRoot::SafeDownCast(new_root);
      
      // if attached under a new root (i.e. a new tree
      // with a root node of type mafRoot) ask for
      // a new Id and set it.
      if (old_root!=new_root)
      {
        SetId(root?root->GetNextNodeId():-1);
        if (parent->IsInitialized())
        {
          if (Initialize())
            return MAF_ERROR;
        }
      }

      // send attachment event
      ForwardUpEvent(&mafEventBase(this,NODE_ATTACHED_TO_TREE));
      m_EventSource->InvokeEvent(this,NODE_ATTACHED_TO_TREE);
      
      Modified();
      return MAF_OK;
    }

    // modified by Stefano 27-10-2004: Changed the error macro to give feedback about node names 
    mafErrorMacro("Cannot reparent the VME: " << GetName() << " under the " << parent->GetTypeName() \
      << " named " << parent->GetName());

    return MAF_ERROR;
  }
  else
  {
    // reparenting to NULL is admitted in any case
    if (m_Parent!=NULL)
    {
      // send event about detachment from the tree
      ForwardUpEvent(&mafEventBase(this,NODE_DETACHED_FROM_TREE));
      m_EventSource->InvokeEvent(this,NODE_DETACHED_FROM_TREE);

      m_Parent=parent;
      Modified();
      
    }
    return MAF_OK;
  }
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
  assert(a);
  if (this->CanCopy(a))
  {
    // Copy attributes
    m_Attributes.clear();
    for (mafAttributesMap::iterator it=a->m_Attributes.begin();it!=a->m_Attributes.end();it++)
    {
      mafAttribute *attr=it->second;
      assert(attr);
      m_Attributes[attr->GetName()]=attr->MakeCopy();
    }
    
    // member variables
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
bool mafNode::Equals(mafNode *node)
//-------------------------------------------------------------------------
{
  if (!node||!node->IsA(GetTypeName()))
    return false;

  // do not check the ID!!!

  if (m_Name != node->m_Name)
    return false;

  // check attributes
  if (m_Attributes.size()!=node->GetAttributes()->size())
    return false;

  mafAttributesMap::iterator att_it;
  mafAttributesMap::iterator att_it2;
  for (att_it=m_Attributes.begin(),att_it2=node->GetAttributes()->begin();att_it!=m_Attributes.end();att_it++,att_it2++)
  {
    if (!att_it->second->Equals(att_it2->second))
      return false;

    if (att_it2==node->GetAttributes()->end())
      return false;
  }

  // check links (poor links checking, only linked VME name)
  if (m_Links.size()!=node->GetLinks()->size())
    return false;

  mafLinksMap::iterator lnk_it;
  mafLinksMap::iterator lnk_it2;
  for (lnk_it=m_Links.begin(),lnk_it2=node->GetLinks()->begin();lnk_it!=m_Links.end();lnk_it++,lnk_it2++)
  {
    if (!(lnk_it->second.m_NodeId==lnk_it2->second.m_NodeId))
      return false;

    if (lnk_it2==node->GetLinks()->end())
      return false;
  }

  return true;
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
void mafNode::RemoveAttribute(const char *name)
//-------------------------------------------------------------------------
{
  m_Attributes.erase(m_Attributes.find(name));
}

//-------------------------------------------------------------------------
void mafNode::RemoveAllAttributes()
//-------------------------------------------------------------------------
{
  m_Attributes.clear();
}

//-------------------------------------------------------------------------
mafTagArray  *mafNode::GetTagArray()
//-------------------------------------------------------------------------
{
  mafTagArray *tarray=mafTagArray::SafeDownCast(GetAttribute("TagArray"));
  if (!tarray)
  {
    tarray=mafTagArray::New();
    tarray->SetName("TagArray");
    SetAttribute("TagArray",tarray);
  }
  return tarray;
}

//-------------------------------------------------------------------------
mafNode *mafNode::GetLink(const char *name)
//-------------------------------------------------------------------------
{
  assert(name);
  mafLinksMap::iterator it=m_Links.find(mafCString(name));
  if (it!=m_Links.end())
  {
    // if the link is still valid return its pointer
    if (it->second.m_NodeId>=0)
    {
      assert(it->second.m_Node);
      return it->second.m_Node;
    }
  }

  return NULL;
}
//-------------------------------------------------------------------------
void mafNode::SetLink(const char *name, mafNode *node)
//-------------------------------------------------------------------------
{
  assert(name);
  assert(node);

  mmuNodeLink newlink;
  if (node->GetRoot()==GetRoot())
  {
    newlink.m_NodeId=node->GetId();
  }

  mafLinksMap::iterator it=m_Links.find(mafString().Set(name));

  if (it!=m_Links.end())
  {
    // if already linked simply return
    if (it->second.m_Node==node)
      return;
   
    // detach old linked node, if present
    if (it->second.m_Node)
      it->second.m_Node->GetEventSource()->RemoveObserver(this);
  }

  // set the link to the new node
  m_Links[name]=mmuNodeLink(node->GetId(),node);

  // attach as observer of the linked node to catch events
  // of de/attachment to the tree and destroy event.
  node->GetEventSource()->AddObserver(this); 
  Modified();
}
//-------------------------------------------------------------------------
void mafNode::RemoveLink(const char *name)
//-------------------------------------------------------------------------
{
  assert(name);
  mafLinksMap::iterator it=m_Links.find(mafCString(name));
  if (it!=m_Links.end())
  {
    assert(it->second.m_Node);
    // detach as observer from the linked node
    it->second.m_Node->GetEventSource()->RemoveObserver(this);
    m_Links.erase(it); // remove linked node from links container
    Modified();
  }
}

//-------------------------------------------------------------------------
void mafNode::RemoveAllLinks()
//-------------------------------------------------------------------------
{
  for (mafLinksMap::iterator it=m_Links.begin();it!=m_Links.end();it++)
  {
    assert(it->second.m_Node);
    // detach as observer from the linked node
    it->second.m_Node->GetEventSource()->RemoveObserver(this);
  }
  m_Links.clear();
  Modified();
}

//-------------------------------------------------------------------------
void mafNode::OnNodeDetachedFromTree(mafEventBase *e)
//-------------------------------------------------------------------------
{
  for (mafLinksMap::iterator it=m_Links.begin();it!=m_Links.end();it++)
  {
    if (it->second.m_Node==e->GetSender())
    {
      it->second.m_NodeId=-1; // reset Id value to invalid value
    }
  }
}

//-------------------------------------------------------------------------
void mafNode::OnNodeAttachedToTree(mafEventBase *e)
//-------------------------------------------------------------------------
{
  for (mafLinksMap::iterator it=m_Links.begin();it!=m_Links.end();it++)
  {
    if (it->second.m_Node==e->GetSender())
    {
      // restore Id value but only if node is attached to this tree!!!
      if (GetRoot()->IsInTree(it->second.m_Node))
        it->second.m_NodeId=it->second.m_Node->GetId();
    }
  }
}
//-------------------------------------------------------------------------
void mafNode::OnNodeDestroyed(mafEventBase *e)
//-------------------------------------------------------------------------
{
  for (mafLinksMap::iterator it=m_Links.begin();it!=m_Links.end();it++)
  {
    if (it->second.m_Node==e->GetSender())
    {
      it->second.m_NodeId=-1; // reset Id value
      it->second.m_Node=NULL; // set VME pointer to NULL
    }
  }
}

//-------------------------------------------------------------------------
void mafNode::OnEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (e->GetChannel()==MCH_UP)
  {
    if (mafEvent *gui_event = mafEvent::SafeDownCast(e))
    {
      if (gui_event->GetSender() == m_Gui)
      {
        switch(gui_event->GetId()) 
        {
          case ID_NAME:
          {
            //mafEvent ev(this,VME_MODIFIED,this);
            //ForwardUpEvent(ev);
            SetName(m_Name.GetCStr());
          }
          break;
          case ID_PRINT_INFO:
          {
          #ifdef VTK_USE_ANSI_STDLIB
            std::stringstream ss1;

            Print(ss1);
            wxString message = ss1.str().c_str();

            wxLogMessage("[VME PRINTOUT:]\n");

            for (int pos = message.Find('\n'); pos >= 0; pos = message.Find('\n'))
            {
              wxString tmp = message.Mid(0,pos);
              wxLogMessage(tmp);
              message = message.Mid(pos+1);
            }
          #else
            std::strstream ss1,ss2;
            Print(ss1);
            ss1 << std::ends;   // Paolo 13/06/2005: needed to close correctly the strstream
            wxLogMessage("[VME PRINTOUT:]\n%s\n", ss1.str()); 
          #endif
          }
          break;
        }
      }
      else
        ForwardUpEvent(e);
    }
    else
    {
      switch (e->GetId())
      {
        case NODE_GET_ROOT:
        {
          mafEventIO *maf_event=mafEventIO::SafeDownCast(e);
          maf_event->SetRoot(GetRoot());
        }
        break;
        default:
          ForwardUpEvent(e);
      }
    }
//    return;
  }
  else if (e->GetChannel()==MCH_DOWN)
  {
    ForwardDownEvent(e);
    return;
  }
  // events arriving directly from another node
  else if (e->GetChannel()==MCH_NODE)
  {
    switch (e->GetId())
    {
    case NODE_DETACHED_FROM_TREE:
      OnNodeDetachedFromTree(e);
    break;
    case NODE_ATTACHED_TO_TREE:
      OnNodeAttachedToTree(e);
    break;
    case NODE_DESTROYED:
      OnNodeDestroyed(e);
    break;
    }
  }
}

//-------------------------------------------------------------------------
int mafNode::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("Name",m_Name);
  parent->SetAttribute("Id",mafString(m_Id));

  // store Attributes into a tmp array
  std::vector<mafObject *> attrs;
  for (mafAttributesMap::iterator it=m_Attributes.begin();it!=m_Attributes.end();it++)
  {
    attrs.push_back(it->second);
  }
  parent->StoreObjectVector("Attributes",attrs);

  // store Links
  mafStorageElement *links_element=parent->AppendChild("Links");
  links_element->SetAttribute("NumberOfLinks",mafString(GetNumberOfLinks()));
  for (mafLinksMap::iterator links_it=m_Links.begin();links_it!=m_Links.end();links_it++)
  {
    mmuNodeLink &link=links_it->second;
    mafStorageElement *link_item_element=links_element->AppendChild("Link");
    link_item_element->SetAttribute("Name",links_it->first);
    link_item_element->SetAttribute("NodeId",mafString(link.m_NodeId));
  }

  // store the visible children into a tmp array
  std::vector<mafObject *> nodes_to_store;
  for (unsigned int i=0;i<GetNumberOfChildren();i++)
  {
    mafNode *node=GetChild(i);
    if (node->IsVisible())
    {
      nodes_to_store.push_back(node);
    }
  }
  parent->StoreObjectVector("Children",nodes_to_store,"Node");

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafNode::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  // first restore node Name
  if (node->GetAttribute("Name",m_Name))
  {
    // restore Id
    mafString id;
    if (node->GetAttribute("Id",id))
    {
      SetId((mafID)atof(id));

      // restore attributes
      RemoveAllAttributes();
      std::vector<mafObject *> attrs;
      if (node->RestoreObjectVector("Attributes",attrs)!=MAF_OK)
      {
        mafErrorMacro("Problems restoring attributes for node "<<GetName());

        // do not return MAF_ERROR when cannot restore an attribute due to missing object type
        if (node->GetStorage()->GetErrorCode()!=mafStorage::IO_WRONG_OBJECT_TYPE)
          return MAF_ERROR;
      }

      for (unsigned int i=0;i<attrs.size();i++)
      {
        mafAttribute *item=mafAttribute::SafeDownCast(attrs[i]);
        assert(item);
        if (item)
        {
          m_Attributes[item->GetName()]=item;
        }
      }
      
      // restore Links
      RemoveAllLinks();
      if (mafStorageElement *links_element=node->FindNestedElement("Links"))
      {
        mafString num_links;
        links_element->GetAttribute("NumberOfLinks",num_links);
        int n=(int)atof(num_links);
        mafStorageElement::ChildrenVector links_vector=links_element->GetChildren();
        assert(links_vector.size()==n);
        unsigned int i;
        for (i=0;i<n;i++)
        {
          mafString link_name;
          links_vector[i]->GetAttribute("Name",link_name);
          mafID link_node_id;
          links_vector[i]->GetAttributeAsInteger("NodeId",link_node_id);
          m_Links[link_name]=mmuNodeLink(link_node_id);
        }

        // restore children
        RemoveAllChildren();
        std::vector<mafObject *> children;
        if (node->RestoreObjectVector("Children",children,"Node")!=MAF_OK)
        {
          if (node->GetStorage()->GetErrorCode()!=mafStorage::IO_WRONG_OBJECT_TYPE)
            return MAF_ERROR;
          // error messaged issued by failing node
        }
        m_Children.resize(children.size());
        
        for (i=0;i<children.size();i++)
        {
          mafNode *node=mafNode::SafeDownCast(children[i]);
          assert(node);
          if (node)
          {
            node->m_Parent=this;
            m_Children[i]=node;
          }
        }
        return MAF_OK;
      }
      else
      {
        mafErrorMacro("I/O error restoring node "<<GetName()<<" of type "<<GetTypeName()<<" : problems restoring links.");
      }
    }
    else
    {
      mafErrorMacro("I/O error restoring node "<<GetName()<<" of type "<<GetTypeName()<<" : cannot found Id attribute.");
    }
  }
  else
  {
    mafErrorMacro("I/O error restoring node of type "<<GetTypeName()<<" : cannot found Name attribute.");
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
void mafNode::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  mafIndent next_indent(indent.GetNextIndent());

  Superclass::Print(os,indent);
  os << indent << "Name: \"" << m_Name.GetCStr() << "\"" << std::endl;
  os << indent << "Initialized: " << m_Initialized << std::endl;
  os << indent << "VisibleToTraverse: " << m_VisibleToTraverse << std::endl;
  os << indent << "Parent: \"" << (m_Parent?m_Parent->m_Name.GetCStr():"NULL") << "\"" << std::endl; 
  os << indent << "Number of Children: " << GetNumberOfChildren() << std::endl;
  os << indent << "Id: " << GetId() << std::endl;
  os << indent << "Attributes:\n";
  for (mafAttributesMap::const_iterator att_it=m_Attributes.begin();att_it!=m_Attributes.end();att_it++)
  {
    att_it->second->Print(os,next_indent);
  }

  os << indent << "Links:" << std::endl;
  for (mafLinksMap::const_iterator lnk_it=m_Links.begin();lnk_it!=m_Links.end();lnk_it++)
  {
    os << next_indent << "Name: " << lnk_it->first.GetCStr() << "\tNodeId: " << lnk_it->second.m_NodeId << std::endl;
  }
}
//-------------------------------------------------------------------------
char** mafNode::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafNode.xpm"
  return mafNode_xpm;
}
//-------------------------------------------------------------------------
mmgGui *mafNode::GetGui()
//-------------------------------------------------------------------------
{
  if (m_Gui==NULL) CreateGui();
  assert(m_Gui);
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafNode::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mmgGui* mafNode::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  
  mafString type_name = GetTypeName();
  m_Gui->Button(ID_PRINT_INFO, type_name, "", "Print node debug information");
  //m_Gui->Label("type: ", type_name);
  m_Gui->String(ID_NAME,"name :", &m_Name);

  return m_Gui;
}
