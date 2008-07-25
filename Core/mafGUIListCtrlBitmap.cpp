/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIListCtrlBitmap.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
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

#include "mafGUIListCtrlBitmap.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
const mafID ID_LIST = 200;

BEGIN_EVENT_TABLE(mafGUIListCtrlBitmap,wxPanel)
    EVT_LIST_ITEM_SELECTED(ID_LIST, mafGUIListCtrlBitmap::OnSelectionChanged)
END_EVENT_TABLE()

mafGUIListCtrlBitmap::mafGUIListCtrlBitmap( wxWindow* parent,wxWindowID id, bool CloseButton, bool HideTitle)
:mafGUINamedPanel(parent,id,CloseButton,HideTitle)
//----------------------------------------------------------------------------
{
  SetWindowStyle(wxNO_BORDER);
  SetBackgroundColour(wxColour(255,255,255));
   
  m_Listener = NULL;

  m_Images = new wxImageList(25,25,FALSE,4);
  m_List = new wxListCtrl(this,ID_LIST,wxDefaultPosition,wxSize(100,100) ,wxLC_SMALL_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE);

  m_Sizer->Add(m_List,1,wxEXPAND);
  m_PreventNotify = false;

  m_List->SetColumnWidth(0,200);
  m_List->SetColumnWidth(1,200);
}
//----------------------------------------------------------------------------
mafGUIListCtrlBitmap::~mafGUIListCtrlBitmap()
//----------------------------------------------------------------------------
{
	cppDEL(m_Images);
}
//----------------------------------------------------------------------------
void mafGUIListCtrlBitmap::Reset()
//----------------------------------------------------------------------------
{
  m_Images->RemoveAll();
  m_List->ClearAll();
  m_List->SetImageList(m_Images,wxIMAGE_LIST_SMALL);
  m_List->SetImageList(m_Images,wxIMAGE_LIST_NORMAL);
  m_List->InsertColumn(0, "");
  m_List->SetColumnWidth(0, 1000);
}
//----------------------------------------------------------------------------
bool mafGUIListCtrlBitmap::AddItem(long item_id, wxString label, wxBitmap *bmp)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id != -1)
    return false;

	if(bmp) 
		m_Images->Add(*bmp);
  else
	{
    wxBitmap foo(50,50);
		m_Images->Add(foo);
  }
  long tmp = m_List->InsertItem(item_id,label,0);
	m_List->SetItemImage(tmp,tmp,tmp); 
  m_List->SetItemData(tmp, item_id);
  return true;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrlBitmap::DeleteItem(long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) 
    return false;
  m_List->DeleteItem(id);  
  m_Images->Remove(id);     //images after id shift index
	 
  int n = m_List->GetItemCount();
  int i;
  for(i = id; i < n; i++)
	{
    m_List->SetItemImage(i,i,i); 
	}
  return true;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrlBitmap::SetItemLabel(long item_id, wxString label)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) 
    return false;
  m_List->SetItemText(id,label);
  return true;
}
//----------------------------------------------------------------------------
bool mafGUIListCtrlBitmap::SetItemIcon(long item_id, wxBitmap *bmp)
//----------------------------------------------------------------------------
{
  long id = m_List->FindItem(-1, item_id);
  if(id == -1) 
    return false;
  if(bmp == NULL)
    return false;
  m_Images->Replace(id,*bmp);
  m_List->SetItemImage(id,id,id);
  return true;
}
//----------------------------------------------------------------------------
wxString mafGUIListCtrlBitmap::GetItemLabel(long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) 
    return "";
  return m_List->GetItemText(id);
}
//----------------------------------------------------------------------------
ITEM_ICONS mafGUIListCtrlBitmap::GetItemIcon(long item_id)
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
bool mafGUIListCtrlBitmap::SelectItem(long item_id)
//----------------------------------------------------------------------------
{
  long id =  m_List->FindItem(-1, item_id);
  if (id == -1) 
    return false;
 
	m_PreventNotify = true;
  m_List->SetItemState(id, wxLIST_STATE_SELECTED, wxLIST_MASK_TEXT);
  m_PreventNotify = false;
  Refresh();
  
	return true;
}
//----------------------------------------------------------------------------
void mafGUIListCtrlBitmap::OnSelectionChanged(wxListEvent& event)
//----------------------------------------------------------------------------
{
  if(m_PreventNotify) 
    return;
   
  mafString s = m_List->GetItemText(event.GetIndex()).c_str();
  long item_id = event.GetData();
  long icon = event.GetImage();

	mafEventMacro(mafEvent(this, ITEM_SELECTED, &s, item_id ));
  event.Skip();
}
//----------------------------------------------------------------------------
void mafGUIListCtrlBitmap::SetColumnLabel(int col, wxString label)
//----------------------------------------------------------------------------
{
  wxListItem li;
  li.m_mask = wxLIST_MASK_TEXT;
  li.m_text = label;
  m_List->SetColumn(col,li);
}
