/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIListCtrl.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIListCtrl.h"
#include "mafDecl.h"
#include "mafString.h"
#include "mafPics.h"
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
const int ID_LIST = 200;

BEGIN_EVENT_TABLE(mafGUIListCtrl,wxPanel)
    EVT_LIST_ITEM_SELECTED(ID_LIST, mafGUIListCtrl::OnSelectionChanged)
END_EVENT_TABLE()

mafGUIListCtrl::mafGUIListCtrl( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:mafGUINamedPanel(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  SetWindowStyle(wxNO_BORDER);
  SetBackgroundColour(wxColour(255,255,255));
   
  m_Listener = NULL;

  m_Images = new wxImageList(15,15,FALSE,4);
  m_Images->Add(mafPics.GetBmp("NODE_YELLOW"));
  m_Images->Add(mafPics.GetBmp("NODE_GRAY"));
  m_Images->Add(mafPics.GetBmp("NODE_RED"));
  m_Images->Add(mafPics.GetBmp("NODE_BLUE"));

  m_List = new wxListCtrl(this,ID_LIST,wxDefaultPosition,wxSize(100,100) ,wxLC_REPORT);
  
  m_Sizer->Add(m_List,1,wxEXPAND);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
mafGUIListCtrl::~mafGUIListCtrl( )
//----------------------------------------------------------------------------
{
  cppDEL(m_Images);
}
//----------------------------------------------------------------------------
void mafGUIListCtrl::Reset ()
//----------------------------------------------------------------------------
{
  m_List->ClearAll();

  m_List->SetImageList(m_Images,wxIMAGE_LIST_SMALL);
  m_List->SetImageList(m_Images,wxIMAGE_LIST_NORMAL);

  m_List->InsertColumn(0, "");
  m_List->SetColumnWidth( 0,1000 );
}
//----------------------------------------------------------------------------
bool mafGUIListCtrl::AddItem (long item_id, wxString label, ITEM_ICONS icon)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if(id != -1)
    return false;
  long tmp = m_List->InsertItem(item_id,label,icon); 
  m_List->SetItemData(tmp, item_id);
  return true;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrl::DeleteItem  (long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) return false;
  m_List->DeleteItem(id);
  return true;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrl::SetItemLabel (long item_id, wxString label)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) return false;
  m_List->SetItemText(id,label);
  return true;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrl::SetItemIcon (long item_id, ITEM_ICONS icon)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if(id == -1) 
    return false;
  m_List->SetItemImage(id,icon,icon);
  return true;
}
//----------------------------------------------------------------------------
wxString mafGUIListCtrl::GetItemLabel (long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) return "";
  return m_List->GetItemText(id);
}
//----------------------------------------------------------------------------
ITEM_ICONS mafGUIListCtrl::GetItemIcon (long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) 
    return ITEM_ERROR;

  wxListItem li;
  li.SetId(id);
  li.m_mask = wxLIST_MASK_IMAGE;
  m_List->GetItem(li);
  int icon = -1;
  if( li.m_mask & wxLIST_MASK_IMAGE )
    icon = li.m_image;
  mafLogMessage("icon = %d",icon);
  mafYield();
  return (ITEM_ICONS) icon;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrl::SelectItem(long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) return false;
 
	m_PreventNotify = true;
  m_List->SetItemState(id,wxLIST_MASK_IMAGE,wxLIST_MASK_IMAGE) ;
  m_PreventNotify = false;
  
	return true;
}
//----------------------------------------------------------------------------
void mafGUIListCtrl::OnSelectionChanged(wxListEvent& event)
//----------------------------------------------------------------------------
{
  if(m_PreventNotify) return;
   
  mafString s = m_List->GetItemText(event.GetIndex());
  long item_id = event.GetData();
  long icon = event.GetImage();

  mafEventMacro(mafEvent(this, ITEM_SELECTED, &s, item_id ));
  event.Skip();
}
//----------------------------------------------------------------------------
void mafGUIListCtrl::SetColumnLabel(int col, wxString label)
//----------------------------------------------------------------------------
{
  wxListItem li;
  li.m_mask = wxLIST_MASK_TEXT;
  li.m_text = label;
  m_List->SetColumn(col,li);
}
