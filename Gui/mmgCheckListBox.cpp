/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCheckListBox.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-29 09:38:08 $
  Version:   $Revision: 1.7 $
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
	m_SelectedItem = -1;
  m_PreventNotify = false;
	m_CheckMode = MODE_CHECK;
  m_CheckListBox = new wxCheckListBox(this, ID_CLB, wxDefaultPosition, size, 0,NULL,wxNO_BORDER);
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
  m_CheckListBox->Clear();
}
//----------------------------------------------------------------------------
void mmgCheckListBox::AddItem(int id, wxString label)
//----------------------------------------------------------------------------
{
  // SIL: 05-may-06
  // in passing from wx242 -> wx263
  // Number() become GetCount()

  //if( m_CheckListBox->Number() == mmgCheckListBox_ArraySize ) 
  if( m_CheckListBox->GetCount() == mmgCheckListBox_ArraySize ) 
  {
    mafLogMessage("mmgCheckListBox:overflow");
    return;
  }
  
  m_CheckListBox->Append(label);

  //m_Array[m_CheckListBox->Number()-1] = id;
  m_Array[m_CheckListBox->GetCount()-1] = id;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::AddItem(int id, wxString label, bool check)
//----------------------------------------------------------------------------
{
  // SIL: 05-may-06
  // in passing from wx242 -> wx263
  // Number() become GetCount()

  //if( m_CheckListBox->Number() == mmgCheckListBox_ArraySize ) 
  if( m_CheckListBox->GetCount() == mmgCheckListBox_ArraySize ) 
  {
    mafLogMessage("mmgCheckListBox:overflow");
    return;
  }

  m_CheckListBox->Append(label);
  //m_Array[m_CheckListBox->Number()-1] = id;
  m_Array[m_CheckListBox->GetCount()-1] = id;

  m_PreventNotify = true;
  //m_CheckListBox->Check(m_CheckListBox->Number()-1,check);
  m_CheckListBox->Check(m_CheckListBox->GetCount()-1,check);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::RemoveItem(int id)
//----------------------------------------------------------------------------
{
  int index,i,n = m_CheckListBox->GetCount(); // number prior to delete

  index=FindItemIndex(id);
  if( index == -1 ) return;

  m_CheckListBox->Delete(index);

  // keep the array consistent
  for(i=index+1; i<n; i++) m_Array[i-1] = m_Array[i];
}
//----------------------------------------------------------------------------
void mmgCheckListBox::CheckItem(int id, bool check)
//----------------------------------------------------------------------------
{
  int index=FindItemIndex(id);
  if( index == -1 ) return;

  m_PreventNotify = true;
  m_CheckListBox->Check(index,check);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::SetItemLabel(int id, const wxString label)
//----------------------------------------------------------------------------
{
  int index;
  index=FindItemIndex(id);
  if( index == -1 ) return;

  m_PreventNotify = true;
  m_CheckListBox->SetString(index,label);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
wxString mmgCheckListBox::GetItemLabel(int id)
//----------------------------------------------------------------------------
{
	wxString label;

  m_PreventNotify = true;
  label = m_CheckListBox->GetString(id);
  m_PreventNotify = false;

	return label;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::Select(int id)
//----------------------------------------------------------------------------
{
  m_PreventNotify = true;
  m_CheckListBox->Select(id);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
bool mmgCheckListBox::IsItemChecked(int id)
//----------------------------------------------------------------------------
{
	return m_CheckListBox->IsChecked(id);
}
//----------------------------------------------------------------------------
int mmgCheckListBox::FindItem(wxString label)
//----------------------------------------------------------------------------
{
	return m_CheckListBox->FindString(label);
}
//----------------------------------------------------------------------------
int mmgCheckListBox::FindItemIndex(int id)
//----------------------------------------------------------------------------
{

  int i = 0;
  //int n = m_CheckListBox->Number();
  int n = m_CheckListBox->GetCount();
  while(i<n)
  {
    if (m_Array[i] == id) return i;
    i++;
  }

  mafLogMessage("mmgCheckListBox: ItemNotFound: %d",id);
  return -1;
}
//----------------------------------------------------------------------------
void mmgCheckListBox::OnCheck(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  if(m_PreventNotify) return;
  int widget_id = this->GetId();
  int index = event.GetInt();

	m_PreventNotify = true;
	m_CheckListBox->Select(index);
	m_SelectedItem = index;
	
	if(m_CheckMode == MODE_RADIO)
	{
    //for(int i=0; i<m_CheckListBox->Number(); i++)
	  for(int i=0; i<m_CheckListBox->GetCount(); i++)
	    m_CheckListBox->Check(i,FALSE);
		m_CheckListBox->Check(index);
	}

	m_PreventNotify = false;

  int item_id = m_Array[index];
  bool checked = m_CheckListBox->IsChecked(index);

  mafEventMacro(mafEvent(this, widget_id, checked, item_id ) );
}
//----------------------------------------------------------------------------
void mmgCheckListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
  m_CheckListBox->SetSize(event.GetSize());
}
//----------------------------------------------------------------------------
void mmgCheckListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  if(m_PreventNotify) return;
  int widget_id = this->GetId();
  int index = event.GetInt();
  int item_id = m_Array[index];
	m_SelectedItem = item_id;

  mafEventMacro(mafEvent(this, widget_id, (long)item_id ) );
}
