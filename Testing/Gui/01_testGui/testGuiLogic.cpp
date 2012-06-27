/*=========================================================================

 Program: MAF2
 Module: testGuiLogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testGuiLogic.h" 
//#include "mafGUIMDIFrame.h"
#include "mafGUIFrame.h"
#include "testGui.h" 
#include "mafGUIPanel.h" 

//----------------------------------------------------------------------------
testGuiLogic::testGuiLogic()
//----------------------------------------------------------------------------
{
  //m_win = new mafGUIMDIFrame("testFrame", wxDefaultPosition, wxSize(800, 600));
  m_win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);
  //mafGUIPanel *p = new mafGUIPanel(m_win,-1);
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
void testGuiLogic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event)) 
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


