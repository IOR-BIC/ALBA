/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgHistogramWidget.h,v $
  Language:  C++
  Date:      $Date: 2006-03-07 09:16:11 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgHistogramWidget_H__
#define __mmgHistogramWidget_H__

#include "mafDecl.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;
class vtkActor2D;
class vtkImageAccumulate;
class vtkGlyph3D;
class vtkImageData;
class vtkImageChangeInformation;
class vtkTextMapper;
class vtkActor2D;
class vtkImageLogarithmicScale;

//----------------------------------------------------------------------------
/** mmgHistogramWidget : widget that encapsulate render window into a gui*/
class mmgHistogramWidget: public wxPanel, public mafObserver
{
public:
  mmgHistogramWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(400,300), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */);
  virtual ~mmgHistogramWidget();

  MAF_ID_DEC(HISTOGRAM_UPDATED);

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent( mafEventBase *event );

  /** Update Histogram pipeline and send to the listener an update UI event.*/
  void UpdateHistogram();

  /** Input data from which generate histogram.*/
  void SetData(vtkImageData *data);

  /** Set the scale factor for the histogram.*/
  void SetScaleFactor(double factor);

  /** Return scale factor from the widget.*/
  double GetScaleFactor() {return m_ScaleFactor;};

  /** Enable/disable logarithmic scale for histogram.*/
  void LogarithmicScale(bool enable = true);

  /** Turn On log scale.*/
  void LogarithmicScaleOn() {LogarithmicScale();};

  /** Turn Off log scale.*/
  void LogarithmicScaleOff() {LogarithmicScale(false);};

  /** Return the status of log scale flag.*/
  bool IsLogarithmicScale() {return m_LogHistogramFlag;};

  /** Enable/Disable auto-scaling for drawing histogram.*/
  void AutoscaleHistogram(bool autoscale = true);

  /** Enable auto-scaling for drawing histogram.*/
  void AutoscaleHistogramOn() {AutoscaleHistogram();};

  /** Disable auto-scaling for drawing histogram.*/
  void AutoscaleHistogramOff() {AutoscaleHistogram(false);};

  /** Return the status of autoscale flag.*/
  bool IsAutoScaled() {return m_AutoscaleHistogram;};

  /** Set the multiplicative constant for logarithmic scale histogram.*/
  void SetLogScaleConstant(double c);

  /** Return the value of the multiplicative constant for logarithmic scale histogram.*/
  double GetLogScaleConstant() {return m_LogScaleConstant;};

protected:
  mafObserver   *m_Listener;
  double         m_ScaleFactor;
  double         m_LogScaleConstant;
  int            m_HisctogramValue;
  int            m_NumberOfBins;
  bool           m_AutoscaleHistogram;
  bool           m_LogHistogramFlag;
  bool           m_Dragging;
  int            m_DragStart;

  vtkTextMapper	*m_TextMapper;
  vtkActor2D    *m_TextActor;

  vtkImageData  *m_Data;
  vtkImageAccumulate *m_Accumulate;
  vtkImageChangeInformation *m_ChangeInfo;
  vtkImageLogarithmicScale  *m_LogScale;
  vtkGlyph3D      *m_Glyph;
  mafRWI          *m_HistogramRWI;
  vtkActor2D      *m_PlotActor;
};
#endif
