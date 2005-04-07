/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgButton.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 11:39:44 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgButton_H__
#define __mmgButton_H__

#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"
//----------------------------------------------------------------------------
// mmgButton :
/**
mmgButton inherit from wxButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.

mmgButton is able to send both wxWindows messages and mafEvents.
mafEvents are sent if m_Listener in not NULL otherwise wxWindows messages are sent.
*/
//----------------------------------------------------------------------------
class mmgButton : public wxButton
{

public:
  mmgButton(wxWindow* parent, wxWindowID id, const wxString& label, 
            const wxPoint& pos, const wxSize& size = wxDefaultSize, long style = 0);

  mmgButton(wxWindow *parent, wxWindowID id = 0 );

  /** Set the Listener that will receive event-notification. */
  void SetListener  (mafEventListener *listener)   {m_Listener=listener;}; 

protected:  
	/** Called to set the button as default. */
  virtual void SetDefault() {};
  
	/** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);
  
	#ifdef __WIN32__
	/** Overriden function member to intercept event generation and forward it to the Listener (MSW Version). */
  virtual bool MSWCommand(WXUINT param, WXWORD id);
  #endif
	
	/** Called when the button take the focus. */
	void OnSetFocus(wxFocusEvent& event) {}; 

  int m_id;
  mafEventListener  *m_Listener;     

  DECLARE_EVENT_TABLE()
};
#endif  // __mmgButton_H__
