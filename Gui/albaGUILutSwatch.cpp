/*=========================================================================

Program: ALBA
Module: albaGUILutSwatch
Authors: Silvano Imboden

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include <math.h>
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include "albaGUILutSwatch.h"
#include "albaColor.h"
#include "albaGUILutEditor.h"

// ugly hack to make DrawText Work
// if you remove this line you will have a Compile-Error "DrawTextA is not defined for wxPaintDC"
// .... waiting a better workaround. SIL 30/11/05 
#ifdef WIN32
#include <wx/msw/winundef.h> 
#endif
// end of hack

//----------------------------------------------------------------------------
// albaGUILutSwatch
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUILutSwatch, wxPanel)
EVT_PAINT(albaGUILutSwatch::OnPaint)
EVT_ERASE_BACKGROUND(albaGUILutSwatch::OnEraseBackground)

EVT_LEFT_DOWN(albaGUILutSwatch::OnLeftMouseButtonDown)
EVT_LEFT_UP(albaGUILutSwatch::OnLeftMouseButtonUp)
EVT_MOTION(albaGUILutSwatch::OnMouseMotion)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUILutSwatch::albaGUILutSwatch(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	:wxPanel(parent, id, pos, size, style)
	//----------------------------------------------------------------------------
{
	m_Listener = NULL;
	m_Editable = false;
	m_Enabled = false;
	SetLut(NULL);

	m_MouseInWindow = false;
	m_Tip = "";
	SetCursor(*wxCROSS_CURSOR);

	m_oldW = m_oldH = -1;

	m_Font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	m_Font.SetPointSize(9);
}
//----------------------------------------------------------------------------
void albaGUILutSwatch::OnPaint(wxPaintEvent &event)
//----------------------------------------------------------------------------
{
	wxPaintDC pdc(this);

	int w = GetSize().GetWidth();
	int h = GetSize().GetHeight();

	if ((m_Lut && m_Lut->GetMTime() > m_UpdateTime) || w != m_oldW || h != m_oldH)
	{
		Update();
		m_oldW = w;
		m_oldH = h;
	}

	wxMemoryDC mdc;
	mdc.SelectObject(m_Bmp);
	pdc.Blit(0, 0, w, h, &mdc, 0, 0);
	mdc.SelectObject(wxNullBitmap);

	if (m_Lut && m_MouseInWindow)
	{
		pdc.SetFont(m_Font);
		pdc.SetTextForeground(*wxBLACK);
		pdc.SetTextBackground(*wxWHITE);
		pdc.SetBackgroundMode(wxSOLID);

		wxCoord tw, th;
		pdc.GetTextExtent(m_Tip, &tw, &th);

		//******************  SIL 29/11/05 - to be reinserted
		if (m_MouseX < w / 2)
			pdc.DrawText(m_Tip, wxPoint(m_MouseX, 1));
		else
			pdc.DrawText(m_Tip, wxPoint(m_MouseX - tw, 1));
		//******************/
	}
}
//----------------------------------------------------------------------------
void albaGUILutSwatch::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
	if (m_Lut && m_Editable)
	{
		if (m_MouseInWindow)
		{
			if (GetCapture() == this) ReleaseMouse();
			m_MouseInWindow = false;
			Refresh();
		}

		albaGUILutEditor::ShowLutDialog(m_Lut, m_Listener, GetId());
		Update();
		Refresh();
	}

	//notify the user
	albaEventMacro(albaEvent(this, GetId()));
}
//----------------------------------------------------------------------------
void albaGUILutSwatch::OnLeftMouseButtonUp(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUILutSwatch::OnMouseMotion(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
	int x = event.m_x;
	int y = event.m_y;
	int w = GetSize().GetWidth();
	int h = GetSize().GetHeight();


	// this is to simulate the non-existing event "MouseLeave"
	if (!m_MouseInWindow)
	{
		CaptureMouse();   // Paolo 30/01/2006 
		m_MouseInWindow = true;
	}
	else
	{
		if (x<0 || x>w || y<0 || y>h)
		{
			if (GetCapture() == this) ReleaseMouse();  // Paolo 30/01/2006 
			m_MouseInWindow = false;
			Refresh();
		}
	}

	//alter the tooltip text according to mouse position
	if (m_Lut && m_MouseInWindow)
	{
		double *range = m_Lut->GetRange();
		double v = range[0] + ((range[1] - range[0]) * x) / w;
		m_Tip = albaString::Format(" value=%g ", v);
		m_MouseX = x;
		Refresh();
	}
}
//----------------------------------------------------------------------------
void albaGUILutSwatch::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
	m_Lut = lut;
	Update();
	Refresh();
}

//----------------------------------------------------------------------------
void albaGUILutSwatch::SetEditable(bool b)
{
	m_Editable = b;
}

//----------------------------------------------------------------------------
void albaGUILutSwatch::Update()
//----------------------------------------------------------------------------
{
	int w = GetSize().GetWidth();
	int h = GetSize().GetHeight();
	float num = 0;

	if (m_Lut)
		num = m_Lut->GetNumberOfTableValues();

	unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char)*w*h * 3);
	unsigned char *p = data;

	int x, y;
	for (y = 0; y<h; y++)
	{
		for (x = 0; x<w; x++)
		{
			float i = (num * x) / w;
			albaColor col = (m_Enabled && m_Lut) ? albaColor(m_Lut->GetTableValue(i)) : albaColor(225, 225, 225);
			albaColor col2 = albaColor::CheckeredColor(col, x, y);

			*p++ = col2.m_Red;
			*p++ = col2.m_Green;
			*p++ = col2.m_Blue;
		}
	}
	wxImage img(w, h, data); // data will be freed by the image
													 //m_Bmp = img.ConvertToBitmap(); // changed in passing from wx242 -> wx263
	m_Bmp = wxBitmap(img);

	if (m_Lut)
		m_UpdateTime = m_Lut->GetMTime();
}

//----------------------------------------------------------------------------
bool albaGUILutSwatch::Enable(bool enable /*= true*/)
{
	m_Enabled = enable;
	Update();
	Refresh();
	return wxPanel::Enable(enable);
}
