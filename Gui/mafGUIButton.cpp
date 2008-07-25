/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIButton.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
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


#include "mafGUIButton.h"

 //SIL. 23-3-2005:  -- hack to be removed
#define BN_CLICKED 0
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIButton,wxButton)
     EVT_SET_FOCUS(mafGUIButton::OnSetFocus) 
     EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   mafGUIButton::Command)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIButton::mafGUIButton(wxWindow* parent, wxWindowID id, const wxString& label, 
                     const wxPoint& pos, const wxSize& size, long style)
: wxButton(parent,id,label,pos,size,style)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_Id = id;
};
//----------------------------------------------------------------------------
void mafGUIButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, m_Id));
}
