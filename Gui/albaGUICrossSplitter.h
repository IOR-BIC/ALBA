/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUICrossSplitter
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUICrossSplitter_H__
#define __albaGUICrossSplitter_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/splitter.h>
#include <wx/hash.h>
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUIPanel.h"
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
// albaGUICrossSplitter :
//----------------------------------------------------------------------------
/** 
Use Put(w,id) to place widgets on the various pane - id should be in 0..3.
Putting a new pane delete the previous
*/
class ALBA_EXPORT albaGUICrossSplitter: public albaGUIPanel
{
public:
  albaGUICrossSplitter(wxWindow* parent,wxWindowID id = -1);
  virtual ~albaGUICrossSplitter();

  void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  //void OnEvent(albaEventBase *event);

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
  albaObserver *m_Listener;

  wxCursor *m_CursorWE;
  wxCursor *m_CursorNS;
  wxCursor *m_CursorNSWE;
  wxPen    *m_Pen;
  
  CrossSplitterModes      m_Mode;
  CrossSplitterDragModes  m_Dragging;

  int   m_Margin;
  int   m_XPos,m_YPos,m_With,m_Height;
  int   m_OldXPos,m_OldYPos;
  float m_RelXPos,m_RelYPos;
  
  wxWindow *m_FocusedPanel;
  wxWindow *m_ViewPanel1;
  wxWindow *m_ViewPanel2;
  wxWindow *m_ViewPanel3;
  wxWindow *m_ViewPanel4;

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

	bool m_Maximized;
};
#endif
