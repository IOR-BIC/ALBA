/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutHistogramEditor
 Authors: Crimi Gianluigi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILutHistogramEditor_H__
#define __albaGUILutHistogramEditor_H__

#include "albaDefines.h"
#include "albaEvent.h"
#include "albaLUTLibrary.h"

#include "albaGUILutWidget.h"
#include "albaGUILutHistogramSwatch.h"
#include "albaGUIColorWidget.h"
#include "albaGUIDialog.h"

#include "vtkLookupTable.h"
/** medGUIHistogramLutEditor : vtkLookupTable Histogram Based Editor

  USAGE:
  - call the static member albaGUILutHistogramEditor::ShowLutHistogramDialog(albaVME *vme, vtkLookupTable *lut,char *name, albaObserver *listener, int id)

  
@sa medGUILutSwatch albaGUILutHistogramSwatch
*/
class vtkDataSet;
class mmaVolumeMaterial;
class albaGUI;

class albaGUIFloatSlider;
class albaGUILutSlider;
class albaGUIHistogramWidget;

class ALBA_EXPORT albaGUILutHistogramEditor: public albaGUIDialog
{
public:
  enum LUT_EDITOR_WIDGET_ID
  {
    ID_ = MINID,
    ID_GAMMA_CORRETION,
    ID_RESET_LUT,
    ID_FULL_SAMPLING,
    ID_LOG_SCALE_VIEW
  };

  /** Constructor. */
  albaGUILutHistogramEditor(vtkDataSet *dataSet,mmaVolumeMaterial *material, char *name="Histogram & Windowing", albaObserver *Listener=NULL, int id=MINID);
  
  /** Destructor. */
  virtual ~albaGUILutHistogramEditor(); 
  
  /** Set the event listener */
  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Get the event listener */
  virtual albaObserver *GetListener() {return m_Listener;};

  /* Main event handler */
  void OnEvent(albaEventBase *alba_event);

  /** Handle resize event */
  void OnSize(wxSizeEvent &event);

  /** Show the dialog.*/
  static void ShowLutHistogramDialog(vtkDataSet *dataSet,mmaVolumeMaterial *material, char *name="Histogram & Windowing", albaObserver *Listener=NULL, int id=MINID);

protected:

  /* Resample the VME because full histogram is slow*/
  static vtkDataArray* Resample(vtkDataArray *inDA, vtkDataArray* outDA);

  /* Resets the dialog values to default */
  void ResetLutDialog(double gamma, double low, double high);

  /* Update the volume lut, if reset is true the update i make from default values */
  void UpdateVolumeLut(bool reset = false);

  /* Set the volume for the histogram */
  void SetMaterial(mmaVolumeMaterial *material);

  /* Set the volume for the histogram */
  void SetDataSet(vtkDataSet *dataSet);

  /** Copy the external Lookup Table given by the user to the internal one.*/
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
	
  albaObserver *m_Listener;
  albaGUI *m_GuiDialog;
  albaGUIFloatSlider *m_GammaSlider;
  albaGUILutSlider *m_Windowing;

  
  mmaVolumeMaterial *m_Material;
  vtkDataSet *m_DataSet;
  vtkDataArray *m_ResampledData;

  int m_FullSampling;
  int m_LogScale;
  double m_Gamma;
  double m_LowRange;
  double m_HiRange;
  

  albaGUIHistogramWidget *m_Histogram;
  albaGUILutHistogramSwatch   *m_LutSwatch;
  vtkLookupTable *m_ExternalLut;  ///< Given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_Lut;          ///< Internal lut -- initialized in SetLut
  
};
#endif
