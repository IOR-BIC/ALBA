/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRWIBase
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaRWIBase_H__
#define __albaRWIBase_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaString.h"
#include "albaViewCompound.h"
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
class albaDeviceButtonsPadMouse;
class vtkImageAppend;
class vtkPNGWriter;
class vtkWindowToImageFilter;

//----------------------------------------------------------------------------
// Constant:
//----------------------------------------------------------------------------
#define ID_albaRWIBase_TIMER 1001

//----------------------------------------------------------------------------
// albaRWIBase :
//----------------------------------------------------------------------------
/** albaRWIBase is a vtkRenderWindowInteractor placed on a wxWindow 
\par Attention:
Normally there is no need to destroy any object
derived from wxWindow, they will be automatically 
destroyed as a result of closing the MainFrame.
albaRWIBase behave differently, and you 
must explicitly destroy them by calling "Delete()"
BEFORE wxWindow destroy it (using "delete"). 
*/
class ALBA_EXPORT albaRWIBase : public wxWindow, public vtkRenderWindowInteractor
{
  DECLARE_DYNAMIC_CLASS(albaRWIBase)

protected:
  wxTimer m_Timer;

public:

  vtkTypeMacro(albaRWIBase,vtkRenderWindowInteractor);

  albaRWIBase();
  albaRWIBase(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition,
	      const wxSize &size = wxDefaultSize, long style = wxWANTS_CHARS , const wxString &name = wxPanelNameStr);
  ~albaRWIBase();
  static albaRWIBase * New();

  void SetListener(albaObserver *listener) {m_Listener = listener;};

	/** Notify mouse click on a view. */
  void NotifyClick();

  /** set the mouse device to which forward events from the view */
  void SetMouse(albaDeviceButtonsPadMouse *mouse);

  /** Set the directory for the Stereo Movie*/
  void SetStereoMovieDirectory(const char *dir);

  /** 
  Enable/disable stereo movie frames generation.*/
  void EnableStereoMovie(bool enable = true);

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // vtk render window interactor methods
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	/** 
  Initialize the render window interactor. */
  void Initialize();
	/** 
  Enable the render window interactor. */
  void Enable();
	/** 
  Disable the render window interactor. */
  void Disable();
	/** 
  Redefined method to take the control on event loop. */
  void Start();
	/** 
  Update the size of the render window. */
  void UpdateSize(int x, int y);
	/** 
  Redefined method to terminate the application. */
  void TerminateApp();
	/** 
  Return an image of the render window. */
  wxBitmap* GetImage(int magnification = 1);
	/** 
  Save an image of the render window. */
  void SaveImage(albaString filename = "", int magnification = 1);
  
  /** 
  Save all images of a view compound creating a view for every subview */
  void SaveImageRecursive(albaString filename = "", albaViewCompound *v = NULL, int magnification = 1);
  void RecursiveSaving(albaString filename="", albaViewCompound *v=NULL,int magnification=1);
  /** 
  Save all images of the compound view. */
  void SaveAllImages(albaString filename = "", albaViewCompound *v = NULL);
	/** 
  Return the current used camera */
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
  /** Capture left mouse double click and notify it.*/
  virtual void OnLeftMouseDoubleClick(wxMouseEvent &event);
	/** Capture mouse and notify the mouse click. */
  virtual void OnLeftMouseButtonDown(wxMouseEvent &event);
  /** Release the mouse and send an event with mouse position. */
  virtual void OnLeftMouseButtonUp(wxMouseEvent &event);
	/** Capture mouse and notify the mouse click. */
  virtual void OnMiddleMouseButtonDown(wxMouseEvent &event);
	/** Release the mouse and send an event with mouse position. */
  virtual void OnMiddleMouseButtonUp(wxMouseEvent &event);
	/** Capture mouse and notify the mouse click. */
  virtual void OnRightMouseButtonDown(wxMouseEvent &event);
	/** Release the mouse and send an event with mouse position. */
  virtual void OnRightMouseButtonUp(wxMouseEvent &event);
	/** Send an event with mouse position. */
	virtual void OnMouseMotion(wxMouseEvent &event);
	/** Send an event with mouse wheel. */
	virtual void OnMouseWheel(wxMouseEvent &event);
	/** Send an event with timer information. */
  virtual void OnTimer(wxTimerEvent &event);
	/** Send an event with key code information. */
  virtual void OnKeyDown(wxKeyEvent &event);
	/** Send an event with key code information. */
  virtual void OnKeyUp(wxKeyEvent &event);
	/** Send an event with key code information. */
  virtual void OnChar(wxKeyEvent &event);
	/** Find the current camera and set UseHorizontalViewAngle depending on the win aspect ratio */
  virtual void OnSize(wxSizeEvent &event);
	/** Do nothing. */
	virtual void OnIdle(wxIdleEvent& event);
	/** Manage Mouse Capture Lost*/
	virtual void OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(evt));

  /** Redefined to set the m_CustomInteractorStyle flag. */
  void SetInteractorStyle(vtkInteractorObserver *o);

  bool m_Hidden;
  int  m_Width;
  int  m_Height;

  /** Generate stereo frames for movie.*/
  void GenerateStereoFrames();

protected:
  albaString m_StereoMovieDir;
  int       m_StereoMovieFrameCounter;
  bool      m_StereoMovieEnable;
  bool      m_StereoFrameGenerate;
  vtkWindowToImageFilter *m_StereoMovieLeftEye;
  vtkWindowToImageFilter *m_StereoMovieRightEye;
  vtkImageAppend *m_StereoImage;
  vtkPNGWriter   *m_StereoMoviewFrameWriter;
  
  albaString  m_SaveDir;
  vtkCamera *m_Camera;
  albaDeviceButtonsPadMouse  *m_Mouse;
  bool       m_CustomInteractorStyle;
  albaObserver *m_Listener;

  int m_LastX;
  int m_LastY;
  
	DECLARE_EVENT_TABLE()
};
#endif
