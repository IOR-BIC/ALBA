/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPicButton.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:02 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmgBitmaps.h"
#include "mmgPicButton.h"

#define BN_CLICKED 0  //SIL. 23-3-2005:  Hack to be Removed
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgPicButton,wxBitmapButton)
     EVT_SET_FOCUS(mmgPicButton::OnSetFocus) 
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mmgPicButton,wxBitmapButton)
//----------------------------------------------------------------------------
mmgPicButton::mmgPicButton(wxWindow *parent, long BitmapId, wxWindowID id)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  if(id == 0) 
		m_id = BitmapId; 
	else 
		m_id = id;

  wxBitmap b = mmgBitmaps(BitmapId);
  wxSize size(b.GetWidth(),b.GetHeight());
   
  Create(parent, m_id, mmgBitmaps(BitmapId), wxDefaultPosition, size);
  SetBitmapFocus(mmgBitmaps(BitmapId));
};
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
void mmgPicButton::SetBitmap(long BitmapId, wxWindowID id )
//----------------------------------------------------------------------------
{
  SetBitmapLabel(mmgBitmaps(BitmapId));
  SetBitmapFocus(mmgBitmaps(BitmapId));
  if(id == 0) m_id = BitmapId; else m_id = id;
  Refresh();
}
