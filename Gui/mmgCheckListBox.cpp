/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCheckListBox.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-22 07:02:27 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
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

#include <wx/laywin.h>
#include <wx/checklst.h>


#include "mafEvent.h"

#include "mmgGui.h"
#include "mmgCheckListBox.h"

//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
#define ID_CLB 100

//----------------------------------------------------------------------------
// mmgCheckListBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgCheckListBox,mmgPanel)
		EVT_CHECKLISTBOX(ID_CLB, mmgCheckListBox::OnCheck)
		EVT_LISTBOX(ID_CLB, mmgCheckListBox::OnSelect)
    EVT_SIZE(mmgCheckListBox::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgCheckListBox::mmgCheckListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:mmgPanel(parent,id,pos,size,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
	m_selectedItem = -1;
  m_prevent_notify = false;
	m_check_mode = MODE_CHECK;
  m_clb = new wxCheckListBox(this, ID_CLB, wxDefaultPosition, size, 0,NULL,wxNO_BORDER);
}
//----------------------------------------------------------------------------
mmgCheckListBox::~mmgCheckListBox( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgCheckListBox::Clear()
//----------------------------------------------------------------------------
{
  m_clb->Clear();
}
//----------------------------------------------------------------------------
void mmgCheckListBox::AddItem(int id, wxString label)
//----------------------------------------------------------------------------
{
  if( m_clb->Number() == mmgCheckListBox_ArraySize ) 
  {
    wxLogMessage("mmgCheckListBox:overflow");
    return;
  }
  
  m_clb->Append(label);
  m_array[m_clb->Number()-1] = id;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::AddItem(int id, wxString label, bool check)
//----------------------------------------------------------------------------
{
  if( m_clb->Number() == mmgCheckListBox_ArraySize ) 
  {
    wxLogMessage("mmgCheckListBox:overflow");
    return;
  }

  m_clb->Append(label);
  m_array[m_clb->Number()-1] = id;

  m_prevent_notify = true;
  m_clb->Check(m_clb->Number()-1,check);
  m_prevent_notify = false;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::RemoveItem(int id)
//----------------------------------------------------------------------------
{
  int index,i,n = m_clb->Number(); // number prior to delete

  if( index=FindItemIndex(id) == -1 ) return;
  m_clb->Delete(index);

  // keep the array consistent
  for(i=index+1; i<n; i++) m_array[i-1] = m_array[i];
}
//----------------------------------------------------------------------------
void mmgCheckListBox::CheckItem(int id, bool check)
//----------------------------------------------------------------------------
{
  int index;
  if( index=FindItemIndex(id) == -1 ) return;

  m_prevent_notify = true;
  m_clb->Check(index,check);
  m_prevent_notify = false;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::SetItemLabel(int id, const wxString label)
//----------------------------------------------------------------------------
{
  int index;
  if( index=FindItemIndex(id) == -1 ) return;

  m_prevent_notify = true;
  m_clb->SetString(index,label);
  m_prevent_notify = false;
}
//----------------------------------------------------------------------------
wxString mmgCheckListBox::GetItemLabel(int id)
//----------------------------------------------------------------------------
{
	wxString label;

  m_prevent_notify = true;
  label = m_clb->GetString(id);
  m_prevent_notify = false;

	return label;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::Select(int id)
//----------------------------------------------------------------------------
{
  m_prevent_notify = true;
  m_clb->Select(id);
  m_prevent_notify = false;
}
//----------------------------------------------------------------------------
bool mmgCheckListBox::IsItemChecked(int id)
//----------------------------------------------------------------------------
{
	return m_clb->IsChecked(id);
}
//----------------------------------------------------------------------------
int mmgCheckListBox::FindItem(wxString label)
//----------------------------------------------------------------------------
{
	return m_clb->FindString(label);
}
//----------------------------------------------------------------------------
int mmgCheckListBox::FindItemIndex(int id)
//----------------------------------------------------------------------------
{
  int i = 0;
  int n = m_clb->Number();
  while(i<n)
  {
    if (m_array[i] == id) return i;
    i++;
  }

  wxLogMessage("mmgCheckListBox: ItemNotFound: %d",id);
  return -1;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::OnCheck(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  if(m_prevent_notify) return;
  int widget_id = this->GetId();
  int index = event.GetInt();

	m_prevent_notify = true;
	m_clb->Select(index);
	m_selectedItem = index;
	
	if(m_check_mode == MODE_RADIO)
	{
	  for(int i=0; i<m_clb->Number(); i++)
	    m_clb->Check(i,FALSE);
		m_clb->Check(index);
	}

	m_prevent_notify = false;

  int item_id = m_array[index];
  bool checked = m_clb->IsChecked(index);

  mafEventMacro(mafEvent(this, widget_id, checked, item_id ) );
}
//----------------------------------------------------------------------------
void mmgCheckListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  m_clb->SetSize(event.GetSize());
}
//----------------------------------------------------------------------------
void mmgCheckListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  if(m_prevent_notify) return;
  int widget_id = this->GetId();
  int index = event.GetInt();
  int item_id = m_array[index];
	m_selectedItem = item_id;

  mafEventMacro(mafEvent(this, widget_id, (long)item_id ) );
}
