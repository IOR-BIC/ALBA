/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIPicButton
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIPicButton_H__
#define __albaGUIPicButton_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"

//----------------------------------------------------------------------------
// albaGUIPicButton :
/**
albaGUIPicButton inherit from wxBitmapButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.


To create a albaGUIPicButton you must specify
- parent
- Bitmap_ID : which should be a valid pic-index in the pic-factory (see albaGUIPics)
- ID: the ID of the Event that will be sent to the albaListener --- must be in the range MINID-MAXID or PIC_START-PIC-STOP (see albaDecl.h)
- obviously you must also specify a Listener */
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUIPicButton : public wxBitmapButton
{
DECLARE_DYNAMIC_CLASS(albaGUIPicButton)

public:
  albaGUIPicButton() {};
  albaGUIPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id = 0, albaObserver *listener = NULL, int offset = 0);
  albaGUIPicButton(wxWindow *parent, wxBitmap *b, wxWindowID id = 0, albaObserver *listener = NULL, int offset = 0);

  /** Set the Listener that will receive event-notification. */
  void SetListener(albaObserver *listener);

  /** allow to change the Event-Id at run time */
  void SetEventId(long EventId); 

  /**  Used to change Bitmap and Event-ID at run time.
  Example: the Time Bar Play button once pressed become a Stop Button 
  which send a Stop-Event-Id, when pressed again is reverted to Play. */
  void SetBitmap(wxString BitmapId, wxWindowID id = 0); 

protected:  
  /** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);

  void OnSetFocus(wxFocusEvent& event) {}; 

  int m_Id;
  albaObserver  *m_Listener;

	DECLARE_EVENT_TABLE()
};
#endif // __albaGUIPicButton_H__
