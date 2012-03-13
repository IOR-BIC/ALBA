/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:40 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testMenuLogic.h" 
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;

//----------------------------------------------------------------------------
testMenuLogic::testMenuLogic()
//----------------------------------------------------------------------------
{
  m_win = new mafGUIMDIFrame("testMenu", wxDefaultPosition, wxSize(800, 600));
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
void testMenuLogic::OnEvent(mafEventBase *maf_event)
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

