/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERoot
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



#include "albaVMERoot.h"
#include "albaGUI.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaAbsMatrixPipe.h"
#include "albaAttribute.h"
#include "albaTransform.h"
#include "albaVMEOutputNULL.h"
#include "albaIndent.h"
#include "albaVMEStorage.h"
#include "albaStorageElement.h"
#include "albaEventIO.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMERoot)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMERoot::albaVMERoot()
//-------------------------------------------------------------------------
{
	m_MaxNodeId = 0;
  m_ApplicationStamp = "";
  m_MaxItemId=-1;
	m_Storage = NULL;
  albaNEW(m_Transform);
  albaVMEOutputNULL *output=albaVMEOutputNULL::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
}

//-------------------------------------------------------------------------
albaVMERoot::~albaVMERoot()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
}

//-------------------------------------------------------------------------
bool albaVMERoot::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  { 
    return m_Transform->GetMatrix()==((albaVMERoot *)vme)->m_Transform->GetMatrix();
  }
  return false;
}

//-------------------------------------------------------------------------
int albaVMERoot::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMERoot *vme_root=albaVMERoot::SafeDownCast(a);
    m_Transform->SetMatrix(vme_root->m_Transform->GetMatrix());
    return ALBA_OK;
  }
  else
  {
    albaErrorMacro("Cannot copy VME of type "<<a->GetTypeName()<<" into a VME \
    VME of type "<<GetTypeName());

    return ALBA_ERROR;
  }
}


//-------------------------------------------------------------------------
void albaVMERoot::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void albaVMERoot::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
  albaTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}

//-------------------------------------------------------------------------
void albaVMERoot::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
          albaTagItem app_stamp_item;
          app_stamp_item.SetName("APP_STAMP");
          app_stamp_item.SetValue(m_ApplicationStamp);
          GetTagArray()->SetTag(app_stamp_item);
        }
      }
      else
      {
        // Bug Fixing for the root GUI event management
        albaVME::OnEvent(alba_event);
      }

			GetLogicManager()->VmeModified(this);
    }
		else if(m_Storage)
			m_Storage->OnEvent(alba_event);
  }
  else if (alba_event->GetChannel()==MCH_UP)
  {
    switch (alba_event->GetId())
    {
      case VME_GET_NEWITEM_ID:
        if (albaEventIO *event_io=albaEventIO::SafeDownCast(alba_event))
        {
          event_io->SetItemId(albaVMERoot::SafeDownCast(GetRoot())->GetNextItemId()); // retrieve and return an item ID
        }
      break;
			case NODE_GET_STORAGE : 
				{
					// return the storage pointer: here the hypothesis sis the root node listener is a storage.
					albaEventIO *io_event = albaEventIO::SafeDownCast(alba_event);
					io_event->SetStorage(GetStorage());
				}
				break;
			default:
				if(m_Storage)
					m_Storage->OnEvent(alba_event);
    };
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}

//-------------------------------------------------------------------------
void albaVMERoot::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  albaVME::Print(os,tabs);
  os << albaIndent(tabs) << "MaxItemId: " << m_MaxItemId << "\n";
}
//-------------------------------------------------------------------------
char** albaVMERoot::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMERoot.xpm"
  return albaVMERoot_xpm;
}

//----------------------------------------------------------------------------
albaVMEStorage * albaVMERoot::GetStorage() const
{
	return m_Storage;
}

//----------------------------------------------------------------------------
void albaVMERoot::SetStorage(albaVMEStorage * storage)
{
	m_Storage = storage;
	SetListener(storage);
}

//-------------------------------------------------------------------------
int albaVMERoot::InternalStore(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    StoreRoot(parent);
    parent->StoreInteger("MaxItemId",m_MaxItemId);
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMERoot::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------------------------
{
  RestoreRoot(node);
  int max_item_id;
  node->RestoreInteger("MaxItemId",max_item_id);
  m_MaxItemId = max_item_id;

  if (Superclass::InternalRestore(node)==ALBA_OK)
  {  
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
      return ALBA_OK;
    }
  }
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
void albaVMERoot::Update()
//-------------------------------------------------------------------------
{
  albaVME::Update();
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
albaGUI *albaVMERoot::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  albaVME::CreateGui();

  if(GetTagArray()->IsTagPresent("APP_STAMP"))
  {
    m_ApplicationStamp = GetTagArray()->GetTag("APP_STAMP")->GetValue();
  }
  m_Gui->String(ID_APPLICATION_STAMP, "App Stamp", &m_ApplicationStamp, "Tag to associate a msf file \nto a particular application.");
  m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
int albaVMERoot::StoreRoot(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
	parent->SetAttribute("MaxNodeId", albaString(m_MaxNodeId));
	return ALBA_OK;
}
//-------------------------------------------------------------------------
int albaVMERoot::RestoreRoot(albaStorageElement *element)
//-------------------------------------------------------------------------
{
	albaID max_id;
	if (!element->GetAttributeAsInteger("MaxNodeId", max_id))
		return ALBA_ERROR;

	SetMaxNodeId(max_id);

	return ALBA_OK;
}