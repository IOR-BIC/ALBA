/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIHistogramWidget
 Authors: Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIHistogramWidget_H__
#define __albaGUIHistogramWidget_H__


//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIPanel.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "vtkALBAHistogram.h"

//----------------------------------------------------------------------------
// forward refs
//----------------------------------------------------------------------------
class albaRWI;
class vtkDataArray;
class albaGUI;
class vtkImageData;
class albaGUIRangeSlider;
class albaGUILutSlider;
class vtkLookupTable;

//----------------------------------------------------------------------------
/** albaGUIHistogramWidget : widget that encapsulate render window into a gui*/
class ALBA_EXPORT albaGUIHistogramWidget: public albaGUIPanel, public albaObserver
{
public:
  /** Constructor */
  albaGUIHistogramWidget(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, 
           const wxSize& size = wxSize(400,300), long style = wxTAB_TRAVERSAL /*| wxSUNKEN_BORDER */, bool showThresholds = false);
  /** Destructor. */
  virtual ~albaGUIHistogramWidget();

  /** Set the listener for the widget */
  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  
  /** Main Event handler */
  void OnEvent(albaEventBase *event);

  ALBA_ID_DEC(RANGE_MODIFIED);

  enum HISTOGRAM_WIDGET_ID
  {
    ID_AUTOSCALE = MINID,
    ID_SCALE_FACTOR,
    ID_LOGSCALE,
    ID_LOGFACTOR,
    ID_REPRESENTATION,
    ID_RESET,
    ID_RANGE_SLICER,
    ID_SLIDER_THRESHOLD,
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
  Possible values are: vtkALBAHistogram::POINT_REPRESENTATION, vtkALBAHistogram::LINE_REPRESENTATION or vtkALBAHistogram::BAR_REPRESENTATION*/
  void SetRepresentation(int represent);

  
  /** Enable/Disable threshold lines visualization */
  void ShowLines(int value=1);

  /** Enable/Disable the visualization of the number of samples in the bin under the mouse cursor */
  void ShowText(int value=1){m_ShowText=value;};
  
  /** Set the reference to the lut which range will be adjusted by the slider.
  The reference to the Lookup Table should be set before asking for the Histogram's Gui, so the slider to 
  change the lookup table's range will appear on the UI.*/
  void SetLut(vtkLookupTable *lut);

  /** Return a reference to the UI.*/
  albaGUI *GetGui();

  /** Get the threshold of the histogram if setted */
  void GetThresholds(double *lower, double *upper);

  /* Get the number of samples in the bin associated to the x coordinate */
  long int GetHistogramValue(int x, int y);

  /* Get the scalar vale of the histogram associated to the x coordinate */
  double GetHistogramScalarValue(int x, int y);
  
  /** Update position of the gizmo lines */
  void UpdateLines(int min,int max);
protected:

  /** Create GUI for histogram widget.*/
  void CreateGui();

  /** Update UI parameters according to the vtkALBAHistogram.*/
  void UpdateGui();

  /** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Reset to default histogram parameters.*/
  void ResetHistogram();

  albaObserver   *m_Listener;
  double         m_ScaleFactor;
  double         m_LogScaleConstant;
  int            m_HisctogramValue;
  int            m_NumberOfBins;
  int            m_Representation;
  int            m_AutoscaleHistogram;
  int            m_LogHistogramFlag;
  bool           m_Dragging;
  int            m_DragStart;
  double         m_LowerThreshold;
  double         m_UpperThreshold;
  int            m_ShowText;

  albaGUI        *m_Gui;

  albaGUIRangeSlider *m_Slider;
  albaGUILutSlider *m_SliderThresholds;
  double m_SelectedRange[2];
  
  vtkLookupTable*m_Lut;
  vtkDataArray  *m_Data;
  albaRWI        *m_HistogramRWI;
  vtkALBAHistogram  *m_Histogram;
	vtkImageData  *m_HistogramData;
};
#endif
