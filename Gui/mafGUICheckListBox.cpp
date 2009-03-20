/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUICheckListBox.cpp,v $
  Language:  C++
  Date:      $Date: 2009-03-20 08:21:58 $
  Version:   $Revision: 1.1.2.1 $
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

#include "mafGUI.h"
#include "mafGUICheckListBox.h"

//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
#define ID_CLB 100

//----------------------------------------------------------------------------
// mafGUICheckListBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUICheckListBox,mafGUIPanel)
		EVT_CHECKLISTBOX(ID_CLB, mafGUICheckListBox::OnCheck)
		EVT_LISTBOX(ID_CLB, mafGUICheckListBox::OnSelect)
    EVT_SIZE(mafGUICheckListBox::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUICheckListBox::mafGUICheckListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:mafGUIPanel(parent,id,pos,size,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
	m_SelectedItem = -1;
  m_PreventNotify = false;
  m_CheckEvent = false;
	m_CheckMode = MODE_CHECK;
  m_CheckListBox = new wxCheckListBox(this, ID_CLB, wxDefaultPosition, size, 0,NULL,wxNO_BORDER);
}
//----------------------------------------------------------------------------
mafGUICheckListBox::~mafGUICheckListBox( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::Clear()
//----------------------------------------------------------------------------
{
  m_CheckListBox->Clear();
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::AddItem(int id, wxString label)
//----------------------------------------------------------------------------
{
  // SIL: 05-may-06
  // in passing from wx242 -> wx263
  // Number() become GetCount()

  //if( m_CheckListBox->Number() == mafGUICheckListBox_ArraySize ) 
  if( m_CheckListBox->GetCount() == mafGUICheckListBox_ArraySize ) 
  {
    mafLogMessage("mafGUICheckListBox:overflow");
    return;
  }
  
  m_CheckListBox->Append(label);

  //m_Array[m_CheckListBox->Number()-1] = id;
  m_Array[m_CheckListBox->GetCount()-1] = id;
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::AddItem(int id, wxString label, bool check)
//----------------------------------------------------------------------------
{
  // SIL: 05-may-06
  // in passing from wx242 -> wx263
  // Number() become GetCount()

  //if( m_CheckListBox->Number() == mafGUICheckListBox_ArraySize ) 
  if( m_CheckListBox->GetCount() == mafGUICheckListBox_ArraySize ) 
  {
    mafLogMessage("mafGUICheckListBox:overflow");
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
void mafGUICheckListBox::RemoveItem(int id)
//----------------------------------------------------------------------------
{
  int index,i,n = m_CheckListBox->GetCount(); // number prior to delete

  index=FindItemIndex(id);
  if( index == -1 ) return;

  m_CheckListBox->Delete(index);

  // keep the array consistent
  for(i=index+1; i<n; i++) 
    m_Array[i-1] = m_Array[i];
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::CheckItem(int id, bool check)
//----------------------------------------------------------------------------
{
  int index=FindItemIndex(id);
  if( index == -1 ) return;

  m_PreventNotify = true;
  m_CheckListBox->Check(index,check);
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::SetItemLabel(int id, const mafString label)
//----------------------------------------------------------------------------
{
  int index;
  index = FindItemIndex(id);
  if( index == -1 ) return;

  m_PreventNotify = true;
  m_CheckListBox->SetString(index,label.GetCStr());
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
mafString mafGUICheckListBox::GetItemLabel(int id)
//----------------------------------------------------------------------------
{
	mafString label;

  m_PreventNotify = true;
  int index = FindItemIndex(id);
  label = m_CheckListBox->GetString(index).c_str();
  m_PreventNotify = false;

	return label;
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::Select(int id)
//----------------------------------------------------------------------------
{
  m_PreventNotify = true;
  m_CheckListBox->Select(id);
  m_SelectedItem = id;
  m_PreventNotify = false;
}
//----------------------------------------------------------------------------
bool mafGUICheckListBox::IsItemChecked(int id)
//----------------------------------------------------------------------------
{
	return m_CheckListBox->IsChecked(id);
}
//----------------------------------------------------------------------------
int mafGUICheckListBox::FindItem(wxString label)
//----------------------------------------------------------------------------
{
	return m_CheckListBox->FindString(label);
}
//----------------------------------------------------------------------------
int mafGUICheckListBox::FindItemIndex(int id)
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

  mafLogMessage("mafGUICheckListBox: ItemNotFound: %d",id);
  return -1;
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::OnCheck(wxCommandEvent &event)
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

  m_CheckEvent = true;
  mafEventMacro(mafEvent(this, widget_id, checked, item_id ) );
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{
  //wxSize s = event.GetSize();
  //m_CheckListBox->SetSize(s);
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::OnSelect(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  if(m_PreventNotify) return;
  int widget_id = this->GetId();
  int index = event.GetInt();
  int item_id = m_Array[index];
	m_SelectedItem = item_id;

  m_CheckEvent = false;
  mafEventMacro(mafEvent(this, widget_id, (long)item_id ));
}
//----------------------------------------------------------------------------
void mafGUICheckListBox::HighlightItem(int index, int rgbText[3], int rgbBack[3])
//----------------------------------------------------------------------------
{
   m_CheckListBox->GetItem(index)->SetTextColour(wxColor(rgbText[0], rgbText[1], rgbText[2]));
   m_CheckListBox->GetItem(index)->SetBackgroundColour(wxColor(rgbBack[0], rgbBack[1], rgbBack[2]));
}