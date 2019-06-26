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
#ifndef __albaGUIListBox_H__
#define __albaGUIListBox_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUIPanel.h"

//----------------------------------------------------------------------------
// albaGUIListBox :
/**
a ListBox that send albaEventNotification in the form
albaEvent(this, widget_id, item_id, checked)
*/
//----------------------------------------------------------------------------

class albaGUIListBox: public albaGUIPanel
{
public:
    albaGUIListBox(wxWindow* parent, 
                    wxWindowID id, 
                    const wxPoint& pos = wxDefaultPosition, 
                    const wxSize& size = wxDefaultSize, 
                    long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

    virtual  ~albaGUIListBox();

  void Clear();
  void AddItem(wxString label, bool checked = false);
  void RemoveItem(long id);
  bool IsChecked();
	void CheckItem(long id);
  void CheckSelectedItem();
  void SetItemLabel(long id, wxString label);
  void SetItemLabel(wxString label);
	wxString GetStringSelection() {return m_ListBox->GetStringSelection();};
	int GetNumberOfItems()	{return m_ListBox->GetCount();};
  void SetListener(albaObserver *listener) {m_Listener = listener;};
  void Select(int id);

  wxListBox *m_ListBox;

protected:
  void OnSelect(wxCommandEvent &event);
  void OnSize(wxSizeEvent& event);

  wxString     m_ItemSelected;
	albaObserver *m_Listener;
  bool         m_PreventNotify;

  DECLARE_EVENT_TABLE()
};
#endif
