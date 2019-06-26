/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardManager
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardManager_H__
#define __albaWizardManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVect3d.h"
#include "albaOpManager.h"
#include "albaWizard.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaWizardWaitOp;
class albaGUISettingsDialog;
class albaWizardSettings;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaWizard *);
#endif

/**
  Class Name: albaWizardManager.
  Class for wizards management 
  This class creates the menu, starts the wizards and manage wizard related events
*/
class ALBA_EXPORT albaWizardManager : albaObserver
{
public:

  /** Default constructor */
  albaWizardManager();

  /** Default destructor */
  ~albaWizardManager();

  /** Event Management */
  virtual void OnEvent(albaEventBase *alba_event);


private:

  /** Add the Wizard 'op' to the list of available operations. */
  virtual void WizardAdd(albaWizard *wizard, wxString menuPath = "");

  std::vector<albaWizard *> GetWizardList(){return m_WizardList;};

  /** Fill the application men with the operations name.	*/
  virtual void FillMenu(wxMenu* wizardMenu);

  /** Call this to exec an operation with user interaction and undo/redo services. */
  virtual void WizardRun(albaWizard *wizard, void *wizard_param = NULL);

  /** Run the operation by id. */
  virtual void WizardRun(int wizardId);

  /** Called on operation termination to continue the work flow*/
  virtual void WizardContinue(int opSuccess);

  /** Record the selected vme and enable the menu_entries relative to the compatible wizard. */
  virtual void VmeSelected(albaVME* node);

	/** Record the modified vme and enable the menu_entries relative to the compatible wizard. */
	virtual void VmeModified(albaVME* node);
	
  /** Enable/Disable the men items operation. */
  virtual void EnableWizardMenus(bool CanEnable = true);

  /** Set a reference to the main toolbar. */
  virtual void SetToolbar(wxToolBar *tb) {m_ToolBar = tb;};

  /** Set a reference to the main men. */
  virtual void SetMenubar(wxMenuBar *mb) {m_MenuBar = mb;};

  /** Enable/Disable the toolbar's buttons. */
  virtual void EnableToolbar(bool CanEnable = true);

  /** Set the event listener */
  virtual void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Set the flag for warning the user if the operation is undoable. */
  virtual void WarningIfCantUndo (bool warn) {m_Warn = warn;};

  /** Get the flag for warning the user if the operation is undoable. */
  virtual bool GetWarningIfCantUndo(){return m_Warn;};


  /** returns a description about current step in wizard */
  albaString GetDescription();

  /** Fill the setting dialog with the settings associated to the plugged operations.*/
  void FillSettingDialog(albaGUISettingsDialog *settingDialog);

  /**Function called on run op event to manage special cases*/
  void OnRunOp(albaEvent *e);
    
  /** Stops the execution of the wizard*/
  virtual void WizzardStop();

  /** Sent a string message to the listener. */
  virtual void Notify(int msg);

  int m_NumWizard;
  std::vector<albaWizard *> m_WizardList; ///< List of pointer of plugged wizzard.
  albaWizard *m_RunningWizard;
  albaWizardSettings *m_Settings;
  wxMenuBar         *m_MenuBar; ///< Pointer to the Application's main menù
  wxToolBar         *m_ToolBar; ///< Pointer to the application's Toolbal
  albaObserver       *m_Listener; 
  albaVME						*m_Selected; ///< Pointer to the current selected node.
  bool               m_Warn; ///< Flag to warn the user when an operation that can not undo is starting.
  albaWizardWaitOp   *m_WaitOp;

  friend class albaLogicWithManagers; // class albaWizardManager can now access data directly
  friend class albaWizardManagerTest; // for testing 
};
#endif
