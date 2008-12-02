/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMDIFrame.h,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:06:44 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIMDIFrame_H__
#define __mafGUIMDIFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/mdi.h>

#include "mafGUIDockManager.h"
#include "mafGUIDockSettings.h" // tmp //SIL. 05-jun-2006 : 

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafObserver;
class vtkProcessObject;
class vtkViewport;
class vtkObject;
class mafGUIMDIFrameCallback; 
//class mafGUIDockSettings;
class mafGUI;
//----------------------------------------------------------------------------
// mafGUIMDIFrame :
//----------------------------------------------------------------------------
class mafGUIMDIFrame: public wxMDIParentFrame
{
 public:
  mafGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
 ~mafGUIMDIFrame(); 
  
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  

  /** Show the Docking Settings Dialog */
  mafGUI* GetDockSettingGui() {return m_DockSettings->GetGui();};  

  /** Retrieve the Docking Manager */
  mafGUIDockManager& GetDockManager()  {return m_DockManager;};

  /** must be called @ shutdown*/
  void OnQuit();

  /** add a new Dockable Pane containig the given Window.
  pane_info specify the Pane settings */
  void AddDockPane(wxWindow *window, wxPaneInfo& pane_info, const wxString &menu = _("&View"), const wxString &subMenu = wxEmptyString);
  
  /** remove a Dockable Pane  - the contained window is not destroyed */
  void RemoveDockPane(wxString pane_name);

  /** Show/Hide a Dockable Pane -- the Pane can be specified by name or by its contents */
  void ShowDockPane(wxString pane_name, bool show = true);
  void ShowDockPane(wxWindow *window, bool show = true);

  /** tell if a given Dockable Pane is shown -- the Pane can be specified by name or by its contents */
  bool DockPaneIsShown(wxString pane_name);
  bool DockPaneIsShown(wxWindow *window);

	/** Show the 'Busy' text on the status bar. */
  void Busy();

	/** End the progress. */
  void Ready();

	/** Show the progress bar. */
  void ProgressBarShow();

	/** Hide the progress bar. */
  void ProgressBarHide();

	/** Set the progress value for the progress bar. */
  void ProgressBarSetVal(int progress);

	/** Set the progress text for the progress bar. */
  void ProgressBarSetText(wxString *msg);

	/** Start rendering progress. */
  void RenderStart();

	/** End rendering progress. */
  void RenderEnd();

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  /** Link a vtk object to the progress bar. -  */
  void BindToProgressBar(vtkObject* vtkobj);

  /** Link a vtk process object (filter) to the progress bar. */
  void BindToProgressBar(vtkProcessObject* filter);

  /** Link a vtkViewport to the progress bar. */
  void BindToProgressBar(vtkViewport* ren);

  /** Redraw the frame. */
  void OnSize(wxSizeEvent& event);

protected:
  mafGUIMDIFrameCallback *m_StartCallback; 
  mafGUIMDIFrameCallback *m_EndCallback; ; 
  mafGUIMDIFrameCallback *m_ProgressCallback; 

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
protected:
 
	/** Send the men event. */
  void OnMenu(wxCommandEvent& e);

	/** Send the 'Operations' men event. */
  void OnMenuOp(wxCommandEvent& e);

	/** Send the 'Views' men event. */
  void OnMenuView(wxCommandEvent& e);

	/** Send the UI event. */
  void OnUpdateUI(wxUpdateUIEvent& e);

	/** Send the MENU_FILE_QUIT event. */
  void OnCloseWindow(wxCloseEvent& event);

	/** Redraw sash panels. */
  void OnSashDrag(wxSashEvent& event);

	/** Call LayoutWindow method. */
  void OnLayout(wxCommandEvent& event);

	/** Update the memory status on the status bas. */
  void OnIdle(wxIdleEvent& event);

  /** Send an event to open the msf or zip file. */
  void OnDropFile(wxDropFilesEvent &event);

	/** Redraw widgets on interface. */
  void LayoutWindow();

	/** Create the status bar on main application frame. */
  void CreateStatusbar();

  mafObserver *m_Listener;

  // added members to handle the progress bar
  bool         *m_Busy;
  wxPanel      *m_BusyPanel;
  wxStaticText *m_BusyLabel;
  wxGauge      *m_Gauge;

  int  m_MemoryLimitAlert; ///< Used to alert the user when memory decrease under this limit value.
  bool m_UserAlerted;

  // AUI Layout Manager -  //SIL. 23-may-2006 : 
  mafGUIDockManager    m_DockManager;
  mafGUIDockSettings  *m_DockSettings;

  DECLARE_EVENT_TABLE()
};
#endif
