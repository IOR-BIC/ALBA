/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListBox.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmgListBox.h"
//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
#define ID_CLB 100
//----------------------------------------------------------------------------
// mmgListBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgListBox,mmgPanel)
    EVT_LISTBOX ( ID_CLB, mmgListBox::OnSelect)
    EVT_SIZE(mmgListBox::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgListBox::mmgListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:mmgPanel(parent,id,pos,size,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_prevent_notify = false;
  m_lb = new wxListBox(this, ID_CLB, wxDefaultPosition, wxDefaultSize, 0,NULL,wxLB_SINGLE | wxLB_HSCROLL | wxLB_OWNERDRAW );
}
//----------------------------------------------------------------------------
mmgListBox::~mmgListBox( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgListBox::Clear()
//----------------------------------------------------------------------------
{
   m_lb->Clear();
}
//----------------------------------------------------------------------------
void mmgListBox::AddItem(wxString label, bool checked)
//----------------------------------------------------------------------------
{
	m_lb->Append(label);
	int id = m_lb->GetCount()-1;
	m_lb->SetSelection(id);

	if(checked)
	{
		this->CheckItem(id);
	}
}
//----------------------------------------------------------------------------
void mmgListBox::RemoveItem(long id)
//----------------------------------------------------------------------------
{
	m_lb->Delete(id);
}
//----------------------------------------------------------------------------
bool mmgListBox::IsChecked()
//----------------------------------------------------------------------------
{
	int id = m_lb->GetSelection();
	//@@@ if (m_lb->GetItem(id)->GetBackgroundColour() == wxColour("YELLOW"))
	//@@@	 return true;
	return false;
}
//----------------------------------------------------------------------------
void mmgListBox::CheckItem(long id)
//----------------------------------------------------------------------------
{
	//@@@ m_lb->GetItem(id)->SetBackgroundColour(wxColour("YELLOW"));
}
//----------------------------------------------------------------------------
void mmgListBox::CheckSelectedItem()
//----------------------------------------------------------------------------
{
	CheckItem((long)this->m_lb->GetSelection());
}
//----------------------------------------------------------------------------
void mmgListBox::SetItemLabel(long id, wxString label)
//----------------------------------------------------------------------------
{
	//bool check = this->IsChecked();
	//m_lb->Delete(id);
	//this->AddItem(label,check);

  int idx = m_lb->GetSelection();
  m_lb->SetSelection(idx,false);
  m_lb->SetString(id,label); 
  m_lb->SetSelection(idx,true);
}
//----------------------------------------------------------------------------
void mmgListBox::SetItemLabel(wxString label)
//----------------------------------------------------------------------------
{
  int idx = m_lb->GetSelection();
	SetItemLabel(idx,label);
}
//----------------------------------------------------------------------------
void mmgListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
   int widget_id = this->GetId();
   int index = event.GetInt();
   m_item_selected = this->m_lb->GetStringSelection();
   mafEventMacro(mafEvent(this, widget_id, (long)index) );
}
//----------------------------------------------------------------------------
void mmgListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
   m_lb->SetSize(event.GetSize());
}


