/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageNew
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIWizardPageNew_H__
#define __mafGUIWizardPageNew_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/wizard.h"
#include "mafDefines.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafRWI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIWizard;
class mafGUI;
class mafGUILutSlider;

//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
/** IDs for the Dialog Style */
enum DIALOG_EXSTYLES
{
	medUSEGUI = 1,
	medUSERWI = 2,
};

/** IDs for the view GUI */
enum GUI_WIZARD_WIDGET_ID
{
  ID_LUT_CHOOSER = 3,
};

/**
  Class Name: mafGUIWizardPageNew.
  This class is the simplest possible mafGUIWizard implementation. 
  Add gui in panel positions:
  - lower left
  - lower right
  - lower center
  - lower center bottom
*/
class MAF_EXPORT mafGUIWizardPageNew : public wxWizardPageSimple, public mafObserver  
{
public:
  /** construct. */
	mafGUIWizardPageNew (mafGUIWizard *wizardParent,long style = medUSEGUI | medUSERWI, bool ZCrop = false, wxString label="");
  /** destructor. */
	virtual ~mafGUIWizardPageNew ();
  /** Set the Listener that will receive event-notification. */
	void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  /** Precess events coming from other objects. */
	void OnEvent(mafEventBase *maf_event);

	mafObserver *m_Listener;
  
  /** Add in wizard gui, another gui in lower left position. */
	void AddGuiLowerLeft(mafGUI *gui);
  /** Add in wizard gui, another gui in lower right position. */
  void AddGuiLowerRight(mafGUI *gui);
  /** Add in wizard gui, another gui in lower center position. */
  void AddGuiLowerCenter(mafGUI *gui);
  /** Add in wizard gui, another gui in lower center bottom position. */
  void AddGuiLowerUnderLeft(mafGUI *gui);

  /** Add in wizard gui, another gui in lower center bottom position. */
  void AddGuiLowerUnderCenter(mafGUI *gui);

  

  /** Remove in wizard gui, another gui in lower left position. */
  void RemoveGuiLowerLeft(mafGUI *gui);

  /** Remove in wizard gui, another gui under lower left position. */
  void RemoveGuiLowerUnderLeft(mafGUI *gui);

	/** Create a chain between this page ad nextPage. */
	void SetNextPage(mafGUIWizardPageNew *nextPage);

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
	mafRWI* GetRWI(){return m_Rwi;};

protected:

	wxBoxSizer *m_GUISizer; ///< Sizer used for the Lower GUI
  wxBoxSizer *m_GUIUnderSizer; ///< Sizer used for the under Lower GUI
  wxBoxSizer *m_LUTSizer; ///< Sizer used for the LUT Slider GUI
  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI
  wxBoxSizer *m_SizerAll; ///< Vertical sizer used to include all other sizer

	mafRWI     *m_Rwi; ///< Render window

  mafGUI     *m_GuiLowerLeft; ///< Gui variable used to plug custom widgets localized in LOWER LEFT
  mafGUI     *m_GuiLowerCenter; ///< Gui variable used to plug custom widgets localized in LOWER CENTER
  mafGUI     *m_GuiLowerUnderCenter; ///< Gui variable used to plug custom widgets localized under LOWER CENTER
  mafGUI     *m_GuiLowerUnderLeft; ///< Gui variable used to plug custom widgets localized under LOWER LEFT
  mafGUI     *m_GuiView;

  mafGUILutSlider		*m_LutSlider;
  mafGUILutSlider		*m_ZCropSlider;
  vtkLookupTable	*m_ColorLUT;

  bool m_ZCropOn;

  

  friend class mafGUIWizardPageNewTest;
	//DECLARE_EVENT_TABLE()
};
#endif
