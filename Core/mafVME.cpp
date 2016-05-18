/*=========================================================================

 Program: MAF2
 Module: mafVME
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

#include "mafVME.h"
#include "mafDecl.h"
#include "mafGUI.h"

#include "mafVMEItem.h"
#include "mafVMEOutput.h"
#include "mafAbsMatrixPipe.h"
#include "mafMatrixPipe.h"
#include "mafDataPipe.h"
#include "mafEventIO.h"
#include "mafEvent.h"
#include "mafEventSource.h"
#include "mafTagArray.h"
#include "mafOBB.h"
#include "mafTransform.h"
#include "mmuTimeSet.h"
#include "mafIndent.h"
#include "mafStorageElement.h"
#include "mafVMEIterator.h"

#include <assert.h>
#include "mafStorage.h"
#include "mafRoot.h"
#include "wx\tokenzr.h"
#include "mafVMELandmarkCloud.h"

//-------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafVME)
//-------------------------------------------------------------------------
/** IDs for the GUI */
enum NODE_WIDGET_ID
{
	ID_HELP = MINID,
	ID_LAST
};

//-------------------------------------------------------------------------
mafVME::mafVME()
//-------------------------------------------------------------------------
{
	m_Parent = NULL;
	m_Initialized = false;
	m_DependsOnLinkedNode = false;
	m_VisibleToTraverse = true;
	m_Id = -1; // invalid ID
	m_Gui = NULL;
	m_EventSource = new mafEventSource;
	m_EventSource->SetChannel(MCH_NODE);

  m_TestMode = false;

  m_Output        = NULL;
  m_Behavior      = NULL;

  m_AbsMatrixPipe = mafAbsMatrixPipe::New();

  m_CurrentTime   = 0.0;
  m_Crypting      = 0;

  m_VisualMode = DEFAULT_VISUAL_MODE;
}

//-------------------------------------------------------------------------
mafVME::~mafVME()
//-------------------------------------------------------------------------
{
  // Pipes must be destroyed in the right order
  // to take into consideration dependencies
  cppDEL(m_Output);

  m_DataPipe=NULL; // smart pointer
  
  m_AbsMatrixPipe->SetVME(NULL);
  m_AbsMatrixPipe=NULL; // smart pointer
    
  m_MatrixPipe=NULL; // smart pointer

	RemoveAllLinks();

	// advise observers this is being destroyed
	m_EventSource->InvokeEvent(this, NODE_DESTROYED);

	// remove all the children
	RemoveAllChildren();

	SetParent(NULL);
	cppDEL(m_EventSource);
	cppDEL(m_Gui);
}

//-------------------------------------------------------------------------
int mafVME::InternalInitialize()
//-------------------------------------------------------------------------
{
	mafVME *root = GetRoot();

	//Setting pointers to links
	for (mafLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		mafVMELink &link = it->second;
		if (link.m_Node == NULL && link.m_NodeId >= 0)
		{
			mafVME *node = root->FindInTreeById(link.m_NodeId);
			assert(node);
			if (node)
			{
				// attach linked node to this one
				link.m_Node = node;
				node->GetEventSource()->AddObserver(this);
			}
		}
	}

	//Back compatibility code for landmark clouds
	//We reassign the link sub id to the a new link
	for (int i = 0; i < m_OldSubIdLinks.size(); i++)
	{
		mafOldSubIdLink oldLink = m_OldSubIdLinks[i];

		mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(root->FindInTreeById(oldLink.m_NodeId));
		if (cloud)
		{
			//If we are opening an old closed landmark cloud we need to call initialize to create the children landmark VMEs
			cloud->Initialize();
			mafVME *lm = (mafVME*) cloud->GetLandmark(oldLink.m_NodeSubId);
			SetLink(oldLink.m_Name, lm);
		}
	}
	m_OldSubIdLinks.clear();


	// initialize children
	for (int i = 0; i < GetNumberOfChildren(); i++)
	{
		mafVME *child = GetChild(i);
		if (child->Initialize() == MAF_ERROR)
			return MAF_ERROR;
	}

  assert(m_AbsMatrixPipe);
  // force the abs matrix pipe to update its inputs
  m_AbsMatrixPipe->SetVME(this);

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVME::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
	assert(a);
	if (this->CanCopy(a))
	{
		// Copy attributes
		m_Attributes.clear();
		for (mafAttributesMap::iterator it = a->m_Attributes.begin(); it != a->m_Attributes.end(); it++)
		{
			mafAttribute *attr = it->second;
			assert(attr);
			m_Attributes[attr->GetName()] = attr->MakeCopy();
		}

		// member variables
		SetName(a->GetName());

		// Copy links
		mafLinksMap::iterator lnk_it;
		for (lnk_it = a->GetLinks()->begin(); lnk_it != a->GetLinks()->end(); lnk_it++)
		{
			SetLink(lnk_it->first, lnk_it->second.m_Node);
		}

		SetMatrixPipe(a->GetMatrixPipe() ? a->GetMatrixPipe()->MakeACopy() : NULL);
		SetDataPipe(a->GetDataPipe() ? a->GetDataPipe()->MakeACopy() : NULL);

		// Runtime properties
		//AutoUpdateAbsMatrix=vme->GetAutoUpdateAbsMatrix();
		SetTimeStamp(a->GetTimeStamp());

		SetMatrix(*a->GetOutput()->GetMatrix());
		//SetAbsMatrix(*vme->GetOutput()->GetAbsMatrix());

		return MAF_OK;
	}
	else
  {
    mafErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME of type "<<GetTypeName());

    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
int mafVME::ShallowCopy(mafVME *a)
//-------------------------------------------------------------------------
{  
  // for basic VME ShallowCopy is the same of DeepCopy (no data stored inside)
  return DeepCopy(a);
}

//-------------------------------------------------------------------------
bool mafVME::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
	if (!vme || !vme->IsA(GetTypeName()))
		return false;

	// do not check the ID!!!

	if (m_Name != vme->m_Name)
		return false;

	// check attributes
	if (m_Attributes.size() != vme->GetAttributes()->size())
		return false;

	mafAttributesMap::iterator att_it;
	mafAttributesMap::iterator att_it2;
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

	mafLinksMap::iterator lnk_it;
	mafLinksMap::iterator lnk_it2;
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
int mafVME::SetParent(mafVME *parent)
//-------------------------------------------------------------------------
{
	if (parent)
	{
		if (this->CanReparentTo(parent))
		{
			mafVME *old_root = (m_Parent ? m_Parent->GetRoot() : NULL);
			mafVME *new_root = parent->GetRoot();

			// if the Node was attached to another tree, first send detaching event
			if (old_root && (new_root != old_root))
			{
				ForwardUpEvent(&mafEventBase(this, NODE_DETACHED_FROM_TREE));
				m_EventSource->InvokeEvent(this, NODE_DETACHED_FROM_TREE);
			}

			m_Parent = parent;

			// if it's being attached to a new tree and this has 'mafRoot' root node, ask for a new Id
			mafRoot *root = mafRoot::SafeDownCast(new_root);

			// if attached under a new root (i.e. a new tree
			// with a root node of type mafRoot) ask for
			// a new Id and set it.
			if (old_root != new_root)
			{
				if (root)
				{
					//SetId(root->GetNextNodeId());
					// Update the Ids also to the imported subtree
					mafVMEIterator *iter = NewIterator();
					for (mafVME *n = iter->GetFirstNode(); n; n = iter->GetNextNode())
					{
						n->UpdateId();
					}
					mafDEL(iter);
				}
				else
				{
					SetId(-1);
				}
				if (parent->IsInitialized())
				{
					if (Initialize())
						return MAF_ERROR;
				}
			}

			Modified();

			m_AbsMatrixPipe->SetVME(this);

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
		if (m_Parent != NULL)
		{
			// send event about detachment from the tree
			ForwardUpEvent(&mafEventBase(this, NODE_DETACHED_FROM_TREE));
			m_EventSource->InvokeEvent(this, NODE_DETACHED_FROM_TREE);

			m_Parent = parent;
			Modified();

		}
		return MAF_OK;
	}
}

//-------------------------------------------------------------------------
//Set the time for this VME (not for the whole tree) without notifying listeners. 
//This method is called by SetTimeStamp method (that also notifies listeners).
/*virtual*/ void mafVME::InternalSetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
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
void mafVME::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{
	InternalSetTimeStamp(t);

  // TODO: consider if to add a flag to disable event issuing
  GetEventSource()->InvokeEvent(this,VME_TIME_SET);
}

//-------------------------------------------------------------------------
mafTimeStamp mafVME::GetTimeStamp() 
//-------------------------------------------------------------------------
{
  return m_CurrentTime;
}

//-------------------------------------------------------------------------
void mafVME::SetTreeTime(mafTimeStamp t)
//-------------------------------------------------------------------------
{
	//BES: 26.11.2012 - avoid calling SetTimeStamp because it notifies our listeners
	//before all VMEs are correctly set, which would cause time inconsistency between VMEs
  this->OnEvent(&mafEventBase(this,VME_TIME_SET,&t, MCH_DOWN));

	//now all VMEs have consistent times, so notify our listeners
	this->OnEvent(&mafEventBase(this,VME_TIME_SET, NULL, MCH_DOWN));
}

//-------------------------------------------------------------------------
bool mafVME::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVME::IsDataAvailable()
//-------------------------------------------------------------------------
{
  return true;
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfLocalTimeStamps()
//-------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> timestamps;
  GetLocalTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
int mafVME::GetNumberOfTimeStamps()
//-------------------------------------------------------------------------
{
  std::vector<mafTimeStamp> timestamps;
  GetTimeStamps(timestamps);
  return timestamps.size();
}

//-------------------------------------------------------------------------
void mafVME::GetTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  GetLocalTimeStamps(kframes);
  
  std::vector<mafTimeStamp> subKFrames;

  for (int i=0;i<GetNumberOfChildren();i++)
  {
    if (mafVME *vme=GetChild(i))
    {
      vme->GetTimeStamps(subKFrames);
    }

    mmuTimeSet::Merge(kframes,subKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
void mafVME::GetAbsTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  GetLocalTimeStamps(kframes);
  
  std::vector<mafTimeStamp> parentKFrames;

  for (mafVME *parent=GetParent() ; parent ; parent=parent->GetParent())
  {
    parent->GetLocalTimeStamps(parentKFrames);

    mmuTimeSet::Merge(kframes,parentKFrames,kframes);
  }
}

//-------------------------------------------------------------------------
bool mafVME::CanReparentTo(mafVME *parent)
//-------------------------------------------------------------------------
{
  return (parent == NULL)|| (!IsInTree(parent));
}

//-------------------------------------------------------------------------
void mafVME::SetPose(const mafMatrix &mat,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafMatrix new_mat(mat);
  new_mat.SetTimeStamp(t);
  SetMatrix(new_mat);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetPose(double xyz[3],double rxyz[3], mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?t=m_CurrentTime:t;

  mafMatrix matrix;

  mafTransform::SetOrientation(matrix,rxyz);
  mafTransform::SetPosition(matrix,xyz);
  matrix.SetTimeStamp(t);
 
  SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void mafVME::ApplyMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafTransform new_pose;
  mafMatrix pose;
  GetOutput()->GetMatrix(pose,t);
  new_pose.SetMatrix(pose);
  new_pose.Concatenate(matrix,premultiply);
  new_pose.SetTimeStamp(t);
  SetMatrix(new_pose.GetMatrix());
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  double txyz[3],trxyz[3];
  txyz[0]=x; txyz[1]=y; txyz[2]=z;
  trxyz[0]=rx; trxyz[1]=ry; trxyz[2]=rz;
  SetAbsPose(txyz,trxyz,t);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsPose(double xyz[3],double rxyz[3], mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  
  mafMatrix matrix;

  mafTransform::SetOrientation(matrix,rxyz);
  mafTransform::SetPosition(matrix,xyz);
  matrix.SetTimeStamp(t);

  SetAbsMatrix(matrix);
}

//-------------------------------------------------------------------------
void mafVME::SetAbsMatrix(const mafMatrix &matrix,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;

  mafMatrix mat=matrix;
  mat.SetTimeStamp(t);
  SetAbsMatrix(mat);
}
//-------------------------------------------------------------------------
void mafVME::SetAbsMatrix(const mafMatrix &matrix)
//-------------------------------------------------------------------------
{
  if (GetParent())
  {
    mafMatrix pmat;
    GetParent()->GetOutput()->GetAbsMatrix(pmat,matrix.GetTimeStamp());

    pmat.Invert();

    mafMatrix::Multiply4x4(pmat,matrix,pmat);

    //inherit timestamp from user provided!
    pmat.SetTimeStamp(matrix.GetTimeStamp()); //modified by Vladik Aranov 25-03-2005
    
    SetMatrix(pmat);
    return;
  }
  
  SetMatrix(matrix);
}

//----------------------------------------------------------------------------
void mafVME::ApplyAbsMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t)
//----------------------------------------------------------------------------
{
  t=(t<0)?m_CurrentTime:t;
  mafTransform new_pose;
  mafMatrix pose;
  GetOutput()->GetAbsMatrix(pose,t);
  new_pose.SetMatrix(pose);
  new_pose.Concatenate(matrix,premultiply);
  new_pose.SetTimeStamp(t);
  SetAbsMatrix(new_pose.GetMatrix());
}


//-------------------------------------------------------------------------
void mafVME::SetOutput(mafVMEOutput *output)
//-------------------------------------------------------------------------
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
int mafVME::SetMatrixPipe(mafMatrixPipe *mpipe)
//-------------------------------------------------------------------------
{
  if (mpipe!=m_MatrixPipe)
  {
    if (mpipe==NULL||mpipe->SetVME(this)==MAF_OK)
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

      GetEventSource()->InvokeEvent(this,VME_MATRIX_CHANGED);

      return MAF_OK;
    }
    else
    {
      return MAF_ERROR;
    }
  }

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafVME::Update()
//-------------------------------------------------------------------------
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
void mafVME::SetCrypting(int crypting)
//-------------------------------------------------------------------------
{
  if(crypting > 0)
    m_Crypting = 1;
  else
    m_Crypting = 0;

  if (m_Gui != NULL)
  {
    m_Gui->Update();
  }

  Modified();
  mafEvent ev(this,VME_MODIFIED,this);
  ForwardUpEvent(ev);
}

//-------------------------------------------------------------------------
int mafVME::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
int mafVME::SetDataPipe(mafDataPipe *dpipe)
//-------------------------------------------------------------------------
{
  if (dpipe==m_DataPipe.GetPointer())
    return MAF_OK;

  if (dpipe==NULL||dpipe->SetVME(this)==MAF_OK)
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
    GetEventSource()->InvokeEvent(this,VME_OUTPUT_DATA_CHANGED);

    return MAF_OK;
  }
  else
  {
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
void mafVME::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
		NodeOnEvent(maf_event);
  }
  else if (maf_event->GetId() == mafVMEItem::VME_ITEM_DATA_MODIFIED)
  {
    // Paolo 25-05-2007: Intercept the item data modified to update the output
    this->GetOutput()->Update();
  }
  else if (maf_event->GetChannel()==MCH_DOWN)
  {
		switch (maf_event->GetId())
		{
		case VME_TIME_SET:
			{
				mafTimeStamp* pTS = ((mafTimeStamp *)maf_event->GetData());
				if (pTS != NULL) {	//valid timestamp passed, change the current time of this VME
					InternalSetTimeStamp(*pTS);
				} 
				else {	//no valid timestamp passed, so this is notification that time has been changed, notify our listeners						
					GetEventSource()->InvokeEvent(this,VME_TIME_SET);
				}
				break;
			}			
		}

		//Forward the event to our children (default behaviour of mafVME, which is our parent)
		NodeOnEvent(maf_event);
  }
  else if (maf_event->GetChannel()==MCH_UP)
  {
    switch (maf_event->GetId())
    {
      case VME_OUTPUT_DATA_PREUPDATE:      
        InternalPreUpdate();  // self process the event
        GetEventSource()->InvokeEvent(maf_event); // forward event to observers
      break;
      case VME_OUTPUT_DATA_UPDATE:
        InternalUpdate();   // self process the event
        GetEventSource()->InvokeEvent(maf_event); // forward event to observers
      break;
      case VME_MATRIX_UPDATE:
			{
				mafEventBase absEvent(this, VME_ABSMATRIX_UPDATE);
				if (maf_event->GetSender() == m_AbsMatrixPipe)
				{
					GetEventSource()->InvokeEvent(&absEvent);
				}
				else
        {
					GetEventSource()->InvokeEvent(maf_event); // forward event to observers
        }

				for (int i = 0; i < this->GetNumberOfChildren(); i++)
				{
					GetChild(i)->GetEventSource()->InvokeEvent(&absEvent);
				}
			}
      break;
      default:
				NodeOnEvent(maf_event);
    }
  }
  else if (maf_event->GetChannel() == MCH_NODE)
  {
		NodeOnEvent(maf_event);
  }
}

//-------------------------------------------------------------------------
int mafVME::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("Name", m_Name);
	parent->SetAttribute("Id", mafString(m_Id));

	// store Attributes into a tmp array
	std::vector<mafObject *> attrs;
	for (mafAttributesMap::iterator it = m_Attributes.begin(); it != m_Attributes.end(); it++)
	{
		attrs.push_back(it->second);
	}
	parent->StoreObjectVector("Attributes", attrs);

	// store Links
	mafStorageElement *links_element = parent->AppendChild("Links");
	links_element->SetAttribute("NumberOfLinks", mafString(GetNumberOfLinks()));
	for (mafLinksMap::iterator links_it = m_Links.begin(); links_it != m_Links.end(); links_it++)
	{
		mafVMELink &link = links_it->second;
		mafStorageElement *link_item_element = links_element->AppendChild("Link");
		link_item_element->SetAttribute("Name", links_it->first);
		link_item_element->SetAttribute("NodeId", link.m_NodeId);
	}

	// store the visible children into a tmp array
	std::vector<mafObject *> nodes_to_store;
	for (unsigned int i = 0; i < GetNumberOfChildren(); i++)
	{
		mafVME *node = GetChild(i);
		if (node->IsVisible())
		{
			nodes_to_store.push_back(node);
		}
	}
	parent->StoreObjectVector("Children", nodes_to_store, "Node");

  parent->SetAttribute("Crypting",mafString(m_Crypting));
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVME::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{

	// first restore node Name
	if (node->GetAttribute("Name", m_Name))
	{
		m_GuiName = m_Name;

		// restore Id
		mafString id;
		if (node->GetAttribute("Id", id))
		{
			SetId((mafID)atof(id));

			// restore attributes
			RemoveAllAttributes();
			std::vector<mafObject *> attrs;
			if (node->RestoreObjectVector("Attributes", attrs) != MAF_OK)
			{
				mafErrorMacro("Problems restoring attributes for node " << GetName());

				// do not return MAF_ERROR when cannot restore an attribute due to missing object type
				if (node->GetStorage()->GetErrorCode() != mafStorage::IO_WRONG_OBJECT_TYPE)
					return MAF_ERROR;
			}

			for (unsigned int i = 0; i < attrs.size(); i++)
			{
				mafAttribute *item = mafAttribute::SafeDownCast(attrs[i]);
				assert(item);
				if (item)
				{
					m_Attributes[item->GetName()] = item;
				}
			}

			// restore Links
			RemoveAllLinks();
			if (mafStorageElement *links_element = node->FindNestedElement("Links"))
			{
				mafString num_links;
				links_element->GetAttribute("NumberOfLinks", num_links);
				int n = (int)atof(num_links);
				mafStorageElement::ChildrenVector links_vector = links_element->GetChildren();
				assert(links_vector.size() == n);
				unsigned int i;
				for (i = 0; i < n; i++)
				{
					mafString link_name;
					links_vector[i]->GetAttribute("Name", link_name);
					mafID link_node_id, link_node_subid;
					links_vector[i]->GetAttributeAsInteger("NodeId", link_node_id);
					int hasSubId=links_vector[i]->GetAttributeAsInteger("NodeSubId", link_node_subid);

					hasSubId = hasSubId && (link_node_subid != -1);

					if ((link_node_id != -1) && hasSubId)
					{
						SetOldSubIdLink(link_name, link_node_id, link_node_subid);
					}
					else if (link_node_id != -1)
					{
						m_Links[link_name] = mafVMELink(link_node_id, NULL);
					}
				}

				// restore children
				RemoveAllChildren();
				std::vector<mafObject *> children;
				if (node->RestoreObjectVector("Children", children, "Node") != MAF_OK)
				{
					if (node->GetStorage()->GetErrorCode() != mafStorage::IO_WRONG_OBJECT_TYPE)
						return MAF_ERROR;
					// error messaged issued by failing node
				}
				

				for (i = 0; i < children.size(); i++)
				{
					mafVME *node = mafVME::SafeDownCast(children[i]);
					assert(node);
					if (node)
					{
						node->m_Parent = this;
						AddChild(node);
					}
				}

				mafID crypt;
				node->GetAttributeAsInteger("Crypting", crypt);
				SetCrypting(crypt);

				return MAF_OK;
			}
			else
			{
				mafErrorMacro("I/O error restoring node " << GetName() << " of type " << GetTypeName() << " : problems restoring links.");
			}
		}
		else
		{
			mafErrorMacro("I/O error restoring node " << GetName() << " of type " << GetTypeName() << " : cannot found Id attribute.");
		}
	}
	else
	{
		mafErrorMacro("I/O error restoring node of type " << GetTypeName() << " : cannot found Name attribute.");
	}

	return MAF_ERROR;
}

//----------------------------------------------------------------------------
void mafVME::SetOldSubIdLink(mafString link_name, mafID link_node_id, mafID link_node_subid)
{
	m_OldSubIdLinks.push_back(mafOldSubIdLink(link_name, link_node_id, link_node_subid));
}

//-------------------------------------------------------------------------
mafGUI *mafVME::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

	mafString type_name = GetTypeName();
	if ((*GetMAFExpertMode()) == TRUE)
		m_Gui->Button(ID_PRINT_INFO, type_name, "", "Print node debug information");

	m_Gui->String(ID_NAME, "name :", &m_GuiName);

	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	ForwardUpEvent(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		m_Gui->Button(ID_HELP, "Help", "");
	}

	m_Gui->Divider();

  mafString anim_text;
  anim_text = _("not animated");
  if (IsAnimated())
  {
    anim_text = _("animated");
  }
  
  m_Gui->Label(anim_text);
  m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVME::SetVisualMode(int mode)
//-------------------------------------------------------------------------
{
  if (m_VisualMode != mode)
  {
    m_VisualMode = mode;
    mafEvent updateModalityEvent(this, VME_VISUAL_MODE_CHANGED, this);
    NodeOnEvent(&updateModalityEvent);
  }
}

//------------------------------------------------------------------------------
void mafVME::SetId(mafID id)
//------------------------------------------------------------------------------
{
	m_Id = id;
	Modified();
}

//------------------------------------------------------------------------------
mafID mafVME::GetId() const
//------------------------------------------------------------------------------
{
	return m_Id;
}

//------------------------------------------------------------------------------
int mafVME::Initialize()
//------------------------------------------------------------------------------
{
	if (m_Initialized)
		return MAF_OK;

	//Setting m_Initialized to true before internalInitialize to avoid loops
	m_Initialized = true;

	if (this->InternalInitialize() != MAF_OK)
	{
		m_Initialized = false;
		return MAF_ERROR;
	}
	
	return MAF_OK;
}

//------------------------------------------------------------------------------
void mafVME::Shutdown()
//------------------------------------------------------------------------------
{
	if (m_Initialized)
	{
		InternalShutdown();
		m_Initialized = false;
	}
}

//-------------------------------------------------------------------------
void mafVME::ForwardUpEvent(mafEventBase &maf_event)
//-------------------------------------------------------------------------
{
	ForwardUpEvent(&maf_event);
}

//-------------------------------------------------------------------------
void mafVME::ForwardUpEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	if (m_Parent)
	{
		maf_event->SetChannel(MCH_UP);
		m_Parent->OnEvent(maf_event);
	}
}
//-------------------------------------------------------------------------
void mafVME::ForwardDownEvent(mafEventBase &maf_event)
//-------------------------------------------------------------------------
{
	ForwardDownEvent(&maf_event);
}

//-------------------------------------------------------------------------
void mafVME::ForwardDownEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	if (GetNumberOfChildren()>0)
	{
		maf_event->SetChannel(MCH_DOWN);
		for (unsigned int i = 0; i<GetNumberOfChildren(); i++)
		{
			mafVME *child = m_Children[i];
			child->OnEvent(maf_event);
		}
	}
}
//-------------------------------------------------------------------------
void mafVME::SetName(const char *name)
//-------------------------------------------------------------------------
{
	m_GuiName=m_Name=name; // force string copy
	Modified();
	mafEvent ev(this, VME_MODIFIED, this);
	ForwardUpEvent(ev);
}

//-------------------------------------------------------------------------
mafVMEIterator *mafVME::NewIterator()
//-------------------------------------------------------------------------
{
	mafVMEIterator *iter = mafVMEIterator::New();
	iter->SetRootNode(this);
	return iter;
}

//-------------------------------------------------------------------------
unsigned long mafVME::GetNumberOfChildren() const
//-------------------------------------------------------------------------
{
	return m_Children.size();
}

//-------------------------------------------------------------------------
unsigned long mafVME::GetNumberOfChildren(bool onlyVisible /*=false*/)
//-------------------------------------------------------------------------
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
bool mafVME::IsAChild(mafVME *a)
//-------------------------------------------------------------------------
{
	return (a->GetParent() == this);
}

//-------------------------------------------------------------------------
mafVME *mafVME::GetFirstChild(bool onlyVisible /*=false*/)
//-------------------------------------------------------------------------
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
mafVME *mafVME::GetLastChild(bool onlyVisible /*=false*/)
//-------------------------------------------------------------------------
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
mafVME * mafVME::GetChild(mafID idx, bool onlyVisible /*=false*//*=false*/)
//-------------------------------------------------------------------------
{
	if (onlyVisible)
	{
		mafID currentVisible = -1;
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
int mafVME::FindNodeIdx(mafVME *a, bool onlyVisible /*=false*/)
//-------------------------------------------------------------------------
{
	int nChild = -1;
	for (mafID i = 0; i<m_Children.size(); i++)
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
int mafVME::FindNodeIdx(const char *name, bool onlyVisible /*=false*/)
//-------------------------------------------------------------------------
{
	int nChild = -1;
	for (mafID i = 0; i<m_Children.size(); i++)
	{
		//if onlyVisible is true we count only Visible VME 
		if (!onlyVisible || m_Children[i].GetPointer()->IsVisible())
			nChild++;
		if (mafString::Equals(m_Children[i]->GetName(), name))
		{
			return nChild;
		}
	}
	return -1;
}
//-------------------------------------------------------------------------
mafVME *mafVME::FindInTreeByTag(const char *name, const char *value, int type)
//-------------------------------------------------------------------------
{
	mafTagItem *titem = GetTagArray()->GetTag(name);
	if (titem&&mafCString(titem->GetName()) == name)
		return this;

	for (mafID i = 0; i<m_Children.size(); i++)
	{
		if (mafVME *node = m_Children[i]->FindInTreeByTag(name, value, type))
			return node;
	}
	return NULL;
}
//-------------------------------------------------------------------------
mafVME *mafVME::FindInTreeByName(const char *name, bool match_case, bool whole_word)
//-------------------------------------------------------------------------
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

	for (mafID i = 0; i < m_Children.size(); i++)
	{
		if (mafVME *node = m_Children[i]->FindInTreeByName(name, match_case, whole_word))
			return node;
	}
	return NULL;
}
//-------------------------------------------------------------------------
mafVME *mafVME::FindInTreeById(const mafID id)
//-------------------------------------------------------------------------
{
	if (GetId() == id)
		return this;

	for (mafID i = 0; i<m_Children.size(); i++)
	{
		if (mafVME *node = m_Children[i]->FindInTreeById(id))
			return node;
	}
	return NULL;
}
//-------------------------------------------------------------------------
int mafVME::AddChild(mafVME *node)
//-------------------------------------------------------------------------
{
	if (node->SetParent(this) == MAF_OK)
	{
		m_Children.push_back(node);
		// send attachment event from the child node
		node->ForwardUpEvent(&mafEventBase(node, NODE_ATTACHED_TO_TREE));
		node->GetEventSource()->InvokeEvent(node, NODE_ATTACHED_TO_TREE);

		Modified();
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-------------------------------------------------------------------------
void mafVME::RemoveChild(mafVME *node)
//-------------------------------------------------------------------------
{
	RemoveChild(FindNodeIdx(node));
}

//-------------------------------------------------------------------------
void mafVME::RemoveChild(const mafID idx, bool onlyVisible /*=false*/)
//-------------------------------------------------------------------------
{
	mafVME *oldnode = GetChild(idx, onlyVisible);
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
			mafErrorMacro("Wrong Parent pointer found in child node while removing it: should point to \"" << (m_Parent ? m_Parent->GetName() : "(NULL)") << "\", instead points to " << (oldnode->GetParent() ? oldnode->GetParent()->GetName() : "(NULL)") << "\"");
		}
		m_Children.erase(m_Children.begin() + idx);
		Modified();
	}
	else
	{
		mafWarningMacro("Trying to remove a child node with wrong index: " << idx);
	}
}

//-------------------------------------------------------------------------
int mafVME::ReparentTo(mafVME *newparent)
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
		mafVME *oldparent = m_Parent;

		if (oldparent != newparent)
		{
			// self register to preserve from distruction
			Register(this);

			if (oldparent)
			{
				oldparent->RemoveChild(this);
			}

			if (newparent)
			{
				if (newparent->AddChild(this) == MAF_ERROR)
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
void mafVME::Import(mafVME *tree)
//-------------------------------------------------------------------------
{
	if (tree&&tree->GetNumberOfChildren()>0)
	{
		int num = tree->GetNumberOfChildren();
		for (int i = 0; i<num; i++)
		{
			mafVME *vme = tree->GetFirstChild();
			vme->ReparentTo(this);
		}
	}
}

//-------------------------------------------------------------------------
mafVME *mafVME::GetRoot()
//-------------------------------------------------------------------------
{
	mafVME *node;
	for (node = this; node->GetParent(); node = node->GetParent());
	return node;
}

//-------------------------------------------------------------------------
bool mafVME::IsInTree(mafVME *a)
//-------------------------------------------------------------------------
{
	for (mafVME* node = a; node; node = node->GetParent())
	{
		if (this == node)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------
void mafVME::UnRegister(void *o)
//-------------------------------------------------------------------------
{
	if (this->m_ReferenceCount <= 1)
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
void mafVME::CleanTree()
//-------------------------------------------------------------------------
{
	for (unsigned long i = 0; i<this->GetNumberOfChildren(); i++)
	{
		mafVME *curr = this->GetChild(i);
		if (curr)
			curr->CleanTree();
	}

	this->RemoveAllChildren();

}

//-------------------------------------------------------------------------
void mafVME::RemoveAllChildren()
//-------------------------------------------------------------------------
{

	for (unsigned long i = 0; i<this->GetNumberOfChildren(); i++)
	{
		mafVME *curr = this->GetChild(i);
		if (curr)
			curr->SetParent(NULL);
	}

	m_Children.clear();
}

//-------------------------------------------------------------------------
mafVME *mafVME::MakeCopy(mafVME *a)
//-------------------------------------------------------------------------
{
	mafVME* newnode = a->NewInstance();
	if (newnode)
		newnode->DeepCopy(a);
	return newnode;
}


//-------------------------------------------------------------------------
bool mafVME::CanCopy(mafVME *node)
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
bool mafVME::CompareTree(mafVME *vme)
//-------------------------------------------------------------------------
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
mafVME *mafVME::CopyTree(mafVME *vme, mafVME *parent)
//-------------------------------------------------------------------------
{

	mafVME *v = vme->MakeCopy();
	v->Register(NULL);
	v->ReparentTo(parent);

	for (unsigned long i = 0; i<vme->GetNumberOfChildren(); i++)
	{
		if (mafVME *child = vme->GetChild(i))
			if (child->IsVisible())
				mafVME::CopyTree(child, v);
	}
	v->SetReferenceCount(v->GetReferenceCount() - 1); // this hack avoid that 'v' node die when return
	return v;
}

//-------------------------------------------------------------------------
void mafVME::SetAttribute(const char *name, mafAttribute *a)
//-------------------------------------------------------------------------
{
	m_Attributes[name] = a;
}

//-------------------------------------------------------------------------
mafAttribute *mafVME::GetAttribute(const char *name)
//-------------------------------------------------------------------------
{
	mafAttributesMap::iterator it = m_Attributes.find(name);
	return (it != m_Attributes.end()) ? (*it).second.GetPointer() : NULL;
}

//-------------------------------------------------------------------------
void mafVME::RemoveAttribute(const char *name)
//-------------------------------------------------------------------------
{
	m_Attributes.erase(m_Attributes.find(name));
}

//-------------------------------------------------------------------------
void mafVME::RemoveAllAttributes()
//-------------------------------------------------------------------------
{
	m_Attributes.clear();
}

//-------------------------------------------------------------------------
mafTagArray  *mafVME::GetTagArray()
//-------------------------------------------------------------------------
{
	mafTagArray *tarray = mafTagArray::SafeDownCast(GetAttribute("TagArray"));
	if (!tarray)
	{
		tarray = mafTagArray::New();
		tarray->SetName("TagArray");
		SetAttribute("TagArray", tarray);
	}
	return tarray;
}

//-------------------------------------------------------------------------
mafVME *mafVME::GetLink(const char *name)
//-------------------------------------------------------------------------
{
	assert(name);
	mafLinksMap::iterator it = m_Links.find(mafCString(name));
	if (it != m_Links.end())
	{
		// if the link is still valid return its pointer
		// Check node validity instead of checking 'm_NodeId'
		// then if m_NodeId is different from m_Id, the link will
		// be updated.
		if (it->second.m_Node != NULL && it->second.m_Node->IsValid())
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
void mafVME::SetLink(const char *name, mafVME *node)
//-------------------------------------------------------------------------
{
	assert(name);
	assert(node);

	if (node == NULL)
	{
		mafLogMessage(_("Warning!! NULL node can not be set as link."));
		return;
	}

	mafVMELink newlink;
	if (node->GetRoot() == GetRoot())
	{
		newlink.m_NodeId = node->GetId();
	}

	mafLinksMap::iterator it = m_Links.find(mafString().Set(name));

	if (it != m_Links.end())
	{
		// if already linked simply return
		if (it->second.m_Node == node)
			return;

		// detach old linked node, if present
		if (it->second.m_Node)
			it->second.m_Node->GetEventSource()->RemoveObserver(this);
	}

	// set the link to the new node
	m_Links[name] = mafVMELink(node->GetId(), node);

	// attach as observer of the linked node to catch events
	// of de/attachment to the tree and destroy event.
	node->GetEventSource()->AddObserver(this);
	Modified();
}
//-------------------------------------------------------------------------
void mafVME::RemoveLink(const char *name)
//-------------------------------------------------------------------------
{
	assert(name);
	mafLinksMap::iterator it = m_Links.find(mafCString(name));
	if (it != m_Links.end())
	{
		// assert(it->second.m_Node);
		// detach as observer from the linked node
		if (it->second.m_Node != NULL)
		{
			it->second.m_Node->GetEventSource()->RemoveObserver(this);
		}

		m_Links.erase(it); // remove linked node from links container
		Modified();
	}
}

//-------------------------------------------------------------------------
void mafVME::RemoveAllLinks()
//-------------------------------------------------------------------------
{
	for (mafLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		// detach as observer from the linked node
		if (it->second.m_Node)
			it->second.m_Node->GetEventSource()->RemoveObserver(this);
	}
	m_Links.clear();
	Modified();
}
//-------------------------------------------------------------------------
unsigned long mafVME::GetMTime()
//-------------------------------------------------------------------------
{
	unsigned long mtime = this->mafTimeStamped::GetMTime();
	if (m_DependsOnLinkedNode)
	{
		unsigned long mtimelink;
		for (mafLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
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
void mafVME::OnNodeDetachedFromTree(mafEventBase *e)
//-------------------------------------------------------------------------
{
	for (mafLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		if (it->second.m_Node == e->GetSender())
		{
			it->second.m_NodeId = -1; // reset Id value to invalid value
		}
	}
}

//-------------------------------------------------------------------------
void mafVME::OnNodeAttachedToTree(mafEventBase *e)
//-------------------------------------------------------------------------
{
	for (mafLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
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
void mafVME::OnNodeDestroyed(mafEventBase *e)
//-------------------------------------------------------------------------
{
	for (mafLinksMap::iterator it = m_Links.begin(); it != m_Links.end(); it++)
	{
		if (it->second.m_Node == e->GetSender())
		{
			it->second.m_NodeId = -1; // reset Id value
			it->second.m_Node = NULL; // set VME pointer to NULL
		}
	}
}

//-------------------------------------------------------------------------
void mafVME::NodeOnEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
	// events to be sent up or down in the tree are simply forwarded
	if (e->GetChannel() == MCH_UP)
	{
		if (mafEvent *gui_event = mafEvent::SafeDownCast(e))
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

					mafEvent helpEvent;
					helpEvent.SetSender(this);
					mafString vmeTypeName = this->GetTypeName();
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

					mafLogMessage("[VME PRINTOUT:]\n");

					for (int pos = message.Find('\n'); pos >= 0; pos = message.Find('\n'))
					{
						wxString tmp = message.Mid(0, pos);
						mafLogMessage(tmp.c_str());
						message = message.Mid(pos + 1);
					}
#else
					std::strstream ss1, ss2;
					Print(ss1);
					ss1 << std::ends;   // Paolo 13/06/2005: needed to close correctly the strstream
															//mafLogMessage("[VME PRINTOUT:]\n%s\n", ss1.str()); 
					mafLogMessage("[VME PRINTOUT:]\n");
					mafLogMessage(ss1.str());
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
				mafEventIO *maf_event = mafEventIO::SafeDownCast(e);
				maf_event->SetRoot(GetRoot());
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

//-------------------------------------------------------------------------
void mafVME::Print(std::ostream& os, const int tabs)// const
																										//-------------------------------------------------------------------------
{
	mafIndent indent(tabs);
	mafIndent next_indent(indent.GetNextIndent());

	Superclass::Print(os, indent);
	os << indent << "Name: \"" << m_Name.GetCStr() << "\"" << std::endl;
	os << indent << "Initialized: " << m_Initialized << std::endl;
	os << indent << "VisibleToTraverse: " << m_VisibleToTraverse << std::endl;
	os << indent << "Parent: \"" << (m_Parent ? m_Parent->m_Name.GetCStr() : "NULL") << "\"" << std::endl;
	os << indent << "Number of Children: " << GetNumberOfChildren() << std::endl;
	os << indent << "Id: " << GetId() << std::endl;
	os << indent << "Attributes:\n";
	for (mafAttributesMap::const_iterator att_it = m_Attributes.begin(); att_it != m_Attributes.end(); att_it++)
	{
		att_it->second->Print(os, next_indent);
	}

	os << indent << "Links:" << std::endl;
	os << indent << "Number of links:" << m_Links.size() << std::endl;
	for (mafLinksMap::const_iterator lnk_it = m_Links.begin(); lnk_it != m_Links.end(); lnk_it++)
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
char** mafVME::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVME.xpm"
	return mafVME_xpm;
}
//-------------------------------------------------------------------------
mafGUI *mafVME::GetGui()
//-------------------------------------------------------------------------
{
	if (m_Gui == NULL)
		CreateGui();
	assert(m_Gui);
	return m_Gui;
}
//-------------------------------------------------------------------------
void mafVME::DeleteGui()
//-------------------------------------------------------------------------
{
	cppDEL(m_Gui);
}

//-------------------------------------------------------------------------
void mafVME::UpdateId()
//-------------------------------------------------------------------------
{
	// If the node was attached under another root its Id is different from -1
	// when it is attached to the new root it has to be updated anyway.
	// So 'if' test below has been commented.
	//if (this->m_Id == -1)
	//{
	mafRoot *root = mafRoot::SafeDownCast(GetRoot());
	if (root)
	{
		SetId(root->GetNextNodeId());
	}
	//}
}

//-------------------------------------------------------------------------
mafVME * mafVME::GetByPath(const char *path, bool onlyVisible /*=true*/)
//-------------------------------------------------------------------------
{
	wxStringTokenizer tkz(wxT(path), wxT("/"));

	mafVME *currentNode = this;
	mafVME *tmpParent;
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
				mafLogMessage("Node path error: root does not have next");
				currentNode = NULL;
				break;
			}
			//getting node index
			tmpIndex = tmpParent->FindNodeIdx(currentNode, onlyVisible);
			//Size check
			if (tmpIndex == tmpParent->GetNumberOfChildren(onlyVisible) - 1)
			{
				mafLogMessage("Node path error: asked 'next' on last node");
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
				mafLogMessage("Node path error: root does not have next");
				currentNode = NULL;
				break;
			}
			//getting node index
			tmpIndex = tmpParent->FindNodeIdx(currentNode, onlyVisible);
			//Size check
			if (tmpIndex == 0)
			{
				mafLogMessage("Node path error: asked 'prec' on first node");
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
				mafLogMessage("Node path error: root does not have next");
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
				mafLogMessage("Node path error: root does not have next");
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
				mafLogMessage("Node path error: asked 'firstChild' on no child node");
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
				mafLogMessage("Node path error: asked 'lastChild' on no child node");
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
				mafLogMessage("Node path error: pair[] wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(5, token.size() - 2);

			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				mafLogMessage("Node path error: root does not have pairs");
				currentNode = NULL;
				break;
			}

			//Number checking
			if (!tmpString.IsNumber())
			{
				mafLogMessage("Node path error: wrong pair[] argument");
				currentNode = NULL;
				break;
			}

			tmpString.ToLong(&tmpIndex);

			//Checking bounds
			if (tmpIndex < 0 || tmpIndex > tmpParent->GetNumberOfChildren(onlyVisible) - 1)
			{
				mafLogMessage("Node path error: pair[] value outside bounds");
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
				mafLogMessage("Node path error: pair{} wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(5, token.size() - 2);

			tmpParent = currentNode->GetParent();
			//Root case: root does not ave next
			if (tmpParent == NULL)
			{
				mafLogMessage("Node path error: root does not have pairs");
				currentNode = NULL;
				break;
			}

			//getting node index
			tmpIndex = tmpParent->FindNodeIdx(tmpString, onlyVisible);
			if (tmpIndex == -1)
			{
				mafLogMessage("Node path error: pair{%s}, not found", tmpString);
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
				mafLogMessage("Node path error: child[] wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(6, token.size() - 2);

			//Number checking
			if (!tmpString.IsNumber())
			{
				mafLogMessage("Node path error: wrong child[] argument");
				currentNode = NULL;
				break;
			}

			tmpString.ToLong(&tmpIndex);

			//Checking bounds
			if (tmpIndex < 0 || tmpIndex > currentNode->GetNumberOfChildren(onlyVisible) - 1)
			{
				mafLogMessage("Node path error: child[] value outside bounds");
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
				mafLogMessage("Node path error: child{} wrong format");
				currentNode = NULL;
				break;
			}

			//getting the number substring
			tmpString = token.SubString(6, token.size() - 2);

			//getting node index
			tmpIndex = currentNode->FindNodeIdx(tmpString, onlyVisible);
			if (tmpIndex == -1)
			{
				mafLogMessage("Node path error: pair{%s}, not found", tmpString);
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
				mafLogMessage("Node path error: root does not have parent");
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
			mafLogMessage("Node path error: unknown token:%s", token);
			break;
		}
		tmpString = currentNode->GetName();

	}
	//While end
	return currentNode;
}
