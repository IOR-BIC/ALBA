/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutSlider
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


#include "albaGUILutSlider.h"

#include "albaDecl.h"
#include "albaEvent.h"

#define BUTT_H 20
#define BUTT_W 5 // min size of mid button

//----------------------------------------------------------------------------
// albaGUILutButt 
class albaGUILutButt : public wxButton
{
public:
  albaGUILutButt() { };

	albaGUILutButt(wxWindow *parent, wxWindowID id,
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
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(evt));
	//  void OnSetFocus(wxFocusEvent& event) {}; 
  int m_X0;

DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// albaGUILutButt EVENT_TABLE
BEGIN_EVENT_TABLE(albaGUILutButt,wxButton)
  EVT_LEFT_DOWN(albaGUILutButt::OnMouse)
  EVT_LEFT_UP(albaGUILutButt::OnMouse)
  EVT_RIGHT_DOWN(albaGUILutButt::OnMouse)
  EVT_MOTION(albaGUILutButt::OnMouse)
	EVT_MOUSE_CAPTURE_LOST(albaGUILutButt::OnMouseCaptureLost)
  EVT_LEFT_DCLICK(albaGUILutButt::OnMouse)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
void albaGUILutButt::OnMouse(wxMouseEvent &event)
{
	if(event.RightDown())
	{
		((albaGUILutSlider*)GetParent())->ShowRangeEntry(GetId()); 
		return;
	}
	else if(event.LeftDown())
	{
    CaptureMouse();
    m_X0 = event.GetX();
	}
	else if(event.LeftUp())
	{
		if( GetCapture() == this )
    {
      ReleaseMouse();
      ((albaGUILutSlider*)GetParent())->ReleaseButton();
    }
	}
	else if (event.ButtonDClick(wxMOUSE_BTN_LEFT))
  {
		((albaGUILutSlider*)GetParent())->ShowSubRangeEntry(GetId());
    return;
  }
	else if(event.LeftIsDown())
	{
		//workaround to avoid mouse events lost 
		if (GetCapture() != this)
			CaptureMouse();

		int sx = event.GetX() - m_X0,sy = 0;
		ClientToScreen(&sx,&sy);
		GetParent()->ScreenToClient(&sx,&sy);
		((albaGUILutSlider*)GetParent())->MoveButton(GetId(),sx);
		SetFocus();
	}
  else
  {
    if( GetCapture() == this )
    {
      //the mouse is captured and the button isn't pressed anymore -- so we lost a LeftMouseUp
      // >> force ReleaseMouse
      ReleaseMouse();
    }
  }
}
//----------------------------------------------------------------------------
void albaGUILutButt::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(evt))
{
	wxMouseEvent evt = wxMouseEvent();
	evt.SetEventType(wxEVT_LEFT_UP);
	OnMouse(evt);
}

//----------------------------------------------------------------------------
// albaGUILutSlider Class Begin


//----------------------------------------------------------------------------
// albaGUILutSlider EVENT_TABLE
BEGIN_EVENT_TABLE(albaGUILutSlider,wxPanel)
  EVT_SIZE(albaGUILutSlider::OnSize)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
albaGUILutSlider::albaGUILutSlider(wxWindow *parent, wxWindowID id, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, long style /* = 0 */, const char* middleButtonTitle /* = "windowing" */)
:wxPanel(parent,id,pos,size, style )
{
  m_Listener = NULL;
  m_MinValue = 0;
  m_MaxValue = 100;
  m_LowValue = 0;
  m_HighValue= 100;
  
  m_MinLabel      = new wxStaticText(this, -1, "", wxPoint(0,0), wxSize(35, BUTT_H));
  m_MinButton     = new albaGUILutButt(this, 1, "0", wxPoint(0,0), wxSize(50, BUTT_H));
  m_MiddleButton  = new albaGUILutButt(this, 3, middleButtonTitle, wxPoint(25,0),	wxSize(100,BUTT_H));
  m_MaxButton     = new albaGUILutButt(this, 2, "100", wxPoint(125,0), wxSize(50, BUTT_H));
  m_MaxLabel      = new wxStaticText(this, -1, "", wxPoint(0,0), wxSize(35, BUTT_H));

  SetMinSize(size);
	

  m_FloatingPointText = false;
  m_FixedText = false;
}
//----------------------------------------------------------------------------
albaGUILutSlider::~albaGUILutSlider()
{
}
//----------------------------------------------------------------------------
void albaGUILutSlider::MoveButton(int id, int pos)
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
		case MIN_BUTTON: //min
		{
			pos = (pos<0) ? 0 : pos;  
			x1 = pos;   

      m_LowValue = ( x1 * (m_MaxValue - m_MinValue) ) / (w-w2-w2-BUTT_W) + m_MinValue;

      if(!m_FloatingPointText)
        m_LowValue=round(m_LowValue);
      
      if (m_LowValue>m_HighValue)
        m_LowValue=m_HighValue;
 
      SetSubRange(m_LowValue,m_HighValue);
		}
		break;
		case MAX_BUTTON: //max
		{
      pos = (pos>w-w2) ? w-w2 : pos;  
      x2 = pos;
      
      m_HighValue  =( (x2-w2-BUTT_W)*(m_MaxValue - m_MinValue) ) / (w-w2-w2-BUTT_W) + m_MinValue;

      if(!m_FloatingPointText)
        m_HighValue=round(m_HighValue);
      
      if (m_HighValue<m_LowValue) 
        m_HighValue=m_LowValue;

      SetSubRange(m_LowValue,m_HighValue);
		}
		break;
		case MIDDLE_BUTTON: //mid
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

      if(!m_FloatingPointText)
      {
        m_HighValue=round(m_HighValue);
        m_LowValue=round(m_LowValue);
      }

      if(false == m_FixedText)
      {
	    m_MinButton->SetLabel(albaString::Format(m_FloatingPointText?"%.2f":"%.0f",m_LowValue));
		  m_MaxButton->SetLabel(albaString::Format(m_FloatingPointText?"%.2f":"%.0f",m_HighValue));
      }
		
			m_MinLabel->SetSize(0,0,x1,BUTT_H);
			m_MaxLabel->SetSize(x2+w2,0,w-(x2+w2),BUTT_H);

		}
		break;
  }

	albaYield();

  albaEvent event;
  event = albaEvent(this,ID_RANGE_MODIFIED,(wxObject *)this);
  event.SetArg(ID_MOUSE_MOVE);
  albaEventMacro(event);
}
//----------------------------------------------------------------------------
void albaGUILutSlider::ReleaseButton()
{
  albaEvent event;
  event = albaEvent(this,ID_RANGE_MODIFIED,(wxObject *)this);
  event.SetArg(ID_MOUSE_RELEASE);
  albaEventMacro(event);
}
//----------------------------------------------------------------------------
void albaGUILutSlider::SetText(long i, wxString text)
{
	switch(i)
	{
		case 0:
			m_MinLabel->SetLabel(text);
		break;
		case 1:
			m_MiddleButton->SetLabel(text);
		break;
		case 2:
			m_MaxLabel->SetLabel(text);
		break;
	}
}
//----------------------------------------------------------------------------
void albaGUILutSlider::SetColour(long i, wxColour colour)
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
void albaGUILutSlider::SetRange(double rmin, double rmax )
{
  m_MinValue = rmin;
  m_MaxValue = rmax;

  m_MaxValue = (m_MaxValue <= m_MinValue) ? m_MinValue+1 : m_MaxValue;
  m_LowValue = (m_LowValue <  m_MinValue) ? m_MinValue   : m_LowValue;
  m_HighValue  = (m_HighValue  >  m_MaxValue) ? m_MaxValue   : m_HighValue;

  UpdateButtons();
}
//----------------------------------------------------------------------------
void albaGUILutSlider::SetSubRange(double  low, double  hi )
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
		m_MinButton->SetLabel(albaString::Format((m_FloatingPointText && m_LowValue < 10000) ? "%.2f" : "%.0f", m_LowValue));
		m_MaxButton->SetLabel(albaString::Format((m_FloatingPointText && m_HighValue < 10000) ? "%.2f" : "%.0f", m_HighValue));
  }
  
  UpdateButtons();
}
//----------------------------------------------------------------------------
void albaGUILutSlider::UpdateButtons()
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
void albaGUILutSlider::OnSize(wxSizeEvent &event)
{
  UpdateButtons();
}

//----------------------------------------------------------------------------
void albaGUILutSlider::ShowRangeEntry(int id)
{

	wxString s;
	wxString msg;
	double val;

	switch (id)
	{
		case 1: //min
		{
			if (true == m_FixedText) return;
			msg = albaString::Format(m_FloatingPointText ? "Enter a value < %.2f" : "Enter a value < %.0f]", m_MaxValue);
			s = albaString::Format(m_FloatingPointText ? "%.2f" : "%.0f", m_MinValue);
			s = wxGetTextFromUser(msg, "Lookup Table Limit Minimum", s, this);
			if (s.ToDouble(&val) && val < m_MaxValue)
			{
				SetRange(val, m_MaxValue);
				SetSubRange(m_LowValue, m_HighValue);
				albaEvent event = albaEvent(this, ID_RANGE_MODIFIED, this);
				albaEventMacro(event);
			}
		}
		break;
		case 2: //max
		{
			msg = albaString::Format(m_FloatingPointText ? "Enter a value > %.2f" : "Enter a value > %.0f]", m_MinValue);
			s = albaString::Format(m_FloatingPointText ? "%.2f" : "%.0f", m_MaxValue);
			s = wxGetTextFromUser(msg, "Lookup Table Limit Maximum", s, this);
			if (s.ToDouble(&val) && val > m_MinValue)
			{
				SetRange(m_MinValue, val);
				SetSubRange(m_LowValue, m_HighValue);
				albaEvent event = albaEvent(this, ID_RANGE_MODIFIED, this);
				albaEventMacro(event);
			}
		}
		break;
	}
}



//----------------------------------------------------------------------------
void albaGUILutSlider::ShowSubRangeEntry(int id)
{
  wxString s;
  wxString msg;
	double val;

	switch(id)
	{
		case 1: //min
		{
      if(true == m_FixedText) return;
			msg = albaString::Format(m_FloatingPointText?"Enter a value in [%.2f .. %.2f]":"Enter a value in [%.0f .. %.0f]",m_MinValue, m_HighValue);
			s = albaString::Format(m_FloatingPointText?"%.2f":"%.0f",m_LowValue);
			s = wxGetTextFromUser(msg, "Lookup Table Minimum",s,this);
      if( s.ToDouble(&val) )
			{
				val = (val>=m_MinValue) ? val : m_MinValue;
				val = (val< m_HighValue)  ? val : m_HighValue;
				SetSubRange(val,m_HighValue);
        albaEvent event = albaEvent(this,ID_RANGE_MODIFIED,this);
		    albaEventMacro(event);
			}
		}
		break;
		case 2: //max
		{
      if(true == m_FixedText) return;
      msg = albaString::Format(m_FloatingPointText?"Enter a value in [%.2f .. %.2f]":"Enter a value in [%.0f .. %.0f]",m_LowValue, m_MaxValue);
			s = albaString::Format(m_FloatingPointText?"%.2f":"%.0f",m_HighValue);
			s = wxGetTextFromUser(msg, "Lookup Table Maximum",s,this);
      if( s.ToDouble(&val) ) 
			{
				val = (val> m_LowValue) ? val : m_LowValue;
				val = (val<=m_MaxValue) ? val : m_MaxValue;
				SetSubRange(m_LowValue,val);
        albaEvent event = albaEvent(this,ID_RANGE_MODIFIED,this);
        albaEventMacro(event);
			}
		}
		break;
	}
}
//----------------------------------------------------------------------------
void albaGUILutSlider::SetFixedTextMinButton(const char* label)
{
  if(m_MinButton)
    m_MinButton->SetLabel(label);
}
//----------------------------------------------------------------------------
void albaGUILutSlider::SetFixedTextMaxButton(const char* label)
{
  if(m_MaxButton)
    m_MaxButton->SetLabel(label);
}
//----------------------------------------------------------------------------
void albaGUILutSlider::EnableMiddleButton(bool enable)
{
  if(m_MiddleButton)
    m_MiddleButton->Enable(enable);
}
//----------------------------------------------------------------------------
void albaGUILutSlider::EnableMaxButton(bool enable)
{
  if(m_MaxButton)
    m_MaxButton->Enable(enable);
}
//----------------------------------------------------------------------------
void albaGUILutSlider::EnableMinButton(bool enable)
{
  if(m_MinButton)
    m_MinButton->Enable(enable);
}
