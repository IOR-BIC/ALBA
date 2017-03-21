/*=========================================================================

 Program: MAF2
 Module: mafVMERoot
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



#include "mafVMERoot.h"
#include "mafGUI.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttribute.h"
#include "mafTransform.h"
#include "mafVMEOutputNULL.h"
#include "mafIndent.h"
#include "mafVMEStorage.h"
#include "mafStorageElement.h"
#include "mafEventIO.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERoot)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERoot::mafVMERoot()
//-------------------------------------------------------------------------
{
	m_MaxNodeId = 0;
  m_ApplicationStamp = "";
  m_MaxItemId=-1;
	m_Storage = NULL;
  mafNEW(m_Transform);
  mafVMEOutputNULL *output=mafVMEOutputNULL::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
}

//-------------------------------------------------------------------------
mafVMERoot::~mafVMERoot()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
bool mafVMERoot::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  { 
    return m_Transform->GetMatrix()==((mafVMERoot *)vme)->m_Transform->GetMatrix();
  }
  return false;
}

//-------------------------------------------------------------------------
int mafVMERoot::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMERoot *vme_root=mafVMERoot::SafeDownCast(a);
    m_Transform->SetMatrix(vme_root->m_Transform->GetMatrix());
    return MAF_OK;
  }
  else
  {
    mafErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<GetTypeName());

    return MAF_ERROR;
  }
}


//-------------------------------------------------------------------------
void mafVMERoot::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMERoot::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}

//-------------------------------------------------------------------------
void mafVMERoot::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if (e->GetSender() == m_Gui)
    {
      if (e->GetId() == ID_APPLICATION_STAMP)
      {
        if(GetTagArray()->IsTagPresent("APP_STAMP"))
        {
          GetTagArray()->GetTag("APP_STAMP")->SetValue(m_ApplicationStamp);
        }
        else
        {
          mafTagItem app_stamp_item;
          app_stamp_item.SetName("APP_STAMP");
          app_stamp_item.SetValue(m_ApplicationStamp);
          GetTagArray()->SetTag(app_stamp_item);
        }
      }
      else
      {
        // Bug Fixing for the root GUI event management
        mafVME::OnEvent(maf_event);
      }

			GetLogicManager()->VmeModified(this);
    }
		else if(m_Storage)
			m_Storage->OnEvent(maf_event);
  }
  else if (maf_event->GetChannel()==MCH_UP)
  {
    switch (maf_event->GetId())
    {
      case VME_GET_NEWITEM_ID:
        if (mafEventIO *event_io=mafEventIO::SafeDownCast(maf_event))
        {
          event_io->SetItemId(mafVMERoot::SafeDownCast(GetRoot())->GetNextItemId()); // retrieve and return an item ID
        }
      break;
			case NODE_GET_STORAGE : 
				{
					// return the storage pointer: here the hypothesis sis the root node listener is a storage.
					mafEventIO *io_event = mafEventIO::SafeDownCast(maf_event);
					io_event->SetStorage(GetStorage());
				}
				break;
			default:
				if(m_Storage)
					m_Storage->OnEvent(maf_event);
    };
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

//-------------------------------------------------------------------------
void mafVMERoot::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafVME::Print(os,tabs);
  os << mafIndent(tabs) << "MaxItemId: " << m_MaxItemId << "\n";
}
//-------------------------------------------------------------------------
char** mafVMERoot::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMERoot.xpm"
  return mafVMERoot_xpm;
}

//----------------------------------------------------------------------------
mafVMEStorage * mafVMERoot::GetStorage() const
{
	return m_Storage;
}

//----------------------------------------------------------------------------
void mafVMERoot::SetStorage(mafVMEStorage * storage)
{
	m_Storage = storage;
	SetListener(storage);
}

//-------------------------------------------------------------------------
int mafVMERoot::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    StoreRoot(parent);
    parent->StoreInteger("MaxItemId",m_MaxItemId);
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMERoot::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  RestoreRoot(node);
  int max_item_id;
  node->RestoreInteger("MaxItemId",max_item_id);
  m_MaxItemId = max_item_id;

  if (Superclass::InternalRestore(node)==MAF_OK)
  {  
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMERoot::Update()
//-------------------------------------------------------------------------
{
  mafVME::Update();
  if(GetTagArray()->IsTagPresent("APP_STAMP"))
  {
    m_ApplicationStamp = GetTagArray()->GetTag("APP_STAMP")->GetValue();
    if (m_Gui)
    {
      m_Gui->Update();
    }
  }
}

//-------------------------------------------------------------------------
mafGUI *mafVMERoot::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  mafVME::CreateGui();

  if(GetTagArray()->IsTagPresent("APP_STAMP"))
  {
    m_ApplicationStamp = GetTagArray()->GetTag("APP_STAMP")->GetValue();
  }
  m_Gui->String(ID_APPLICATION_STAMP, "app stamp", &m_ApplicationStamp, "Tag to associate a msf file \nto a particular application.");
  m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
int mafVMERoot::StoreRoot(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
	parent->SetAttribute("MaxNodeId", mafString(m_MaxNodeId));
	return MAF_OK;
}
//-------------------------------------------------------------------------
int mafVMERoot::RestoreRoot(mafStorageElement *element)
//-------------------------------------------------------------------------
{
	mafID max_id;
	if (!element->GetAttributeAsInteger("MaxNodeId", max_id))
		return MAF_ERROR;

	SetMaxNodeId(max_id);

	return MAF_OK;
}