/*=========================================================================

 Program: MAF2Medical
 Module: medVect3d
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
/**
  Class Name: medVect3d.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medWizardManager : public mafOpManager
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
  virtual void WizardContinue();

  /** Record the selected vme and enable the menu_entries relative to the compatible wizard. */
  virtual void VmeSelected(mafNode* node);

  /** Enable/Disable the men items operation. */
  virtual void EnableOp(bool CanEnable = true);

  
private:

  /** Enable/Disable the men items operation. */
  virtual void EnableWizardMenus(bool CanEnable = true);


  virtual void WizzardStop();

  int m_NumWizard;
  std::vector<medWizard *> m_WizardList; ///< List of pointer of plugged wizzard.
  medWizard *m_RunningWizard;

};
#endif
