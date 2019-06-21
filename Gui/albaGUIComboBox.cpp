/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIComboBox
 Authors: Daniele Giunchi
 
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


#include "albaGUIComboBox.h"
//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define ID_CLB 100
//----------------------------------------------------------------------------
// albaGUIComboBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIComboBox,wxComboBox)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIComboBox::albaGUIComboBox(wxWindow* parent, wxWindowID id, wxArrayString array,const wxPoint& pos, const wxSize& size, long style)
:wxComboBox(parent,id,"",pos,size,array,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_OldValue = -1;
}
//----------------------------------------------------------------------------
albaGUIComboBox::~albaGUIComboBox( ) 
//----------------------------------------------------------------------------
{
}
#ifdef WIN32
//----------------------------------------------------------------------------
bool albaGUIComboBox::MSWCommand(WXUINT param, WXWORD id)
//----------------------------------------------------------------------------
{
  bool result;
  result = wxComboBox::MSWCommand(param,id);
  switch ( param )
  {
    case CBN_SELCHANGE:
      {
        if(GetSelection() != m_OldValue)
          albaEventMacro(albaEvent(this, GetId(), (long)GetSelection()));
        m_OldValue = GetSelection();
      }
      
      break;
  }
  return result;
}
#endif
//----------------------------------------------------------------------------
void albaGUIComboBox:: SetSelection(int n)
//----------------------------------------------------------------------------
{
  wxComboBox::SetSelection(n);
  m_OldValue = GetSelection();
}
