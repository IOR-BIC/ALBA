/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMDIFrame
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIMDIFrame_H__
#define __albaGUIMDIFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/mdi.h>
#include <vector>
#include "albaGUIDockManager.h"
#include "albaServiceClient.h"


//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaObserver;
class vtkAlgorithm;
class vtkViewport;
class vtkObject;
class albaGUIMDIFrameCallback; 
class vtkAlgorithm;
class albaGUI;
/**
  Class Name: albaGUIMDIFrame.
  Represents the main frame of a ALBA Application. On this frame  they'll be plugged  toolbars,  panels, progress bar etc...
  Closing this frame is equal to close the application.
*/
class ALBA_EXPORT albaGUIMDIFrame: public wxMDIParentFrame, public albaServiceClient
{
 public:
  /** constructor. */
  albaGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style=0);
  /** destructor. */
 ~albaGUIMDIFrame(); 
  
  /** Set the listener object, i.e. the object receiving events sent by this object */
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Retrieve the Docking Manager */
  albaGUIDockManager& GetDockManager()  {return m_DockManager;};

  /** must be called @ shutdown*/
  void OnQuit();

  /** add a new Dockable Pane containing the given Window.
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
#ifdef ALBA_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  /** Link a vtk object to the progress bar. -  */
  void BindToProgressBar(vtkObject* vtkobj);

  /** Link a vtk process object (filter) to the progress bar. */
  void BindToProgressBar(vtkAlgorithm* filter);

  /** Link a vtkViewport to the progress bar. */
  void BindToProgressBar(vtkViewport* ren);

  /** Redraw the frame. */
  void OnSize(wxSizeEvent& event);

protected:
  albaGUIMDIFrameCallback *m_StartCallback; 
  albaGUIMDIFrameCallback *m_EndCallback; ; 
  albaGUIMDIFrameCallback *m_ProgressCallback; 

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //ALBA_USE_VTK
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


  albaObserver *m_Listener;

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
  albaGUIDockManager    m_DockManager;

  /** Event Table Declaration*/
  DECLARE_EVENT_TABLE()
};
#endif
