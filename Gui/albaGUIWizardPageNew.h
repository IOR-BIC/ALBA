/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageNew
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIWizardPageNew_H__
#define __albaGUIWizardPageNew_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/wizard.h"
#include "albaDefines.h"
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaRWI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIWizard;
class albaGUI;
class albaGUILutSlider;

//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
/** IDs for the Dialog Style */
enum DIALOG_EXSTYLES
{
	albaWIZARDUSEGUI = 1,
	albaWIZARDUSERWI = 2,
};

/** IDs for the view GUI */
enum GUI_WIZARD_WIDGET_ID
{
  ID_LUT_CHOOSER = 3,
};

/**
  Class Name: albaGUIWizardPageNew.
  This class is the simplest possible albaGUIWizard implementation. 
  Add gui in panel positions:
  - lower left
  - lower right
  - lower center
  - lower center bottom
*/
class ALBA_EXPORT albaGUIWizardPageNew : public wxWizardPageSimple, public albaObserver  
{
public:
  /** construct. */
	albaGUIWizardPageNew (albaGUIWizard *wizardParent,long style = albaWIZARDUSEGUI | albaWIZARDUSERWI, bool ZCrop = false, wxString label="");
  /** destructor. */
	virtual ~albaGUIWizardPageNew ();
  /** Set the Listener that will receive event-notification. */
	void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  /** Precess events coming from other objects. */
	void OnEvent(albaEventBase *alba_event);

	albaObserver *m_Listener;
  
  /** Add in wizard gui, another gui in lower left position. */
	void AddGuiLowerLeft(albaGUI *gui);
  /** Add in wizard gui, another gui in lower right position. */
  void AddGuiLowerRight(albaGUI *gui);
  /** Add in wizard gui, another gui in lower center position. */
  void AddGuiLowerCenter(albaGUI *gui);
  /** Add in wizard gui, another gui in lower center bottom position. */
  void AddGuiLowerUnderLeft(albaGUI *gui);

  /** Add in wizard gui, another gui in lower center bottom position. */
  void AddGuiLowerUnderCenter(albaGUI *gui);

  

  /** Remove in wizard gui, another gui in lower left position. */
  void RemoveGuiLowerLeft(albaGUI *gui);

  /** Remove in wizard gui, another gui under lower left position. */
  void RemoveGuiLowerUnderLeft(albaGUI *gui);

	/** Remove in wizard gui, another gui in lower center position. */
	void RemoveGuiLowerCenter(albaGUI *gui);

	/** Create a chain between this page ad nextPage. */
	void SetNextPage(albaGUIWizardPageNew *nextPage);

  /** Set bounds of ZCrop slider. */
  void SetZCropBounds(double ZMin = 0, double ZMax = 1);

  /** Return bounds of ZCrop slider. */
  void GetZCropBounds(double ZCropBpunds[2]);

  /** Update windowing in order to set correct values in lut slider. */
  void UpdateWindowing();

  /** Update windowing (with custom range) in order to set correct values in lut slider.  
  (used in dicom importer because the windowing must be that of the total dicom
  and we see only a slice)*/
  void UpdateWindowing(double *scalarRange,double *scalarSubRange);

  /** Get the windowing range values */
  void GetWindowing(double *scalarRange,double *scalarSubRange);

  /** Update Actor Texture from current lut slider values. */
  void UpdateActor();

  /** Retrieve the current Render Window. */
	albaRWI* GetRWI(){return m_Rwi;};

protected:

	wxBoxSizer *m_GUISizer; ///< Sizer used for the Lower GUI
  wxBoxSizer *m_GUIUnderSizer; ///< Sizer used for the under Lower GUI
  wxBoxSizer *m_LUTSizer; ///< Sizer used for the LUT Slider GUI
  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI
  wxBoxSizer *m_SizerAll; ///< Vertical sizer used to include all other sizer

	albaRWI     *m_Rwi; ///< Render window

  albaGUI     *m_GuiLowerLeft; ///< Gui variable used to plug custom widgets localized in LOWER LEFT
  albaGUI     *m_GuiLowerCenter; ///< Gui variable used to plug custom widgets localized in LOWER CENTER
  albaGUI     *m_GuiLowerUnderCenter; ///< Gui variable used to plug custom widgets localized under LOWER CENTER
  albaGUI     *m_GuiLowerUnderLeft; ///< Gui variable used to plug custom widgets localized under LOWER LEFT
  albaGUI     *m_GuiView;

  albaGUILutSlider		*m_LutSlider;
  albaGUILutSlider		*m_ZCropSlider;
  vtkLookupTable	*m_ColorLUT;

  bool m_ZCropOn;

  

  friend class albaGUIWizardPageNewTest;
	//DECLARE_EVENT_TABLE()
};
#endif
