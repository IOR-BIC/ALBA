/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizardPageNew.h,v $
Language:  C++
Date:      $Date: 2012-01-26 13:48:05 $
Version:   $Revision: 1.1.2.10 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/
#ifndef __medGUIWizardPageNew_H__
#define __medGUIWizardPageNew_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/wizard.h"
#include "medDefines.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafRWI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medGUIWizard;
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
  Class Name: medGUIWizardPageNew.
  This class is the simplest possible medGUIWizard implementation. 
  Add gui in panel positions:
  - lower left
  - lower right
  - lower center
  - lower center bottom
*/
class MED_EXPORT medGUIWizardPageNew : public wxWizardPageSimple, public mafObserver  
{
public:
  /** construct. */
	medGUIWizardPageNew (medGUIWizard *wizardParent,long style = medUSEGUI | medUSERWI, bool ZCrop = false, wxString label="");
  /** destructor. */
	virtual ~medGUIWizardPageNew ();
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
	void SetNextPage(medGUIWizardPageNew *nextPage);

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

  

  friend class medGUIWizardPageNewTest;
	//DECLARE_EVENT_TABLE()
};
#endif
