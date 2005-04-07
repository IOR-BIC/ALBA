/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPicButton.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 11:39:47 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmgPicButton.h"
#include "mafPics.h"

#define BN_CLICKED 0  //SIL. 23-3-2005:  Hack to be Removed
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgPicButton,wxBitmapButton)
     EVT_SET_FOCUS(mmgPicButton::OnSetFocus) 
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mmgPicButton,wxBitmapButton)
//----------------------------------------------------------------------------
mmgPicButton::mmgPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  //if(id == 0) m_id = BitmapId; else m_id = id;
  m_id = id; //SIL. 7-4-2005: 

  wxBitmap b = mafPics.GetBmp(BitmapId);
  wxSize size(b.GetWidth(),b.GetHeight());

  Create(parent, m_id, b, wxDefaultPosition, size);
  SetBitmapFocus(b);
};
//----------------------------------------------------------------------------
void mmgPicButton::SetEventId(long EventId)
//----------------------------------------------------------------------------
{
   m_id = EventId;
}
//----------------------------------------------------------------------------
void mmgPicButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  wxLogMessage("cmd");
}
#ifdef __WIN32__
//----------------------------------------------------------------------------
bool mmgPicButton::MSWCommand(WXUINT param, WXWORD id)
//----------------------------------------------------------------------------
{
  bool processed = FALSE;
  switch ( param )
  {
    case 1:                     // message came from an accelerator
    case BN_CLICKED:            // normal buttons send this
    //case BN_DOUBLECLICKED:      // owner-drawn ones also send this
			if(m_Listener) 
				m_Listener->OnEvent(mafEvent(this, m_id));
			else
				processed = SendClickEvent();
    break;
  }
  return processed;
}
#endif
//----------------------------------------------------------------------------
void mmgPicButton::SetBitmap(wxString BitmapId, wxWindowID id )
//----------------------------------------------------------------------------
{
  SetBitmapLabel(mafPics.GetBmp(BitmapId));
  SetBitmapFocus(mafPics.GetBmp(BitmapId));
  //if(id == 0) m_id = BitmapId; else  //SIL. 7-4-2005: 
    m_id = id;
  Refresh();
}
