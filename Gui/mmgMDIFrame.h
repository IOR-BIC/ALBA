/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIFrame.h,v $
  Language:  C++
  Date:      $Date: 2005-09-28 14:40:57 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgMDIFrame_H__
#define __mmgMDIFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/mdi.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafObserver;
class vtkProcessObject;
class vtkViewport;
class vtkObject;
class mmgMDIFrameCallback; 
//----------------------------------------------------------------------------
// mmgMDIFrame :
//----------------------------------------------------------------------------
class mmgMDIFrame: public wxMDIParentFrame
{
 public:
  mmgMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
 ~mmgMDIFrame(); 
  
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
  /** Set the window shown on the Client Area, (hide the previous contents) */
	void Put(wxWindow* w);

	/** . */
  void Update() {LayoutWindow();};

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

  /* - used for vtk4.2  
  static void ProgressStart(void*);
  static void ProgressUpdate(void*);
  static void ProgressEnd(void*);
  static void ProgressDeleteArgs(void*);
  */

protected:
  mmgMDIFrameCallback *m_StartCallback; 
  mmgMDIFrameCallback *m_EndCallback; ; 
  mmgMDIFrameCallback *m_ProgressCallback; 

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

	/** Redraw the frame. */
  void OnSize(wxSizeEvent& event);

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
  wxWindow         *m_clientwin;

  // added members to handle the progressbar
  bool         *m_busy;
  wxPanel      *m_busyp;
  wxStaticText *m_busyl;
  wxGauge      *m_gauge;


  DECLARE_EVENT_TABLE()
};
#endif
