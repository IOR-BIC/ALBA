/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramEditor.h,v $
  Language:  C++
  Date:      $Date: 2011-07-12 15:46:04 $
  Version:   $Revision: 1.1.2.1 $
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
/** mafGUILutEditor : vtkLookupTable Editor

  USAGE:
  - call the static member mafGUILutEditor::ShowLutDialog(vtkLookupTable *lut)

  FEATURES:
  - 20 preset available @sa mafLutPreset
  - editable number of LUT entries - limited to [1..256]
  - scalars range editing  --- ( 4 programmers - Set and Get it by calling Set/GetRange on your vtkLookupTable )
  - LUT Entry selection:
    - single selection -- mouse left click
    - range selection  -- mouse left click + drag
    - sparse selection -- mouse right click
  - Selected entries can be "shaded" interpolating first and last color both in RGBA and HSV space.

@sa mafColor mafGUIColorSwatch mafGUIColorWidget mafGUILutPreset mafGUILutSwatch mafGUILutWidget
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

  medGUILutHistogramEditor(mafVME *vme, vtkLookupTable *lut, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);
  virtual ~medGUILutHistogramEditor(); 

  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  void OnEvent(mafEventBase *maf_event);

  /** Assign the external lookup table to the widget.*/
  void SetLut(vtkLookupTable *lut);

  /** Show the dialog.*/
  static void ShowLutHistogramDialog(mafVME *vme, vtkLookupTable *lut, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);

protected:

  static void Resample(mafVME *vme, mafVMEVolumeGray* resampled);

  void ResetLutDialog(double gamma, double low, double high);

  void UpdateVolumeLut(bool reset = false);

  void SetVolume(mafVME *vol);
  /** Copy the external Lookup Table given by the user to the internal one.*/
  void CopyLut(vtkLookupTable *from, vtkLookupTable *to);
	
  //void OnComboSelection(wxCommandEvent &event);
  
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
