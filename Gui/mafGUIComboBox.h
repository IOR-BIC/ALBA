/*=========================================================================

 Program: MAF2
 Module: mafGUIComboBox
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

class MAF_EXPORT mafGUIComboBox: public wxComboBox
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
#ifdef WIN32
    /*virtual*/ bool MSWCommand(WXUINT param, WXWORD id);
#endif
    /*virtual*/ void SetSelection(int n);

protected:

  mafObserver *m_Listener;
  int m_OldValue;
  DECLARE_EVENT_TABLE()
};
#endif
