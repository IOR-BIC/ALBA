/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIComboBox
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIComboBox_H__
#define __albaGUIComboBox_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUIPanel.h"

//----------------------------------------------------------------------------
// albaGUIComboBox :
/**
a ListBox that send albaEventNotification in the form
albaEvent(this, widget_id, item_id, checked)
*/
//----------------------------------------------------------------------------

class ALBA_EXPORT albaGUIComboBox: public wxComboBox
{
public:
    albaGUIComboBox(wxWindow* parent, 
                    wxWindowID id,
                    wxArrayString array,
                    const wxPoint& pos = wxDefaultPosition, 
                    const wxSize& size = wxDefaultSize, 
                    long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN);

    virtual  ~albaGUIComboBox();

    void SetListener(albaObserver *listener){m_Listener = listener;}
#ifdef WIN32
    /*virtual*/ bool MSWCommand(WXUINT param, WXWORD id);
#endif
    /*virtual*/ void SetSelection(int n);

protected:

  albaObserver *m_Listener;
  int m_OldValue;
  DECLARE_EVENT_TABLE()
};
#endif
