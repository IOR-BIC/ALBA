/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIComboBox.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:04:57 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi
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


#include "mafGUIComboBox.h"
//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define ID_CLB 100
//----------------------------------------------------------------------------
// mafGUIComboBox
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIComboBox,wxComboBox)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIComboBox::mafGUIComboBox(wxWindow* parent, wxWindowID id, wxArrayString array,const wxPoint& pos, const wxSize& size, long style)
:wxComboBox(parent,id,"",pos,size,array,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_OldValue = -1;
}
//----------------------------------------------------------------------------
mafGUIComboBox::~mafGUIComboBox( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafGUIComboBox::MSWCommand(WXUINT param, WXWORD id)
//----------------------------------------------------------------------------
{
  bool result;
  result = wxComboBox::MSWCommand(param,id);
  switch ( param )
  {
    case CBN_SELCHANGE:
      {
        if(GetSelection() != m_OldValue)
          mafEventMacro(mafEvent(this, GetId(), (long)GetSelection()));
        m_OldValue = GetSelection();
      }
      
      break;
  }
  return result;
}
//----------------------------------------------------------------------------
void mafGUIComboBox:: SetSelection(int n)
//----------------------------------------------------------------------------
{
  wxComboBox::SetSelection(n);
  m_OldValue = GetSelection();
}
