/*=========================================================================

 Program: MAF2Medical
 Module: medLogicWithManagers
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medLogicWithManagers_H__
#define __medLogicWithManagers_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "mafLogicWithManagers.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class medWizardManager;
class medWizard;

/**
  Class Name: medLogicWithManagers.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medLogicWithManagers : public mafLogicWithManagers
{
public:
  /** constructor. */
	medLogicWithManagers();
  /** destructor. */
	virtual     ~medLogicWithManagers(); 

  /** Process events coming from other objects */ 
	virtual void OnEvent(mafEventBase *maf_event);

  /**  Plug a new view */
  virtual void Plug(mafView* view, bool visibleInMenu = true);

  /**  Plug a new operation and its undo flag: if the operation does not support
  undo the undo flag has no effect */
	virtual void Plug(mafOp *op, wxString menuPath = "", bool canUndo = true, mafGUISettings *setting = NULL);
	
  /**  Plug a new wizard */
	virtual void Plug(medWizard *wizard, wxString menuPath = "");
	
  /** Must be called before Configure */
  void PlugWizardManager(bool b){m_UseWizardManager=b;};

  /** Configure the application.
  At this point are plugged all the managers, the side-bar docking panel. 
  Are plugged also all the setting to the dialogs interface. */
  virtual void Configure();

  /** Fill the View and operation menu's and set the application stamp to the VMEManager.*/
  virtual void Show();

  /** Manage application exception and allow to save at least the tree. */
  virtual void HandleException();

  /** Program Initialization */
  virtual void Init(int argc, char **argv);
  
protected:
  /** Show contextual menu for views when right mouse click arrives.*/
  void ViewContextualMenu(bool vme_menu);

  /** Called on Quit event. */
  virtual void OnQuit();

  /** Redefined to add Print buttons */
  virtual void CreateWizardToolbar();
  
  /** Respond to a VME_SELECTED evt. Update the selection on the tree and view representation. */
  virtual void VmeSelected(mafNode *vme);

  /** Redefined to add View,Op,Import,Export, Wizard menu*/
  virtual void CreateMenu();

  /** Called when an wizard starts. Disable all menu and lock the Selection */ 
  virtual void WizardRunStarting();

  /** Called when an wizard stops. Re-enable all menu and unlock the Selection */ 
  virtual void WizardRunTerminated();
  
  /** Called after FileOpen or Save operation */
  virtual void UpdateFrameTitle();

  /** FILE OPEN evt. handler. 
  By default (file_to_open = NULL) it ask the user to choose a file to open,
  otherwise it open the given one.*/
	virtual void OnFileOpen(const char *file_to_open = NULL);
 
  /** FILE SAVE evt. handler */
  virtual void OnFileSave();
  
  /** FILE SAVEAS evt. handler */
  virtual void OnFileSaveAs();

  virtual void ConfigureWizardManager();

  medWizardManager *m_WizardManager;
  bool m_UseWizardManager;
  bool m_WizardRunning;
  wxGauge *m_WizardGauge;
  wxStaticText* m_WizardLabel;
  bool m_CancelledBeforeOpStarting;
  wxMenu *m_WizardMenu;

};
#endif
