/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCrossSplitter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-10 08:50:41 $
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


#include "mafDecl.h"
#include "mmgCrossSplitter.h"

//----------------------------------------------------------------------------
// mmgCrossSplitter
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgCrossSplitter,mmgPanel)
  EVT_SIZE(mmgCrossSplitter::OnSize)
  EVT_LEFT_DOWN(mmgCrossSplitter::OnLeftMouseButtonDown)
  EVT_LEFT_UP(mmgCrossSplitter::OnLeftMouseButtonUp)
  EVT_MOTION(mmgCrossSplitter::OnMouseMotion)
  EVT_LEAVE_WINDOW(mmgCrossSplitter::OnMouseMotion)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mmgCrossSplitter::mmgCrossSplitter(wxWindow* parent,wxWindowID id)
: mmgPanel(parent, id)
//----------------------------------------------------------------------------
{
	m_vp1 = new wxPanel(this,1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_vp2 = new wxPanel(this,2,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_vp3 = new wxPanel(this,3,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_vp4 = new wxPanel(this,4,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         

  m_maximized = false;
	m_focused_panel = m_vp1;

  m_CursorNS   = new wxCursor(wxCURSOR_SIZENS);
  m_CursorWE   = new wxCursor(wxCURSOR_SIZEWE);
  m_CursorNSWE = new wxCursor(wxCURSOR_SIZING);
  m_Pen        = new wxPen(*wxBLACK, 2, wxSOLID);

  m_w = m_h = 100;
  m_x = m_y = m_margin = 20;
  m_dragging = drag_none;
  m_relx = m_rely = 0.5,
	Split(VA_FOUR);
}
//----------------------------------------------------------------------------
mmgCrossSplitter::~mmgCrossSplitter( ) 
//----------------------------------------------------------------------------
{
  delete m_CursorNS;
  delete m_CursorWE;
  delete m_CursorNSWE;
  delete m_Pen;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  wxPanel::OnSize(event);
  mafYield();

  if (m_w == -1 )
  {
    return;
  }

  GetClientSize(&m_w,&m_h); 
  m_x = m_relx * m_w;
  m_y = m_rely * m_h;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::Split(CrossSplitterModes mode)
//----------------------------------------------------------------------------
{
  m_maximized = false;
	m_mode = mode;

	switch(m_mode)
  {
		case VA_ONE:
			m_vp1->Show(true);
			m_vp2->Show(false); 
			m_vp3->Show(false); 
			m_vp4->Show(false);
		break;
		case VA_TWO_VERT:
		case VA_TWO_HORZ:
			m_vp1->Show(true);
			m_vp2->Show(true);
			m_vp3->Show(false);
			m_vp4->Show(false);
		break;
		case VA_THREE_UP:
		case VA_THREE_DOWN:
		case VA_THREE_LEFT:
		case VA_THREE_RIGHT:
			m_vp1->Show(true); 
			m_vp2->Show(true); 
			m_vp3->Show(true); 
			m_vp4->Show(false); 
		break;
		case VA_FOUR:
			m_vp1->Show(true); 
			m_vp2->Show(true); 
			m_vp3->Show(true); 
			m_vp4->Show(true); 
		break;
  }
  m_relx = 0.5;
  m_rely = 0.5;
  
	m_x = m_w * m_relx;
  m_y = m_h * m_rely;
  
  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::SetSplitPos(int x,int y)
//----------------------------------------------------------------------------
{
  if (m_x < m_margin)       m_x =       m_margin;
  if (m_x > m_w - m_margin) m_x = m_w - m_margin;
  if (m_y < m_margin)       m_y =       m_margin;
  if (m_y > m_h - m_margin) m_y = m_h - m_margin;

  m_x = x;
  m_y = y;

  m_relx = (x * 1.0 )/m_w;
  m_rely = (y * 1.0 )/m_h;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::SetSplitPosRel(float x,float y)
//----------------------------------------------------------------------------
{
  m_relx = x;
  m_rely = y;

  m_x = x * m_w;
  m_y = y * m_h;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  CaptureMouse();

  m_dragging = HitTest(event);
  if(m_dragging == 0) return;

  int x = event.GetX();
  int y = event.GetY();
  DrawTracker(x,y);    
  m_oldx = x;
  m_oldy = y;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_dragging != drag_none) 
  {
    DrawTracker(m_oldx,m_oldy);    

    int x = event.GetX();
    int y = event.GetY();

    switch(m_dragging)
    {
      case drag_y:
        SetSplitPos(m_x,y);
      break;
      case drag_x:
        SetSplitPos(x,m_y);
      break;
      case drag_xy:
        SetSplitPos(x,y);
      break;
    }
    m_dragging = drag_none;
  }

  ReleaseMouse();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (m_dragging == drag_none )
  {
    switch(HitTest(event))
    {
      case drag_y:
        SetCursor(*m_CursorNS);
      break;
      case drag_x:
        SetCursor(*m_CursorWE);
      break;
      case drag_xy:
        SetCursor(*m_CursorNSWE);
      break;
      case drag_none:
        SetCursor(*wxSTANDARD_CURSOR);
      break;
    }
  } 
  else 
  {
    int x = event.GetX();
    int y = event.GetY();
    DrawTracker(m_oldx,m_oldy);    
    DrawTracker(x,y);    
    m_oldx = x;
    m_oldy = y;
  }

  if( event.Leaving() )
  {
    SetCursor(*wxSTANDARD_CURSOR);
  }
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnLayout()
//----------------------------------------------------------------------------
{
  if (m_x < m_margin)       m_x =       m_margin;
  if (m_x > m_w - m_margin) m_x = m_w - m_margin;
  if (m_y < m_margin)       m_y =       m_margin;
  if (m_y > m_h - m_margin) m_y = m_h - m_margin;
  
  int m  = 3;
  int w  = m_w  -m;
  int h  = m_h  -m;
  int x1 = 0;
  int x2 = m_x + m;
  int y1 = 0;
  int y2 = m_y + m;
  int w1 = m_x - m;
  int w2 = w - m_x -m;
  int h1 = m_y - m;
  int h2 = h - m_y -m;

  if(this->m_maximized)
	{
		this->m_focused_panel->Move(x1, y1);
		this->m_focused_panel->SetSize(w,h);
	} 
	else
	{
		switch(m_mode)
		{
			case VA_ONE:
				m_vp1->Move(x1, y1);
				m_vp1->SetSize(w,h);
			break;
			case VA_TWO_VERT:
				m_vp1->Move(x1, y1);   
				m_vp1->SetSize(w1,h);
				m_vp2->Move(x2, y1);   
				m_vp2->SetSize(w2,h);
			break;
			case VA_TWO_HORZ:
				m_vp1->Move(x1, y1);   
				m_vp1->SetSize(w,h1);
				m_vp2->Move(x1,y2);   
				m_vp2->SetSize(w,h2);
			break;
			case VA_THREE_UP:
				m_vp1->Move(x1, y1);   
				m_vp1->SetSize(w,h1);
				m_vp2->Move(x1, y2);   
				m_vp2->SetSize(w1,h2);
				m_vp3->Move(x2, y2);   
				m_vp3->SetSize(w2,h2);
			break;
			case VA_THREE_DOWN:
				m_vp1->Move(x1, y2);   
				m_vp1->SetSize(w,h2);
				m_vp2->Move(x1, y1);   
				m_vp2->SetSize(w1,h1);
				m_vp3->Move(x2, y1);   
				m_vp3->SetSize(w2,h1);
			break;
			case VA_THREE_LEFT:
				m_vp1->Move(x1, y1);   
				m_vp1->SetSize(w1,h);
				m_vp2->Move(x2, y1);   
				m_vp2->SetSize(w2,h1);
				m_vp3->Move(x2, y2);   
				m_vp3->SetSize(w2,h2);
			break;
			case VA_THREE_RIGHT:
				m_vp1->Move(x2, y1);   
				m_vp1->SetSize(w2,h);
				m_vp2->Move(x1, y1);   
				m_vp2->SetSize(w1,h1);
				m_vp3->Move(x1, y2);   
				m_vp3->SetSize(w1,h2);
			break;
			case VA_FOUR:
				m_vp1->Move(x1, y1);   
				m_vp1->SetSize(w1,h1);
				m_vp2->Move(x2, y1);   
				m_vp2->SetSize(w2,h1);
				m_vp3->Move(x1, y2);   
				m_vp3->SetSize(w1,h2);
				m_vp4->Move(x2, y2);   
				m_vp4->SetSize(w2,h2);
			break;
		}
  }
  
  m_vp1->Layout();
  m_vp2->Layout();
  m_vp3->Layout();
  m_vp4->Layout();

  this->Refresh();
}
//----------------------------------------------------------------------------
CrossSplitterDragModes mmgCrossSplitter::HitTest(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  wxCoord x = (wxCoord)event.GetX(),
          y = (wxCoord)event.GetY();

  if ( abs(m_x-x) < 5 &&  abs(m_y-y) < 5 && m_mode >= VA_THREE_UP) return drag_xy;
  if ( abs(m_x-x) < 5 ) return drag_x;
  if ( abs(m_y-y) < 5 ) return drag_y;

  return drag_none;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::DrawTracker(int x, int y)
//----------------------------------------------------------------------------
{
  int m = 3;
  int x1, y1;
  int x2, y2;

  if (x < m_margin)       x =       m_margin;
  if (x > m_w - m_margin) x = m_w - m_margin;
  if (y < m_margin)       y =       m_margin;
  if (y > m_h - m_margin) y = m_h - m_margin;
    
  wxScreenDC screenDC;
  screenDC.SetLogicalFunction(wxINVERT);
  screenDC.SetPen(*m_Pen);
  screenDC.SetBrush(*wxTRANSPARENT_BRUSH);
    
  if ( m_dragging == 1 || m_dragging == 3 )
  {
    x1 = x; y1 = m;
    x2 = x; y2 = m_h-m;


    ClientToScreen(&x1, &y1);
    ClientToScreen(&x2, &y2);
    screenDC.DrawLine(x1, y1, x2, y2);
  }

  if ( m_dragging == 2 || m_dragging == 3 )
  {
    x1 = m;     y1 = y;
    x2 = m_w-m; y2 = y;

    ClientToScreen(&x1, &y1);
    ClientToScreen(&x2, &y2);
    screenDC.DrawLine(x1, y1, x2, y2);
  }

  screenDC.SetLogicalFunction(wxCOPY);
  screenDC.SetPen(wxNullPen);
  screenDC.SetBrush(wxNullBrush);
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::Put(wxWindow* w,int i)
//----------------------------------------------------------------------------
{
  w->Reparent(this);
  switch(i)
  {
		case 1:
			if (m_vp1) delete m_vp1;
			m_vp1 = w;
			m_vp1->Layout();
		break;
		case 2:
			if (m_vp2) delete m_vp2;
			m_vp2 = w;
			m_vp2->Layout();
		break;
		case 3:
			if (m_vp3) delete m_vp3;
			m_vp3 = w;
			m_vp3->Layout();
		break;
		case 4:
			if (m_vp4) delete m_vp4;
			m_vp4 = w;
			m_vp4->Layout();
		break;
  };
	w->Show(true);
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::SetFocusedPanel(wxWindow* w)
//----------------------------------------------------------------------------
{
	this->m_focused_panel = w;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::Maximize()
//----------------------------------------------------------------------------
{
	m_maximized = !m_maximized;

  if(m_maximized) 
	{
		m_vp1->Show(false);
    m_vp2->Show(false); 
    m_vp3->Show(false); 
    m_vp4->Show(false);
		this->m_focused_panel->Show(true);
    OnLayout();
	}
	else
	{
    Split(m_mode);
    OnLayout();
	}
	this->Layout();
}
