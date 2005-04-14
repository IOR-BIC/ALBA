/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testCheckTreeLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-14 15:17:31 $
  Version:   $Revision: 1.2 $
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


#include "testCheckTreeLogic.h"
#include "mmgMDIFrame.h"
#include "mmgNamedPanel.h"
#include "mmgSashPanel.h"
#include "mafWXLog.h"
//#include "testCheckTreeGui.h" 
#include "mafPics.h"

#include "mafSideBar.h"
#include "mafView.h"
#include "mafNode.h"
#include "mmgTreeContextualMenu.h"
#include "mmgCheckTree.h"


mafCxxTypeMacro(mafNodeA);
mafCxxTypeMacro(mafNodeB);

//----------------------------------------------------------------------------
testCheckTreeLogic::testCheckTreeLogic()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("testTree", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  //m_node  = mafNode::New();
  mafNEW(m_nodea);
  m_nodea->SetName("m_nodea");
  mafNEW(m_nodeb);
  m_nodeb->SetName("m_nodeb");
  m_nodeb->ReparentTo(m_nodea);

  #include "mafNode.xpm"  
  mafADDVMEPIC(mafNode); 
  #include "mafNodeA.xpm"
  mafADDVMEPIC(mafNodeA);
  #include "mafNodeB.xpm"
  mafADDVMEPIC(mafNodeB);

/*
  #include "pic/FILE_NEW.xpm"  
  #include "pic/FILE_OPEN.xpm"  
  #include "pic/FILE_SAVE.xpm"  
  mafPics.AddVmePic("mafNode",FILE_NEW_xpm); 
  mafPics.AddVmePic("mafNodeA",FILE_OPEN_xpm); 
  mafPics.AddVmePic("mafNodeB",FILE_SAVE_xpm); 
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
  mafDEL(m_nodea);
  mafDEL(m_nodeb);
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
void testCheckTreeLogic::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId())
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
      mmgTreeContextualMenu *contextMenu = new mmgTreeContextualMenu();
      contextMenu->SetListener(this);
      //mafView *v = m_ViewManager->GetSelectedView();
      mafNode *vme = e.GetVme();
      bool vme_menu = e.GetBool();
      bool autosort = e.GetArg() != 0;
      contextMenu->ShowContextualMenu((mmgCheckTree *)e.GetSender(),NULL,vme,vme_menu);
      delete contextMenu;
      contextMenu = NULL;
    }
    break;
    case UPDATE_UI:
    break; 
    default:
      e.Log();
    break; 
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
  mafWXLog *m_logger = new mafWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  mmgNamedPanel *log_panel = new mmgNamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_bar = new mmgSashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
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
  m_toolbar->AddTool(MENU_FILE_NEW,mafPics.GetBmp("FILE_NEW"),    "new msf storage file");
  m_toolbar->AddTool(MENU_FILE_OPEN,mafPics.GetBmp("FILE_OPEN"),  "open msf storage file");
  m_toolbar->AddTool(MENU_FILE_SAVE,mafPics.GetBmp("FILE_SAVE"),  "save current msf storage file");
  m_toolbar->AddSeparator();

  m_toolbar->AddTool(OP_UNDO,mafPics.GetBmp("OP_UNDO"),  "undo (ctrl+z)");
  m_toolbar->AddTool(OP_REDO,mafPics.GetBmp("OP_REDO"),  "redo (ctrl+shift+z)");
  m_toolbar->AddSeparator();

  m_toolbar->AddTool(OP_CUT,  mafPics.GetBmp("OP_CUT"),  "cut selected vme (ctrl+x)");
  m_toolbar->AddTool(OP_COPY, mafPics.GetBmp("OP_COPY"), "copy selected vme (ctrl+c)");
  m_toolbar->AddTool(OP_PASTE,mafPics.GetBmp("OP_PASTE"),"paste vme (ctrl+v)");
  m_toolbar->AddSeparator();
  m_toolbar->AddTool(CAMERA_RESET,mafPics.GetBmp("ZOOM_ALL"),"reset camera to fit all (ctrl+f)");
  m_toolbar->AddTool(CAMERA_FIT,  mafPics.GetBmp("ZOOM_SEL"),"reset camera to fit selected object (ctrl+shift+f)");
  m_toolbar->AddTool(CAMERA_FLYTO,mafPics.GetBmp("FLYTO"),"fly to object under mouse (press f inside a 3Dview)");
  m_toolbar->Realize();
  m_win->SetToolBar(m_toolbar);
}
//----------------------------------------------------------------------------
void testCheckTreeLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = NULL;
  m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,330,"Side Bar \tCtrl+S");

  m_tree = new mmgCheckTree(m_side_bar,-1,true);
  m_tree->SetListener(this);
  m_tree->SetTitle("mmgCheckTree");
  m_side_bar->Put(m_tree);

  m_tree->VmeAdd(m_nodea);
  m_tree->VmeAdd(m_nodeb);
}