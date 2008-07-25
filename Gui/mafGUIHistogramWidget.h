/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIHistogramWidget.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIHistogramWidget_H__
#define __mafGUIHistogramWidget_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUIPanel.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "vtkMAFHistogram.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class mafRWI;
class vtkDataArray;
class mafGUI;
class vtkImageData;
class mafGUIRangeSlider;
class vtkLookupTable;

//----------------------------------------------------------------------------
/** mafGUIHistogramWidget : widget that encapsulate render window into a gui*/
class mafGUIHistogramWidget: public mafGUIPanel, public mafObserver
{
public:
  mafGUIHistogramWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(400,300), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */);
  virtual ~mafGUIHistogramWidget();

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
  void OnEvent(mafEventBase *event);

  MAF_ID_DEC(RANGE_MODIFIED);

  enum HISTOGRAM_WIDGET_ID
  {
    ID_AUTOSCALE = MINID,
    ID_SCALE_FACTOR,
    ID_LOGSCALE,
    ID_LOGFACTOR,
    ID_REPRESENTATION,
    ID_RESET,
    ID_RANGE_SLICER,
  };

	/** Set a pre-calculated histogram, just visualize it instead calculate. It is used like a cache.*/
	void SetHistogramData(vtkImageData *histogram);

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
  Possible values are: vtkMAFHistogram::POINT_REPRESENTATION, vtkMAFHistogram::LINE_REPRESENTATION or vtkMAFHistogram::BAR_REPRESENTATION*/
  void SetRepresentation(int represent);

  void GetSelectedRange(double range[2]);

  /** Set the reference to the lut which range will be adjusted by the slider.
  The reference to the Lookup Table should be set before asking for the Histogram's Gui, so the slider to 
  change the lookup table's range will appear on the UI.*/
  void SetLut(vtkLookupTable *lut) {m_Lut = lut;};

  /** Return a reference to the UI.*/
  mafGUI *GetGui();

protected:
  /** Create GUI for histogram widget.*/
  void CreateGui();

  /** Update UI parameters according to the vtkMAFHistogram.*/
  void UpdateGui();

  /** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Reset to default histogram parameters.*/
  void ResetHistogram();

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

  mafGUI        *m_Gui;

  mafGUIRangeSlider *m_Slider;
  double m_SelectedRange[2];
  
  vtkLookupTable*m_Lut;
  vtkDataArray  *m_Data;
  mafRWI        *m_HistogramRWI;
  vtkMAFHistogram  *m_Histogram;
	vtkImageData  *m_HistogramData;
};
#endif
