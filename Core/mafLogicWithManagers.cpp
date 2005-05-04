/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithManagers.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:43:08 $
  Version:   $Revision: 1.11 $
  Authors:   Silvano Imboden, Paolo Quadrani
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


#include "mafLogicWithManagers.h"

#include "mafView.h"
#include "mafViewManager.h"
#include "mafVMEManager.h"
#include "mafOpManager.h"

#include "mmgMDIFrame.h"
#include "mmgMDIChild.h"
#include "mafSideBar.h"
#include "mmgTimeBar.h"
//----------------------------------------------------------------------------
mafLogicWithManagers::mafLogicWithManagers()
: mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  m_SideBar      = NULL;
  m_VMEManager   = false;
  m_ViewManager   = false;
  m_OpManager     = false;

  m_ImportMenu; 
  m_ExportMenu; 
  m_RecentFileMenu;
  m_OpMenu;
  m_ViewMenu; 
}
//----------------------------------------------------------------------------
mafLogicWithManagers::~mafLogicWithManagers( ) 
//----------------------------------------------------------------------------
{
  // Managers are distructed in the OnClose 
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithGUI::Configure(); // create the GUI - and calls CreateMenu

  if(m_SideSash)
  {
    m_SideBar = new mafSideBar(m_SideSash,-1,this);
    m_SideSash->Put(m_SideBar->m_Notebook);
  }

  if(m_UseVMEManager)
  {
    m_VMEManager = new mafVMEManager();
    m_VMEManager->SetListener(this); 
  }
  if(m_UseViewManager)
  {
    m_ViewManager = new mafViewManager();
    m_ViewManager->SetListener(this); 
  }
  if(m_UseOpManager)
  {
    m_OpManager = new mafOpManager();
    m_OpManager->SetListener(this); 
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafView* view) 
//----------------------------------------------------------------------------
{
  if(m_ViewManager) m_ViewManager->ViewAdd(view);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafOp *op)
//----------------------------------------------------------------------------
{
  if(m_OpManager) m_OpManager->OpAdd(op);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Show()
//----------------------------------------------------------------------------
{
  if(m_VMEManager && m_RecentFileMenu)
    m_VMEManager->SetFileHistoryMenu(m_RecentFileMenu);

  if(m_UseViewManager && m_ViewMenu)
    m_ViewManager->FillMenu(m_ViewMenu);

  if(m_OpManager )
  {
    if(m_MenuBar && m_ImportMenu && m_OpMenu && m_ExportMenu) 
    {
      m_OpManager->FillMenu(m_ImportMenu,m_ExportMenu,m_OpMenu);
      m_OpManager->SetMenubar(m_MenuBar);
    }
    if(m_TooBar)  m_OpManager->SetToolbar(m_TooBar);
  }

  mafLogicWithGUI::Show();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Init(int argc, char **argv)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
    if(argc > 1 )
	  {
		  wxString file = argv[1];
		  if(wxFileExists(file))
		  {
			  m_VMEManager->MSFOpen(file);
			  UpdateFrameTitle();
		  }
	  }
    else
    {
      m_VMEManager->MSFNew();
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateMenu()
//----------------------------------------------------------------------------
{
  m_MenuBar  = new wxMenuBar;
  wxMenu    *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_NEW,   "&New");
  file_menu->Append(MENU_FILE_OPEN,  "&Open ..");
  file_menu->Append(MENU_FILE_SAVE,  "&Save");
  file_menu->Append(MENU_FILE_SAVEAS,"Save &As ..");
  file_menu->AppendSeparator();

  wxMenu *import_menu = NULL;
  wxMenu *export_menu = NULL;

  m_ImportMenu = new wxMenu;
  file_menu->Append(0,"Import",m_ImportMenu );
  m_ExportMenu = new wxMenu;
  file_menu->Append(0,"Export",m_ExportMenu);					
  file_menu->AppendSeparator();

  m_RecentFileMenu = new wxMenu;
  file_menu->Append(0,"Recent Files",m_RecentFileMenu);
  //m_VmeManager->SetFileHistoryMenu(recentFile_menu);

  file_menu->AppendSeparator();
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_MenuBar->Append(file_menu, "&File");

  wxMenu    *edit_menu = new wxMenu;
  edit_menu->Append(OP_UNDO,   "Undo  \tCtrl+Z");
  edit_menu->Append(OP_REDO,   "Redo  \tCtrl+Shift+Z");
  edit_menu->AppendSeparator();
  edit_menu->Append(OP_DELETE, "Delete");
  edit_menu->Append(OP_CUT,   "Cut   \tCtrl+Shift+X");
  edit_menu->Append(OP_COPY,  "Copy  \tCtrl+Shift+C");
  edit_menu->Append(OP_PASTE, "Paste \tCtrl+Shift+V");
  m_MenuBar->Append(edit_menu, "&Edit");

  m_ViewMenu = new wxMenu;
  //m_ViewManager->FillMenu(m_ViewMenu);  
  m_ViewMenu->AppendSeparator();
  if(this->m_PlugToolbar) m_ViewMenu->Append(MENU_VIEW_TOOLBAR, "Toolbar","",true);
  m_MenuBar->Append(m_ViewMenu, "&View");

  m_OpMenu = new wxMenu;
  m_MenuBar->Append(m_OpMenu, "&Operations");

  m_Win->SetMenuBar(m_MenuBar);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateFrameTitle()
//----------------------------------------------------------------------------
{
  wxString title(m_AppTitle);
  title += "   " + m_VMEManager->GetFileName();
  m_Win->SetTitle(title);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    if(e->GetId()!= UPDATE_UI)
    {
      e->Log(); // for debugging pourpose
      int foo=0;
    }

    switch(e->GetId())
    {
      // ###############################################################
      // commands related to FILE MENU  
    case MENU_FILE_NEW:
      OnFileNew();
      break; 
    case MENU_FILE_OPEN:
      OnFileOpen();
      break; 
    case wxID_FILE1:
    case wxID_FILE2:
    case wxID_FILE3:
    case wxID_FILE4:
      OnFileHistory(e->GetId());
      break;
    case MENU_FILE_SAVE:
      OnFileSave();
      break; 
    case MENU_FILE_SAVEAS:
      OnFileSaveAs();
      break; 
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
      // ###############################################################
      // commands related to VME
    case VME_SELECT:	
      VmeSelect(*e);		
      break; 
    case VME_SELECTED: 
      VmeSelected(e->GetVme());
      break;
    case VME_SHOW:
      VmeShow(e->GetVme(), e->GetBool());
      break;
    case VME_MODIFIED:
      VmeModified(e->GetVme());
      break; 
    case VME_ADD:
      VmeAdd(e->GetVme());
      break; 
    case VME_ADDED:
      VmeAdded(e->GetVme());
      break; 
    case VME_REMOVE:
      VmeRemove(e->GetVme());
      break; 
    case VME_REMOVING:
      VmeRemoving(e->GetVme());
      break; 
      // ###############################################################
      // commands related to OP
    case MENU_OP:
      if(m_OpManager) m_OpManager->OpRun(e->GetArg());
      break;
    case OP_RUN_STARTING:
      OpRunStarting();
      break; 
    case OP_RUN_TERMINATED:
      OpRunTerminated();
      break; 
    case OP_SHOW_GUI:
      OpShowGui(!e->GetBool(), (mmgPanel*)e->GetWin());
      break; 
    case OP_HIDE_GUI:
      OpHideGui(e->GetBool());
      break; 
      // ###############################################################
      // commands related to VIEWS
    case VIEW_CREATE:
      ViewCreate(e->GetArg());
      break;
    case VIEW_CREATED:
      ViewCreated(e->GetView());
      break;
    case VIEW_DELETE:
      if(m_PlugSidebar)
        this->m_SideBar->ViewDeleted(e->GetView());
      break;	
    case VIEW_SELECT:
      ViewSelect();
      break;
    case CAMERA_UPDATE:
      if(m_ViewManager) m_ViewManager->CameraUpdate();
      break; 
    case CAMERA_RESET:
      if(m_ViewManager) m_ViewManager->CameraReset();
      break; 
    case CAMERA_FIT:
      if(m_ViewManager) m_ViewManager->CameraReset(true);
      break;
    case CAMERA_FLYTO:
      //if(m_ViewManager) m_ViewManager->CameraFlyToMode();
      break;
    case TIME_SET:
      TimeSet(e->GetFloat());
      break; 
    default:
      mafLogicWithGUI::OnEvent(event);
      break; 
    } // end switch case
  } // end if SafeDowncast
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileNew()
//----------------------------------------------------------------------------
{
	if(m_VMEManager)
  {
    if( m_VMEManager->AskConfirmAndSave())
	  {
		  m_VMEManager->MSFNew();
		  if(m_OpManager) m_OpManager->ClearUndoStack(); 
	  }
  }
	m_Win->SetTitle(m_AppTitle);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileOpen()
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	  if(m_VMEManager->AskConfirmAndSave())
	  {
		  wxString m_wildc    = "Multimod Storage Format file (*.msf)|*.msf";
		  wxString m_msf_dir  = mafGetApplicationDirectory().c_str();
               m_msf_dir += "/Data/MSF/";
		  wxString file				= mafGetOpenFile(m_msf_dir, m_wildc).c_str();
		  if(file == "") return;

		  m_VMEManager->MSFOpen(file);
		  if(m_OpManager) m_OpManager->ClearUndoStack(); 
	  }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileHistory(int menuId)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) 
  {
    this->m_VMEManager->MSFOpen(menuId);
    UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSave()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) m_VMEManager->MSFSave();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSaveAs()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) 
  {
    m_VMEManager->MSFSaveAs();
    UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{
  /*
  if(m_VMEManager)
  {
    bool quit = false;
    if (m_VMEManager->MSFIsModified())
    {
      int answer = wxMessageBox
          (
          "would you like to save your work before quitting ?",
          "Confirm", 
          wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
          );
      if(answer == wxYES) m_VMEManager->MSFSave();
      quit = answer != wxCANCEL;
    }
    else 
    {
    int answer = wxMessageBox("quit program?", "Confirm", wxYES_NO | wxICON_QUESTION , m_Win);
    quit = answer == wxYES;
    }
    if(!quit) return;
  }
  */
  mmgMDIChild::OnQuit(); 

  cppDEL(m_VMEManager);
  cppDEL(m_ViewManager);
  cppDEL(m_OpManager);

  // must be deleted after m_VMEManager
  cppDEL(m_SideBar);

  mafLogicWithGUI::OnQuit();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelect(mafEvent& e)	//modified by Paolo 10-9-2003
//----------------------------------------------------------------------------
{
  mafNode *vme = NULL;

	if(m_PlugSidebar && (e.GetSender() == this->m_SideBar->GetTree()))
    vme = (mafNode*)e.GetArg();//sender == tree => the vme is in e.arg
  else
    vme = e.GetVme();          //sender == ISV  => the vme is in e.vme  

	if(vme == NULL)
	{
		if(m_VMEManager)
    {
/*
      //vme can be selected by its name
		  wxString *vme_name = e.GetString()->c_str(); //@@@@@ ????
		  mafNodeRoot *root = this->m_VMEManager->GetRoot();
		  if (vme_name && root)
		  {
			  vme = root->FindInTreeByName(vme_name->c_str());        // not yet implemented
			  e.SetVme(vme);
		  }
*/
    }
	}
  if(vme != NULL && m_OpManager)
		m_OpManager->OpSelect(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_ViewManager) m_ViewManager->VmeSelect(vme);
  if(m_OpManager)   m_OpManager->VmeSelected(vme);
	if(m_SideBar)    m_SideBar->VmeSelected(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	if(m_ViewManager) m_ViewManager->VmeShow(vme, visibility);
	if(m_SideBar)    m_SideBar->VmeShow(vme,visibility);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_PlugTimebar) UpdateTimeBounds();
	if(m_SideBar)    m_SideBar->VmeModified(vme);
	if(m_VMEManager) m_VMEManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) m_VMEManager->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_ViewManager)
    m_ViewManager->VmeAdd(vme);
	if(m_SideBar)
    m_SideBar->VmeAdd(vme);
  UpdateTimeBounds();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
    m_VMEManager->VmeRemove(vme);
	UpdateTimeBounds();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemoving(mafNode *vme)
//----------------------------------------------------------------------------
{
	if(m_SideBar)
    m_SideBar->VmeRemove(vme);
	if(m_ViewManager)
    m_ViewManager->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunStarting()
//----------------------------------------------------------------------------
{
	EnableMenuAndToolbar(false);
  if(m_ViewManager) m_ViewManager->EnableSelect(false);
	if(m_SideBar)    m_SideBar->EnableSelect(false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunTerminated()
//----------------------------------------------------------------------------
{
	EnableMenuAndToolbar(true);
  if(m_ViewManager) m_ViewManager->EnableSelect(true);
  if(m_SideBar)    m_SideBar->EnableSelect(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::EnableMenuAndToolbar(bool enable)
//----------------------------------------------------------------------------
{
  EnableItem(MENU_FILE_NEW,enable);
  EnableItem(MENU_FILE_OPEN,enable);
  EnableItem(MENU_FILE_SAVE,enable);
  EnableItem(MENU_FILE_SAVEAS,enable );
  EnableItem(MENU_FILE_MERGE,enable );
  EnableItem(MENU_FILE_QUIT,enable );
  EnableItem(wxID_FILE1,enable );
  EnableItem(wxID_FILE2,enable );
  EnableItem(wxID_FILE3,enable );
  EnableItem(wxID_FILE4,enable );
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpShowGui(bool push_gui, mmgPanel *panel)
//----------------------------------------------------------------------------
{
	if(m_SideBar) m_SideBar->OpShowGui(push_gui, panel);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpHideGui(bool view_closed)
//----------------------------------------------------------------------------
{
	if(m_SideBar) m_SideBar->OpHideGui(view_closed);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreate(int viewId)
//----------------------------------------------------------------------------
{
	if(m_ViewManager)
  {
    mafView* v = m_ViewManager->ViewCreate(viewId);
    if(m_OpManager) VmeShow(m_OpManager->GetSelectedVme(),true);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewSelect()
//----------------------------------------------------------------------------
{
	// non e' corretto nel caso non ci sia il viewManager 
  // --- fornire modo diverso di ottenere la SelectedView
  if(m_ViewManager) 
  {
    mafView *view = m_ViewManager->GetSelectedView();
    if(m_SideBar)	m_SideBar->ViewSelect(view);

    EnableItem(CAMERA_RESET, view!=NULL);
    EnableItem(CAMERA_FIT,   view!=NULL);
    EnableItem(CAMERA_FLYTO, view!=NULL);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreated(mafView *v)
//----------------------------------------------------------------------------
{
  // tolto il supporto alle View esterne -- per il momento
  if(v) 
	{
      mmgMDIChild *c = new mmgMDIChild(m_Win,v);   
			c->SetListener(m_ViewManager);
			v->SetFrame(c);
 			//v->ShowSettings();
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TimeSet(float t)
//----------------------------------------------------------------------------
{
  if(m_VMEManager) m_VMEManager->TimeSet(t);
  if(m_ViewManager) m_ViewManager->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateTimeBounds()
//----------------------------------------------------------------------------
{
  float min,max; 
  if(m_VMEManager) m_VMEManager->TimeGetBounds(&min,&max);
  if(m_TimePanel)  m_TimePanel->SetBounds(min,max);
  if(m_TimeSash)   m_TimeSash->Show(min!=max);
}
//----------------------------------------------------------------------------
mafNode* mafLogicWithManagers::VmeChoose()
//----------------------------------------------------------------------------
{
  return NULL;
}
