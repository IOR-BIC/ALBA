/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPicButton.h,v $
  Language:  C++
  Date:      $Date: 2006-01-20 14:05:56 $
  Version:   $Revision: 1.13 $
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
#include "mafObserver.h"

//----------------------------------------------------------------------------
// mmgPicButton :
/**
mmgPicButton inherit from wxBitmapButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.


To create a mmgPicButton you must specify
- parent
- Bitmap_ID : which should be a valid pic-index in the pic-factory (see mmgPics)
- ID: the ID of the Event that will be sent to the mafListener --- must be in the range MINID-MAXID or PIC_START-PIC-STOP (see mafDecl.h)
- obviously you must also specify a Listener */
//----------------------------------------------------------------------------
class mmgPicButton : public wxBitmapButton
{
DECLARE_DYNAMIC_CLASS(mmgPicButton)

public:
  mmgPicButton() {};
  mmgPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id = 0, mafObserver *listener = NULL);
  mmgPicButton(wxWindow *parent, wxBitmap *b, wxWindowID id = 0, mafObserver *listener = NULL);

  /** Set the Listener that will receive event-notification. */
  void SetListener(mafObserver *listener);

  /** allow to change the Event-Id at run time */
  void SetEventId(long EventId); 

  /**  Used to change Bitmap and Event-ID at run time.
  Example: the Time Bar Play button once pressed become a Stop Button 
  which send a Stop-Event-Id, when pressed again is reverted to Play. */
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
