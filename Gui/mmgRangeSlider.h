/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgRangeSlider.h,v $
  Language:  C++
  Date:      $Date: 2008-05-27 12:49:50 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgRangeSlider_H__
#define __mmgRangeSlider_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"

//----------------------------------------------------------------------------
// mmgRangeSlider :
//----------------------------------------------------------------------------
class mmgRangeSlider : public wxControl 
{
public:
  mmgRangeSlider(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr);

  void SetListener(mafObserver *listener) {m_Listener = listener;}

  /** Get the 'min', 'max' or 'mid' value according to 'i'. */
	double GetValue(int i) const { return this->m_Value[i]; }

  /** Set the 'min', 'max' or 'mid' value according to 'i'. */
  void SetValue(int i, double value);

  /** Get the range of the slider. */
	const double *GetRange() const { return this->m_Range; }

  /** Set the range for the slider. */
  void SetRange(double *range);

  /** Set the range for the slider. */
  void SetRange(double minR, double maxR) { double range[2] = {minR, maxR}; this->SetRange(range); }

  /** Set the number of widgets for the slider. */
  void SetNumberOfWidgets(int num) { if (num == this->m_NumberOfWidgets || num < 1 || num > 3) return; this->m_NumberOfWidgets = num; this->Refresh(); }
  
	/** Enable/Disable the center cursor for the slider. */
  void EnableCenterWidget(bool enable) { this->m_NumberOfWidgets = (enable) ? 3 : 2; this->Refresh(); }

  /** Hide / show the circles on the boundary of the slider.*/
  void EnableBoundaryHandles(bool enable) {this->m_BorderWidgetRadius = (enable) ? 3 : 0; this->Refresh();}
  
	/** Return true if one of the cursors are moving. */
  bool IsDragging() const { return this->m_SelectedTriangleWidget != -1 || this->m_SelectedRangeWidget != -1; }

protected:
	/** Initialize the values for the slider. */
  void Initialize();
	
	/** Refresh the slider shape. */
  void OnPaint(wxPaintEvent &event);
	
	/** Move the cursors on mouse event. */
  void OnMouse(wxMouseEvent &event);

  /** Intercept slider events and forward to the listener.*/
  void OnRangeModified(wxCommandEvent &event);
	
	/** Redraw the widgets in the new position. */
  void RedrawWidgets(wxDC &dc, bool eraseWidgets = false);

  double m_Range[2];
  double m_Value[3];

  int m_NumberOfWidgets;
  mafObserver *m_Listener;

private:
  // ui coordinates
  int m_LineStartX;
  int m_LineStartY;
  int m_LineWidth;
  int m_BorderWidgetRadius;
  wxPoint m_TrianglePosition[3];

  static wxPoint m_TriangleWidgetPoints[3];
  static wxPoint m_TriangleWidgetCenter;
  static int     m_TriangleWidgetRadius;

  wxPoint m_PrevMousePosition;
  int     m_SelectedTriangleWidget;
  int     m_SelectedRangeWidget;
  
DECLARE_EVENT_TABLE()
};
#endif // _mmgRangeSlider_H_
