/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramEditor.h,v $
  Language:  C++
  Date:      $Date: 2011-07-14 08:23:37 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medGUILutHistogramEditor_H__
#define __medGUILutHistogramEditor_H__

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafLUTLibrary.h"

#include "mafGUILutWidget.h"
#include "medGUILutHistogramSwatch.h"
#include "mafGUIColorWidget.h"
#include "mafGUIDialog.h"

#include "vtkLookupTable.h"
#include "mafVMEVolumeGray.h"
/** medGUIHistogramLutEditor : vtkLookupTable Histogram Based Editor

  USAGE:
  - call the static member medGUILutHistogramEditor::ShowLutHistogramDialog(mafVME *vme, vtkLookupTable *lut,char *name, mafObserver *listener, int id)

  
@sa medGUILutSwatch medGUILutHistogramSwatch
*/
class mafVME;
class mafGUI;

class mafGUIFloatSlider;
class mafGUILutSlider;

class medGUILutHistogramEditor: public mafGUIDialog
{
public:
  enum LUT_EDITOR_WIDGET_ID
  {
    ID_ = MINID,
    ID_GAMMA_CORRETION,
    ID_RESET_LUT,
  };

  /** Constructor. */
  medGUILutHistogramEditor(mafVME *vme, vtkLookupTable *lut, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);
  
  /** Destructor. */
  virtual ~medGUILutHistogramEditor(); 
  
  /** Set the event listener */
  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Get the event listener */
  virtual mafObserver *GetListener() {return m_Listener;};

  /* Main event handler */
  void OnEvent(mafEventBase *maf_event);

  /** Assign the external lookup table to the widget.*/
  void SetLut(vtkLookupTable *lut);

  /** Assign the external lookup table to the widget.*/
  vtkLookupTable *GetLut(){return m_ExternalLut;};

  /** Show the dialog.*/
  static void ShowLutHistogramDialog(mafVME *vme, vtkLookupTable *lut, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);

protected:

  /* Resample the VME becouse full histogram is slow*/
  static void Resample(mafVME *vme, mafVMEVolumeGray* resampled);

  /* Resets the dialog values to default */
  void ResetLutDialog(double gamma, double low, double high);

  /* Update the volume lut, if reset is true the update i make from default values */
  void UpdateVolumeLut(bool reset = false);

  /* Set the volume for the histogram */
  void SetVolume(mafVME *vol);

  /** Copy the external Lookup Table given by the user to the internal one.*/
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
	
  mafObserver *m_Listener;
  mafGUI *m_GuiDialog;
  mafGUIFloatSlider *m_GammaSlider;
  mafGUILutSlider *m_Windowing;

  double m_Gamma;
  double m_LowRange;
  double m_HiRange;


  medGUILutHistogramSwatch   *m_LutSwatch;
  vtkLookupTable *m_ExternalLut;  ///< Given lut that will be modified by "ok" or "apply"
  vtkLookupTable *m_Lut;          ///< Internal lut -- initialized in SetLut
  mafVME *m_Volume;
  
};
#endif
