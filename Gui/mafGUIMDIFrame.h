/*=========================================================================

 Program: MAF2
 Module: mafGUIMDIFrame
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
/**
  Class Name: mafGUIMDIFrame.
  Represents the main frame of a MAF Application. On this frame  they'll be plugged  toolbars,  panels, progress bar etc...
  Closing this frame is equal to close the application.
*/
class MAF_EXPORT mafGUIMDIFrame: public wxMDIParentFrame
{
 public:
  /** constructor. */
  mafGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  /** destructor. */
 ~mafGUIMDIFrame(); 
  
  /** Set the listener object, i.e. the object receiving events sent by this object */
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

  /** Show/Hide a Dockable Pane -- the Pane is specified by name */
  void ShowDockPane(wxString pane_name, bool show = true);
  /** Show/Hide a Dockable Pane -- the Pane is specified by content */
  void ShowDockPane(wxWindow *window, bool show = true);

  /** tell if a given Dockable Pane is shown -- the Paneis specified by name */
  bool DockPaneIsShown(wxString pane_name);
  /** tell if a given Dockable Pane is shown -- the Pane is specified by content */
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

  /** Get the progress bar status. */
  int ProgressBarGetStatus();

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

  /** Update the memory status on the status bas , with information that needs to walk the heap (bad performances)*/
  void FreeMemorySizeOnIdle(wxIdleEvent& event);

  /** Send an event to open the msf or zip file. */
  void OnDropFile(wxDropFilesEvent &event);

  /** Redraw widgets on interface. */
  void LayoutWindow();

  /** Create the status bar on main application frame. */
  void CreateStatusbar();
  
  /** Send the 'Wizard' men event. */
  void OnMenuWizard(wxCommandEvent& e);


  mafObserver *m_Listener;

  // added members to handle the progress bar
  bool         *m_Busy;
  wxPanel      *m_BusyPanel;
  wxStaticText *m_BusyLabel;
  wxGauge      *m_Gauge;
  std::vector<int>    m_PBCalls;
  int  m_ID_PBCall;

  int  m_MemoryLimitAlert; ///< Used to alert the user when memory decrease under this limit value.
  bool m_UserAlerted;

  // AUI Layout Manager -  //SIL. 23-may-2006 : 
  mafGUIDockManager    m_DockManager;
  mafGUIDockSettings  *m_DockSettings;

  /** Event Table Declaration*/
  DECLARE_EVENT_TABLE()
};
#endif
