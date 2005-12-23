/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPicButton.h,v $
  Language:  C++
  Date:      $Date: 2005-12-23 11:59:11 $
  Version:   $Revision: 1.8 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgPicButton_H__
#define __mmgPicButton_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
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
  mmgPicButton() {};
  mmgPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id = 0, mafObserver *listener = NULL);

  /** Set the Listener that will receive event-notification. */
  void SetListener(mafObserver *listener)   {m_Listener=listener;}; 
  void SetEventId(long EventId); 

  /** Link the bitmap to the button through the id. */
  void SetBitmap(wxString BitmapId, wxWindowID id = 0); 

protected:  
  virtual void SetDefault() {};

  /** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);

  /** Overriden function member to intercept event generation and forward it to the Listener (MSW Version) .*/
/*  #ifdef __WIN32__
	virtual bool MSWCommand(WXUINT param, WXWORD id);
  #endif*/
  void OnSetFocus(wxFocusEvent& event) {}; 

  int m_Id;
  mafObserver  *m_Listener;     

DECLARE_EVENT_TABLE()
};
#endif // __mmgPicButton_H__
