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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaDecl.h" //per albaYield
#include "albaGUIFlagSplitter.h"

//----------------------------------------------------------------------------
// albaGUICrossSplitter
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIFlagSplitter,albaGUIPanel)
  EVT_SIZE(albaGUIFlagSplitter::OnSize)
  EVT_LEFT_DOWN(albaGUIFlagSplitter::OnLeftMouseButtonDown)
  EVT_LEFT_UP(albaGUIFlagSplitter::OnLeftMouseButtonUp)
  EVT_MOTION(albaGUIFlagSplitter::OnMouseMotion)
  EVT_LEAVE_WINDOW(albaGUIFlagSplitter::OnMouseMotion)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
albaGUIFlagSplitter::albaGUIFlagSplitter(wxWindow* parent,wxWindowID id)
: albaGUIPanel(parent, id)
//----------------------------------------------------------------------------
{
	m_ViewPanel1 = new wxPanel(this,1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_ViewPanel2 = new wxPanel(this,2,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_ViewPanel3 = new wxPanel(this,3,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         

  m_Maximized = false;
	m_FocusedPanel = m_ViewPanel1; 

  m_CursorWE   = new wxCursor(wxCURSOR_SIZEWE);
  m_Pen        = new wxPen(*wxBLACK, 2, wxSOLID);

  m_With = m_Height = 100;
  m_XPos1 = 33;
	m_XPos2 = 66;
  m_XPosOld = 0;
	m_Margin = 20;
  m_Dragging = drag_none;
}
//----------------------------------------------------------------------------
albaGUIFlagSplitter::~albaGUIFlagSplitter( ) 
//----------------------------------------------------------------------------
{
	delete m_CursorWE;
  delete m_Pen;
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{
  // resize the window and the sub panels keeping the previous relative positions.
  float rx1,rx2;
	this->Refresh(false);
	Layout();
  albaYield(); 
  wxFrame *frame = (wxFrame *)GetParent(); // may be a wxChild - error prone anyway
  {
    rx1 = m_XPos1/(float)m_With;
    rx2 = m_XPos2/(float)m_With;

    GetClientSize(&m_With,&m_Height); 
    m_XPos1 = rx1 * m_With;
    m_XPos2 = rx2 * m_With;

    OnLayout();
  }
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::SetSplitPos(int x1,int x2)
//----------------------------------------------------------------------------
{
  if (m_XPos1 < m_Margin)          m_XPos1 =         m_Margin;
  if (m_XPos1 > m_With - 2*m_Margin)  m_XPos1 = m_With - 2*m_Margin;
  if (m_XPos2 < 2*m_Margin)        m_XPos2 =       2*m_Margin;
  if (m_XPos2 > m_With - m_Margin)    m_XPos2 = m_With - m_Margin;
  if (m_XPos1 > m_XPos2 - m_Margin)   m_XPos1 = m_XPos2 - m_Margin;

  m_XPos1 = x1;
  m_XPos2 = x2;

  OnLayout();
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  CaptureMouse();

  m_Dragging = HitTest(event);
  if(m_Dragging == drag_none) 
		return;

  int x = event.GetX();
  DrawTracker(x);    
  m_XPosOld = x;
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Dragging != drag_none) 
  {
    DrawTracker(m_XPosOld);    

    int x = event.GetX();

    switch(m_Dragging)
    {
      case drag_x1:
        SetSplitPos(x,m_XPos2);
      break;
      case drag_x2:
        SetSplitPos(m_XPos1,x);
      break;
    }
    m_Dragging = drag_none;
  }
  ReleaseMouse();
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (m_Dragging == drag_none )
  {
    switch(HitTest(event))
    {
      case drag_x1:
      case drag_x2:
        SetCursor(*m_CursorWE);
      break;
      default:
        SetCursor(*wxSTANDARD_CURSOR);
      break;
    }
  } 
  else 
  {
    int x = event.GetX();
    if(m_Dragging == drag_x1)
		{
			if (x < m_Margin)        x =        m_Margin;
			if (x > m_XPos2 - m_Margin) x = m_XPos2 - m_Margin;
		}
    if(m_Dragging == drag_x2)
		{
			if (x < m_XPos1 + m_Margin + 1) x = m_XPos1 + m_Margin + 1;
			if (x > m_With  - m_Margin) x = m_With  - m_Margin;
		}

    DrawTracker(m_XPosOld);    
    DrawTracker(x);
    m_XPosOld = x;
  }

  if( event.Leaving() )
  {
    SetCursor(*wxSTANDARD_CURSOR);
  }
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::OnLayout()
//----------------------------------------------------------------------------
{
  if (m_XPos1 < m_Margin)          m_XPos1 =         m_Margin;
  if (m_XPos1 > m_With - 2*m_Margin)  m_XPos1 = m_With - 2*m_Margin;
  if (m_XPos2 < 2*m_Margin)        m_XPos2 =       2*m_Margin;
  if (m_XPos2 > m_With - m_Margin)    m_XPos2 = m_With - m_Margin;
  if (m_XPos1 > m_XPos2 - m_Margin)   m_XPos1 = m_XPos2 - m_Margin;

  int m  = 3;
  int w  = (m_With -2*m)/3;

  m_ViewPanel1->SetSize(0,0,m_XPos1,m_Height);
	m_ViewPanel2->SetSize(m+m_XPos1,0,m_XPos2 - m_XPos1 - m,m_Height);
	m_ViewPanel3->SetSize(m+m_XPos2,0,m_With-m_XPos2-m,m_Height);
	
  m_ViewPanel1->Layout();
	m_ViewPanel2->Layout();
  m_ViewPanel3->Layout();
}
//----------------------------------------------------------------------------
FlagSplitterDragModes albaGUIFlagSplitter::HitTest(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.GetX();
  if ( abs(m_XPos1-x) < 5 ) return drag_x1;
  if ( abs(m_XPos2-x) < 5 ) return drag_x2;
  return drag_none;
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::DrawTracker(int x)
//----------------------------------------------------------------------------
{
  int m = 3;
  int y1 = m;
	int y2 = m_Height-m;
	int foo;
    
  wxScreenDC screenDC;
  screenDC.SetLogicalFunction(wxINVERT);
  screenDC.SetPen(*m_Pen);
  screenDC.SetBrush(*wxTRANSPARENT_BRUSH);
  
  ClientToScreen(&x,   &y1);
  ClientToScreen(&foo, &y2);
  screenDC.DrawLine(x, y1, x, y2);

  screenDC.SetLogicalFunction(wxCOPY);
  screenDC.SetPen(wxNullPen);
  screenDC.SetBrush(wxNullBrush);
}
//----------------------------------------------------------------------------
void albaGUIFlagSplitter::Put(wxWindow* w,int i) 
//----------------------------------------------------------------------------
{
  w->Reparent(this);
  switch(i)
  {
		case 0:
			if (m_ViewPanel1) delete m_ViewPanel1;
			m_ViewPanel1 = w;
			m_ViewPanel1->Layout();
		break;
		case 1:
			if (m_ViewPanel2) delete m_ViewPanel2;
			m_ViewPanel2 = w;
			m_ViewPanel2->Layout();
		break;
		case 2:
			if (m_ViewPanel3) delete m_ViewPanel3;
			m_ViewPanel3 = w;
			m_ViewPanel3->Layout();
		break;
    default:
		  assert(false); 
		break;
  };
  
	w->Reparent(this);
	w->Show(true);
}
