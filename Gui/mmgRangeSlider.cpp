/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgRangeSlider.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:26 $
  Version:   $Revision: 1.2 $
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

int     mmgRangeSlider::TriangleWidgetRadius = 5;
wxPoint mmgRangeSlider::TriangleWidgetCenter(0, 5);
wxPoint mmgRangeSlider::TriangleWidgetPoints[3] = { wxPoint(0, 0), wxPoint(5, 10), wxPoint(-5, 10) };

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
  if (this->Value[i] == value)
    return;

  this->Value[i] = value;
  if (i == 0 && this->Value[0] > this->Value[1])
    this->Value[1] = this->Value[0];
  if (i == 0 && this->Value[0] > this->Value[2])
    this->Value[2] = this->Value[0];

  if (i == 1 && this->Value[1] < this->Value[0])
    this->Value[0] = this->Value[1];
  if (i == 1 && this->Value[1] > this->Value[2])
    this->Value[2] = this->Value[1];

  if (i == 2 && this->Value[2] < this->Value[0])
    this->Value[0] = this->Value[2];
  if (i == 2 && this->Value[2] < this->Value[1])
    this->Value[1] = this->Value[2];

  if (this->Value[i] < this->Range[0])
    this->Range[0] = this->Value[i];

  if (this->Value[i] > this->Range[1])
    this->Range[1] = this->Value[i];
  this->Refresh();
}
//----------------------------------------------------------------------------
void mmgRangeSlider::SetRange(float *range) 
//----------------------------------------------------------------------------
{
  this->Range[0] = range[0];
  this->Range[1] = range[1];

  if (this->Value[0] < this->Range[0])
    this->Range[0] = this->Value[0];

  if (this->Value[2] > this->Range[1])
    this->Range[1] = this->Value[2];

  this->Refresh();
}
//----------------------------------------------------------------------------
void mmgRangeSlider::Initialize() 
//----------------------------------------------------------------------------
{
  this->NumberOfWidgets = 3;

  this->Range[0] = 0.f;
  this->Range[1] = 1.f;
  
  this->Value[0] = 0.0f;
  this->Value[1] = 0.5f;
  this->Value[2] = 1.0f;

  this->SelectedTriangleWidget = this->SelectedRangeWidget = -1;
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
  this->LineStartX = lineX;
  this->LineStartY = lineY + 1;
  this->LineWidth = lineWidth;

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
  float range = this->Range[1] - this->Range[0];
  dc.SetPen(eraseWidgets ? bkgPen : *wxGREY_PEN);
  for (int i = 0; i < 3; i++) 
	{
    if (this->NumberOfWidgets == 1 && i > 0 || this->NumberOfWidgets == 2 && i == 1)
      continue;

    int positionX = this->LineStartX + int(0.5f + this->LineWidth / range * (this->Value[i] - this->Range[0]));
    int positionY = this->LineStartY + 1;
    this->TrianglePosition[i].x = positionX;
    this->TrianglePosition[i].y = positionY;
    
    if (eraseWidgets) 
		{
      dc.SetBrush(dc.GetBackground());
      dc.DrawPolygon(3, TriangleWidgetPoints, positionX, positionY);
    }
    else 
		{
      dc.SetBrush(i == 1 ? *wxWHITE_BRUSH : *wxLIGHT_GREY_BRUSH);
      dc.DrawPolygon(3, TriangleWidgetPoints, positionX, positionY);

      dc.SetPen(darkGreyPen);
      dc.DrawLine(positionX + TriangleWidgetPoints[0].x, positionY + TriangleWidgetPoints[0].y, positionX + TriangleWidgetPoints[1].x, positionY + TriangleWidgetPoints[1].y);
      dc.DrawLine(positionX + TriangleWidgetPoints[1].x, positionY + TriangleWidgetPoints[1].y, positionX + TriangleWidgetPoints[2].x, positionY + TriangleWidgetPoints[2].y);
      dc.SetPen(*wxWHITE_PEN);
      dc.DrawLine(positionX + TriangleWidgetPoints[0].x, positionY + TriangleWidgetPoints[0].y, positionX + TriangleWidgetPoints[2].x, positionY + TriangleWidgetPoints[2].y);
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
    if (this->SelectedTriangleWidget != -1) 
		{
      // change value
      float dx = float(x - this->PrevMousePosition.x) / this->LineWidth;
      float newVal = this->Value[this->SelectedTriangleWidget] + dx * (this->Range[1] - this->Range[0]);
      // clipping
      if (this->NumberOfWidgets > 1) 
			{
        if (this->SelectedTriangleWidget == 0)
          newVal = clip(newVal, this->Range[0], this->NumberOfWidgets != 2 ? this->Value[1] : this->Value[2]);
        else if (this->SelectedTriangleWidget == 1)
          newVal = clip(newVal, this->Value[0], this->Value[2]);
        else
          newVal = clip(newVal, this->NumberOfWidgets != 2 ? this->Value[1] : this->Value[0], this->Range[1]);
      }

      if (this->Value[this->SelectedTriangleWidget] == newVal)
        return; // the value did not change

      wxClientDC dc(this);
      PrepareDC(dc);
      dc.BeginDrawing();

      this->RedrawWidgets(dc, true);
      this->Value[this->SelectedTriangleWidget] = newVal;

      this->RedrawWidgets(dc);
      dc.EndDrawing();
      
      // send event
      wxCommandEvent event(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
    else if (this->SelectedRangeWidget != -1) 
		{
      float dx = float(x - this->PrevMousePosition.x) / this->LineWidth;
      float newrange = this->Range[this->SelectedRangeWidget] + (this->Range[1] - this->Range[0]) * dx;
      if (this->SelectedRangeWidget == 0 && newrange > this->Value[0])
        newrange = this->Value[0];
      else if (this->SelectedRangeWidget == 1 && newrange < this->Value[2])
        newrange = this->Value[2];
      if (newrange == this->Range[this->SelectedRangeWidget])
        return;
      this->Range[this->SelectedRangeWidget] = newrange;

      this->Refresh();
    }
    
    this->PrevMousePosition.x = x;
    this->PrevMousePosition.y = y;
  }
  else if (event.LeftDown()) 
	{                                  // click
    this->PrevMousePosition.x = x;
    this->PrevMousePosition.y = y;

    // is it one of the triangles
    this->SelectedTriangleWidget = -1;
    this->SelectedRangeWidget = -1;
    int closestDistance = 99;

    for (i = 0; i < 3; i++) 
		{
      if (this->NumberOfWidgets == 1 && i > 0 || this->NumberOfWidgets == 2 && i == 1)
        continue;
      //SIL 16/03/04 changed dx,dy from int to float -- to disambiguate the call to sqrt
      float dx = this->TrianglePosition[i].x + TriangleWidgetCenter.x - x;
      float dy = this->TrianglePosition[i].y + TriangleWidgetCenter.y - y;
      float distance = sqrt(dx * dx + dy * dy);
      if (distance < TriangleWidgetRadius) 
			{
        if (distance < (0.5f * closestDistance) ||
            distance < (1.5f * closestDistance) && (i == 1 && (this->Value[1] - this->Range[0]) < 0.05 * (this->Range[1] - this->Range[0])) ||
            distance < (1.5f * closestDistance) && (i == 2 && (this->Value[2] - this->Range[0]) < 0.05 * (this->Range[1] - this->Range[0]))) 
				{
          closestDistance = distance;
          this->SelectedTriangleWidget = i;
        }
      }
    }
    if (this->SelectedTriangleWidget != -1)
      return;

    // is the border widget
    for (i = 0; i < 2; i++) 
		{
      //SIL 16/03/04 changed dx,dy from int to float -- to disambiguate the call to sqrt
      float dx = this->LineStartX - x + (i ? (this->LineWidth + BORDER_WIDGET_RADIUS) : -BORDER_WIDGET_RADIUS);
      float dy = this->LineStartY - y;
      int distance = int(sqrt(dx * dx + dy * dy) + 0.5f);
      if (distance < BORDER_WIDGET_RADIUS)
        this->SelectedRangeWidget = i;
    }
    if (this->SelectedRangeWidget != -1)
      this->CaptureMouse();
  }
  else if (event.LeftUp()) 
	{                              // click
    if (this->SelectedTriangleWidget != -1) 
		{
      // send event
			wxCommandEvent c = wxCommandEvent(wxEVT_COMMAND_SLIDER_UPDATED, this->GetId());
      GetEventHandler()->ProcessEvent(c);
      this->SelectedTriangleWidget = -1;
      this->Refresh();
    }
    else if (this->SelectedRangeWidget != -1) 
		{
      // no events
      this->ReleaseMouse();
      this->SelectedRangeWidget = -1;
      this->Refresh();
    }
  }
}
