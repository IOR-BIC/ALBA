/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVME
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

#include "albaVME.h"
#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVMEItem.h"
#include "albaVMEOutput.h"
#include "albaAbsMatrixPipe.h"
#include "albaMatrixPipe.h"
#include "albaDataPipe.h"
#include "albaEventIO.h"
#include "albaEvent.h"
#include "albaTagArray.h"
#include "albaOBB.h"
#include "albaTransform.h"
#include "mmuTimeSet.h"
#include "albaIndent.h"
#include "albaStorageElement.h"
#include "albaVMEIterator.h"
#include "albaVMERoot.h"
#include <assert.h>
#include "albaStorage.h"
#include "wx\tokenzr.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEFactory.h"

//-------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaVME)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVME::albaVME()
{
	m_Parent = NULL;
	m_Initialized = false;
	m_DependsOnLinkedNode = false;
	m_VisibleToTraverse = true;
	m_Id = -1; // invalid ID
	m_Gui = NULL;
	SetChannel(MCH_NODE);

  m_TestMode = false;

  m_Output        = NULL;
  m_Behavior      = NULL;

  m_AbsMatrixPipe = albaAbsMatrixPipe::New();

  m_CurrentTime   = 0.0;
  m_Crypting      = 0;
	m_VMEExpandedWhenStored = true;

  m_VisualMode = DEFAULT_VISUAL_MODE;
}

//-------------------------------------------------------------------------
albaVME::~albaVME()
{
  // Pipes must be destroyed in the right order
  // to take into consideration dependencies
  cppDEL(m_Output);

  m_DataPipe=NULL; // smart pointer
  
  m_AbsMatrixPipe->SetVME(NULL);
  m_AbsMatrixPipe=NULL; // smart pointer
    
  m_MatrixPipe=NULL; // smart pointer

	RemoveAllLinks();
	RemoveAllBackLinks();

	// advise observers this is being destroyed
	InvokeEvent(this, NODE_DESTROYED);

	// remove all the children
	RemoveAllChildren();

	SetParent(NULL);
	cppDEL(m_Gui);
}

//-------------------------------------------------------------------------
int albaVME::InternalInitialize()
{
	albaVME *root = GetRoot();

	//Setting pointers to links 
	for (albaLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		albaVMELink &link = it->second;
		if (link.m_Node == NULL && link.m_NodeId >= 0)
		{
			albaVME *node = root->FindInTreeById(link.m_NodeId);
			if (node)
			{
				// attach linked node to this one
				link.m_Node = node;
				node->AddObserver(this);
				
				// if this is a mandatory link we need to recreate the back link on target VME
				if (link.m_Type == MANDATORY_LINK)
					node->AddBackLink(it->first, this);
			}
		}
	}

	//Back compatibility code for landmark clouds
	//We reassign the link sub id to the a new link
	for (int i = 0; i < m_OldSubIdLinks.size(); i++)
	{
		albaOldSubIdLink oldLink = m_OldSubIdLinks[i];

		albaVMELandmarkCloud *cloud = albaVMELandmarkCloud::SafeDownCast(root->FindInTreeById(oldLink.m_NodeId));
		if (cloud)
		{
			//If we are opening an old closed landmark cloud we need to call initialize to create the children landmark VMEs
			cloud->Initialize();
			albaVME *lm = (albaVME*) cloud->GetLandmark(oldLink.m_NodeSubId);
			SetLink(oldLink.m_Name, lm);
		}
	}
	m_OldSubIdLinks.clear();
	
	// initialize children
	for (int i = 0; i < GetNumberOfChildren(); i++)
	{
		albaVME *child = GetChild(i);
		if (child->Initialize() == ALBA_ERROR)
			return ALBA_ERROR;
	}

  assert(m_AbsMatrixPipe);
  // force the abs matrix pipe to update its inputs
  m_AbsMatrixPipe->SetVME(this);

  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVME::DeepCopy(albaVME *a)
{ 
	assert(a);
	if (this->CanCopy(a))
	{
		// Copy attributes
		m_Attributes.clear();
		for (albaAttributesMap::iterator it = a->m_Attributes.begin(); it != a->m_Attributes.end(); it++)
		{
			albaAttribute *attr = it->second;
			assert(attr);
			m_Attributes[attr->GetName()] = attr->MakeCopy();
		}

		// member variables
		SetName(a->GetName());

		// Copy links
		albaLinksMap::iterator lnk_it;
		for (lnk_it = a->GetLinks()->begin(); lnk_it != a->GetLinks()->end(); lnk_it++)
		{
			SetLink(lnk_it->first, lnk_it->second.m_Node, lnk_it->second.m_Type);
		}

		SetMatrixPipe(a->GetMatrixPipe() ? a->GetMatrixPipe()->MakeACopy() : NULL);
		SetDataPipe(a->GetDataPipe() ? a->GetDataPipe()->MakeACopy() : NULL);

		// Runtime properties
		SetTimeStamp(a->GetTimeStamp());

		SetMatrix(*a->GetOutput()->GetMatrix());

		return ALBA_OK;
	}
	else
  {
    albaErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME of type "<<GetTypeName());

    return ALBA_ERROR;
  }
}

//-------------------------------------------------------------------------
int albaVME::ShallowCopy(albaVME *a)
{  
  // for basic VME ShallowCopy is the same of DeepCopy (no data stored inside)
  return DeepCopy(a);
}

//-------------------------------------------------------------------------
bool albaVME::Equals(albaVME *vme)
{
	if (!vme || !vme->IsA(GetTypeName()))
		return false;

	// do not check the ID!!!

	if (m_Name != vme->m_Name)
		return false;

	// check attributes
	if (m_Attributes.size() != vme->GetAttributes()->size())
		return false;

	albaAttributesMap::iterator att_it;
	albaAttributesMap::iterator att_it2;
	for (att_it = m_Attributes.begin(), att_it2 = vme->GetAttributes()->begin(); att_it != m_Attributes.end(); att_it++, att_it2++)
	{
		if (!att_it->second->Equals(att_it2->second))
			return false;

		if (att_it2 == vme->GetAttributes()->end())
			return false;
	}

	// check links (poor links checking, only linked VME name)
	if (m_Links.size() != vme->GetLinks()->size())
		return false;

	albaLinksMap::iterator lnk_it;
	albaLinksMap::iterator lnk_it2;
	for (lnk_it = m_Links.begin(), lnk_it2 = vme->GetLinks()->begin(); lnk_it != m_Links.end(); lnk_it++, lnk_it2++)
	{
		if (!(lnk_it->second.m_NodeId == lnk_it2->second.m_NodeId))
			return false;

		if (lnk_it2 == vme->GetLinks()->end())
			return false;
	}

  if (GetTimeStamp() == vme->GetTimeStamp())
  {
    if (GetParent())
    {
      if (GetOutput()->GetAbsMatrix()->Equals(vme->GetOutput()->GetAbsMatrix()))
      {
        return true;
      }
    }
    else
    {
      if (GetOutput()->GetMatrix()->Equals(vme->GetOutput()->GetMatrix()))
      {
        return true;
      }
    }
  }

  return false;
}

//-------------------------------------------------------------------------
int albaVME::SetParent(albaVME *parent)
{
	if (parent)
	{
		if (this->CanReparentTo(parent))
		{
			albaVME *old_root = (m_Parent ? m_Parent->GetRoot() : NULL);
			albaVME *new_root = parent->GetRoot();
						
			// if the Node was attached to another tree, first send detaching event
			if (old_root && (new_root != old_root))
			{
				ForwardUpEvent(&albaEventBase(this, NODE_DETACHED_FROM_TREE));
				InvokeEvent(this, NODE_DETACHED_FROM_TREE);
			}

			m_Parent = parent;

			// if it's being attached to a new tree and this has 'albaRoot' root node, ask for a new Id
			albaVMERoot *root = albaVMERoot::SafeDownCast(new_root);

			// if attached under a new root (i.e. a new tree
			// with a root node of type albaRoot) ask for
			// a new Id and set it.
			if (old_root != new_root)
			{
				if (root)
				{
					//SetId(root->GetNextNodeId());
					// Update the Ids also to the imported subtree
					albaVMEIterator *iter = NewIterator();
					for (albaVME *n = iter->GetFirstNode(); n; n = iter->GetNextNode())
					{
						n->UpdateId();
					}
					albaDEL(iter);
				}
				else
				{
					SetId(-1);
				}
				if (parent->IsInitialized())
				{
					if (Initialize())
						return ALBA_ERROR;
				}
			}

			Modified();

			m_AbsMatrixPipe->SetVME(this);

			// if the Node was attached to another tree, first attaching event
			if (old_root && (new_root != old_root))
			{
				ForwardUpEvent(&albaEventBase(this, NODE_ATTACHED_TO_TREE));
				InvokeEvent(this, NODE_ATTACHED_TO_TREE);
			}

			return ALBA_OK;
		}

		// modified by Stefano 27-10-2004: Changed the error macro to give feedback about node names 
		albaErrorMacro("Cannot reparent the VME: " << GetName() << " under the " << parent->GetTypeName() \
			<< " named " << parent->GetName());

		return ALBA_ERROR;
	}
	else
	{
		// re parenting to NULL is admitted in any case
		if (m_Parent != NULL)
		{
			// send event about detachment from the tree
			ForwardUpEvent(&albaEventBase(this, NODE_DETACHED_FROM_TREE));
			InvokeEvent(this, NODE_DETACHED_FROM_TREE);

			m_Parent = parent;
			Modified();
		}
		return ALBA_OK;
	}
}

//-------------------------------------------------------------------------
//Set the time for this VME (not for the whole tree) without notifying listeners. 
//This method is called by SetTimeStamp method (that also notifies listeners).
/*virtual*/ void albaVME::InternalSetTimeStamp(albaTimeStamp t)
{
	 if (t<0)
    t=0;

  m_CurrentTime = t;

  // Must keep a time variable also on the
  // pipes to allow multiple pipes contemporary 
  // working at different times
  // 
  if (m_DataPipe)
    m_DataPipe->SetTimeStamp(t);

  if (m_MatrixPipe)
    m_MatrixPipe->SetTimeStamp(t);
  
  if (m_AbsMatrixPipe)
    m_AbsMatrixPipe->SetTimeStamp(t);

  Modified();
}

//-------------------------------------------------------------------------
void albaVME::SetTimeStamp(albaTimeStamp t)
{
	InternalSetTimeStamp(t);

  // TODO: consider if to add a flag to disable event issuing
  InvokeEvent(this, VME_TIME_SET);
}

//-------------------------------------------------------------------------
albaTimeStamp albaVME::GetTimeStamp() 
{
  return m_CurrentTime;
}

//-------------------------------------------------------------------------
void albaVME::SetTreeTime(albaTimeStamp t)
{
	//BES: 26.11.2012 - avoid calling SetTimeStamp because it notifies our listeners
	//before all VMEs are correctly set, which would cause time inconsistency between VMEs
  this->OnEvent(&albaEventBase(this,VME_TIME_SET,&t, MCH_DOWN));

	//now all VMEs have consistent times, so notify our listeners
	this->OnEvent(&albaEventBase(this,VME_TIME_SET, NULL, MCH_DOWN));
}

//-------------------------------------------------------------------------
bool albaVME::IsAnimated()
{
  return false;
}

//-------------------------------------------------------------------------
bool albaVME::IsDataAvailable()
{
  return true;
}

//-------------------------------------------------------------------------
int albaVME::GetNumberOfLocalTimeStamps()
{
  std::vector<albaTimeStamp> timestamps;
  GetLocalTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
int albaVME::GetNumberOfTimeStamps()
{
  std::vector<albaTimeStamp> timestamps;
  GetTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
void albaVME::GetTimeStamps(std::vector<albaTimeStamp> &kframes)
{
  GetLocalTimeStamps(kframes);
  
  std::vector<albaTimeStamp> subKFrames;

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    if (albaVME *vme=GetChild(i))
    {
      vme->GetTimeStamps(subKFrames);
    }

    mmuTimeSet::Merge(kframes,subKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
void albaVME::GetAbsTimeStamps(std::vector<albaTimeStamp> &kframes)
{
  GetLocalTimeStamps(kframes);
  
  std::vector<albaTimeStamp> parentKFrames;

  for (albaVME *parent=GetParent() ; parent ; parent=parent->GetParent())
  {
    parent->GetLocalTimeStamps(parentKFrames);

    mmuTimeSet::Merge(kframes,parentKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
bool albaVME::CanReparentTo(albaVME *parent)
{
  return (parent == NULL)|| (!IsInTree(parent));
}

//-------------------------------------------------------------------------
void albaVME::SetPose(const albaMatrix &mat,albaTimeStamp t)
{
  albaMatrix new_mat(mat);
  new_mat.SetTimeStamp(t);
  SetMatrix(new_mat);
}
//-------------------------------------------------------------------------
void albaVME::SetPose(double x,double y,double z,double rx,double ry,double rz, albaTimeStamp t)
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetPose(txyz,trxyz,t);
}
//-------------------------------------------------------------------------
void albaVME::SetPose(double xyz[3],double rxyz[3], albaTimeStamp t)
{
  t=(t<0)?t=m_CurrentTime:t;

  albaMatrix matrix;

  albaTransform::SetOrientation(matrix,rxyz);
  albaTransform::SetPosition(matrix,xyz);
  matrix.SetTimeStamp(t);
 
  SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void albaVME::ApplyMatrix(const albaMatrix &matrix,int premultiply,albaTimeStamp t)
{
  t=(t<0)?m_CurrentTime:t;

  albaTransform new_pose;
  albaMatrix pose;
  GetOutput()->GetMatrix(pose,t);
  new_pose.SetMatrix(pose);
  new_pose.Concatenate(matrix,premultiply);
  new_pose.SetTimeStamp(t);
  SetMatrix(new_pose.GetMatrix());
}

//-------------------------------------------------------------------------
void albaVME::SetAbsPose(double x,double y,double z,double rx,double ry,double rz, albaTimeStamp t)
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetAbsPose(txyz,trxyz,t);
}
//-------------------------------------------------------------------------
void albaVME::SetAbsPose(double xyz[3],double rxyz[3], albaTimeStamp t)
{
  t=(t<0)?m_CurrentTime:t;
  
  albaMatrix matrix;

  albaTransform::SetOrientation(matrix,rxyz);
  albaTransform::SetPosition(matrix,xyz);
  matrix.SetTimeStamp(t);

  SetAbsMatrix(matrix);
}

//-------------------------------------------------------------------------
void albaVME::SetAbsMatrix(const albaMatrix &matrix,albaTimeStamp t)
{
  t=(t<0)?m_CurrentTime:t;

  albaMatrix mat=matrix;
  mat.SetTimeStamp(t);
  SetAbsMatrix(mat);
}
//-------------------------------------------------------------------------
void albaVME::SetAbsMatrix(const albaMatrix &matrix)
{
  if (GetParent())
  {
    albaMatrix pmat;
    GetParent()->GetOutput()->GetAbsMatrix(pmat,matrix.GetTimeStamp());

    pmat.Invert();

    albaMatrix::Multiply4x4(pmat,matrix,pmat);

    //inherit timestamp from user provided!
    pmat.SetTimeStamp(matrix.GetTimeStamp()); //modified by Vladik Aranov 25-03-2005
    
    SetMatrix(pmat);
    return;
  }
  
  SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void albaVME::ApplyAbsMatrix(const albaMatrix &matrix,int premultiply,albaTimeStamp t)
{
  t=(t<0)?m_CurrentTime:t;
  albaTransform new_pose;
  albaMatrix pose;
  GetOutput()->GetAbsMatrix(pose,t);
  new_pose.SetMatrix(pose);
  new_pose.Concatenate(matrix,premultiply);
  new_pose.SetTimeStamp(t);
  SetAbsMatrix(new_pose.GetMatrix());
}

//-------------------------------------------------------------------------
void albaVME::SetOutput(albaVMEOutput *output)
{
  cppDEL(m_Output);

  m_Output=output;
  
  if (m_Output)
  {
    m_Output->SetVME(this);
  }
  
  // force the update of the abs matrix pipe
  if (m_AbsMatrixPipe.GetPointer())
    m_AbsMatrixPipe->SetVME(this);
}

//-------------------------------------------------------------------------
int albaVME::SetMatrixPipe(albaMatrixPipe *mpipe)
{
  if (mpipe!=m_MatrixPipe)
  {
    if (mpipe==NULL||mpipe->SetVME(this)==ALBA_OK)
    { 
      if (m_MatrixPipe)
      {
        // detach the old pipe
        m_MatrixPipe->SetVME(NULL);
        m_MatrixPipe->SetTimeStamp(m_CurrentTime);
      }
      
      m_MatrixPipe = mpipe;

      if (mpipe)
      {
        mpipe->SetVME(this);
        mpipe->SetTimeStamp(GetTimeStamp());
      }
      
      // this forces the the pipe to Update its input and input frame
      if (m_AbsMatrixPipe)
        m_AbsMatrixPipe->SetVME(this);

      InvokeEvent(this, VME_MATRIX_CHANGED);

      return ALBA_OK;
    }
    else
    {
      return ALBA_ERROR;
    }
  }

  return ALBA_OK;
}

//-------------------------------------------------------------------------
void albaVME::Update()
{
  //InternalPreUpdate();
  //InternalUpdate();

  if (GetMatrixPipe())
    GetMatrixPipe()->Update();

  if (GetDataPipe())
    GetDataPipe()->Update();

  m_VisualMode = IsDataAvailable() ? DEFAULT_VISUAL_MODE : NO_DATA_VISUAL_MODE;
}

//-------------------------------------------------------------------------
void albaVME::SetCrypting(int crypting)
{
	if (m_Crypting != crypting)
	{
		m_Crypting = (crypting > 0) ? 1 : 0;

		if (m_Gui != NULL)
		{
			m_Gui->Update();
		}

		Modified();

		GetLogicManager()->VmeModified(this);
	}
}

//-------------------------------------------------------------------------
int albaVME::GetCrypting()
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
int albaVME::SetDataPipe(albaDataPipe *dpipe)
{
  if (dpipe==m_DataPipe.GetPointer())
    return ALBA_OK;

  if (dpipe==NULL||dpipe->SetVME(this)==ALBA_OK)
  { 
    // if we had an observer...
    if (m_DataPipe)
    {
      // detach the old pipe
      m_DataPipe->SetVME(NULL);
    }

    m_DataPipe = dpipe;
    
    if (m_DataPipe)
    {
      m_DataPipe->SetVME(this);
      m_DataPipe->SetTimeStamp(m_CurrentTime);
    }

    // advise listeners the data pipe has changed
    InvokeEvent(this, VME_OUTPUT_DATA_CHANGED);

    return ALBA_OK;
  }
  else
  {
    return ALBA_ERROR;
  }
}

//-------------------------------------------------------------------------
void albaVME::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
		NodeOnEvent(alba_event);
  }
  else if (alba_event->GetId() == albaVMEItem::VME_ITEM_DATA_MODIFIED)
  {
    // Paolo 25-05-2007: Intercept the item data modified to update the output
    this->GetOutput()->Update();
  }
  else if (alba_event->GetChannel()==MCH_DOWN)
  {
		switch (alba_event->GetId())
		{
		case VME_TIME_SET:
			{
				albaTimeStamp* pTS = ((albaTimeStamp *)alba_event->GetData());
				if (pTS != NULL) {	//valid timestamp passed, change the current time of this VME
					InternalSetTimeStamp(*pTS);
				} 
				else {	//no valid timestamp passed, so this is notification that time has been changed, notify our listeners						
					InvokeEvent(this, VME_TIME_SET);
				}
				break;
			}			
		}

		//Forward the event to our children (default behaviour of albaVME, which is our parent)
		NodeOnEvent(alba_event);
  }
  else if (alba_event->GetChannel()==MCH_UP)
  {
    switch (alba_event->GetId())
    {
      case VME_OUTPUT_DATA_PREUPDATE:      
        InternalPreUpdate();  // self process the event
        InvokeEvent(alba_event); // forward event to observers
      break;
      case VME_OUTPUT_DATA_UPDATE:
        InternalUpdate();   // self process the event
        InvokeEvent(alba_event); // forward event to observers
      break;
      case VME_MATRIX_UPDATE:
			{
				albaEventBase absEvent(this, VME_ABSMATRIX_UPDATE);
				if (alba_event->GetSender() == m_AbsMatrixPipe)
				{
					InvokeEvent(&absEvent);
				}
				else
        {
					InvokeEvent(alba_event); // forward event to observers
        }

				for (int i = 0; i < this->GetNumberOfChildren(); i++)
				{
					GetChild(i)->InvokeEvent(&absEvent);
				}
			}
      break;
      default:
				NodeOnEvent(alba_event);
    }
  }
  else if (alba_event->GetChannel() == MCH_NODE)
  {
		NodeOnEvent(alba_event);
  }
}

//-------------------------------------------------------------------------
int albaVME::InternalStore(albaStorageElement *parent)
{
  parent->SetAttribute("Name", m_Name);
	parent->SetAttribute("Id", albaString(m_Id));

	// store Attributes into a tmp array
	std::vector<albaObject *> attrs;
	for (albaAttributesMap::iterator it = m_Attributes.begin(); it != m_Attributes.end(); it++)
	{
		attrs.push_back(it->second);
	}
	parent->StoreObjectVector("Attributes", attrs);

	//Store Links
	albaStorageElement *links_element = parent->AppendChild("Links");
	links_element->SetAttribute("NumberOfLinks", albaString(GetNumberOfLinks()));
	for (albaLinksMap::iterator links_it = m_Links.begin(); links_it != m_Links.end(); links_it++)
	{
		albaVMELink &link = links_it->second;
		if (link.m_Node)
		{
			albaStorageElement *link_item_element = links_element->AppendChild("Link");
			link_item_element->SetAttribute("Name", links_it->first);
			link_item_element->SetAttribute("NodeId", link.m_Node->GetId());
			link_item_element->SetAttribute("linkType", (albaID)link.m_Type);
		}
	}

	// store the visible children into a tmp array
	std::vector<albaObject *> nodes_to_store;
	for (unsigned int i = 0; i < GetNumberOfChildren(); i++)
	{
		albaVME *node = GetChild(i);
		if (node->IsVisible())
		{
			nodes_to_store.push_back(node);
		}
	}
	parent->StoreObjectVector("Children", nodes_to_store, "Node");

  parent->SetAttribute("Crypting",albaString(m_Crypting));

	parent->SetAttribute("ExpandedVME", albaString(GetLogicManager()->IsVMEExpanded(this)));

  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVME::InternalRestore(albaStorageElement *node)
{
	//First restore node Name
	if (!node->GetAttribute("Name", m_Name))
	{
		albaErrorMacro("I/O error restoring node of type " << GetTypeName() << " : cannot found Name attribute.");
		return ALBA_ERROR;
	}
	m_GuiName = m_Name;

	//Restore Id
	albaString id;
	if (!node->GetAttribute("Id", id))
	{
		albaErrorMacro("I/O error restoring node " << GetName() << " of type " << GetTypeName() << " : cannot found Id attribute.");
		return ALBA_ERROR;
	}
	SetId((albaID)atof(id));

	//Restore attributes
	RemoveAllAttributes();
	std::vector<albaObject *> attrs;
	if (node->RestoreObjectVector("Attributes", attrs) != ALBA_OK)
	{
		albaErrorMacro("Problems restoring attributes for node " << GetName());
		// do not return ALBA_ERROR when cannot restore an attribute due to missing object type
		if (node->GetStorage()->GetErrorCode() != albaStorage::IO_WRONG_OBJECT_TYPE)
			return ALBA_ERROR;
	}
	for (unsigned int i = 0; i < attrs.size(); i++)
	{
		albaAttribute *item = albaAttribute::SafeDownCast(attrs[i]);
		assert(item);
		if (item)
		{
			m_Attributes[item->GetName()] = item;
		}
	}

	//Restore Links
	RemoveAllLinks();
	if (albaStorageElement *links_element = node->FindNestedElement("Links"))
	{
		albaString num_links;
		links_element->GetAttribute("NumberOfLinks", num_links);
		int n = (int)atof(num_links);
		albaStorageElement::ChildrenVector links_vector = links_element->GetChildren();
		if (links_vector.size() != n)
		{
			albaWarningMessage("There is a problem with this file.\nThe link number of %s is wrong, this may lead in loosing of data.\n", GetName());
			n = links_vector.size();
		}

		unsigned int i;
		for (i = 0; i < n; i++)
		{
			albaString link_name;
			links_vector[i]->GetAttribute("Name", link_name);
			albaID link_node_id, link_node_subid,link_type;
			links_vector[i]->GetAttributeAsInteger("NodeId", link_node_id);
			
			//Restoring link type old links without link type are treated as normal links
			int hasLinkType = links_vector[i]->GetAttributeAsInteger("linkType", link_type);
			if (!hasLinkType)
				link_type = NORMAL_LINK;

			int hasSubId=links_vector[i]->GetAttributeAsInteger("NodeSubId", link_node_subid);

			hasSubId = hasSubId && (link_node_subid != -1);

			if ((link_node_id != -1) && hasSubId)
			{
				SetOldSubIdLink(link_name, link_node_id, link_node_subid);
			}
			else if (link_node_id != -1)
			{
				//Adding link, node pointer now is set to NULL and will be updated on InternalInitialize()
				m_Links[link_name] = albaVMELink(link_node_id, NULL,(LinkType)link_type);
			}
		}
	}

	//Clear BackLinks, Backlinks are restored from linker vme.
	RemoveAllBackLinks();

	//Restore children
	RemoveAllChildren();
	std::vector<albaObject *> children;
	if (node->RestoreObjectVector("Children", children, "Node") != ALBA_OK && node->GetStorage()->GetErrorCode() != albaStorage::IO_WRONG_OBJECT_TYPE)
	{
		albaErrorMacro("Problems restoring children for node " << GetName());
		return ALBA_ERROR;
	}

	node->GetAttributeAsInteger("ExpandedVME", m_VMEExpandedWhenStored);


	for (int i = 0; i < children.size(); i++)
	{
		albaVME *node = albaVME::SafeDownCast(children[i]);
		assert(node);
		if (node)
		{
			node->m_Parent = this;
			AddChild(node);
		}
	}

	//Restore crypt value
	albaID crypt;
	node->GetAttributeAsInteger("Crypting", crypt);
	SetCrypting(crypt);

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaVME::SetOldSubIdLink(albaString link_name, albaID link_node_id, albaID link_node_subid)
{
	m_OldSubIdLinks.push_back(albaOldSubIdLink(link_name, link_node_id, link_node_subid));
}

//-------------------------------------------------------------------------
albaGUI *albaVME::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	albaString type = GetTypeName();
	albaString typeName = GetTypeNameFromType(type);
	albaEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetString(&type);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	ForwardUpEvent(buildHelpGui);

	
	if (buildHelpGui.GetArg() == true)
		m_Gui->ButtonAndHelp(ID_PRINT_INFO, ID_HELP, typeName, "Print node debug information");
	else
		m_Gui->Button(ID_PRINT_INFO, typeName, "", "Print node debug information");

	m_Gui->Divider(1);
	m_Gui->String(ID_NAME, "Name", &m_GuiName);
	m_Gui->Divider();

  return m_Gui;
}

//-------------------------------------------------------------------------
void albaVME::SetVisualMode(int mode)
{
  if (m_VisualMode != mode)
  {
    m_VisualMode = mode;
// 		albaEvent updateModalityEvent(this, VME_VISUAL_MODE_CHANGED, this);
// 		NodeOnEvent(&updateModalityEvent);
		GetLogicManager()->VmeVisualModeChanged(this);
  }
}

//------------------------------------------------------------------------------
void albaVME::SetId(albaID id)
{
	m_Id = id;
	Modified();
}

//------------------------------------------------------------------------------
albaID albaVME::GetId() const
{
	return m_Id;
}

//------------------------------------------------------------------------------
int albaVME::Initialize()
{
	if (m_Initialized)
		return ALBA_OK;

	//Setting m_Initialized to true before internalInitialize to avoid loops
	m_Initialized = true;

	if (this->InternalInitialize() != ALBA_OK)
	{
		m_Initialized = false;
		return ALBA_ERROR;
	}
	
	return ALBA_OK;
}

//------------------------------------------------------------------------------
void albaVME::Shutdown()
{
	if (m_Initialized)
	{
		InternalShutdown();
		m_Initialized = false;
	}
}

//-------------------------------------------------------------------------
void albaVME::ForwardUpEvent(albaEventBase &alba_event)
{
	ForwardUpEvent(&alba_event);
}
//-------------------------------------------------------------------------
void albaVME::ForwardUpEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
	if (m_Parent)
	{
		alba_event->SetChannel(MCH_UP);
		m_Parent->OnEvent(alba_event);
	}
	else if (this->IsA("albaVMERoot"))
	{
		alba_event->SetChannel(MCH_UP);
		OnEvent(alba_event);
	}
}
//-------------------------------------------------------------------------
void albaVME::ForwardDownEvent(albaEventBase &alba_event)
{
	ForwardDownEvent(&alba_event);
}

//-------------------------------------------------------------------------
void albaVME::ForwardDownEvent(albaEventBase *alba_event)
{
	if (GetNumberOfChildren()>0)
	{
		alba_event->SetChannel(MCH_DOWN);
		for (unsigned int i = 0; i<GetNumberOfChildren(); i++)
		{
			albaVME *child = m_Children[i];
			child->OnEvent(alba_event);
		}
	}
}

//-------------------------------------------------------------------------
void albaVME::SetName(const char *name)
{
	m_GuiName=m_Name=name; // force string copy
	Modified();

	GetLogicManager()->VmeModified(this);
}

//-------------------------------------------------------------------------
albaVMEIterator *albaVME::NewIterator()
{
	albaVMEIterator *iter = albaVMEIterator::New();
	iter->SetRootNode(this);
	return iter;
}

//-------------------------------------------------------------------------
unsigned long albaVME::GetNumberOfChildren() const
{
	return m_Children.size();
}

//-------------------------------------------------------------------------
unsigned long albaVME::GetNumberOfChildren(bool onlyVisible /*=false*/)
{
	//This function is redefined because the original is defined const and 
	//here we call non-const functions
	if (onlyVisible)
	{
		unsigned long visibleNodes = 0;
		//counting visible nodes
		for (int i = 0; i<m_Children.size(); i++)
			if (m_Children[i].GetPointer()->IsVisible())
				visibleNodes++;
		return visibleNodes;
	}
	else
	{
		return m_Children.size();
	}
}

//-------------------------------------------------------------------------
bool albaVME::IsAChild(albaVME *a)
{
	return (a->GetParent() == this);
}

//-------------------------------------------------------------------------
albaVME *albaVME::GetFirstChild(bool onlyVisible /*=false*/)
{
	if (onlyVisible)
	{
		//searching for first visible node
		for (int i = 0; i<m_Children.size(); i++)
			if (m_Children[i].GetPointer()->IsVisible())
				return m_Children[i].GetPointer();
		//if no visible node was found return NULL
		return NULL;
	}
	else
	{
		return this->GetChild(0);
	}
}

//-------------------------------------------------------------------------
albaVME *albaVME::GetLastChild(bool onlyVisible /*=false*/)
{
	if (onlyVisible)
	{
		//searching for last visible node
		for (int i = m_Children.size() - 1; i >= 0; i--)
			if (m_Children[i].GetPointer()->IsVisible())
				return m_Children[i].GetPointer();
		//if no visible node was found return NULL
		return NULL;
	}
	else
	{
		return this->GetChild(this->GetNumberOfChildren() - 1);
	}
}

//-------------------------------------------------------------------------
albaVME * albaVME::GetChild(albaID idx, bool onlyVisible /*=false*//*=false*/)
{
	if (onlyVisible)
	{
		albaID currentVisible = -1;
		for (int i = 0; i<m_Children.size(); i++)
			if (m_Children[i].GetPointer()->IsVisible())
			{
				currentVisible++;
				if (currentVisible == idx)
					return m_Children[i].GetPointer();
			}
		//if node was found return NULL
		return NULL;
	}
	else
	{
		return (idx >= 0 && idx<m_Children.size()) ? m_Children[idx].GetPointer() : NULL;
	}
}

//-------------------------------------------------------------------------
int albaVME::FindNodeIdx(albaVME *a, bool onlyVisible /*=false*/)
{
	int nChild = -1;
	for (albaID i = 0; i<m_Children.size(); i++)
	{
		//if onlyVisible is true we count only Visible VME 
		if (!onlyVisible || m_Children[i].GetPointer()->IsVisible())
			nChild++;
		if (m_Children[i].GetPointer() == a)
		{
			return nChild;
		}
	}
	return -1;
}
//-------------------------------------------------------------------------
int albaVME::FindNodeIdx(const char *name, bool onlyVisible /*=false*/)
{
	int nChild = -1;
	for (albaID i = 0; i<m_Children.size(); i++)
	{
		//if onlyVisible is true we count only Visible VME 
		if (!onlyVisible || m_Children[i].GetPointer()->IsVisible())
			nChild++;
		if (albaString::Equals(m_Children[i]->GetName(), name))
		{
			return nChild;
		}
	}
	return -1;
}
//-------------------------------------------------------------------------
albaVME *albaVME::FindInTreeByTag(const char *name, const char *value, int type)
{
	albaTagItem *titem = GetTagArray()->GetTag(name);
	if (titem&&albaCString(titem->GetName()) == name)
		return this;

	for (albaID i = 0; i<m_Children.size(); i++)
	{
		if (albaVME *node = m_Children[i]->FindInTreeByTag(name, value, type))
			return node;
	}
	return NULL;
}
//-------------------------------------------------------------------------
albaVME *albaVME::FindInTreeByName(const char *name, bool match_case, bool whole_word)
{
	wxString word_to_search;
	word_to_search = name;
	wxString myName = GetName();

	if (!match_case)
	{
		word_to_search.MakeLower();
		myName.MakeLower();
	}

	if (whole_word)
	{
		if (myName == word_to_search)
		{
			return this;
		}
	}
	else
	{
		if (myName.Find(word_to_search) != -1)
		{
			return this;
		}
	}

	for (albaID i = 0; i < m_Children.size(); i++)
	{
		if (albaVME *node = m_Children[i]->FindInTreeByName(name, match_case, whole_word))
			return node;
	}
	return NULL;
}
//-------------------------------------------------------------------------
albaVME *albaVME::FindInTreeById(const albaID id)
{
	if (GetId() == id)
		return this;

	for (albaID i = 0; i<m_Children.size(); i++)
	{
		if (albaVME *node = m_Children[i]->FindInTreeById(id))
			return node;
	}
	return NULL;
}

//-------------------------------------------------------------------------
int albaVME::AddChild(albaVME *node)
{
	if (node->SetParent(this) == ALBA_OK)
	{
		m_Children.push_back(node);
		// send attachment event from the child node
		node->ForwardUpEvent(&albaEventBase(node, NODE_ATTACHED_TO_TREE));
		node->InvokeEvent(this, NODE_ATTACHED_TO_TREE);

		Modified();
		return ALBA_OK;
	}
	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
void albaVME::RemoveChild(albaVME *node)
{
	RemoveChild(FindNodeIdx(node));
}
//-------------------------------------------------------------------------
void albaVME::RemoveChild(const albaID idx, bool onlyVisible /*=false*/)
{
	albaVME *oldnode = GetChild(idx, onlyVisible);
	if (oldnode)
	{
		oldnode->Shutdown();

		// when called by ReparentTo the parent is already changed
		if (oldnode->GetParent() == this)
		{
			oldnode->SetParent(NULL);
		}
		else
		{
			albaErrorMacro("Wrong Parent pointer found in child node while removing it: should point to \"" << (m_Parent ? m_Parent->GetName() : "(NULL)") << "\", instead points to " << (oldnode->GetParent() ? oldnode->GetParent()->GetName() : "(NULL)") << "\"");
		}
		m_Children.erase(m_Children.begin() + idx);

		GetLogicManager()->VmeRemoved();

		Modified();
	}
	else
	{
		albaWarningMacro("Trying to remove a child node with wrong index: " << idx);
	}
}

//-------------------------------------------------------------------------
int albaVME::ReparentTo(albaVME *newparent)
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
		albaVME *oldparent = m_Parent;

		if (oldparent != newparent)
		{
			// self register to preserve from destruction
			Register(this);

			if (oldparent)
			{
				oldparent->RemoveChild(this);
			}

			if (newparent)
			{
				//Set m_Parent to old parent in order to avoid new id generation
				//when we reparent we don't need new id and m_Parent will be updated from AddChild
				m_Parent = oldparent;
				if (newparent->AddChild(this) == ALBA_ERROR)
					return ALBA_ERROR;
			}
			else
			{
				this->SetParent(NULL);
			}

			// remove self registration
			UnRegister(this);
		}

		return ALBA_OK;
	}
	else
	{
		return ALBA_ERROR;
	}
}

//----------------------------------------------------------------------------
void albaVME::Import(albaVME *tree)
{
	if (tree&&tree->GetNumberOfChildren()>0)
	{
		int num = tree->GetNumberOfChildren();
		for (int i = 0; i<num; i++)
		{
			albaVME *vme = tree->GetFirstChild();
			vme->ReparentTo(this);
		}
	}
}

//-------------------------------------------------------------------------
albaVME *albaVME::GetRoot()
{
	albaVME *node;
	for (node = this; node->GetParent(); node = node->GetParent());
	return node;
}

//-------------------------------------------------------------------------
bool albaVME::IsInTree(albaVME *a)
{
	for (albaVME* node = a; node; node = node->GetParent())
	{
		if (this == node)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------
void albaVME::UnRegister(void *o)
{
	if (this->m_ReferenceCount <= 1)
	{
		// m_Parent should already be set to NULL when deallocating memory
		if (m_Parent)
		{
			albaWarningMacro("Deallocating a node still attached to the tree, detaching it immediatelly");
			m_Parent->RemoveChild(this);
			return;
		}
	}

	Superclass::UnRegister(o);
}

//-------------------------------------------------------------------------
void albaVME::CleanTree()
{
	for (unsigned long i = 0; i<this->GetNumberOfChildren(); i++)
	{
		albaVME *curr = this->GetChild(i);
		if (curr)
			curr->CleanTree();
	}

	this->RemoveAllChildren();

}

//-------------------------------------------------------------------------
void albaVME::RemoveAllChildren()
{
	for (unsigned long i = 0; i<this->GetNumberOfChildren(); i++)
	{
		albaVME *curr = this->GetChild(i);
		if (curr)
			curr->SetParent(NULL);
	}

	m_Children.clear();
}

//-------------------------------------------------------------------------
albaVME *albaVME::MakeCopy(albaVME *a)
{
	albaVME* newnode = a->NewInstance();
	if (newnode)
		newnode->DeepCopy(a);

	return newnode;
}

//-------------------------------------------------------------------------
bool albaVME::CanCopy(albaVME *node)
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
bool albaVME::CompareTree(albaVME *vme)
{
	if (!this->Equals(vme))
		return false;

	if (vme->GetNumberOfChildren() != this->GetNumberOfChildren())
	{
		return false;
	}

	for (unsigned long i = 0; i<this->GetNumberOfChildren(); i++)
	{
		if (!this->GetChild(i)->CompareTree(vme->GetChild(i)))
		{
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------
albaVME *albaVME::CopyTree(albaVME *vme, albaVME *parent)
{
	std::vector<VmePointerMap> vmePointerMap;

	albaVMEIterator *iter = vme->NewIterator();
	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		// Save Old VME Pointer
		VmePointerMap value;
		value.oldPointer = node;
		vmePointerMap.push_back(value);
	}
	iter->Delete();

	//////////////////////////////////////////////////////////////////////////
	// Make Copy of Tree
	albaVME *copy = InternalCopyTree(vme, parent);

	copy->Register(NULL); //temp registration to avoid destruction on iterator delete 

	//////////////////////////////////////////////////////////////////////////	
	albaVMEIterator *copyIter = copy->NewIterator();

	int index = 0;
	for (albaVME *node = copyIter->GetFirstNode(); node; node = copyIter->GetNextNode())
	{
		// Find new VME pointer
		vmePointerMap[index].newPointer = node;
		index++;
	}
	copyIter->Delete();

	//////////////////////////////////////////////////////////////////////////
	std::vector<VmeLinks> linkToUpdate;

	albaVMEIterator *copyIter2 = copy->NewIterator();
	for (albaVME *node = copyIter2->GetFirstNode(); node; node = copyIter2->GetNextNode())
	{
		linkToUpdate.clear();

		for (albaVME::albaLinksMap::iterator it = node->GetLinks()->begin(); it != node->GetLinks()->end(); it++)
		{
			bool found = false;

			void *oldPointer = it->second.m_Node;
			for (int i = 0; i < vmePointerMap.size(); i++)
			{
				if (oldPointer == vmePointerMap[i].oldPointer)
				{
					// Update new Link Pointers
					VmeLinks newLink;
					newLink.name = it->first.GetCStr();
					newLink.vme = vmePointerMap[i].newPointer;
					linkToUpdate.push_back(newLink);
					found = true;
					break;
				}
			}
		}

		// Update new Links
		for (int i = 0; i < linkToUpdate.size(); i++)
			node->SetLink(linkToUpdate[i].name, linkToUpdate[i].vme);
	}
	copyIter2->Delete();
	
	linkToUpdate.clear();

	copy->SetReferenceCount(copy->GetReferenceCount() - 1); // this hack avoid that 'v' node die when return

	//////////////////////////////////////////////////////////////////////////
	return copy;
}

//-------------------------------------------------------------------------
albaVME *albaVME::InternalCopyTree(albaVME * vme, albaVME * parent)
{
	albaVME *v = vme->MakeCopy();
	v->Register(NULL);
	v->ReparentTo(parent);

	for (unsigned long i = 0; i < vme->GetNumberOfChildren(); i++)
	{
		if (albaVME *child = vme->GetChild(i))
			if (child->IsVisible())
				albaVME::InternalCopyTree(child, v);
	}
	v->SetReferenceCount(v->GetReferenceCount() - 1); // this hack avoid that 'v' node die when return

	return v;
}

//-------------------------------------------------------------------------
void albaVME::SetAttribute(const char *name, albaAttribute *a)
{
	m_Attributes[name] = a;
}

//-------------------------------------------------------------------------
albaAttribute *albaVME::GetAttribute(const char *name)
{
	albaAttributesMap::iterator it = m_Attributes.find(name);
	return (it != m_Attributes.end()) ? (*it).second.GetPointer() : NULL;
}

//-------------------------------------------------------------------------
void albaVME::RemoveAttribute(const char *name)
{
	m_Attributes.erase(m_Attributes.find(name));
}

//-------------------------------------------------------------------------
void albaVME::RemoveAllAttributes()
{
	m_Attributes.clear();
}

//-------------------------------------------------------------------------
albaTagArray  *albaVME::GetTagArray()
{
	albaTagArray *tarray = albaTagArray::SafeDownCast(GetAttribute("TagArray"));
	if (!tarray)
	{
		tarray = albaTagArray::New();
		tarray->SetName("TagArray");
		SetAttribute("TagArray", tarray);
	}
	return tarray;
}

//-------------------------------------------------------------------------
albaVME *albaVME::GetLink(const char *name)
{
	assert(name);
	albaLinksMap::iterator it = m_Links.find(albaCString(name));
	if (it != m_Links.end())
	{
		// if the link is still valid return its pointer
		// Check node validity instead of checking 'm_NodeId'
		// then if m_NodeId is different from m_Id, the link will
		// be updated.
		if (it->second.m_Node != NULL)
		{
			if (it->second.m_NodeId != it->second.m_Node->GetId())
			{
				it->second.m_NodeId = it->second.m_Node->GetId();
			}
			assert(it->second.m_Node);
			return it->second.m_Node;
		}
	}

	return NULL;
}

//-------------------------------------------------------------------------
void albaVME::SetLink(const char *name, albaVME *node)
{
	assert(name);
	
	//Remove current link if exist	
	RemoveLink(name);

	if (node)
	{
		// set the link to the new node
		m_Links[name] = albaVMELink(node->GetId(), node);

		// attach as observer of the linked node to catch events
		// of de/attachment to the tree and destroy event.
		node->AddObserver(this);
	}
	Modified();
}

//----------------------------------------------------------------------------
void albaVME::SetLink(const char *name, albaVME *node, LinkType type)
{
	if (type == MANDATORY_LINK)
		SetMandatoryLink(name, node);
	else
		SetLink(name, node);
}

//----------------------------------------------------------------------------
void albaVME::SetMandatoryLink(const char *name, albaVME *node)
{		
	SetLink(name, node);
	if (node)
	{
		m_Links[name].m_Type = MANDATORY_LINK;
		node->AddBackLink(name, this);
	}
}
//----------------------------------------------------------------------------
void albaVME::SetLinkToMandatory(char* link)
{
	SetMandatoryLink(link, GetLink(link));
}
//----------------------------------------------------------------------------
void albaVME::SetLinkToNonMandatory(char* link)
{
	SetLink(link, GetLink(link));
}

//----------------------------------------------------------------------------
void albaVME::AddBackLink(const char *name, albaVME *node)
{
	for (int i = 0; i < m_BackLinks.size(); i++)
	{
		//if back link already exist we only update the node id
		if (m_BackLinks[i].m_Node == node && m_BackLinks[i].m_Name.Equals(name))
		{
			return;
		}
	}

	albaVMEBackLink backLink(albaString(name), node);

	m_BackLinks.push_back(backLink);
}

//----------------------------------------------------------------------------
void albaVME::RemoveBackLink(const char *name, albaVME *node)
{
	for (int i = 0; i < m_BackLinks.size(); i++)
	{
		//if back link already exist we only update the node id
		if (m_BackLinks[i].m_Node == node && m_BackLinks[i].m_Name.Equals(name))
		{
			m_BackLinks.erase(m_BackLinks.begin() + i);
			return;
		}
	}

	albaLogMessage("Error Cannot delete backlink from %s to %s, the link does not exist", this->GetName(), node->GetName());
}

//-------------------------------------------------------------------------
void albaVME::RemoveLink(const char *name)
{
	assert(name);
	albaLinksMap::iterator it = m_Links.find(albaCString(name));
	if (it != m_Links.end())
	{
		if (it->second.m_Node && it->second.m_Type == MANDATORY_LINK)
			it->second.m_Node->RemoveBackLink(it->first, this);

		// detach as observer from the linked node
		if (it->second.m_Node)
			it->second.m_Node->RemoveObserver(this);

		m_Links.erase(it); // remove linked node from links container
		Modified();
	}
}

//-------------------------------------------------------------------------
void albaVME::RemoveAllLinks()
{
	for (albaLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		if (it->second.m_Node && it->second.m_Type == MANDATORY_LINK)
			it->second.m_Node->RemoveBackLink(it->first, this);

		// detach as observer from the linked node
		if (it->second.m_Node)
			it->second.m_Node->RemoveObserver(this);
	}
	m_Links.clear();
	Modified();
}

//----------------------------------------------------------------------------
albaVME::albaVMESet albaVME::GetDependenciesVMEs()
{
	albaVMESet dependencies;
	//use protected recursive function to calculate dependencies
	GetDependenciesVMEs(dependencies, this);
	return dependencies;
}

//----------------------------------------------------------------------------
void albaVME::GetDependenciesVMEs(albaVMESet &dependencies, albaVME *vme)
{
	//Search dependencies in children
	albaVMEIterator *iter = vme->NewIterator();
	iter->GetFirstNode(); //Skip current node to avoid infinite recursion
	for (albaVME *node = iter->GetNextNode(); node; node = iter->GetNextNode())
	{
		if (dependencies.find(node) == dependencies.end())
			GetDependenciesVMEs(dependencies, node);
	}
	albaDEL(iter);

	//Adding dependencies and sub-dependencies
	for (int i = 0; i < vme->m_BackLinks.size(); i++)
	{
		albaVME *node = vme->m_BackLinks[i].m_Node;
	
		if (dependencies.find(node) == dependencies.end())
		{
			//Calculating dependencies only if the vme is not already in the dependencies list
			dependencies.insert(node);
			GetDependenciesVMEs(dependencies, node);
		}
	}
}

//----------------------------------------------------------------------------
void albaVME::RemoveDependenciesVMEs()
{
	albaVME *root = GetRoot();
	albaVMESet dependencies = GetDependenciesVMEs();
	albaVMESet::iterator it;

	//Remove all the VMEs in the dependencies list
	for (it = dependencies.begin(); it != dependencies.end(); ++it)
	{
		albaVME *vme = *it;
		//Check if the VME is in tree because it can be already removed if is a descendant of another dependence 
		if (root->IsInTree(vme))
			vme->ReparentTo(NULL);
	}
}

//----------------------------------------------------------------------------
bool albaVME::WillBeRemovedWithDependencies(albaVME *vme)
{
	//Getting dependencies	
	albaVMESet dependenciesVMEs = GetDependenciesVMEs();

	
	//The vme will be removed if is a dependence of this VME or if is contained in a sub-tree of a dependent vme
	//search if the vme 
	while (!vme->IsA("albaVMERoot"))
	{
		if (dependenciesVMEs.find(vme) != dependenciesVMEs.end())
		{
			return true;
		}
		vme = vme->GetParent();
	}

	return false;
}

//-------------------------------------------------------------------------
unsigned long albaVME::GetMTime()
{
	unsigned long mtime = this->albaTimeStamped::GetMTime();
	if (m_DependsOnLinkedNode)
	{
		unsigned long mtimelink;
		for (albaLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
		{
			// check linked node timestamp
			if (it->second.m_Node)
			{
				mtimelink = it->second.m_Node->GetMTime();
				mtime = (mtimelink > mtime) ? mtimelink : mtime;
			}
		}
	}
	return mtime;
}

//-------------------------------------------------------------------------
void albaVME::OnNodeDetachedFromTree(albaEventBase *e)
{
	for (albaLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		if (it->second.m_Node == e->GetSender())
		{
			it->second.m_NodeId = -1; // reset Id value to invalid value
		}
	}
}

//-------------------------------------------------------------------------
void albaVME::OnNodeAttachedToTree(albaEventBase *e)
{
	for (albaLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		if (it->second.m_Node == e->GetSender())
		{
			// restore Id value but only if node is attached to this tree!!!
			if (GetRoot()->IsInTree(it->second.m_Node))
				it->second.m_NodeId = it->second.m_Node->GetId();
		}
	}
}
//-------------------------------------------------------------------------
void albaVME::OnNodeDestroyed(albaEventBase *e)
{
	for (albaLinksMap::iterator it = m_Links.begin(); it != m_Links.end();)
	{
		if (it->second.m_Node == e->GetSender())
			it = m_Links.erase(it);
		else
			++it;
	}
}

//-------------------------------------------------------------------------
void albaVME::NodeOnEvent(albaEventBase *e)
{
	// events to be sent up or down in the tree are simply forwarded
	if (e->GetChannel() == MCH_UP)
	{
		if (albaEvent *gui_event = albaEvent::SafeDownCast(e))
		{
			if (gui_event->GetSender() == m_Gui)
			{
				switch (gui_event->GetId())
				{
				case ID_NAME:
				{
					SetName(m_GuiName.GetCStr());
				}
				break;

				case ID_HELP:
				{
					albaEvent helpEvent;
					helpEvent.SetSender(this);
					albaString vmeTypeName = this->GetTypeName();
					helpEvent.SetString(&vmeTypeName);
					helpEvent.SetId(OPEN_HELP_PAGE);
					ForwardUpEvent(helpEvent);
				}
				break;

				case ID_PRINT_INFO:
				{
#ifdef VTK_USE_ANSI_STDLIB
					std::stringstream ss1;

					Print(ss1);
					wxString message = ss1.str().c_str();

					albaLogMessage("[VME PRINTOUT:]\n");

					for (int pos = message.Find('\n'); pos >= 0; pos = message.Find('\n'))
					{
						wxString tmp = message.Mid(0, pos);
						albaLogMessage(tmp.ToAscii());
						message = message.Mid(pos + 1);
					}
#else
					std::strstream ss1, ss2;
					Print(ss1);
					ss1 << std::ends;   // Paolo 13/06/2005: needed to close correctly the strstream
															//albaLogMessage("[VME PRINTOUT:]\n%s\n", ss1.str()); 
					albaLogMessage("[VME PRINTOUT:]\n");
					albaLogMessage(ss1.str());
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
				albaEventIO *alba_event = albaEventIO::SafeDownCast(e);
				alba_event->SetRoot(GetRoot());
			}
			break;
			default:
				ForwardUpEvent(e);
			}
		}
		//    return;
	}
	else if (e->GetChannel() == MCH_DOWN)
	{
		ForwardDownEvent(e);
		return;
	}
	// events arriving directly from another node
	else if (e->GetChannel() == MCH_NODE)
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

//----------------------------------------------------------------------------
albaString albaVME::GetTypeNameFromType(albaString typeName)
{
	albaVMEFactory *vmeFactory = albaVMEFactory::GetInstance();
	std::string name = vmeFactory->GetObjectTypeName(typeName);
	return name.c_str();
}

//-------------------------------------------------------------------------
void albaVME::Print(std::ostream& os, const int tabs)// const
{
	albaIndent indent(tabs);
	albaIndent next_indent(indent.GetNextIndent());

	Superclass::Print(os, indent);
	os << indent << "Name: \"" << m_Name.GetCStr() << "\"" << std::endl;
	os << indent << "Initialized: " << m_Initialized << std::endl;
	os << indent << "VisibleToTraverse: " << m_VisibleToTraverse << std::endl;
	os << indent << "Parent: \"" << (m_Parent ? m_Parent->m_Name.GetCStr() : "NULL") << "\"" << std::endl;
	os << indent << "Number of Children: " << GetNumberOfChildren() << std::endl;
	os << indent << "Id: " << GetId() << std::endl;
	os << indent << "Attributes:\n";
	for (albaAttributesMap::const_iterator att_it = m_Attributes.begin(); att_it != m_Attributes.end(); att_it++)
	{
		att_it->second->Print(os, next_indent);
	}

	os << indent << "Links:" << std::endl;
	os << indent << "Number of links:" << m_Links.size() << std::endl;
	for (albaLinksMap::const_iterator lnk_it = m_Links.begin(); lnk_it != m_Links.end(); lnk_it++)
	{
		os << next_indent << "Name: " << lnk_it->first.GetCStr() << "\tNodeId: " << lnk_it->second.m_NodeId << std::endl;
	}

	os << indent << "Current Time: " << m_CurrentTime << "\n";

	os << indent << "Output:\n";
	GetOutput()->Print(os, indent.GetNextIndent());

	os << indent << "Matrix Pipe: ";
	if (m_MatrixPipe)
	{
		os << "\n";
		m_MatrixPipe->Print(os, indent.GetNextIndent());
	}
	else
		os << std::endl;

	os << indent << "DataPipe: ";
	if (m_DataPipe) // allocate data pipe if not done yet
	{
		os << "\n";
		m_DataPipe->Print(os, indent.GetNextIndent());
	}
	else
		os << std::endl;
}
//-------------------------------------------------------------------------
char** albaVME::GetIcon()
{
#include "albaVME.xpm"
	return albaVME_xpm;
}
//-------------------------------------------------------------------------
albaGUI *albaVME::GetGui()
{
	if (m_Gui == NULL)
		CreateGui();
	assert(m_Gui);
	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVME::DeleteGui()
{
	cppDEL(m_Gui);
}

//-------------------------------------------------------------------------
void albaVME::UpdateId()
{
	// If the node was attached under another root its Id is different from -1
	// when it is attached to the new root it has to be updated anyway.
	// So 'if' test below has been commented.
	albaVMERoot *root = albaVMERoot::SafeDownCast(GetRoot());
	if (root)
	{
		SetId(root->GetNextNodeId());
	}
}

//-------------------------------------------------------------------------
albaVME * albaVME::GetByPath(const char *path, bool onlyVisible /*=true*/)
{
	wxStringTokenizer tkz(path, wxT("/"));

	albaVME *currentNode = this;
	albaVME *tmpParent;
	wxString tmpString;
	long tmpIndex;

	while (tkz.HasMoreTokens() && currentNode != NULL)
	{
		wxString token = tkz.GetNextToken();

		if (token == "next")
		{
			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				albaLogMessage("Node path error: root does not have next");
				currentNode = NULL;
				break;
			}
			//getting node index
			tmpIndex = tmpParent->FindNodeIdx(currentNode, onlyVisible);
			//Size check
			if (tmpIndex == tmpParent->GetNumberOfChildren(onlyVisible) - 1)
			{
				albaLogMessage("Node path error: asked 'next' on last node");
				currentNode = NULL;
				break;
			}
			//updating current node
			currentNode = tmpParent->GetChild(tmpIndex + 1, onlyVisible);
		}

		else if (token == "prev")
		{
			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				albaLogMessage("Node path error: root does not have next");
				currentNode = NULL;
				break;
			}
			//getting node index
			tmpIndex = tmpParent->FindNodeIdx(currentNode, onlyVisible);
			//Size check
			if (tmpIndex == 0)
			{
				albaLogMessage("Node path error: asked 'prec' on first node");
				currentNode = NULL;
				break;
			}
			//updating current node
			currentNode = tmpParent->GetChild(tmpIndex - 1, onlyVisible);
		}

		else if (token == "firstPair")
		{
			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				albaLogMessage("Node path error: root does not have next");
				currentNode = NULL;
				break;
			}

			//updating current node
			currentNode = tmpParent->GetFirstChild(onlyVisible);
		}

		else if (token == "lastPair")
		{
			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				albaLogMessage("Node path error: root does not have next");
				currentNode = NULL;
				break;
			}
			//updating current node
			currentNode = tmpParent->GetLastChild(onlyVisible);
		}

		else if (token == "firstChild")
		{
			//Root case: root does not ave next
			if (currentNode->GetNumberOfChildren(onlyVisible) == 0)
			{
				albaLogMessage("Node path error: asked 'firstChild' on no child node");
				currentNode = NULL;
				break;
			}
			//updating current node
			currentNode = currentNode->GetFirstChild(onlyVisible);
		}

		else if (token == "lastChild")
		{
			//Root case: root does not ave next
			if (currentNode->GetNumberOfChildren(onlyVisible) == 0)
			{
				albaLogMessage("Node path error: asked 'lastChild' on no child node");
				currentNode = NULL;
				break;
			}
			//updating current node
			currentNode = currentNode->GetLastChild(onlyVisible);
		}

		else if (token.StartsWith("pair["))
		{
			//checking match bracket 
			if (token[token.size() - 1] != ']')
			{
				albaLogMessage("Node path error: pair[] wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(5, token.size() - 2);

			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				albaLogMessage("Node path error: root does not have pairs");
				currentNode = NULL;
				break;
			}

			//Number checking
			if (!tmpString.IsNumber())
			{
				albaLogMessage("Node path error: wrong pair[] argument");
				currentNode = NULL;
				break;
			}

			tmpString.ToLong(&tmpIndex);

			//Checking bounds
			if (tmpIndex < 0 || tmpIndex > tmpParent->GetNumberOfChildren(onlyVisible) - 1)
			{
				albaLogMessage("Node path error: pair[] value outside bounds");
				currentNode = NULL;
				break;
			}

			currentNode = tmpParent->GetChild(tmpIndex, onlyVisible);
		}

		else if (token.StartsWith("pair{"))
		{
			//checking match bracket 
			if (token[token.size() - 1] != '}')
			{
				albaLogMessage("Node path error: pair{} wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(5, token.size() - 2);

			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				albaLogMessage("Node path error: root does not have pairs");
				currentNode = NULL;
				break;
			}

			//getting node index
			tmpIndex = tmpParent->FindNodeIdx(tmpString, onlyVisible);
			if (tmpIndex == -1)
			{
				albaLogMessage("Node path error: pair{%s}, not found", tmpString);
				currentNode = NULL;
				break;
			}

			currentNode = tmpParent->GetChild(tmpIndex, onlyVisible);
		}

		else if (token.StartsWith("child["))
		{
			//checking match bracket 
			if (token[token.size() - 1] != ']')
			{
				albaLogMessage("Node path error: child[] wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(6, token.size() - 2);

			//Number checking
			if (!tmpString.IsNumber())
			{
				albaLogMessage("Node path error: wrong child[] argument");
				currentNode = NULL;
				break;
			}

			tmpString.ToLong(&tmpIndex);

			//Checking bounds
			if (tmpIndex < 0 || tmpIndex > currentNode->GetNumberOfChildren(onlyVisible) - 1)
			{
				albaLogMessage("Node path error: child[] value outside bounds");
				currentNode = NULL;
				break;
			}

			currentNode = currentNode->GetChild(tmpIndex, onlyVisible);

		}

		else if (token.StartsWith("child{"))
		{
			//checking match bracket 
			if (token[token.size() - 1] != '}')
			{
				albaLogMessage("Node path error: child{} wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(6, token.size() - 2);

			//getting node index
			tmpIndex = currentNode->FindNodeIdx(tmpString, onlyVisible);
			if (tmpIndex == -1)
			{
				albaLogMessage("Node path error: pair{%s}, not found", tmpString);
				currentNode = NULL;
				break;
			}

			currentNode = currentNode->GetChild(tmpIndex, onlyVisible);
		}

		else if (token == "root")
		{
			currentNode = currentNode->GetRoot();
		}

		else if (token == "..")
		{
			currentNode = currentNode->GetParent();
			if (currentNode == NULL)
			{
				albaLogMessage("Node path error: root does not have parent");
				break;
			}
		}

		else if (token == ".")
		{
			//used to point to current node no update required
		}

		else
		{
			currentNode = NULL;
			albaLogMessage("Node path error: unknown token:%s", token);
			break;
		}
		tmpString = currentNode->GetName();

	}
	//While end
	return currentNode;
}
