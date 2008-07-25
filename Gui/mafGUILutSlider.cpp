/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUILutSlider.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
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


#include "mafGUILutSlider.h"

#include "mafDecl.h"
#include "mafEvent.h"

#define BUTT_H 18
#define BUTT_W 5 // min size of mid button

//----------------------------------------------------------------------------
// mafGUILutButt 
//----------------------------------------------------------------------------
class mafGUILutButt : public wxButton
{
public:
  mafGUILutButt() { };

  mafGUILutButt(wxWindow *parent, wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0)
  {
    Create(parent, id, label, pos, size, style);
    m_X0 = 0;
  };

protected:  
  void OnMouse(wxMouseEvent &event);
  void OnSetFocus(wxFocusEvent& event) {}; 
  int m_X0;

DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// mafGUILutButt EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUILutButt,wxButton)
	//EVT_MOUSE_EVENTS(mafGUILutButt::OnMouse)
  EVT_LEFT_DOWN(mafGUILutButt::OnMouse)
  EVT_LEFT_UP(mafGUILutButt::OnMouse)
  EVT_RIGHT_DOWN(mafGUILutButt::OnMouse)
  EVT_MOTION(mafGUILutButt::OnMouse)
  EVT_SET_FOCUS(mafGUILutButt::OnSetFocus) 
END_EVENT_TABLE()
//----------------------------------------------------------------------------
void mafGUILutButt::OnMouse(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
	if(event.RightDown())
	{
		((mafGUILutSlider*)GetParent())->ShowEntry(GetId());
		return;
	}
	if(event.LeftDown())
	{
    /*if( GetCapture() == this )
      mafLogMessage("mouse already captured !! - strange");
    else*/
      CaptureMouse();
    m_X0 = event.GetX();
		return;
	}
	if(event.LeftUp())
	{
		if( GetCapture() == this )
    {
      ReleaseMouse();
      ((mafGUILutSlider*)GetParent())->ReleaseButton();
      //mafLogMessage("mouse Released");
    }
		return;
	}

  
  if(event.LeftIsDown())
	{
		int sx = event.GetX() - m_X0,sy = 0;
		ClientToScreen(&sx,&sy);
		GetParent()->ScreenToClient(&sx,&sy);
		((mafGUILutSlider*)GetParent())->MoveButton(GetId(),sx);
	}
  else
  {
    if( GetCapture() == this )
    {
      //the mouse is captured and the button isn't pressed anymore -- so we lost a LeftMouseUp
      // >> force ReleaseMouse
      ReleaseMouse();
      //mafLogMessage("mouse Released (forced)");
    }
  }
}
//----------------------------------------------------------------------------
// mafGUILutSlider EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUILutSlider,wxPanel)
  EVT_SIZE(mafGUILutSlider::OnSize)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mafGUILutSlider::mafGUILutSlider(wxWindow *parent, wxWindowID id, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, long style /* = 0 */, const char* middleButtonTitle /* = "windowing" */)
:wxPanel(parent,id,pos,size,wxSUNKEN_BORDER | wxCLIP_CHILDREN )
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_MinValue = 0;
  m_MaxValue = 100;
  m_LowValue = 0;
  m_HighValue= 100;
  
  m_MinLabel      = new wxStaticText(this, -1, "", wxPoint(0,0), wxSize(35, BUTT_H));
  m_MinButton     = new mafGUILutButt(this, 1, "0", wxPoint(0,0), wxSize(50, BUTT_H));
  m_MiddleButton  = new mafGUILutButt(this, 3, middleButtonTitle, wxPoint(25,0),	wxSize(100,BUTT_H));
  m_MaxButton     = new mafGUILutButt(this, 2, "100", wxPoint(125,0), wxSize(50, BUTT_H));
  m_MaxLabel      = new wxStaticText(this, -1, "", wxPoint(0,0), wxSize(35, BUTT_H));

  SetMinSize(size);

  m_FloatingPointText = false;
  m_FixedText = false;
}
//----------------------------------------------------------------------------
mafGUILutSlider::~mafGUILutSlider()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUILutSlider::MoveButton(int id, int pos)
//----------------------------------------------------------------------------
{
	int x1=0,y1=0,w1=0,h1=0;
  int x2=0,y2=0,w2=0,h2=0;
  int x3=0,y3=0,w3=0,h3=0;
  m_MinButton->GetPosition(&x1,&y1);
  m_MinButton->GetSize(&w1,&h1);
  m_MaxButton->GetPosition(&x2,&y2);
  m_MaxButton->GetSize(&w2,&h2);
  m_MiddleButton->GetPosition(&x3,&y3);
  m_MiddleButton->GetSize(&w3,&h3);

  int w=0,h=0;
  GetSize(&w,&h); w -=6;
   
  switch(id)
  {
		case 1: //min
		{
			pos = (pos<0) ? 0 : pos;  
			pos = (pos>x2-w1-BUTT_W) ? x2-w1-BUTT_W : pos;  
			m_MinButton->Move(pos,0);
			m_MiddleButton->SetSize(pos+w1,0,x2-pos-w1,BUTT_H);
			x1 = pos;   

      m_LowValue = ( x1 * (m_MaxValue - m_MinValue) ) / (w-w2-w2-BUTT_W) + m_MinValue;
      if(false == m_FixedText)
      {
        m_MinButton->SetLabel(wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_LowValue));	
      }

			m_MinLabel->SetSize(0,0,pos,BUTT_H);
      Refresh(false);
		}
		break;
		case 2: //max
		{
			pos = (pos<x1+w1+BUTT_W) ? x1+w1+BUTT_W : pos;  
			pos = (pos>w-w2) ? w-w2 : pos;  
			m_MaxButton->Move(pos,0);
			m_MiddleButton->SetSize(x1+w1,0,pos-x1-w1,BUTT_H);
			x2 = pos;

      m_HighValue  =( (x2-w2-BUTT_W)*(m_MaxValue - m_MinValue) ) / (w-w2-w2-BUTT_W) + m_MinValue;
      if(false == m_FixedText)
      {
	    m_MaxButton->SetLabel(wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_HighValue));
      }

			m_MaxLabel->SetSize(pos+w2,0,w-(pos+w2),BUTT_H);
      Refresh(false);
		}
		break;
		case 3: //mid
		{
			pos = (pos<w1) ? w1 : pos;  
			pos = (pos>w-w2-w3) ? w-w2-w3 : pos;  

			m_MiddleButton->Move(pos,0);
			m_MinButton->Move(pos-w1,0);
			m_MaxButton->Move(pos+w3,0);
			x1 = pos-w1;
			x2 = pos+w3;

      m_LowValue = ( x1 * (m_MaxValue - m_MinValue) ) / (w-w2-w2-BUTT_W) + m_MinValue;
      m_HighValue  =( (x2-w2-BUTT_W)*(m_MaxValue - m_MinValue) ) / (w-w2-w2-BUTT_W) + m_MinValue;
      if(false == m_FixedText)
      {
	    m_MinButton->SetLabel(wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_LowValue));
		m_MaxButton->SetLabel(wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_HighValue));
      }
		
			m_MinLabel->SetSize(0,0,x1,BUTT_H);
			m_MaxLabel->SetSize(x2+w2,0,w-(x2+w2),BUTT_H);

		}
		break;
  }
  mafEvent event;
  event = mafEvent(this,ID_RANGE_MODIFIED,(wxObject *)this);
  event.SetArg(ID_MOUSE_MOVE);
  mafEventMacro(event);
}
//----------------------------------------------------------------------------
void mafGUILutSlider::ReleaseButton()
//----------------------------------------------------------------------------
{
  mafEvent event;
  event = mafEvent(this,ID_RANGE_MODIFIED,(wxObject *)this);
  event.SetArg(ID_MOUSE_RELEASE);
  mafEventMacro(event);
}
//----------------------------------------------------------------------------
void mafGUILutSlider::SetText(long i, wxString text)
//----------------------------------------------------------------------------
{
	switch(i)
	{
		case 0:
			m_MinLabel->SetTitle(text);
		break;
		case 1:
			m_MiddleButton->SetTitle(text);
		break;
		case 2:
			m_MaxLabel->SetTitle(text);
		break;
	}
}
//----------------------------------------------------------------------------
void mafGUILutSlider::SetColour(long i, wxColour colour)
//----------------------------------------------------------------------------
{
	switch(i)
	{
		case 0:
			m_MinLabel->SetBackgroundColour(colour);
		break;
		case 1:
			m_MiddleButton->SetBackgroundColour(colour);
		break;
		case 2:
			m_MaxLabel->SetBackgroundColour(colour);
		break;
	}
}
//----------------------------------------------------------------------------
void mafGUILutSlider::SetRange(double rmin, double rmax )
//----------------------------------------------------------------------------
{
  m_MinValue = rmin;
  m_MaxValue = rmax;

  m_MaxValue = (m_MaxValue <= m_MinValue) ? m_MinValue+1 : m_MaxValue;
  m_LowValue = (m_LowValue <  m_MinValue) ? m_MinValue   : m_LowValue;
  m_HighValue  = (m_HighValue  >  m_MaxValue) ? m_MaxValue   : m_HighValue;

  UpdateButtons();
}
//----------------------------------------------------------------------------
void mafGUILutSlider::SetSubRange(double  low, double  hi )
//----------------------------------------------------------------------------
{
  
  m_LowValue = low;
  m_HighValue  = hi;

  m_LowValue = (m_LowValue > m_MinValue)  ? m_LowValue : m_MinValue;
  m_LowValue = (m_LowValue < m_MaxValue)	? m_LowValue : m_MaxValue;
  m_HighValue  = (m_HighValue  > m_MinValue)	? m_HighValue	: m_MinValue;
  m_HighValue  = (m_HighValue  < m_MaxValue)	? m_HighValue  : m_MaxValue;
  m_HighValue  = (m_HighValue  > m_LowValue)	? m_HighValue 	: m_LowValue;

  if(false == m_FixedText)
  {
    m_MinButton->SetLabel(wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_LowValue));	
    m_MaxButton->SetLabel(wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_HighValue));	
  }
  
  UpdateButtons();
}
//----------------------------------------------------------------------------
void mafGUILutSlider::UpdateButtons()
//----------------------------------------------------------------------------
{
	int w=0,h=0;
  GetSize(&w,&h); w -=6;
  int x1=0,y1=0,w1=0,h1=0;
  int x2=0,y2=0,w2=0,h2=0;
  m_MinButton->GetSize(&w1,&h1);
  m_MaxButton->GetSize(&w2,&h2);
	 
  x1 = ( (m_LowValue - m_MinValue) * (w-w2-w2-BUTT_W) ) / (m_MaxValue - m_MinValue);
  x2 = ( (m_HighValue  - m_MinValue) * (w-w2-w2-BUTT_W) ) / (m_MaxValue - m_MinValue) +w2+BUTT_W ;

  m_MinButton->Move(x1,0);
  m_MaxButton->Move(x2,0);
  m_MiddleButton->SetSize(x1+w1,0,x2-x1-w1,BUTT_H);

	m_MinLabel->SetSize(0,0,x1,BUTT_H);
	m_MaxLabel->SetSize(x2+w2,0,w-(x2+w2),BUTT_H);

  Refresh(false);
}
//----------------------------------------------------------------------------
void mafGUILutSlider::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  UpdateButtons();
}
//----------------------------------------------------------------------------
void mafGUILutSlider::ShowEntry(int id)
//----------------------------------------------------------------------------
{
  wxString s;
  wxString msg;
	long val;

	switch(id)
	{
		case 1: //min
		{
      if(true == m_FixedText) return;
			msg = wxString::Format(m_FloatingPointText?"Enter a value in [%.1f .. %.1f]":"Enter a value in [%.0f .. %.0f]",m_MinValue, m_HighValue);
			s = wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_LowValue);
			s = wxGetTextFromUser(msg, "Lookup table Minimum",s,this);
      if( s.ToLong(&val) ) 
			{
				val = (val>=m_MinValue) ? val : m_MinValue;
				val = (val< m_HighValue)  ? val : m_HighValue;
				SetSubRange(val,m_HighValue);
        mafEvent event = mafEvent(this,ID_RANGE_MODIFIED,this);
		    mafEventMacro(event);
			}
		}
		break;
		case 2: //max
		{
      if(true == m_FixedText) return;
      msg = wxString::Format(m_FloatingPointText?"Enter a value in [%.1f .. %.1f]":"Enter a value in [%.0f .. %.0f]",m_LowValue, m_MaxValue);
			s = wxString::Format(m_FloatingPointText?"%.1f":"%.0f",m_HighValue);
			s = wxGetTextFromUser(msg, "Lookup table Maximum",s,this);
      if( s.ToLong(&val) ) 
			{
				val = (val> m_LowValue) ? val : m_LowValue;
				val = (val<=m_MaxValue) ? val : m_MaxValue;
				SetSubRange(m_LowValue,val);
        mafEvent event = mafEvent(this,ID_RANGE_MODIFIED,this);
        mafEventMacro(event);
			}
		}
		break;
	}
}

//----------------------------------------------------------------------------
void mafGUILutSlider::SetFixedTextMinButton(const char* label)
//----------------------------------------------------------------------------
{
  if(NULL != m_MinButton)
  {
    m_MinButton->SetLabel(label);
  }
}
//----------------------------------------------------------------------------
void mafGUILutSlider::SetFixedTextMaxButton(const char* label)
//----------------------------------------------------------------------------
{
  if(NULL != m_MaxButton)
  {
    m_MaxButton->SetLabel(label);
  }
}
