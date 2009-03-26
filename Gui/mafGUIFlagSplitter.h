/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIFlagSplitter.h,v $
  Language:  C++
  Date:      $Date: 2009-03-26 16:52:47 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIFlagSplitter_H__
#define __mafGUIFlagSplitter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/splitter.h>
#include <wx/hash.h>
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUIPanel.h"

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
// mafGUIFlagSplitter :
//----------------------------------------------------------------------------
/** 
Use Put(w,id) to place widgets on the various pane - id should be in 0..2.
Putting a new pane delete the previous
*/
class mafGUIFlagSplitter: public mafGUIPanel
{
public:
  mafGUIFlagSplitter(wxWindow* parent,wxWindowID id = -1);
  virtual ~mafGUIFlagSplitter();

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  //void OnEvent(mafEventBase *maf_event);
  
	/** Set the split position and redraw the four panels. */
  void SetSplitPos(int x1,int x2);

  /** Put the window into the panel 'i'. */
  void Put(wxWindow* w,int i);

protected:
  mafObserver  *m_Listener;
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
