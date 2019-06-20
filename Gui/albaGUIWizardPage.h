/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPage
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIWizardPage_H__
#define __albaGUIWizardPage_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "wx/wizard.h"
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaRWI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIWizard;
class albaGUI;

//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_EXSTYLES
{
	albaWIZARDUSEGUI = 1,
	albaWIZARDUSERWI = 2,
};

/**
  Class Name: albaGUIWizardPage.
  This class is the simplest possible albaGUIWizard implementation.
  Add gui in panel positions:
  - lower left
  - lower right
  - lower center
*/
class ALBA_EXPORT albaGUIWizardPage : public wxWizardPageSimple, public albaObserver  
{
public:
  /** constructor. */
	albaGUIWizardPage (albaGUIWizard *wizardParent,long style = albaWIZARDUSEGUI | albaWIZARDUSERWI,wxString label="",bool plugCentralGui = true,bool plugLeftGui = true,bool plugRightGui = true);
  /** destructor. */
	virtual ~albaGUIWizardPage ();
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

  /** Remove in wizard gui, another gui in lower left position. */
  void RemoveGuiLowerLeft(albaGUI *gui);

  /** Remove in wizard gui, another gui in lower right position. */
  void RemoveGuiLowerRight(albaGUI *gui);

  /** Remove in wizard gui, another gui in lower center position. */
  void RemoveGuiLowerCenter(albaGUI *gui);

	/** Create a chain between this page ad nextPage. */
	void SetNextPage(albaGUIWizardPage *nextPage);

  /** Retrieve the current Render Window. */
	albaRWI* GetRWI(){return m_Rwi;};

protected:

	wxWizardPageSimple *m_FirstPage;

	wxBoxSizer *m_GUISizer; ///< Sizer used for the Lower GUI
  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI
  wxBoxSizer *m_SizerAll; ///< Vertical sizer used to include all other sizer

	albaRWI     *m_Rwi; ///< Render window

  albaGUI     *m_GuiLowerLeft; ///< Gui variable used to plug custom widgets localized in LOWER LEFT
  albaGUI     *m_GuiLowerRight; ///< Gui variable used to plug custom widgets localized in LOWER RIGHT
  albaGUI     *m_GuiLowerCenter; ///< Gui variable used to plug custom widgets localized in LOWER CENTER

  long m_Style;

	//DECLARE_EVENT_TABLE()

  friend class albaGUIWizardPageTest;
};
#endif
