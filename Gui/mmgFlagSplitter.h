/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFlagSplitter.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:44:04 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgFlagSplitter_H__
#define __mmgFlagSplitter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/splitter.h>
#include <wx/hash.h>
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mmgPanel.h"

//----------------------------------------------------------------------------
// constant :
//----------------------------------------------------------------------------
enum FlagSplitterDragModes
{
    drag_none =0,
    drag_x1,  
    drag_x2,  
};

//----------------------------------------------------------------------------
// mmgFlagSplitter :
//----------------------------------------------------------------------------
/** 
Use Put(w,id) to place widgets on the various pane - id should be in 0..2.
Putting a new pane delete the previous
*/
class mmgFlagSplitter: public mmgPanel
{
public:
  mmgFlagSplitter(wxWindow* parent,wxWindowID id = -1);
  virtual ~mmgFlagSplitter();

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  //void OnEvent(mafEventBase *event);
  
	/** Set the split position and redraw the four panels. */
  void SetSplitPos(int x1,int x2);

  /** Put the window into the panel 'i'. */
  void Put(wxWindow* w,int i);

protected:
  mafObserver  *m_Listener;
  wxCursor     *m_CursorWE;
  wxPen        *m_Pen;
  
  FlagSplitterDragModes m_dragging;

  int m_margin;
  int m_w,m_h;
  int m_x1,m_x2;
  int m_oldx;

  wxWindow  *m_focused_panel;
  wxWindow  *m_vp1; 
  wxWindow  *m_vp2; 
  wxWindow  *m_vp3; 
  
	/** Adjust the panels size. */
  void OnSize(wxSizeEvent& event);

	/** Adjust the panels. */
  void OnLayout();

	/** Intercept the mouse button down and call DrawTracker with mouse position. */
  void OnLeftMouseButtonDown(wxMouseEvent &event);

	/** Intercept the mouse button up and call SetSplitPos with new position. */
  void OnLeftMouseButtonUp(wxMouseEvent &event);

	/** Manage the mouse movement to draw the cross of cross splitter. */
  void OnMouseMotion(wxMouseEvent &event);

	/** Hit the mouse position and return the drag modality. */
  FlagSplitterDragModes HitTest(wxMouseEvent &event);

	/** Draw the cross representing the position of creoss splitter. */
  void DrawTracker(int x);

DECLARE_EVENT_TABLE()

  bool m_maximized;
};
#endif
