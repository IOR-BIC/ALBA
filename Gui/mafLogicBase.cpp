/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicBase.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:21 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden, Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#include "mafLogicBase.h"

#include "wx/utils.h"
#include "wx/time.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafView.h"
#include "mmgMDIFrame.h"
#include "mmgMDIChild.h"
#include "mmgFrame.h"
#include "mmgPicButton.h"
#include "mmgSplittedPanel.h"
#include "mmgSashPanel.h"
#include "mmgNamedPanel.h"
#include "mmgCrossSplitter.h"
#include "mmgTimeBar.h"
#include "mafSideBar.h"
#include "mafWXLog.h"
//----------------------------------------------------------------------------
mafLogicBase::mafLogicBase()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("maf", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  m_log_bar				= NULL;
  m_time_bar			= NULL;
	m_SideBarTabbed	= NULL;

	m_log_to_file			= false;
	m_log_all_events	= false;
	m_logger					= NULL;

	m_AppTitle = "";

	m_PlugMenu		= true;
	m_PlugToolbar	= true;
	m_PlugSidebar	= false;
	m_PlugTimebar	= false;
	m_PlugLogbar	= false;

	m_AppIcon		= NULL;
	m_ChildIcon	= NULL;
}
//----------------------------------------------------------------------------
mafLogicBase::~mafLogicBase( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafLogicBase::Show()
//----------------------------------------------------------------------------
{
  if(m_AppIcon) m_win->SetIcon(m_AppIcon);
  m_AppTitle = m_win->GetTitle();

  if(m_PlugMenu)		this->CreateMenu();
  if(m_PlugToolbar) this->CreateToolbar();
  if(m_PlugSidebar) this->CreateSidebar();
  if(m_PlugTimebar) this->CreateTimebar();
  if(m_PlugLogbar)	
	{
		this->CreateLogbar();
	}
	else
	{
		this->CreateNullLog();
	}

	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafLogicBase::CreateMenu()
//----------------------------------------------------------------------------
{
	m_menu_bar  = new wxMenuBar;
    wxMenu    *file_menu = new wxMenu;
      file_menu->Append(MENU_FILE_QUIT,  "&Quit");
    m_menu_bar->Append(file_menu, "&File");

  m_win->SetMenuBar(m_menu_bar);
}
//----------------------------------------------------------------------------
void mafLogicBase::CreateNullLog()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
	m_logger = new mafWXLog(log);
	log->Show(false);
	wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);
}
//----------------------------------------------------------------------------
void mafLogicBase::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId())
  {
  case MENU_FILE_QUIT:
			OnQuit();		
  break; 
  default:
    e.Log();
  break; 
  }
}
//----------------------------------------------------------------------------
void mafLogicBase::OnQuit()
//----------------------------------------------------------------------------
{
/*
	bool quit = false;
  if (m_VmeManager->MSFIsModified())
	{
		int answer = wxMessageBox("would you like to save your work before quitting ?","Confirm", wxYES_NO|wxCANCEL|wxICON_QUESTION , m_win);
		if(answer == wxYES) m_VmeManager->MSFSave();
		quit = answer != wxCANCEL;
	}
	else 
	{
		int answer = wxMessageBox("quit program?", "Confirm", wxYES_NO | wxICON_QUESTION , m_win);
		quit = answer == wxYES;
	}
	if(!quit) return;
*/
  {
		mmgMDIChild::OnQuit(); //prevent all the MDIChild from Notify the Activation (when a Child is deleted, one of the remaining is Activated)

		cppDEL(m_SideBarTabbed); //must be after deleting the vme_manager
		if(m_PlugLogbar) 
		{
			delete wxLog::SetActiveTarget(NULL); 
		}
  }
	m_win->Destroy();
}
