/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgRangeSlider.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
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


#include "wx/event.h"
#include "math.h"
#include "mmgRangeSlider.h"

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))
template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }


BEGIN_EVENT_TABLE(mmgRangeSlider, wxControl)
  EVT_LEFT_DOWN(mmgRangeSlider::OnMouse)	
  EVT_LEFT_UP(mmgRangeSlider::OnMouse)	
  EVT_MOTION(mmgRangeSlider::OnMouse)
  EVT_PAINT(mmgRangeSlider::OnPaint)
END_EVENT_TABLE()

int     mmgRangeSlider::m_TriangleWidgetRadius = 5;
wxPoint mmgRangeSlider::m_TriangleWidgetCenter(0, 5);
wxPoint mmgRangeSlider::m_TriangleWidgetPoints[3] = { wxPoint(0, 0), wxPoint(5, 10), wxPoint(-5, 10) };

//----------------------------------------------------------------------------
mmgRangeSlider::mmgRangeSlider(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxControl(parent, id, pos, size, style) 
//----------------------------------------------------------------------------
{ 
  this->Initialize();
}
//----------------------------------------------------------------------------
void mmgRangeSlider::SetValue(int i, float value) 
//----------------------------------------------------------------------------
{
  assert(i >= 0 && i < 3);
  if (this->m_Value[i] == value)
    return;

  this->m_Value[i] = value;
  if (i == 0 && this->m_Value[0] > this->m_Value[1])
    this->m_Value[1] = this->m_Value[0];
  if (i == 0 && this->m_Value[0] > this->m_Value[2])
    this->m_Value[2] = this->m_Value[0];

  if (i == 1 && this->m_Value[1] < this->m_Value[0])
    this->m_Value[0] = this->m_Value[1];
  if (i == 1 && this->m_Value[1] > this->m_Value[2])
    this->m_Value[2] = this->m_Value[1];

  if (i == 2 && this->m_Value[2] < this->m_Value[0])
    this->m_Value[0] = this->m_Value[2];
  if (i == 2 && this->m_Value[2] < this->m_Value[1])
    this->m_Value[1] = this->m_Value[2];

  if (this->m_Value[i] < this->m_Range[0])
    this->m_Range[0] = this->m_Value[i];

  if (this->m_Value[i] > this->m_Range[1])
    this->m_Range[1] = this->m_Value[i];
  this->Refresh();
}
//----------------------------------------------------------------------------
void mmgRangeSlider::SetRange(float *range) 
//----------------------------------------------------------------------------
{
  this->m_Range[0] = range[0];
  this->m_Range[1] = range[1];

  if (this->m_Value[0] < this->m_Range[0])
    this->m_Range[0] = this->m_Value[0];

  if (this->m_Value[2] > this->m_Range[1])
    this->m_Range[1] = this->m_Value[2];

  this->Refresh();
}
//----------------------------------------------------------------------------
void mmgRangeSlider::Initialize() 
//----------------------------------------------------------------------------
{
  this->m_NumberOfWidgets = 3;

  this->m_Range[0] = 0.f;
  this->m_Range[1] = 1.f;
  
  this->m_Value[0] = 0.0f;
  this->m_Value[1] = 0.5f;
  this->m_Value[2] = 1.0f;

  this->m_SelectedTriangleWidget = this->m_SelectedRangeWidget = -1;
}
//----------------------------------------------------------------------------
void mmgRangeSlider::OnPaint(wxPaintEvent &event) 
//----------------------------------------------------------------------------
{
  wxPaintDC dc(this);
  PrepareDC(dc);
  dc.BeginDrawing();
  dc.Clear();
  const wxSize size = this->GetClientSize();

#define BORDER_WIDGET_RADIUS  6

  const int lineWidth  = size.x - 10 - (4 * BORDER_WIDGET_RADIUS);
  const int lineX = (size.x - lineWidth) >> 1, lineY = 5 + BORDER_WIDGET_RADIUS;
  // draw line
  wxPen darkGreyPen(wxColour(64, 64, 64), 1, wxSOLID);
  dc.SetPen(*wxGREY_PEN);
  dc.DrawLine(lineX, lineY - 2, lineX + lineWidth, lineY - 2);
  dc.DrawLine(lineX, lineY - 1, lineX, lineY);
  dc.SetPen(darkGreyPen);
  dc.DrawLine(lineX + 1, lineY - 1, lineX + lineWidth - 2, lineY - 1);
  dc.SetPen(*wxWHITE_PEN);
  dc.DrawLine(lineX, lineY + 1, lineX + lineWidth, lineY + 1);
  dc.DrawLine(lineX + lineWidth, lineY - 1, lineX + lineWidth, lineY);
  this->m_LineStartX = lineX;
  this->m_LineStartY = lineY + 1;
  this->m_LineWidth = lineWidth;

  // draw borders
  dc.SetPen(*wxGREY_PEN);
  dc.SetBrush(*wxWHITE_BRUSH);
  dc.DrawCircle(lineX - BORDER_WIDGET_RADIUS, lineY, BORDER_WIDGET_RADIUS);
  dc.DrawCircle(lineX + lineWidth + BORDER_WIDGET_RADIUS, lineY, BORDER_WIDGET_RADIUS);

  this->RedrawWidgets(dc);
  dc.EndDrawing();
}
//----------------------------------------------------------------------------
void mmgRangeSlider::RedrawWidgets(wxDC &dc, bool eraseWidgets) 
//----------------------------------------------------------------------------
{
  wxPen darkGreyPen(wxColour(64, 64, 64), 1, wxSOLID);
  wxPen bkgPen(dc.GetBackground().GetColour(), 1, wxSOLID);
  // draw triangles
  float range = this->m_Range[1] - this->m_Range[0];
  dc.SetPen(eraseWidgets ? bkgPen : *wxGREY_PEN);
  for (int i = 0; i < 3; i++) 
	{
    if (this->m_NumberOfWidgets == 1 && i > 0 || this->m_NumberOfWidgets == 2 && i == 1)
      continue;

    int positionX = this->m_LineStartX + int(0.5f + this->m_LineWidth / range * (this->m_Value[i] - this->m_Range[0]));
    int positionY = this->m_LineStartY + 1;
    this->m_TrianglePosition[i].x = positionX;
    this->m_TrianglePosition[i].y = positionY;
    
    if (eraseWidgets) 
		{
      dc.SetBrush(dc.GetBackground());
      dc.DrawPolygon(3, m_TriangleWidgetPoints, positionX, positionY);
    }
    else 
		{
      dc.SetBrush(i == 1 ? *wxWHITE_BRUSH : *wxLIGHT_GREY_BRUSH);
      dc.DrawPolygon(3, m_TriangleWidgetPoints, positionX, positionY);

      dc.SetPen(darkGreyPen);
      dc.DrawLine(positionX + m_TriangleWidgetPoints[0].x, positionY + m_TriangleWidgetPoints[0].y, positionX + m_TriangleWidgetPoints[1].x, positionY + m_TriangleWidgetPoints[1].y);
      dc.DrawLine(positionX + m_TriangleWidgetPoints[1].x, positionY + m_TriangleWidgetPoints[1].y, positionX + m_TriangleWidgetPoints[2].x, positionY + m_TriangleWidgetPoints[2].y);
      dc.SetPen(*wxWHITE_PEN);
      dc.DrawLine(positionX + m_TriangleWidgetPoints[0].x, positionY + m_TriangleWidgetPoints[0].y, positionX + m_TriangleWidgetPoints[2].x, positionY + m_TriangleWidgetPoints[2].y);
    }
  }
}
//----------------------------------------------------------------------------
void mmgRangeSlider::OnMouse(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  const int x = event.GetX(), y = event.GetY();
  int i =0;
	
  if (event.Dragging()) 
	{
    if (this->m_SelectedTriangleWidget != -1) 
		{
      // change value
      float dx = float(x - this->m_PrevMousePosition.x) / this->m_LineWidth;
      float newVal = this->m_Value[this->m_SelectedTriangleWidget] + dx * (this->m_Range[1] - this->m_Range[0]);
      // clipping
      if (this->m_NumberOfWidgets > 1) 
			{
        if (this->m_SelectedTriangleWidget == 0)
          newVal = clip(newVal, this->m_Range[0], this->m_NumberOfWidgets != 2 ? this->m_Value[1] : this->m_Value[2]);
        else if (this->m_SelectedTriangleWidget == 1)
          newVal = clip(newVal, this->m_Value[0], this->m_Value[2]);
        else
          newVal = clip(newVal, this->m_NumberOfWidgets != 2 ? this->m_Value[1] : this->m_Value[0], this->m_Range[1]);
      }

      if (this->m_Value[this->m_SelectedTriangleWidget] == newVal)
        return; // the value did not change

      wxClientDC dc(this);
      PrepareDC(dc);
      dc.BeginDrawing();

      this->RedrawWidgets(dc, true);
      this->m_Value[this->m_SelectedTriangleWidget] = newVal;

      this->RedrawWidgets(dc);
      dc.EndDrawing();
      
      // send event
      wxCommandEvent event(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
    else if (this->m_SelectedRangeWidget != -1) 
		{
      float dx = float(x - this->m_PrevMousePosition.x) / this->m_LineWidth;
      float newrange = this->m_Range[this->m_SelectedRangeWidget] + (this->m_Range[1] - this->m_Range[0]) * dx;
      if (this->m_SelectedRangeWidget == 0 && newrange > this->m_Value[0])
        newrange = this->m_Value[0];
      else if (this->m_SelectedRangeWidget == 1 && newrange < this->m_Value[2])
        newrange = this->m_Value[2];
      if (newrange == this->m_Range[this->m_SelectedRangeWidget])
        return;
      this->m_Range[this->m_SelectedRangeWidget] = newrange;

      this->Refresh();
    }
    
    this->m_PrevMousePosition.x = x;
    this->m_PrevMousePosition.y = y;
  }
  else if (event.LeftDown()) 
	{                                  // click
    this->m_PrevMousePosition.x = x;
    this->m_PrevMousePosition.y = y;

    // is it one of the triangles
    this->m_SelectedTriangleWidget = -1;
    this->m_SelectedRangeWidget = -1;
    int closestDistance = 99;

    for (i = 0; i < 3; i++) 
		{
      if (this->m_NumberOfWidgets == 1 && i > 0 || this->m_NumberOfWidgets == 2 && i == 1)
        continue;
      //SIL 16/03/04 changed dx,dy from int to float -- to disambiguate the call to sqrt
      float dx = this->m_TrianglePosition[i].x + m_TriangleWidgetCenter.x - x;
      float dy = this->m_TrianglePosition[i].y + m_TriangleWidgetCenter.y - y;
      float distance = sqrt(dx * dx + dy * dy);
      if (distance < m_TriangleWidgetRadius) 
			{
        if (distance < (0.5f * closestDistance) ||
            distance < (1.5f * closestDistance) && (i == 1 && (this->m_Value[1] - this->m_Range[0]) < 0.05 * (this->m_Range[1] - this->m_Range[0])) ||
            distance < (1.5f * closestDistance) && (i == 2 && (this->m_Value[2] - this->m_Range[0]) < 0.05 * (this->m_Range[1] - this->m_Range[0]))) 
				{
          closestDistance = distance;
          this->m_SelectedTriangleWidget = i;
        }
      }
    }
    if (this->m_SelectedTriangleWidget != -1)
      return;

    // is the border widget
    for (i = 0; i < 2; i++) 
		{
      //SIL 16/03/04 changed dx,dy from int to float -- to disambiguate the call to sqrt
      float dx = this->m_LineStartX - x + (i ? (this->m_LineWidth + BORDER_WIDGET_RADIUS) : -BORDER_WIDGET_RADIUS);
      float dy = this->m_LineStartY - y;
      int distance = int(sqrt(dx * dx + dy * dy) + 0.5f);
      if (distance < BORDER_WIDGET_RADIUS)
        this->m_SelectedRangeWidget = i;
    }
    if (this->m_SelectedRangeWidget != -1)
      this->CaptureMouse();
  }
  else if (event.LeftUp()) 
	{                              // click
    if (this->m_SelectedTriangleWidget != -1) 
		{
      // send event
			wxCommandEvent c = wxCommandEvent(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
      GetEventHandler()->ProcessEvent(c);
      this->m_SelectedTriangleWidget = -1;
      this->Refresh();
    }
    else if (this->m_SelectedRangeWidget != -1) 
		{
      // no events
      this->ReleaseMouse();
      this->m_SelectedRangeWidget = -1;
      this->Refresh();
    }
  }
}
