/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListBox.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.3 $
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


#include "mmgListBox.h"
//----------------------------------------------------------------------------
// constants :
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
  m_PreventNotify = false;
  m_ListBox = new wxListBox(this, ID_CLB, wxDefaultPosition, wxDefaultSize, 0,NULL,wxLB_SINGLE | wxLB_HSCROLL | wxLB_OWNERDRAW );
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
   m_ListBox->Clear();
}
//----------------------------------------------------------------------------
void mmgListBox::AddItem(wxString label, bool checked)
//----------------------------------------------------------------------------
{
	m_ListBox->Append(label);
	int id = m_ListBox->GetCount()-1;
	m_ListBox->SetSelection(id);

	if(checked)
	{
		this->CheckItem(id);
	}
}
//----------------------------------------------------------------------------
void mmgListBox::RemoveItem(long id)
//----------------------------------------------------------------------------
{
	m_ListBox->Delete(id);
}
//----------------------------------------------------------------------------
bool mmgListBox::IsChecked()
//----------------------------------------------------------------------------
{
	int id = m_ListBox->GetSelection();
	//@@@ if (m_ListBox->GetItem(id)->GetBackgroundColour() == wxColour("YELLOW"))
	//@@@	 return true;
	return false;
}
//----------------------------------------------------------------------------
void mmgListBox::CheckItem(long id)
//----------------------------------------------------------------------------
{
	//@@@ m_ListBox->GetItem(id)->SetBackgroundColour(wxColour("YELLOW"));
}
//----------------------------------------------------------------------------
void mmgListBox::CheckSelectedItem()
//----------------------------------------------------------------------------
{
	CheckItem((long)this->m_ListBox->GetSelection());
}
//----------------------------------------------------------------------------
void mmgListBox::SetItemLabel(long id, wxString label)
//----------------------------------------------------------------------------
{
	//bool check = this->IsChecked();
	//m_ListBox->Delete(id);
	//this->AddItem(label,check);

  int idx = m_ListBox->GetSelection();
  m_ListBox->SetSelection(idx,false);
  m_ListBox->SetString(id,label); 
  m_ListBox->SetSelection(idx,true);
}
//----------------------------------------------------------------------------
void mmgListBox::SetItemLabel(wxString label)
//----------------------------------------------------------------------------
{
  int idx = m_ListBox->GetSelection();
	SetItemLabel(idx,label);
}
//----------------------------------------------------------------------------
void mmgListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
   int widget_id = this->GetId();
   int index = event.GetInt();
   m_ItemSelected = this->m_ListBox->GetStringSelection();
   mafEventMacro(mafEvent(this, widget_id, (long)index) );
}
//----------------------------------------------------------------------------
void mmgListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
   m_ListBox->SetSize(event.GetSize());
}
