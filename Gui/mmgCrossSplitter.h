/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCrossSplitter.h,v $
  Language:  C++
  Date:      $Date: 2005-06-10 08:50:41 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgCrossSplitter_H__
#define __mmgCrossSplitter_H__
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

enum CrossSplitterModes
{
    VA_ONE = LAYOUT_ONE,
    VA_TWO_VERT,  
    VA_TWO_HORZ,  
    VA_THREE_UP,  
    VA_THREE_DOWN,  
    VA_THREE_LEFT,  
    VA_THREE_RIGHT,  
    VA_FOUR  
};

enum CrossSplitterDragModes
{
    drag_none =0,
    drag_x,  
    drag_y,  
    drag_xy  
};
//----------------------------------------------------------------------------
// mmgCrossSplitter :
//----------------------------------------------------------------------------
/** 
Use Put(w,id) to place widgets on the various pane - id should be in 0..3.
Putting a new pane delete the previous
*/
class mmgCrossSplitter: public mmgPanel
{
public:
  mmgCrossSplitter(wxWindow* parent,wxWindowID id = -1);
  virtual ~mmgCrossSplitter();

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  //void OnEvent(mafEventBase *event);

  /** Show/Hide the four panel according to a splitting modality. */
	void Split(CrossSplitterModes mode);
  
	/** Set the split position and redraw the four panels. */
	void SetSplitPos(int x,int y);
  
	/** Set the split position in relative coordinates and redraw the four panels. */
  void SetSplitPosRel(float x,float y);
  
	/** Maximize the founded panel. */
  void Maximize();

  /** Put the window into the panel 'i'. */
	void Put(wxWindow* w,int i);
	
	/** Set the focus on window 'w'. */
	void SetFocusedPanel(wxWindow* w);

protected:
  mafObserver *m_Listener;

  wxCursor *m_CursorWE;
  wxCursor *m_CursorNS;
  wxCursor *m_CursorNSWE;
  wxPen    *m_Pen;
  
  CrossSplitterModes      m_mode;
  CrossSplitterDragModes  m_dragging;

  int   m_margin;
  int   m_x,m_y,m_w,m_h;
  int   m_oldx,m_oldy;
  float m_relx,m_rely;
  
  wxWindow *m_focused_panel;
  wxWindow *m_vp1;
  wxWindow *m_vp2;
  wxWindow *m_vp3;
  wxWindow *m_vp4;

	/** Adjust the panels size. */
  void OnSize(wxSizeEvent &event);

	/** Intercept the mouse button down and call DrawTracker with mouse position. */
  void OnLeftMouseButtonDown(wxMouseEvent &event);

	/** Intercept the mouse button up and call SetSplitPos with new position. */
  void OnLeftMouseButtonUp(wxMouseEvent &event);

	/** Manage the mouse movement to draw the cross of cross splitter. */
  void OnMouseMotion(wxMouseEvent &event);

	/** Adjust the four panel. */
  void OnLayout();

	/** Hit the mouse position and return the drag modality. */
  CrossSplitterDragModes HitTest(wxMouseEvent &event);

	/** Draw the cross representing the position of creoss splitter. */
  void DrawTracker(int x, int y);
  DECLARE_EVENT_TABLE()

	bool m_maximized;
};
#endif
