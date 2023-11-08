/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDictionaryWidget
 Authors: Daniele Giunchi
 
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

#include "albaGUIDictionaryWidget.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaTagArray.h"

#include "albaGUIListCtrl.h"
#include "albaGUIDictionaryWidget.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMELandmarkCloud.h"

#include "albaXMLStorage.h"
#include "albaStorageElement.h"



enum DICTIONARY_WIDGET_ID
{
	LOAD_DICT = MINID,
};
//----------------------------------------------------------------------------
albaGUIDictionaryWidget::albaGUIDictionaryWidget(wxWindow *parent, int id)
//----------------------------------------------------------------------------
{  
  m_Vme   = NULL;
  m_File = "";

  m_List = new albaGUIListCtrl(parent,id,false,true);
  m_List->Show(false);
  m_List->SetListener(this);
  m_List->SetSize(wxSize(100,100));

  if(m_File != "") 
		LoadDictionary(m_File);
}
//----------------------------------------------------------------------------
albaGUIDictionaryWidget::~albaGUIDictionaryWidget()
//----------------------------------------------------------------------------
{
	m_Items.clear();
	m_Selections.clear();
  cppDEL(m_List);
}
//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
	  switch(e->GetId())
	  {
			case ITEM_SELECTED:
	      albaEventMacro(*e);
			break;
	  }
	}
}
//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::LoadDictionary()
//----------------------------------------------------------------------------
{
  wxString wild_dict	= "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
  wxString m_dict		  = albaGetApplicationDirectory().ToAscii();
  m_dict = m_dict + "\\Config\\Dictionary\\";
	wxString file       = albaGetOpenFile(m_dict,wild_dict,"Choose Dictionary File",m_List).ToAscii();
	if(file != "") LoadDictionary(file);
}
//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::LoadDictionary(wxString file)
//----------------------------------------------------------------------------
{
  m_File = file;
  
  // XML storage to restore
  albaXMLStorage restore;
  restore.SetURL(m_File);
  restore.SetFileType("DIC");
  restore.SetVersion("2.0");

  // create a new object to restore into
  albaStorableDictionary *storeDict = new albaStorableDictionary;
  restore.SetDocument(storeDict);
  restore.Restore();
  
	m_Items.clear();
	m_Selections.clear();

  m_List->Reset();
  //m_List->SetColumnLabel(0, "names already in use are displayed with the red icon");

  for(int i=0; i<storeDict->m_StrVector.size(); i++)
  {
		wxString item = storeDict->m_StrVector[i];
		m_Items.push_back(item);
		m_Selections.push_back(false);
    m_List->AddItem(item);
  }
  
  storeDict->Delete();
  ValidateAllItem();
}
//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::InitDictionary(std::vector<wxString> *strVect)
//----------------------------------------------------------------------------
{
	m_Items.clear();
	m_Selections.clear();
	m_List->Reset();

	if(strVect!=NULL)
	{ 
		for (int i = 0; i < strVect->size(); i++)
		{
			wxString item = (*strVect)[i];
			m_Items.push_back(item);
			m_Selections.push_back(false);
			m_List->AddItem(item);
		}
	}

	ValidateAllItem();
}

//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::SetCloud(albaVME *vme)
//----------------------------------------------------------------------------
{
  m_Vme = vme;
	ValidateAllItem();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::AddItem(wxString item)
//----------------------------------------------------------------------------
{
	m_Items.push_back(item);
	m_Selections.push_back(false);
	m_List->AddItem(item);
	
	ValidateAllItem();
}
//----------------------------------------------------------------------------
int albaGUIDictionaryWidget::RemoveItem(albaString itemName)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == itemName)
		{
			return RemoveItem(i);
		}
	}

	return -1;
}
//----------------------------------------------------------------------------
int albaGUIDictionaryWidget::RemoveItem(long itemId)
//----------------------------------------------------------------------------
{
	bool res = m_List->DeleteItem(itemId);

	if (res)
	{
		m_Items.erase(m_Items.begin() + itemId);
		m_Selections.erase(m_Selections.begin() + itemId);
		
		return itemId;
	}

	return -1;
}

//----------------------------------------------------------------------------
int albaGUIDictionaryWidget::GetItemIndex(albaString itemName)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == itemName)
		{
			return i;
		}
	}

	return -1;
}

albaString albaGUIDictionaryWidget::GetItemByIndex(int index)
{
	if (index >= 0 < m_Items.size())
		return m_Items[index];
	else
		return "";
}

//----------------------------------------------------------------------------
int albaGUIDictionaryWidget::UpdateItem(albaString oldItemName, albaString newItemName)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_Items.size(); i++)
	{
		if (m_Items[i] == oldItemName)
		{
			m_Items[i] = newItemName;
			m_List->SetItemLabel(i, newItemName.GetCStr());
			return i;
		}
	}

	return -1;
}
//----------------------------------------------------------------------------
int albaGUIDictionaryWidget::SelectItem(albaString itemName,bool select)
//----------------------------------------------------------------------------
{
	int retVal = -1;
	if(itemName!="")
	{
		for (int i = 0; i < m_Items.size(); i++)
		{
			m_List->DeselectItem(i);

			if (m_Items[i] == itemName)
			{
				m_List->SelectItem(i);
				m_Selections[i] = select;
				retVal=i;
			}
		}

		ValidateAllItem();
	}

	return retVal;
}
//----------------------------------------------------------------------------
int albaGUIDictionaryWidget::DeselectItem(albaString itemName)
//----------------------------------------------------------------------------
{
	return SelectItem(itemName, false);
}
//----------------------------------------------------------------------------
void albaGUIDictionaryWidget::ValidateAllItem(bool valid)
//----------------------------------------------------------------------------
{

	for (int i = 0; i < m_Items.size(); i++)
	{
		if (!m_Vme)
		{
			m_List->SetItemIcon(i, ITEM_GRAY);
		}
		else
		{
			
			albaVMELandmarkCloud* lc = (albaVMELandmarkCloud*)m_Vme;
			ITEM_ICONS icon = ITEM_GRAY;
			if (lc->FindInTreeByName(m_Items[i]))
			{
				if (m_Selections[i])
					icon = ITEM_BLUE;
				else
					icon = ITEM_RED;

			}
			m_List->SetItemIcon(i, icon);
		}
	}
}

// albaStorableDictionary ///////////////////////////////////////////////////////

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaStorableDictionary);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaStorableDictionary::albaStorableDictionary()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
albaStorableDictionary::~albaStorableDictionary()
//------------------------------------------------------------------------------
{
  m_StrVector.resize(0);
}
//------------------------------------------------------------------------------
// example of de-serialization code
int albaStorableDictionary::InternalRestore(albaStorageElement *element)
//------------------------------------------------------------------------------
{
  m_StrVector.resize(element->GetChildren()[0]->GetChildren().size());
  
  if(element->RestoreVectorN("Dictionary",m_StrVector,element->GetChildren()[0]->GetChildren().size(),"DItem"))
    return ALBA_ERROR;
  return ALBA_OK;
}
