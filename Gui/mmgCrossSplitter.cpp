/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCrossSplitter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-04 16:22:15 $
  Version:   $Revision: 1.4 $
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
	m_ViewPanel1 = new wxPanel(this,1,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_ViewPanel2 = new wxPanel(this,2,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_ViewPanel3 = new wxPanel(this,3,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         
  m_ViewPanel4 = new wxPanel(this,4,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);         

  m_Maximized = false;
	m_FocusedPanel = m_ViewPanel1;

  m_CursorNS   = new wxCursor(wxCURSOR_SIZENS);
  m_CursorWE   = new wxCursor(wxCURSOR_SIZEWE);
  m_CursorNSWE = new wxCursor(wxCURSOR_SIZING);
  m_Pen        = new wxPen(*wxBLACK, 2, wxSOLID);

  m_With = m_Height = 100;
  m_XPos = m_YPos = m_Margin = 20;
  m_Dragging = drag_none;
  m_RelXPos = m_RelYPos = 0.5,
	Split(VA_FOUR);
}
//----------------------------------------------------------------------------
mmgCrossSplitter::~mmgCrossSplitter()
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

  if (m_With == -1 )
  {
    return;
  }

  GetClientSize(&m_With,&m_Height); 
  m_XPos = m_RelXPos * m_With;
  m_YPos = m_RelYPos * m_Height;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::Split(CrossSplitterModes mode)
//----------------------------------------------------------------------------
{
  m_Maximized = false;
	m_Mode = mode;

	switch(m_Mode)
  {
		case VA_ONE:
			m_ViewPanel1->Show(true);
			m_ViewPanel2->Show(false); 
			m_ViewPanel3->Show(false); 
			m_ViewPanel4->Show(false);
		break;
		case VA_TWO_VERT:
		case VA_TWO_HORZ:
			m_ViewPanel1->Show(true);
			m_ViewPanel2->Show(true);
			m_ViewPanel3->Show(false);
			m_ViewPanel4->Show(false);
		break;
		case VA_THREE_UP:
		case VA_THREE_DOWN:
		case VA_THREE_LEFT:
		case VA_THREE_RIGHT:
			m_ViewPanel1->Show(true); 
			m_ViewPanel2->Show(true); 
			m_ViewPanel3->Show(true); 
			m_ViewPanel4->Show(false); 
		break;
		case VA_FOUR:
			m_ViewPanel1->Show(true); 
			m_ViewPanel2->Show(true); 
			m_ViewPanel3->Show(true); 
			m_ViewPanel4->Show(true); 
		break;
  }
  m_RelXPos = 0.5;
  m_RelYPos = 0.5;
  
	m_XPos = m_With * m_RelXPos;
  m_YPos = m_Height * m_RelYPos;
  
  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::SetSplitPos(int x,int y)
//----------------------------------------------------------------------------
{
  if (m_XPos < m_Margin)       m_XPos =       m_Margin;
  if (m_XPos > m_With - m_Margin) m_XPos = m_With - m_Margin;
  if (m_YPos < m_Margin)       m_YPos =       m_Margin;
  if (m_YPos > m_Height - m_Margin) m_YPos = m_Height - m_Margin;

  m_XPos = x;
  m_YPos = y;

  m_RelXPos = (x * 1.0 )/m_With;
  m_RelYPos = (y * 1.0 )/m_Height;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::SetSplitPosRel(float x,float y)
//----------------------------------------------------------------------------
{
  m_RelXPos = x;
  m_RelYPos = y;

  m_XPos = x * m_With;
  m_YPos = y * m_Height;

  OnLayout();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  CaptureMouse();

  m_Dragging = HitTest(event);
  if(m_Dragging == 0) return;

  int x = event.GetX();
  int y = event.GetY();
  DrawTracker(x,y);    
  m_OldXPos = x;
  m_OldYPos = y;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if(m_Dragging != drag_none) 
  {
    DrawTracker(m_OldXPos,m_OldYPos);    

    int x = event.GetX();
    int y = event.GetY();

    switch(m_Dragging)
    {
      case drag_y:
        SetSplitPos(m_XPos,y);
      break;
      case drag_x:
        SetSplitPos(x,m_YPos);
      break;
      case drag_xy:
        SetSplitPos(x,y);
      break;
    }
    m_Dragging = drag_none;
  }

  ReleaseMouse();
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  if (m_Dragging == drag_none )
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
    DrawTracker(m_OldXPos,m_OldYPos);    
    DrawTracker(x,y);    
    m_OldXPos = x;
    m_OldYPos = y;
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
  if (m_XPos < m_Margin)       m_XPos =       m_Margin;
  if (m_XPos > m_With - m_Margin) m_XPos = m_With - m_Margin;
  if (m_YPos < m_Margin)       m_YPos =       m_Margin;
  if (m_YPos > m_Height - m_Margin) m_YPos = m_Height - m_Margin;
  
  int m  = 3;
  int w  = m_With  -m;
  int h  = m_Height  -m;
  int x1 = 0;
  int x2 = m_XPos + m;
  int y1 = 0;
  int y2 = m_YPos + m;
  int w1 = m_XPos - m;
  int w2 = w - m_XPos -m;
  int h1 = m_YPos - m;
  int h2 = h - m_YPos -m;

  if(this->m_Maximized)
	{
		this->m_FocusedPanel->Move(x1, y1);
		this->m_FocusedPanel->SetSize(w,h);
	} 
	else
	{
		switch(m_Mode)
		{
			case VA_ONE:
				m_ViewPanel1->Move(x1, y1);
				m_ViewPanel1->SetSize(w,h);
			break;
			case VA_TWO_VERT:
				m_ViewPanel1->Move(x1, y1);   
				m_ViewPanel1->SetSize(w1,h);
				m_ViewPanel2->Move(x2, y1);   
				m_ViewPanel2->SetSize(w2,h);
			break;
			case VA_TWO_HORZ:
				m_ViewPanel1->Move(x1, y1);   
				m_ViewPanel1->SetSize(w,h1);
				m_ViewPanel2->Move(x1,y2);   
				m_ViewPanel2->SetSize(w,h2);
			break;
			case VA_THREE_UP:
				m_ViewPanel1->Move(x1, y1);   
				m_ViewPanel1->SetSize(w,h1);
				m_ViewPanel2->Move(x1, y2);   
				m_ViewPanel2->SetSize(w1,h2);
				m_ViewPanel3->Move(x2, y2);   
				m_ViewPanel3->SetSize(w2,h2);
			break;
			case VA_THREE_DOWN:
				m_ViewPanel1->Move(x1, y2);   
				m_ViewPanel1->SetSize(w,h2);
				m_ViewPanel2->Move(x1, y1);   
				m_ViewPanel2->SetSize(w1,h1);
				m_ViewPanel3->Move(x2, y1);   
				m_ViewPanel3->SetSize(w2,h1);
			break;
			case VA_THREE_LEFT:
				m_ViewPanel1->Move(x1, y1);   
				m_ViewPanel1->SetSize(w1,h);
				m_ViewPanel2->Move(x2, y1);   
				m_ViewPanel2->SetSize(w2,h1);
				m_ViewPanel3->Move(x2, y2);   
				m_ViewPanel3->SetSize(w2,h2);
			break;
			case VA_THREE_RIGHT:
				m_ViewPanel1->Move(x2, y1);   
				m_ViewPanel1->SetSize(w2,h);
				m_ViewPanel2->Move(x1, y1);   
				m_ViewPanel2->SetSize(w1,h1);
				m_ViewPanel3->Move(x1, y2);   
				m_ViewPanel3->SetSize(w1,h2);
			break;
			case VA_FOUR:
				m_ViewPanel1->Move(x1, y1);   
				m_ViewPanel1->SetSize(w1,h1);
				m_ViewPanel2->Move(x2, y1);   
				m_ViewPanel2->SetSize(w2,h1);
				m_ViewPanel3->Move(x1, y2);   
				m_ViewPanel3->SetSize(w1,h2);
				m_ViewPanel4->Move(x2, y2);   
				m_ViewPanel4->SetSize(w2,h2);
			break;
		}
  }
  
  m_ViewPanel1->Layout();
  m_ViewPanel2->Layout();
  m_ViewPanel3->Layout();
  m_ViewPanel4->Layout();

  this->Refresh();
}
//----------------------------------------------------------------------------
CrossSplitterDragModes mmgCrossSplitter::HitTest(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  wxCoord x = (wxCoord)event.GetX(),
          y = (wxCoord)event.GetY();

  if ( abs(m_XPos-x) < 5 &&  abs(m_YPos-y) < 5 && m_Mode >= VA_THREE_UP) return drag_xy;
  if ( abs(m_XPos-x) < 5 ) return drag_x;
  if ( abs(m_YPos-y) < 5 ) return drag_y;

  return drag_none;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::DrawTracker(int x, int y)
//----------------------------------------------------------------------------
{
  int m = 3;
  int x1, y1;
  int x2, y2;

  if (x < m_Margin)       x =       m_Margin;
  if (x > m_With - m_Margin) x = m_With - m_Margin;
  if (y < m_Margin)       y =       m_Margin;
  if (y > m_Height - m_Margin) y = m_Height - m_Margin;
    
  wxScreenDC screenDC;
  screenDC.SetLogicalFunction(wxINVERT);
  screenDC.SetPen(*m_Pen);
  screenDC.SetBrush(*wxTRANSPARENT_BRUSH);
    
  if ( m_Dragging == 1 || m_Dragging == 3 )
  {
    x1 = x; y1 = m;
    x2 = x; y2 = m_Height-m;


    ClientToScreen(&x1, &y1);
    ClientToScreen(&x2, &y2);
    screenDC.DrawLine(x1, y1, x2, y2);
  }

  if ( m_Dragging == 2 || m_Dragging == 3 )
  {
    x1 = m;     y1 = y;
    x2 = m_With-m; y2 = y;

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
			if (m_ViewPanel1) delete m_ViewPanel1;
			m_ViewPanel1 = w;
			m_ViewPanel1->Layout();
		break;
		case 2:
			if (m_ViewPanel2) delete m_ViewPanel2;
			m_ViewPanel2 = w;
			m_ViewPanel2->Layout();
		break;
		case 3:
			if (m_ViewPanel3) delete m_ViewPanel3;
			m_ViewPanel3 = w;
			m_ViewPanel3->Layout();
		break;
		case 4:
			if (m_ViewPanel4) delete m_ViewPanel4;
			m_ViewPanel4 = w;
			m_ViewPanel4->Layout();
		break;
  };
	w->Show(true);
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::SetFocusedPanel(wxWindow* w)
//----------------------------------------------------------------------------
{
	this->m_FocusedPanel = w;
}
//----------------------------------------------------------------------------
void mmgCrossSplitter::Maximize()
//----------------------------------------------------------------------------
{
	m_Maximized = !m_Maximized;

  if(m_Maximized) 
	{
		m_ViewPanel1->Show(false);
    m_ViewPanel2->Show(false); 
    m_ViewPanel3->Show(false); 
    m_ViewPanel4->Show(false);
		this->m_FocusedPanel->Show(true);
    OnLayout();
	}
	else
	{
    Split(m_Mode);
    OnLayout();
	}
	this->Layout();
}
