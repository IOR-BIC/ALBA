/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPicButton.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:02 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mmgPicButton_H_
#define _mmgPicButton_H_

#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"
//----------------------------------------------------------------------------
// mmgPicButton :
/**
mmgPicButton inherit from wxBitmapButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.

mmgPicButton is able to send both wxWindows messages and mafEvents.
mafEvents are sent if m_Listener in not NULL otherwise wxWindows messages are sent.
*/
//----------------------------------------------------------------------------
class mmgPicButton : public wxBitmapButton
{
DECLARE_DYNAMIC_CLASS(mmgPicButton)

public:
  mmgPicButton() { };
  mmgPicButton(wxWindow *parent, long BitmapId, wxWindowID id = 0 );

  /** Set the Listener that will receive event-notification. */
  void SetListener(mafEventListener *listener)   {m_Listener=listener;}; 

  /** Link the bitmap to the button through the id. */
  void SetBitmap(long BitmapId, wxWindowID id = 0); 

protected:  
  virtual void SetDefault() {};

  /** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);

  /** Overriden function member to intercept event generation and forward it to the Listener (MSW Version) .*/
  #ifdef __WIN32__
	virtual bool MSWCommand(WXUINT param, WXWORD id);
  #endif
  void OnSetFocus(wxFocusEvent& event) {}; 

  int m_id;
  mafEventListener  *m_Listener;     

DECLARE_EVENT_TABLE()
};
#endif // _mmgPicButton_H_
