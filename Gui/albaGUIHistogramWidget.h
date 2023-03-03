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
		ID_LOGSCALE = MINID,
		ID_NUMBER_OF_BIN,
    ID_RANGE_SLICER,
    ID_SLIDER_THRESHOLD,
		ID_EXPORT_DATA,
  };

	/** Set a pre-calculated histogram, just visualize it instead calculate. It is used like a cache.*/
	void SetHistogramData(vtkImageData *histogram);

  /** Input data from which generate histogram.*/
  void SetData(vtkDataArray *data);

  /** Enable/disable logarithmic scale for histogram.*/
  void LogarithmicScale(int enable = 1);

  /** Turn On log scale.*/
  void LogarithmicScaleOn() {LogarithmicScale();};

  /** Turn Off log scale.*/
  void LogarithmicScaleOff() {LogarithmicScale(0);};

  /** Return the status of log scale flag.*/
  bool IsLogarithmicScale() {return m_LogHistogramFlag != 0;};

    
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

  albaObserver   *m_Listener;
  int            m_HisctogramValue;
  int            m_NumberOfBins;
  int            m_LogHistogramFlag;
  double         m_Threshold[2];
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
private:
	void ExportData();
};
#endif
