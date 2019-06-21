/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIButton
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


#include "albaGUIButton.h"

 //SIL. 23-3-2005:  -- hack to be removed
#define BN_CLICKED 0
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIButton,wxButton)
     EVT_SET_FOCUS(albaGUIButton::OnSetFocus) 
     EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   albaGUIButton::Command)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIButton::albaGUIButton(wxWindow* parent, wxWindowID id, const wxString& label, 
                     const wxPoint& pos, const wxSize& size, long style)
: wxButton(parent,id,label,pos,size,style)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_Id = id;
};
//----------------------------------------------------------------------------
void albaGUIButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this, m_Id));
}
