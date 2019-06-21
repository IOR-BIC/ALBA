/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIListBox
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


#include "albaGUIListBox.h"
#include "albaGUIValidator.h"

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define ID_CLB 100
//----------------------------------------------------------------------------
// albaGUIListBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIListBox,albaGUIPanel)
    EVT_LISTBOX ( ID_CLB, albaGUIListBox::OnSelect)
    EVT_SIZE(albaGUIListBox::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIListBox::albaGUIListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:albaGUIPanel(parent,id,pos,size,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_PreventNotify = false;
  m_ListBox = new wxListBox(this, ID_CLB, wxDefaultPosition, wxDefaultSize, 0,NULL,wxLB_SINGLE | wxLB_HSCROLL | wxLB_OWNERDRAW );
}
//----------------------------------------------------------------------------
albaGUIListBox::~albaGUIListBox( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIListBox::Clear()
//----------------------------------------------------------------------------
{
   m_ListBox->Clear();
}
//----------------------------------------------------------------------------
void albaGUIListBox::AddItem(wxString label, bool checked)
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
void albaGUIListBox::RemoveItem(long id)
//----------------------------------------------------------------------------
{
	m_ListBox->Delete(id);
}
//----------------------------------------------------------------------------
bool albaGUIListBox::IsChecked()
//----------------------------------------------------------------------------
{
	int id = m_ListBox->GetSelection();
	//@@@ if (m_ListBox->GetItem(id)->GetBackgroundColour() == wxColour("YELLOW"))
	//@@@	 return true;
	return false;
}
//----------------------------------------------------------------------------
void albaGUIListBox::CheckItem(long id)
//----------------------------------------------------------------------------
{
	//@@@ m_ListBox->GetItem(id)->SetBackgroundColour(wxColour("YELLOW"));
}
//----------------------------------------------------------------------------
void albaGUIListBox::CheckSelectedItem()
//----------------------------------------------------------------------------
{
	CheckItem((long)this->m_ListBox->GetSelection());
}
//----------------------------------------------------------------------------
void albaGUIListBox::SetItemLabel(long id, wxString label)
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
void albaGUIListBox::SetItemLabel(wxString label)
//----------------------------------------------------------------------------
{
  int idx = m_ListBox->GetSelection();
	SetItemLabel(idx,label);
}
//----------------------------------------------------------------------------
void albaGUIListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
   int widget_id = this->GetId();
   int index = event.GetInt();
   m_ItemSelected = this->m_ListBox->GetStringSelection();
   albaEventMacro(albaEvent(this, widget_id, (long)index) );
}
//----------------------------------------------------------------------------
void albaGUIListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
   m_ListBox->SetSize(event.GetSize());
}
//----------------------------------------------------------------------------
void albaGUIListBox::Select( int id )
//----------------------------------------------------------------------------
{
  m_ListBox->SetSelection(id);// needed line below because 
                              // SetSelection doesn't rise event
                              // wxEVT_COMMAND_LISTBOX_SELECTED
                              // trapped from albaGUI
  ((albaGUIValidator *)m_ListBox->GetValidator())->TransferFromWindow();
}
