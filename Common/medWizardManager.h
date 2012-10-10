/*=========================================================================

 Program: MAF2Medical
 Module: medWizardManager
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardManager_H__
#define __medWizardManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medVect3d.h"
#include "mafOpManager.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class medWizard;
class medWizardWaitOp;
class mafGUISettingsDialog;
class medWizardSettings;
/**
  Class Name: medWizardManager.
  Class for wizards management 
  This class creates the menu, starts the wizards and manage wizard related events
*/
class MED_COMMON_EXPORT medWizardManager : mafObserver
{
public:

  /** Default constructor */
  medWizardManager();

  /** Default destructor */
  ~medWizardManager();

  /** Event Management */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Add the Wizard 'op' to the list of available operations. */
  virtual void WizardAdd(medWizard *wizard, wxString menuPath = "");

  /** Fill the application men with the operations name.	*/
  virtual void FillMenu(wxMenu* wizardMenu);
  
  /** Call this to exec an operation with user interaction and undo/redo services. */
  virtual void WizardRun(medWizard *wizard, void *wizard_param = NULL);

  /** Run the operation by id. */
  virtual void WizardRun(int wizardId);

  /** Called on opeation termination to contuinue the workflow*/
  virtual void WizardContinue(int opSuccess);

  /** Record the selected vme and enable the menu_entries relative to the compatible wizard. */
  virtual void VmeSelected(mafNode* node);

  /** Enable/Disable the men items operation. */
  virtual void EnableWizardMenus(bool CanEnable = true);

  /** Set a reference to the main toolbar. */
  virtual void SetToolbar(wxToolBar *tb) {m_ToolBar = tb;};

  /** Set a reference to the main men. */
  virtual void SetMenubar(wxMenuBar *mb) {m_MenuBar = mb;};

  /** Enable/Disable the toolbar's buttons. */
  virtual void EnableToolbar(bool CanEnable = true);

  /** Set the event listener */
  virtual void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Set the flag for warning the user if the operation is undoable. */
  virtual void WarningIfCantUndo (bool warn) {m_Warn = warn;};

  /** returns a description about current step in wizard */
  mafString GetDescription();

  /** Fill the setting dialog with the settings associated to the plugged operations.*/
  void FillSettingDialog(mafGUISettingsDialog *settingDialog);

private:

  /**Function called on run op event to manage special cases*/
  void OnRunOp(mafEvent *e);
    
  /** Stops the execution of the wizard*/
  virtual void WizzardStop();

  /** Sent a string message to the listener. */
  virtual void Notify(int msg);

  int m_NumWizard;
  std::vector<medWizard *> m_WizardList; ///< List of pointer of plugged wizzard.
  medWizard *m_RunningWizard;
  medWizardSettings *m_Settings;
  wxMenuBar         *m_MenuBar; ///< Pointer to the Application's main menù
  wxToolBar         *m_ToolBar; ///< Pointer to the application's Toolbal
  mafObserver       *m_Listener; 
  mafNode						*m_Selected; ///< Pointer to the current selected node.
  bool               m_Warn; ///< Flag to warn the user when an operation that can not undo is starting.
  medWizardWaitOp   *m_WaitOp;
};
#endif
