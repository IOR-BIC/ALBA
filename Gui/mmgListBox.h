/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListBox.h,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgListBox_H__
#define __mmgListBox_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include "mafEvent.h"
#include "mafObserver.h"
#include "mmgPanel.h"

//----------------------------------------------------------------------------
// mmgListBox :
/**
a ListBox that send mafEventNotification in the form
mafEvent(this, widget_id, item_id, checked)
*/
//----------------------------------------------------------------------------

class mmgListBox: public mmgPanel
{
public:
    mmgListBox(wxWindow* parent, 
                    wxWindowID id, 
                    const wxPoint& pos = wxDefaultPosition, 
                    const wxSize& size = wxDefaultSize, 
                    long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

    virtual  ~mmgListBox();

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
  void SetListener(mafObserver *listener) {m_Listener = listener;};
  void Select(int id) {m_ListBox->SetSelection(id);};

  wxListBox *m_ListBox;

protected:
  void OnSelect(wxCommandEvent &event);
  void OnSize(wxSizeEvent& event);

  wxString     m_ItemSelected;
	mafObserver *m_Listener;
  bool         m_PreventNotify;

  DECLARE_EVENT_TABLE()
};
#endif
