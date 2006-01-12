/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPicButton.cpp,v $
  Language:  C++
  Date:      $Date: 2006-01-12 11:16:31 $
  Version:   $Revision: 1.14 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmgPicButton.h"
#include "mafDecl.h"
#include "mafPics.h"

//#define BN_CLICKED 0  //SIL. 23-3-2005:  Hack to be Removed
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgPicButton,wxBitmapButton)
     EVT_SET_FOCUS(mmgPicButton::OnSetFocus) 
     EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   mmgPicButton::Command)
     EVT_COMMAND_RANGE(PIC_START,PIC_END,wxEVT_COMMAND_BUTTON_CLICKED,   mmgPicButton::Command)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mmgPicButton,wxBitmapButton)
//----------------------------------------------------------------------------
mmgPicButton::mmgPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id, mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_Id = id;

  wxBitmap b = mafPics.GetBmp(BitmapId);
  wxSize size(b.GetWidth(),b.GetHeight());

  Create(parent, m_Id, b, wxDefaultPosition, size,0);
  SetBitmap(BitmapId,id);
};
//----------------------------------------------------------------------------
void mmgPicButton::SetEventId(long EventId)
//----------------------------------------------------------------------------
{
   m_Id = EventId;
}
//----------------------------------------------------------------------------
void mmgPicButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, m_Id));
}
/*#ifdef __WIN32__
//----------------------------------------------------------------------------
bool mmgPicButton::MSWCommand(WXUINT param, WXWORD id)
//----------------------------------------------------------------------------
{
  bool processed = FALSE;
  switch ( param )
  {
    case 1:                     // message came from an accelerator
    case BN_CLICKED:            // normal buttons send this.
    //case BN_DOUBLECLICKED:    // 
			if(m_Listener) 
				m_Listener->OnEvent(&mafEvent(this, m_Id));
			else
				processed = SendClickEvent();
    break;
  }
  return processed;
}
#endif*/
//----------------------------------------------------------------------------
void mmgPicButton::SetBitmap(wxString BitmapId, wxWindowID id )
//----------------------------------------------------------------------------
{
  wxBitmap b = mafPics.GetBmp(BitmapId);
  SetBitmapLabel(b);
  SetBitmapDisabled(GrayScale(b));
  m_Id = id;
  Refresh();
}
