/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testCheckTreeLogic
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


#include "testCheckTreeLogic.h"
#include "albaGUIMDIFrame.h"
#include "albaGUINamedPanel.h"
#include "albaGUISashPanel.h"
#include "albaWXLog.h"
//#include "testCheckTreeGui.h" 
#include "albaPics.h"

#include "albaSideBar.h"
#include "albaView.h"
#include "albaNode.h"
#include "albaGUITreeContextualMenu.h"
#include "albaGUICheckTree.h"


albaCxxTypeMacro(albaNodeA);
albaCxxTypeMacro(albaNodeB);

//----------------------------------------------------------------------------
testCheckTreeLogic::testCheckTreeLogic()
//----------------------------------------------------------------------------
{
  m_win = new albaGUIMDIFrame("testTree", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  //m_node  = albaNode::New();
  albaNEW(m_nodea);
  m_nodea->SetName("m_nodea");
  albaNEW(m_nodeb);
  m_nodeb->SetName("m_nodeb");
  m_nodeb->ReparentTo(m_nodea);

  #include "albaNode.xpm"  
  albaADDVMEPIC(albaNode); 
  #include "albaNodeA.xpm"
  albaADDVMEPIC(albaNodeA);
  #include "albaNodeB.xpm"
  albaADDVMEPIC(albaNodeB);

/*
  #include "pic/FILE_NEW.xpm"  
  #include "pic/FILE_OPEN.xpm"  
  #include "pic/FILE_SAVE.xpm"  
  albaPics.AddVmePic("albaNode",FILE_NEW_xpm); 
  albaPics.AddVmePic("albaNodeA",FILE_OPEN_xpm); 
  albaPics.AddVmePic("albaNodeB",FILE_SAVE_xpm); 
*/

  CreateMenu();
  CreateToolBar();
  CreateLogBar();
  CreateSideBar();
}
//----------------------------------------------------------------------------
testCheckTreeLogic::~testCheckTreeLogic()
//----------------------------------------------------------------------------
{
  albaDEL(m_nodea);
  albaDEL(m_nodeb);
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::OnQuit()
//----------------------------------------------------------------------------
{
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testCheckTreeLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)) 
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
    case MENU_VIEW_LOGBAR:
      if(m_log_bar) m_log_bar->Show(!m_log_bar->IsShown());
      break; 
    case MENU_VIEW_SIDEBAR:
      if(m_side_bar) m_side_bar->Show(!m_side_bar->IsShown());
      break; 
    case SHOW_CONTEXTUAL_MENU:
      {
        albaGUITreeContextualMenu *contextMenu = new albaGUITreeContextualMenu();
        contextMenu->SetListener(this);
        //albaView *v = m_ViewManager->GetSelectedView();
        albaNode *vme = e->GetVme();
        bool vme_menu = e->GetBool();
        bool autosort = e->GetArg() != 0;
        contextMenu->CreateContextualMenu((albaGUICheckTree *)e->GetSender(),NULL,vme,vme_menu);
        contextMenu->ShowContextualMenu();
        delete contextMenu;
        contextMenu = NULL;
      }
      break;
    case UPDATE_UI:
      break; 
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::CreateMenu()
//----------------------------------------------------------------------------
{
  m_menu_bar  = new wxMenuBar;
  wxMenu    *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");
  wxMenu    *view_menu = new wxMenu;
  m_menu_bar->Append(view_menu, "&View");
  m_win->SetMenuBar(m_menu_bar);
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::CreateLogBar()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  albaWXLog *m_logger = new albaWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  albaGUINamedPanel *log_panel = new albaGUINamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_bar = new albaGUISashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_log_bar->Put(log_panel);
  //m_log_bar->Show(false);
  wxLogMessage("buongiorno");
}

//----------------------------------------------------------------------------
void testCheckTreeLogic::CreateToolBar()
//----------------------------------------------------------------------------
{
  m_toolbar = new wxToolBar(m_win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_toolbar->SetMargins(0,0);
  m_toolbar->SetToolSeparation(2);
  m_toolbar->SetToolBitmapSize(wxSize(20,20));
  m_toolbar->AddTool(MENU_FILE_NEW,albaPics.GetBmp("FILE_NEW"),    "new msf storage file");
  m_toolbar->AddTool(MENU_FILE_OPEN,albaPics.GetBmp("FILE_OPEN"),  "open msf storage file");
  m_toolbar->AddTool(MENU_FILE_SAVE,albaPics.GetBmp("FILE_SAVE"),  "save current msf storage file");
  m_toolbar->AddSeparator();

  m_toolbar->AddTool(OP_UNDO,albaPics.GetBmp("OP_UNDO"),  "undo (ctrl+z)");
  m_toolbar->AddTool(OP_REDO,albaPics.GetBmp("OP_REDO"),  "redo (ctrl+shift+z)");
  m_toolbar->AddSeparator();

  m_toolbar->AddTool(OP_CUT,  albaPics.GetBmp("OP_CUT"),  "cut selected vme (ctrl+x)");
  m_toolbar->AddTool(OP_COPY, albaPics.GetBmp("OP_COPY"), "copy selected vme (ctrl+c)");
  m_toolbar->AddTool(OP_PASTE,albaPics.GetBmp("OP_PASTE"),"paste vme (ctrl+v)");
  m_toolbar->AddSeparator();
  m_toolbar->AddTool(CAMERA_RESET,albaPics.GetBmp("ZOOM_ALL"),"reset camera to fit all (ctrl+f)");
  m_toolbar->AddTool(CAMERA_FIT,  albaPics.GetBmp("ZOOM_SEL"),"reset camera to fit selected object (ctrl+shift+f)");
  m_toolbar->AddTool(CAMERA_FLYTO,albaPics.GetBmp("FLYTO"),"fly to object under mouse (press f inside a 3Dview)");
  m_toolbar->Realize();
  m_win->SetToolBar(m_toolbar);
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = NULL;
  m_side_bar = new albaGUISashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,330,"Side Bar \tCtrl+S");

  m_tree = new albaGUICheckTree(m_side_bar,-1,true);
  m_tree->SetListener(this);
  m_tree->SetTitle("albaGUICheckTree");
  m_side_bar->Put(m_tree);

  m_tree->VmeAdd(m_nodea);
  m_tree->VmeAdd(m_nodeb);
}
