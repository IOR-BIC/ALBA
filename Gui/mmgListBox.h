/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgListBox.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgListBox_H__
#define __mmgListBox_H__

#include "mafDefines.h" //important: mafDefines should always be included as first

#include <wx/laywin.h>
#include <wx/listctrl.h>

#include "mafEvent.h"
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

    void		Clear			();
    void		AddItem			(wxString label, bool checked = false);
    void		RemoveItem		(long id);
    bool		IsChecked		();
	void		CheckItem		(long id);
    void		CheckSelectedItem();
    void		SetItemLabel	(long id, wxString label);
    void		SetItemLabel	(wxString label);
	wxString	GetStringSelection() {return m_lb->GetStringSelection();};
	int			GetNumberOfItems()	{return m_lb->GetCount();};
    void		SetListener		(mafEventListener *listener) {m_Listener = listener;};
	void		Select			(int id) {m_lb->SetSelection(id);};

    wxListBox *m_lb;

protected:
    void      OnSelect       (wxCommandEvent &event);
    void      OnSize         (wxSizeEvent& event);

    wxString			m_item_selected;
	mafEventListener    *m_Listener;
    bool                m_prevent_notify;

    DECLARE_EVENT_TABLE()
};

#endif





























