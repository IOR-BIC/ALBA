/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIComboBox.h,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:04:57 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIComboBox_H__
#define __mafGUIComboBox_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUIPanel.h"

//----------------------------------------------------------------------------
// mafGUIComboBox :
/**
a ListBox that send mafEventNotification in the form
mafEvent(this, widget_id, item_id, checked)
*/
//----------------------------------------------------------------------------

class mafGUIComboBox: public wxComboBox
{
public:
    mafGUIComboBox(wxWindow* parent, 
                    wxWindowID id,
                    wxArrayString array,
                    const wxPoint& pos = wxDefaultPosition, 
                    const wxSize& size = wxDefaultSize, 
                    long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

    virtual  ~mafGUIComboBox();

    void SetListener(mafObserver *listener){m_Listener = listener;}
    /*virtual*/ bool MSWCommand(WXUINT param, WXWORD id);
    /*virtual*/ void SetSelection(int n);

protected:

  mafObserver *m_Listener;
  int m_OldValue;
  DECLARE_EVENT_TABLE()
};
#endif
