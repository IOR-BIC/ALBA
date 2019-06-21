/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIFlagSplitter
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIFlagSplitter_H__
#define __albaGUIFlagSplitter_H__

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
enum FlagSplitterDragModes
{
    drag_none =0,
    drag_x1,  
    drag_x2,  
};

//----------------------------------------------------------------------------
// albaGUIFlagSplitter :
//----------------------------------------------------------------------------
/** 
Use Put(w,id) to place widgets on the various pane - id should be in 0..2.
Putting a new pane delete the previous
*/
class albaGUIFlagSplitter: public albaGUIPanel
{
public:
  albaGUIFlagSplitter(wxWindow* parent,wxWindowID id = -1);
  virtual ~albaGUIFlagSplitter();

  void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  //void OnEvent(albaEventBase *alba_event);
  
	/** Set the split position and redraw the four panels. */
  void SetSplitPos(int x1,int x2);

  /** Put the window into the panel 'i'. */
  void Put(wxWindow* w,int i);

protected:
  albaObserver  *m_Listener;
  wxCursor     *m_CursorWE;
  wxPen        *m_Pen;
  
  FlagSplitterDragModes m_Dragging;

  int m_Margin;
  int m_With,m_Height;
  int m_XPos1,m_XPos2;
  int m_XPosOld;

  wxWindow  *m_FocusedPanel;
  wxWindow  *m_ViewPanel1; 
  wxWindow  *m_ViewPanel2; 
  wxWindow  *m_ViewPanel3; 
  
	/** Adjust the panels size. */
  void OnSize(wxSizeEvent &event);

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

	/** Draw the cross representing the position of cross splitter. */
  void DrawTracker(int x);

DECLARE_EVENT_TABLE()

  bool m_Maximized;
};
#endif
