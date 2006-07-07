/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgHistogramWidget.h,v $
  Language:  C++
  Date:      $Date: 2006-07-07 13:17:26 $
  Version:   $Revision: 1.9 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgHistogramWidget_H__
#define __mmgHistogramWidget_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgPanel.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "vtkHistogram.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;
class vtkDataArray;
class mmgGui;

//----------------------------------------------------------------------------
/** mmgHistogramWidget : widget that encapsulate render window into a gui*/
class mmgHistogramWidget: public mmgPanel, public mafObserver
{
public:
  mmgHistogramWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(400,300), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */);
  virtual ~mmgHistogramWidget();

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent( mafEventBase *event );

  enum HISTOGRAM_WIDGET_ID
  {
    ID_AUTOSCALE = MINID,
    ID_SCALE_FACTOR,
    ID_LOGSCALE,
    ID_LOGFACTOR,
    ID_REPRESENTATION
  };

  /** Input data from which generate histogram.*/
  void SetData(vtkDataArray *data);

  /** Set the scale factor for the histogram.*/
  void SetScaleFactor(double factor);

  /** Return scale factor from the widget.*/
  double GetScaleFactor() {return m_ScaleFactor;};

  /** Enable/disable logarithmic scale for histogram.*/
  void LogarithmicScale(int enable = 1);

  /** Turn On log scale.*/
  void LogarithmicScaleOn() {LogarithmicScale();};

  /** Turn Off log scale.*/
  void LogarithmicScaleOff() {LogarithmicScale(0);};

  /** Return the status of log scale flag.*/
  bool IsLogarithmicScale() {return m_LogHistogramFlag != 0;};

  /** Enable/Disable auto-scaling for drawing histogram.*/
  void AutoscaleHistogram(int autoscale = 1);

  /** Enable auto-scaling for drawing histogram.*/
  void AutoscaleHistogramOn() {AutoscaleHistogram();};

  /** Disable auto-scaling for drawing histogram.*/
  void AutoscaleHistogramOff() {AutoscaleHistogram(0);};

  /** Return the status of autoscale flag.*/
  bool IsAutoScaled() {return m_AutoscaleHistogram != 0;};

  /** Set the multiplicative constant for logarithmic scale histogram.*/
  void SetLogScaleConstant(double c);

  /** Return the value of the multiplicative constant for logarithmic scale histogram.*/
  double GetLogScaleConstant() {return m_LogScaleConstant;};

  /** Allow to set the histogram representation.
  Possible values are: vtkHistogram::POINT_REPRESENTATION, vtkHistogram::LINE_REPRESENTATION or vtkHistogram::BAR_REPRESENTATION*/
  void SetRepresentation(int represent);

  mmgGui *GetGui() {return m_Gui;};

protected:
  /** Create GUI for histogram widget.*/
  void CreateGui();

  /** Update gui parameters according to the vtkHistogram.*/
  void UpdateGui();

  /** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  mafObserver   *m_Listener;
  double         m_ScaleFactor;
  double         m_LogScaleConstant;
  int            m_HisctogramValue;
  int            m_NumberOfBins;
  int            m_Representation;
  int            m_AutoscaleHistogram;
  int            m_LogHistogramFlag;
  bool           m_Dragging;
  int            m_DragStart;

  mmgGui        *m_Gui;

  vtkDataArray  *m_Data;
  mafRWI        *m_HistogramRWI;
  vtkHistogram  *m_Histogram;
};
#endif
