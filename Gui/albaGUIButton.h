/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIButton
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIButton_H__
#define __albaGUIButton_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
//----------------------------------------------------------------------------
// albaGUIButton :
/**
albaGUIButton inherit from wxButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.

albaGUIButton is able to send both wxWindows messages and albaEvents.
albaEvents are sent if m_Listener in not NULL otherwise wxWindows messages are sent.
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUIButton : public wxButton
{

public:
  albaGUIButton(wxWindow* parent, wxWindowID id, const wxString& label, 
            const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0);

  albaGUIButton(wxWindow *parent, wxWindowID id = 0 );

  /** Set the Listener that will receive event-notification. */
  void SetListener  (albaObserver *listener)   {m_Listener = listener;}; 

protected:  
	  
	/** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);
  
	/** Called when the button take the focus. */
	void OnSetFocus(wxFocusEvent& event) {}; 

  int m_Id;
  albaObserver  *m_Listener;     

  DECLARE_EVENT_TABLE()
};
#endif  // __albaGUIButton_H__
