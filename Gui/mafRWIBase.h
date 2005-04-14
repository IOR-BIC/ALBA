/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWIBase.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 13:44:56 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef _mafRWIBase_H_
#define _mafRWIBase_H_ 1
//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "vtkRenderWindowInteractor.h"
//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class wxPaintEvent;
class wxMouseEvent;
class wxTimerEvent;
class wxKeyEvent;
class wxSizeEvent;
class vtkInteractorObserver;
class vtkCamera;
class mafAction;
//----------------------------------------------------------------------------
// Constant:
//----------------------------------------------------------------------------
#define ID_mafRWIBase_TIMER 1001
//----------------------------------------------------------------------------
// mafRWIBase :
//----------------------------------------------------------------------------
/** mafRWIBase is a vtkRenderWindowInteractor placed on a wxWindow 
\par Attention:
Normally there is no need to destroy any object
derived from wxWindow, they will be automatically 
destroyed as a result of closing the MainFrame.
mafRWIBase behave differently, and you 
must explicitly destroy them by calling "Delete()"
BEFORE wxWindow destroy it (using "delete"). 
*/
class mafRWIBase : public wxScrolledWindow, virtual public vtkRenderWindowInteractor
{
  DECLARE_DYNAMIC_CLASS(mafRWIBase)

protected:
  wxTimer timer;

public:
  mafRWIBase();
  mafRWIBase(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition,
	      const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS , const wxString &name = wxPanelNameStr);
  ~mafRWIBase();
  static mafRWIBase * New();

	/** Notify mouse click on a view. */
  void NotifyClick();

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // vtk render window interactor methods
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	/** Initialize the render window interactor. */
  void Initialize();
	/** Enable the render window interactor. */
  void Enable();
	/** Disable the render window interactor. */
  void Disable();
	/** Redefined method to take the control on event loop. */
  void Start();
	/** Update the size of the render window. */
  void UpdateSize(int x, int y);
	/** Redefined method to terminate the application. */
  void TerminateApp();
	/** Return an image of the render window. */
  wxBitmap* GetImage();
	/** Save an image of the render window. */
  void SaveImage(wxString view_name = "", int magnification = 1);
	/** Return the current used camera */
	vtkCamera* GetCamera();

  int CreateTimer(int timertype);
  int DestroyTimer();

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // event handlers 
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	
	/** Method called whe the render window as to be repainted. */
  void OnPaint(wxPaintEvent &event);
	/**  */
  void OnEraseBackground (wxEraseEvent& event);
	/** Capture mouse and notify the mouse click. */
  void OnLeftMouseButtonDown(wxMouseEvent &event);
  /** Release the mouse and send an event with mouse position. */
  void OnLeftMouseButtonUp(wxMouseEvent &event);
	/** Capture mouse and notify the mouse click. */
  void OnMiddleMouseButtonDown(wxMouseEvent &event);
	/** Release the mouse and send an event with mouse position. */
  void OnMiddleMouseButtonUp(wxMouseEvent &event);
	/** Capture mouse and notify the mouse click. */
  void OnRightMouseButtonDown(wxMouseEvent &event);
	/** Release the mouse and send an event with mouse position. */
  void OnRightMouseButtonUp(wxMouseEvent &event);
	/** Send an event with mouse position. */
  void OnMouseMotion(wxMouseEvent &event);
	/** Send an event with timer information. */
  void OnTimer(wxTimerEvent &event);
	/** Send an event with key code information. */
  void OnKeyDown(wxKeyEvent &event);
	/** Send an event with key code information. */
  void OnKeyUp(wxKeyEvent &event);
	/** Send an event with key code information. */
  void OnChar(wxKeyEvent &event);
	/** find the current camera and set UseHorizontalViewAngle depending on the win aspect ratio */
  void OnSize(wxSizeEvent &event);
	/** Do nothing. */
	void OnIdle(wxIdleEvent& event);
  /** Initialize the action for the mouse device. */
  void SetMouseAction(mafAction *action);

  /** 
  Redefined to set the m_CustomInteractorStyle flag. */
  void SetInteractorStyle(vtkInteractorObserver *o);

  bool m_hidden;
  int  m_w;
  int  m_h;

protected:
	wxString   m_save_dir;
  vtkCamera *m_cam;
//mafAction *m_MouseAction;
  bool       m_CustomInteractorStyle;
  
	DECLARE_EVENT_TABLE()
};
#endif
