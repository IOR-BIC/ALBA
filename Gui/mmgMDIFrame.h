/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIFrame.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:23 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first

#include <wx/laywin.h>
#include <wx/mdi.h>
//#include <vtkViewport.h>
//#include <vtkProcessObject.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafEventListener;

//----------------------------------------------------------------------------
// mmgMDIFrame :
//----------------------------------------------------------------------------
class mmgMDIFrame: public wxMDIParentFrame
{
 public:
  mmgMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
 ~mmgMDIFrame(); 
  
  void SetListener(mafEventListener *Listener) {m_Listener = Listener;};
  
  /** Set the window shown on the Client Area, (hide the previous contents) */
	void Put(wxWindow* w);

	/** . */
  void Update() {LayoutWindow();};

	/** Show the 'Busy' text on the status bar. */
  void Busy();

	/** End the progress. */
  void Ready();

	/** Link a vtk object to the progress bar. */
  //void BindToProgressBar(vtkObject* vtkobj,	wxString  *msg);

	/** Link a vtk process object (filter) to the progress bar. */
  //void BindToProgressBar(vtkProcessObject* filter, wxString  *msg);

	/** Link a vtkViewport to the progress bar. */
  //void BindToProgressBar(vtkViewport* ren, wxString  *msg);

	/** Show the progress bar. */
  void ProgressBarShow();

	/** Hide the progress bar. */
  void ProgressBarHide();

	/** Set the progress value for the progress bar. */
  void ProgressBarSetVal(int progress);

	/** Set the progress text for the progress bar. */
  void ProgressBarSetText(wxString *msg);

	/** Start rendering progress. */
  static void RenderStart(void*);

	/** End rendering progress. */
  static void RenderEnd(void*);


  /** Start the progress bar visualization. */
  //static void ProgressStart(void*);

	/** Update the progress bar visualization. */
  //static void ProgressUpdate(void*);

	/** End the progress bar visualization. */
  //static void ProgressEnd(void*);

	/** Delete the progress bar argument. */
  //static void ProgressDeleteArgs(void*);
 
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

	/** Redraw widgets on interface. */
  void LayoutWindow();

	/** Create the status bar on main application frame. */
  void CreateStatusbar();

  mafEventListener *m_Listener;
  wxWindow         *m_clientwin;

  // added members to handle the progressbar
  bool         *m_busy;
  wxPanel      *m_busyp;
  wxStaticText *m_busyl;
  wxGauge      *m_gauge;

DECLARE_EVENT_TABLE()
};
#endif
