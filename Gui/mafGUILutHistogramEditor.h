/*=========================================================================

 Program: MAF2
 Module: mafGUILutHistogramEditor
 Authors: Crimi Gianluigi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUILutHistogramEditor_H__
#define __mafGUILutHistogramEditor_H__

#include "mafDefines.h"
#include "mafEvent.h"
#include "mafLUTLibrary.h"

#include "mafGUILutWidget.h"
#include "mafGUILutHistogramSwatch.h"
#include "mafGUIColorWidget.h"
#include "mafGUIDialog.h"

#include "vtkLookupTable.h"
/** medGUIHistogramLutEditor : vtkLookupTable Histogram Based Editor

  USAGE:
  - call the static member mafGUILutHistogramEditor::ShowLutHistogramDialog(mafVME *vme, vtkLookupTable *lut,char *name, mafObserver *listener, int id)

  
@sa medGUILutSwatch mafGUILutHistogramSwatch
*/
class vtkDataSet;
class mmaVolumeMaterial;
class mafGUI;

class mafGUIFloatSlider;
class mafGUILutSlider;
class mafGUIHistogramWidget;

class MAF_EXPORT mafGUILutHistogramEditor: public mafGUIDialog
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
  mafGUILutHistogramEditor(vtkDataSet *dataSet,mmaVolumeMaterial *material, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);
  
  /** Destructor. */
  virtual ~mafGUILutHistogramEditor(); 
  
  /** Set the event listener */
  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Get the event listener */
  virtual mafObserver *GetListener() {return m_Listener;};

  /* Main event handler */
  void OnEvent(mafEventBase *maf_event);

  /** Handle resize event */
  void OnSize(wxSizeEvent &event);

  /** Show the dialog.*/
  static void ShowLutHistogramDialog(vtkDataSet *dataSet,mmaVolumeMaterial *material, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);

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
	
  mafObserver *m_Listener;
  mafGUI *m_GuiDialog;
  mafGUIFloatSlider *m_GammaSlider;
  mafGUILutSlider *m_Windowing;

  
  mmaVolumeMaterial *m_Material;
  vtkDataSet *m_DataSet;
  vtkDataArray *m_ResampledData;

  int m_FullSampling;
  int m_LogScale;
  double m_Gamma;
  double m_LowRange;
  double m_HiRange;
  

  mafGUIHistogramWidget *m_Histogram;
  mafGUILutHistogramSwatch   *m_LutSwatch;
  vtkLookupTable *m_ExternalLut;  ///< Given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_Lut;          ///< Internal lut -- initialized in SetLut
  
};
#endif
