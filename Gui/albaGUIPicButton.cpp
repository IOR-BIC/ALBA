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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGUIPicButton.h"
#include "albaDecl.h"
#include "albaPics.h"

//#define BN_CLICKED 0  //SIL. 23-3-2005:  Hack to be Removed
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIPicButton,wxBitmapButton)
     EVT_SET_FOCUS(albaGUIPicButton::OnSetFocus) 
     EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   albaGUIPicButton::Command)
     EVT_COMMAND_RANGE(PIC_START,PIC_END,wxEVT_COMMAND_BUTTON_CLICKED,   albaGUIPicButton::Command)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(albaGUIPicButton,wxBitmapButton)
//----------------------------------------------------------------------------
albaGUIPicButton::albaGUIPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id, albaObserver *listener, int offset)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_Id = id;

  wxBitmap b = albaPictureFactory::GetPictureFactory()->GetBmp(BitmapId);
  wxSize size(b.GetWidth()+offset,b.GetHeight()+offset);

  //Create(parent, m_Id, b, wxDefaultPosition, size,0);
  Create(parent, m_Id, b, wxDefaultPosition, size);
  SetBitmap(BitmapId,id);
}
//----------------------------------------------------------------------------
albaGUIPicButton::albaGUIPicButton(wxWindow *parent, wxBitmap *b, wxWindowID id, albaObserver *listener, int offset)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  m_Id = id;

  wxSize size(b->GetWidth()+offset,b->GetHeight()+offset);
  //Create(parent, m_Id, *b, wxDefaultPosition, size,0);
  Create(parent, m_Id, *b, wxDefaultPosition, size);

  SetBitmapLabel(*b);
  //SetBitmapDisabled(albaGrayScale(*b));
  Refresh();
}
//----------------------------------------------------------------------------
void albaGUIPicButton::SetEventId(long EventId)
//----------------------------------------------------------------------------
{
   m_Id = EventId;
}
//----------------------------------------------------------------------------
void albaGUIPicButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this, m_Id));
}
//----------------------------------------------------------------------------
void albaGUIPicButton::SetBitmap(wxString BitmapId, wxWindowID id )
//----------------------------------------------------------------------------
{
  wxBitmap b = albaPictureFactory::GetPictureFactory()->GetBmp(BitmapId);
  SetBitmapLabel(b);
  //SetBitmapDisabled(albaGrayScale(b));
  m_Id = id;
  Refresh();
}
//----------------------------------------------------------------------------
void albaGUIPicButton::SetListener(albaObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener = listener;
}
