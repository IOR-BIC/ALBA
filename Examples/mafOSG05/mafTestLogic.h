/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafTestLogic.h,v $
Language:  C++
Date:      $Date: 2006-02-21 16:14:11 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTestLogic_H__
#define __mafTestLogic_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/notebook.h>
#include <wx/icon.h>
#include <iostream>
#include <fstream>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgFileHistory.h"
#include "mmgFrame.h"

#include <osg/ref_ptr>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGuiHolder;
class mmgPanel;
class mmgNamedPanel;
class mmgSashPanel;
class mafWXLog;
class mafViewSettings;

class mafOsgCanvas;
class mafOsgTerrainManipulator;
class mafOsgTerrainManipulatorGui;
class mafOsgSky;
//----------------------------------------------------------------------------
// mafTestLogic :
//----------------------------------------------------------------------------
class mafTestLogic : public mafObserver //SIL - cambiata la classe base
{
public:
               mafTestLogic();
	virtual     ~mafTestLogic(); 

  /** receive incoming mafEvents */
	virtual void OnEvent(mafEventBase *maf_event);  //SIL - cambiato il tipo degli eventi

	/** shows the main window */
	virtual void Show();
	
  /** parse the application command line */
  virtual void Init(int argc, char **argv);

	/** returns the pointer to the main window. */
	virtual mmgFrame *GetTopWin()	{return m_win;};

protected:
	
  /** terminate app */
  virtual void OnQuit();

  wxString					 m_app_title;    
  wxConfigBase			*m_config;

  /////////////////////////////////////////////////////////////////////
  // I/O :

  /** load a file */
	virtual void FileOpen(wxString filename);

  /** respond to Menu-File-Open, calls FileOpen */
  virtual void MenuFileOpen();

  /** respond to the History menu */
	virtual void HistoryFileOpen(int menuId);
  
  mmgFileHistory	   m_file_history;

  /////////////////////////////////////////////////////////////////////
  // Gui components :
  mmgFrame          *m_win;
  wxToolBar         *m_toolbar;
  mmgSashPanel      *m_log_bar;
  wxMenuBar         *m_menu_bar;
	mmgSashPanel			*m_side_bar;
  mafOsgCanvas      *m_canvas;

  mafViewSettings   *m_settings; 
  mmgGuiHolder      *m_settings_panel;
  wxNotebook        *m_nb;

  /** Create the Menu */
  virtual void CreateMenu();

  /** Create the Toolbar. */
  virtual void CreateToolbar();

  /** Create the side sash and its contents (the view-settings at the moment) */
  virtual void CreateSidebar();

  /** Create the log panel sash and the log-widget */
  virtual void CreateLogbar();


  /////////////////////////////////////////////////////////////////////
  // Logging features : 
  // at this moment mafMessages and wxMessages are sent do the Log window
  // OSG messages are sent to a File

  wxTextCtrl        *m_log;
  std::ofstream      m_logfile; 
  mafWXLog          *m_logger;  /// not used now -- allow echoes the log window to a file too

  /** redirect stdout and stderr to a log file.
  used to intercept osg-notifications */
  void mafTestLogic::StartLogging();
  /** restore the redirected stdout and stderr
  to be called on application exit */
  void mafTestLogic::StopLogging();

  std::streambuf    *m_stdout; /// used to restore stdout and stderr after redirection
  std::streambuf    *m_stderr; /// used to restore stdout and stderr after redirection    


  /////////////////////////////////////////////////////////////////////
  // manipulator : 
  osg::ref_ptr<mafOsgTerrainManipulator> m_manip;
  mafOsgTerrainManipulatorGui *m_manip_gui;

  /////////////////////////////////////////////////////////////////////
  // sky : 
  mafOsgSky *m_sky;
};

#endif



