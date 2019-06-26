/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testMenuLogic
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


#include "testMenuLogic.h" 
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaGUIMDIFrame;
class albaGUISashPanel;

//----------------------------------------------------------------------------
testMenuLogic::testMenuLogic()
//----------------------------------------------------------------------------
{
  m_win = new albaGUIMDIFrame("testMenu", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

	m_menu_bar  = new wxMenuBar;
  wxMenu    *file_menu = new wxMenu;
    file_menu->Append(MENU_FILE_QUIT,  "&Quit");
    m_menu_bar->Append(file_menu, "&File");

  m_win->SetMenuBar(m_menu_bar);
}
//----------------------------------------------------------------------------
void testMenuLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testMenuLogic::OnQuit()
//----------------------------------------------------------------------------
{
	m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testMenuLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testMenuLogic::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)) 
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
    default:
      e->Log();
      break; 
    }
  }
}

