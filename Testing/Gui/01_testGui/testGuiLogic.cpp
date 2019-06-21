/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testGuiLogic
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


#include "testGuiLogic.h" 
//#include "albaGUIMDIFrame.h"
#include "albaGUIFrame.h"
#include "testGui.h" 
#include "albaGUIPanel.h" 

//----------------------------------------------------------------------------
testGuiLogic::testGuiLogic()
//----------------------------------------------------------------------------
{
  //m_win = new albaGUIMDIFrame("testFrame", wxDefaultPosition, wxSize(800, 600));
  m_win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);
  //albaGUIPanel *p = new albaGUIPanel(m_win,-1);
  //p->Show(true);
  //p->SetSize(100,100);
  m_testGui = new testGui(m_win);
  //p->Add(m_testGui->GetGui());
}
//----------------------------------------------------------------------------
testGuiLogic::~testGuiLogic()
//----------------------------------------------------------------------------
{
  cppDEL(m_testGui);
}
//----------------------------------------------------------------------------
void testGuiLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testGuiLogic::OnQuit()
//----------------------------------------------------------------------------
{
	m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testGuiLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testGuiLogic::OnEvent(albaEventBase *alba_event)
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


