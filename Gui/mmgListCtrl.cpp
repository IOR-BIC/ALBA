/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListCtrl.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 11:39:46 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#include "mmgListCtrl.h"

#include "mafDecl.h"
#include "mafString.h"
#include "mafPics.h"

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
const int ID_LIST = 200;

BEGIN_EVENT_TABLE(mmgListCtrl,wxPanel)
    EVT_LIST_ITEM_SELECTED(ID_LIST, mmgListCtrl::OnSelectionChanged)
END_EVENT_TABLE()

mmgListCtrl::mmgListCtrl( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:mmgNamedPanel(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  SetWindowStyle(wxNO_BORDER);
  SetBackgroundColour(wxColour(255,255,255));
   
  m_Listener = NULL;

  m_images = new wxImageList(15,15,FALSE,4);
  m_images->Add(mafPics.GetBmp("PIC_NODE_YELLOW"));
  m_images->Add(mafPics.GetBmp("PIC_NODE_GRAY"));
  m_images->Add(mafPics.GetBmp("PIC_NODE_RED"));
  m_images->Add(mafPics.GetBmp("PIC_NODE_BLUE"));

  m_list = new wxListCtrl(this,ID_LIST,wxDefaultPosition,wxSize(100,100) ,wxLC_REPORT);
  
  m_sizer->Add(m_list,1,wxEXPAND);
  m_prevent_notify = false;
}
//----------------------------------------------------------------------------
mmgListCtrl::~mmgListCtrl( )
//----------------------------------------------------------------------------
{
  cppDEL(m_images);
}
//----------------------------------------------------------------------------
void mmgListCtrl::Reset ()
//----------------------------------------------------------------------------
{
  m_list->ClearAll();

  m_list->SetImageList(m_images,wxIMAGE_LIST_SMALL);
  m_list->SetImageList(m_images,wxIMAGE_LIST_NORMAL);

  m_list->InsertColumn(0, "");
  m_list->SetColumnWidth( 0,1000 );
}
//----------------------------------------------------------------------------
bool mmgListCtrl::AddItem (long item_id, wxString label, ITEM_ICONS icon)
//----------------------------------------------------------------------------
{
  if( ItemExist(item_id) )return false;
  long tmp = m_list->InsertItem(item_id,label,icon); 
  m_list->SetItemData(tmp, item_id);
  return true;
}
//----------------------------------------------------------------------------
bool mmgListCtrl::DeleteItem  (long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  if (id == -1) return false;
  m_list->DeleteItem(id);
  return true;
}
//----------------------------------------------------------------------------
bool mmgListCtrl::SetItemLabel (long item_id, wxString label)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  if (id == -1) return false;
  m_list->SetItemText(id,label);
  return true;
}
//----------------------------------------------------------------------------
bool mmgListCtrl::SetItemIcon (long item_id, ITEM_ICONS icon)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  if(id == -1) 
    return false;
  m_list->SetItemImage(id,icon,icon);
  return true;
}
//----------------------------------------------------------------------------
wxString mmgListCtrl::GetItemLabel (long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  if (id == -1) return "";
  return m_list->GetItemText(id);
}
//----------------------------------------------------------------------------
ITEM_ICONS mmgListCtrl::GetItemIcon (long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  if (id == -1) 
    return ITEM_ERROR;

  wxListItem li;
  li.SetId(id);
  li.m_mask = wxLIST_MASK_IMAGE;
  m_list->GetItem(li);
  int icon = -1;
  if( li.m_mask & wxLIST_MASK_IMAGE )
    icon = li.m_image;
  wxLogMessage("icon = %d",icon);
  mafYield();
  return (ITEM_ICONS) icon;
}
//----------------------------------------------------------------------------
bool mmgListCtrl::SelectItem(long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  if (id == -1) return false;
 
	m_prevent_notify = true;
  m_list->SetItemState(id,wxLIST_MASK_IMAGE,wxLIST_MASK_IMAGE) ;
  m_prevent_notify = false;
  
	return true;
}
//----------------------------------------------------------------------------
void mmgListCtrl::OnSelectionChanged(wxListEvent& event)
//----------------------------------------------------------------------------
{
  if(m_prevent_notify) return;
   
  mafString s = m_list->GetItemText(event.GetIndex());
  long item_id = event.GetData();
  long icon = event.GetImage(); //vfc - non va

  mafEventMacro(mafEvent(this, ITEM_SELECTED, &s, item_id ));
  event.Skip();
}
//----------------------------------------------------------------------------
bool mmgListCtrl::ItemExist(long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_list->FindItem(-1, item_id);
  return (id != -1);
}
//----------------------------------------------------------------------------
void mmgListCtrl::SetColumnLabel(int col, wxString label)
//----------------------------------------------------------------------------
{
  wxListItem li;
  li.m_mask = wxLIST_MASK_TEXT;
  li.m_text = label;
  m_list->SetColumn(col,li);
}
