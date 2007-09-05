/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgRangeSlider.h,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:03 $
  Version:   $Revision: 1.3 $
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

//----------------------------------------------------------------------------
// mmgRangeSlider :
//----------------------------------------------------------------------------
class mmgRangeSlider : public wxControl {
public:
  mmgRangeSlider(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr);

  /** Get the 'min', 'max' or 'mid' value according to 'i'. */
	float GetValue(int i) const { return this->m_Value[i]; }

  /** Set the 'min', 'max' or 'mid' value according to 'i'. */
  void SetValue(int i, float value);

  /** Get the range of the slider. */
	const float *GetRange() const { return this->m_Range; }

  /** Set the range for the slider. */
  void SetRange(float *range);

  /** Set the range for the slider. */
  void SetRange(float minR, float maxR) { float range[2] = {minR, maxR}; this->SetRange(range); }

  /** Set the number of widgets for the slider. */
  void SetNumberOfWidgets(int num) { if (num == this->m_NumberOfWidgets || num < 1 || num > 3) return; this->m_NumberOfWidgets = num; this->Refresh(); }
  
	/** Enable/Disable the center cursor for the slider. */
  void EnableCenterWidget(bool enable) { this->m_NumberOfWidgets = (enable) ? 3 : 2; this->Refresh(); }
  
	/** Return true if one of the cursors are moveing. */
  bool IsDragging() const { return this->m_SelectedTriangleWidget != -1 || this->m_SelectedRangeWidget != -1; }

protected:
	/** Initialize the values for the slider. */
  void Initialize();
	
	/** Refresh the slider shape. */
  void OnPaint(wxPaintEvent &event);
	
	/** Move the cursors on mouse event. */
  void OnMouse(wxMouseEvent &event);
	
	/** Redraw the widgets in the new position. */
  void RedrawWidgets(wxDC &dc, bool eraseWidgets = false);

  float m_Range[2];
  float m_Value[3];

  int   m_NumberOfWidgets;

private:
  // ui coordinates
  int m_LineStartX;
  int m_LineStartY;
  int m_LineWidth;
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
