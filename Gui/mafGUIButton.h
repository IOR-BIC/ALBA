/*=========================================================================

 Program: MAF2
 Module: mafGUIButton
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIButton_H__
#define __mafGUIButton_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
//----------------------------------------------------------------------------
// mafGUIButton :
/**
mafGUIButton inherit from wxButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.

mafGUIButton is able to send both wxWindows messages and mafEvents.
mafEvents are sent if m_Listener in not NULL otherwise wxWindows messages are sent.
*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafGUIButton : public wxButton
{

public:
  mafGUIButton(wxWindow* parent, wxWindowID id, const wxString& label, 
            const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0);

  mafGUIButton(wxWindow *parent, wxWindowID id = 0 );

  /** Set the Listener that will receive event-notification. */
  void SetListener  (mafObserver *listener)   {m_Listener = listener;}; 

protected:  
	/** Called to set the button as default. */
  virtual void SetDefault() {};
  
	/** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);
  
	/** Called when the button take the focus. */
	void OnSetFocus(wxFocusEvent& event) {}; 

  int m_Id;
  mafObserver  *m_Listener;     

  DECLARE_EVENT_TABLE()
};
#endif  // __mafGUIButton_H__
