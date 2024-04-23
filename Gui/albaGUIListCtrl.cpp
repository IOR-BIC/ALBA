/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIListCtrl
 Authors: Silvano Imboden
 
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


#include "albaGUIListCtrl.h"
#include "albaDecl.h"
#include "albaString.h"
#include "albaPics.h"
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
const int ID_LIST = 200;

BEGIN_EVENT_TABLE(albaGUIListCtrl,wxPanel)
    EVT_LIST_ITEM_SELECTED(ID_LIST, albaGUIListCtrl::OnSelectionChanged)
END_EVENT_TABLE()

albaGUIListCtrl::albaGUIListCtrl( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:albaGUINamedPanel(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  SetWindowStyle(wxNO_BORDER);
  SetBackgroundColour(wxColour(255,255,255));
   
  m_Listener = NULL;

  m_Images = new wxImageList(15,15,false,4);
  m_Images->Add(albaPictureFactory::GetPictureFactory()->GetBmp("NODE_YELLOW"));
  m_Images->Add(albaPictureFactory::GetPictureFactory()->GetBmp("NODE_GRAY"));
  m_Images->Add(albaPictureFactory::GetPictureFactory()->GetBmp("NODE_RED"));
  m_Images->Add(albaPictureFactory::GetPictureFactory()->GetBmp("NODE_BLUE"));

  m_List = new wxListCtrl(this,ID_LIST,wxDefaultPosition,wxSize(100,100) ,wxLC_REPORT);
  
  m_Sizer->Add(m_List,1,wxEXPAND);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
albaGUIListCtrl::~albaGUIListCtrl( )
//----------------------------------------------------------------------------
{
  cppDEL(m_Images);
}
//----------------------------------------------------------------------------
void albaGUIListCtrl::Reset ()
//----------------------------------------------------------------------------
{
  m_List->ClearAll();

  m_List->SetImageList(m_Images,wxIMAGE_LIST_SMALL);
  m_List->SetImageList(m_Images,wxIMAGE_LIST_NORMAL);

  m_List->InsertColumn(0, "");
  m_List->SetColumnWidth( 0,1000 );
}
//----------------------------------------------------------------------------
bool albaGUIListCtrl::AddItem (wxString label, ITEM_ICONS icon)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
  long tmp = m_List->InsertItem(itemCount,label,icon);
  //m_List->SetItemData(tmp, itemCount);
  return true;
}
//----------------------------------------------------------------------------
bool albaGUIListCtrl::DeleteItem  (long index)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
	if (index < 0 || index > itemCount)
		return false;

  m_List->DeleteItem(index);
  return true;
}
//----------------------------------------------------------------------------
void albaGUIListCtrl::DeselectItem(long index)
//----------------------------------------------------------------------------
{
   m_List->SetItemState(index,0,wxLIST_STATE_SELECTED);
}
//----------------------------------------------------------------------------
bool albaGUIListCtrl::SetItemLabel (long index, wxString label)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
  if (index < 0 || index > itemCount) 
		return false;

	m_List->SetItemText(index,label);
  return true;
}
//----------------------------------------------------------------------------
bool albaGUIListCtrl::SetItemIcon (long index, ITEM_ICONS icon)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
	if (index < 0 || index > itemCount) 
		return false;

  m_List->SetItemImage(index,icon,icon);
  return true;
}
//----------------------------------------------------------------------------
wxString albaGUIListCtrl::GetItemLabel (long index)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
	if (index < 0 || index > itemCount)  
		return "";

  return m_List->GetItemText(index);
}
//----------------------------------------------------------------------------
ITEM_ICONS albaGUIListCtrl::GetItemIcon (long index)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
	if (index < 0 || index > itemCount) 
		return  ITEM_ERROR;

  wxListItem li;
  li.SetId(index);
  li.m_mask = wxLIST_MASK_IMAGE;
  m_List->GetItem(li);
  int icon = -1;
  if( li.m_mask & wxLIST_MASK_IMAGE )
    icon = li.m_image;
  albaLogMessage("icon = %d",icon);
  albaYield();
  return (ITEM_ICONS) icon;
}
//----------------------------------------------------------------------------
bool albaGUIListCtrl::SelectItem(long index)
//----------------------------------------------------------------------------
{
	int itemCount = m_List->GetItemCount();
	if (index < 0 || index > itemCount) 
		return false;

	m_PreventNotify = true;
  m_List->SetItemState(index,wxLIST_MASK_IMAGE,wxLIST_MASK_IMAGE) ;
  m_PreventNotify = false;
  
	return true;
}
//----------------------------------------------------------------------------
void albaGUIListCtrl::OnSelectionChanged(wxListEvent& event)
//----------------------------------------------------------------------------
{
  if(m_PreventNotify) return;
   
  albaString s = m_List->GetItemText(event.GetIndex());
  long item_id = event.GetData();
  long icon = event.GetImage();

  albaEventMacro(albaEvent(this, ITEM_SELECTED, &s, item_id ));
  event.Skip();
}
//----------------------------------------------------------------------------
void albaGUIListCtrl::SetColumnLabel(int col, wxString label)
//----------------------------------------------------------------------------
{
  wxListItem li;
  li.m_mask = wxLIST_MASK_TEXT;
  li.m_text = label;
  m_List->SetColumn(col,li);
}
