/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFlagSplitter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:44:03 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDecl.h" //per mafYield
#include "mmgFlagSplitter.h"

//----------------------------------------------------------------------------
// mmgCrossSplitter
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgFlagSplitter,mmgPanel)
  EVT_SIZE(mmgFlagSplitter::OnSize)
  EVT_LEFT_DOWN(mmgFlagSplitter::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mmgFlagSplitter::OnLeftMouseButtonUp)
  EVT_MOTION(mmgFlagSplitter::OnMouseMotion)
  EVT_LEAVE_WINDOW(mmgFlagSplitter::OnMouseMotion)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mmgFlagSplitter::mmgFlagSplitter(wxWindow* parent,wxWindowID id)
: mmgPanel(parent, id)
//----------------------------------------------------------------------------
{
	m_vp1 = new wxPanel(this,1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_vp2 = new wxPanel(this,2,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_vp3 = new wxPanel(this,3,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         

  m_maximized = false;
	m_focused_panel = m_vp1; 

  m_CursorWE   = new wxCursor(wxCURSOR_SIZEWE);
  m_Pen        = new wxPen(*wxBLACK, 2, wxSOLID);

  m_w = m_h = 100;
  m_x1 = 33;
	m_x2 = 66;
  m_oldx = 0;
	m_margin = 20;
  m_dragging = drag_none;
}
//----------------------------------------------------------------------------
mmgFlagSplitter::~mmgFlagSplitter( ) 
//----------------------------------------------------------------------------
{
	delete m_CursorWE;
  delete m_Pen;
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{
  // resize the window and the sub panels keeping the previous relative positions.
  float rx1,rx2;
  wxPanel::OnSize(event);
	this->Refresh(false);
	Layout();
  mafYield(); 
  wxFrame *frame = (wxFrame *)GetParent(); // may be a wxChild - error prone anyway
  {
    rx1 = m_x1/(float)m_w;
    rx2 = m_x2/(float)m_w;

    GetClientSize(&m_w,&m_h); 
    m_x1 = rx1 * m_w;
    m_x2 = rx2 * m_w;

    OnLayout();
  }
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::SetSplitPos(int x1,int x2)
//----------------------------------------------------------------------------
{
  if (m_x1 < m_margin)          m_x1 =         m_margin;
  if (m_x1 > m_w - 2*m_margin)  m_x1 = m_w - 2*m_margin;
  if (m_x2 < 2*m_margin)        m_x2 =       2*m_margin;
  if (m_x2 > m_w - m_margin)    m_x2 = m_w - m_margin;
  if (m_x1 > m_x2 - m_margin)   m_x1 = m_x2 - m_margin;

  m_x1 = x1;
  m_x2 = x2;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  CaptureMouse();

  m_dragging = HitTest(event);
  if(m_dragging == drag_none) 
		return;

  int x = event.GetX();
  DrawTracker(x);    
  m_oldx = x;
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_dragging != drag_none) 
  {
    DrawTracker(m_oldx);    

    int x = event.GetX();

    switch(m_dragging)
    {
      case drag_x1:
        SetSplitPos(x,m_x2);
      break;
      case drag_x2:
        SetSplitPos(m_x1,x);
      break;
    }
    m_dragging = drag_none;
  }
  ReleaseMouse();
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (m_dragging == drag_none )
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
    if(m_dragging == drag_x1)
		{
			if (x < m_margin)        x =        m_margin;
			if (x > m_x2 - m_margin) x = m_x2 - m_margin;
		}
    if(m_dragging == drag_x2)
		{
			if (x < m_x1 + m_margin + 1) x = m_x1 + m_margin + 1;
			if (x > m_w  - m_margin) x = m_w  - m_margin;
		}

    DrawTracker(m_oldx);    
    DrawTracker(x);
    m_oldx = x;
  }

  if( event.Leaving() )
  {
    SetCursor(*wxSTANDARD_CURSOR);
  }
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::OnLayout()
//----------------------------------------------------------------------------
{
  if (m_x1 < m_margin)          m_x1 =         m_margin;
  if (m_x1 > m_w - 2*m_margin)  m_x1 = m_w - 2*m_margin;
  if (m_x2 < 2*m_margin)        m_x2 =       2*m_margin;
  if (m_x2 > m_w - m_margin)    m_x2 = m_w - m_margin;
  if (m_x1 > m_x2 - m_margin)   m_x1 = m_x2 - m_margin;

  int m  = 3;
  int w  = (m_w -2*m)/3;

  m_vp1->SetSize(0,0,m_x1,m_h);
	m_vp2->SetSize(m+m_x1,0,m_x2 - m_x1 - m,m_h);
	m_vp3->SetSize(m+m_x2,0,m_w-m_x2-m,m_h);
	
  m_vp1->Layout();
	m_vp2->Layout();
  m_vp3->Layout();
}
//----------------------------------------------------------------------------
FlagSplitterDragModes mmgFlagSplitter::HitTest(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  int x = event.GetX();
  if ( abs(m_x1-x) < 5 ) return drag_x1;
  if ( abs(m_x2-x) < 5 ) return drag_x2;
  return drag_none;
}
//----------------------------------------------------------------------------
void mmgFlagSplitter::DrawTracker(int x)
//----------------------------------------------------------------------------
{
  int m = 3;
  int y1 = m;
	int y2 = m_h-m;
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
void mmgFlagSplitter::Put(wxWindow* w,int i) 
//----------------------------------------------------------------------------
{
  w->Reparent(this);
  switch(i)
  {
		case 0:
			if (m_vp1) delete m_vp1;
			m_vp1 = w;
			m_vp1->Layout();
		break;
		case 1:
			if (m_vp2) delete m_vp2;
			m_vp2 = w;
			m_vp2->Layout();
		break;
		case 2:
			if (m_vp3) delete m_vp3;
			m_vp3 = w;
			m_vp3->Layout();
		break;
    default:
		  assert(false); 
		break;
  };
  
	w->Reparent(this);
	w->Show(true);
}