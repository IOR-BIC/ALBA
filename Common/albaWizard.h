/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizard
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizard_H__
#define __albaWizard_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObserver.h"
#include "albaWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class albaObserver;
class albaVME;

#ifdef ALBA_EXPORT
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaWizardBlock *);
#endif


/**
  Class Name: albaWizard.
  Class for wizard creation each wizard must extend this block
*/
class ALBA_EXPORT albaWizard : albaObserver
{
public:

  /** Default constructor */
  albaWizard();

  /** Default constructor */
  albaWizard(const wxString &label, const wxString &name="");

  /** Default destructor */
  ~albaWizard();


  /** Sets the menu path */
  void SetMenuPath(wxString path);
  
  /** Sets the menu path */
  wxString GetMenuPath();

  /** Return the Menu label of the wizard*/
  wxString GetLabel();

  /** Return the name of the wizard*/
  wxString GetName();

  /** Initialize wizard's variables according to the parameter's list. */
  virtual void SetParameters(void *param) {};

  /** Enable/disable the visualization of the progress bar during the wizard*/
  void ShowProgressBar(bool show=true){m_ShowProgressBar=show;};

  /** Return true if progress bar show is enabled*/
  bool GetShowProgressBar(){return m_ShowProgressBar;};
    
protected:
  /** Return true for the acceptable vme type.*/
  virtual bool Accept(albaVME* vme);

  /** Execute the wizard */
  void Execute();

  /** Sets The event listener */
  void SetListener(albaObserver *Listener);

  /** returns a description about current step in wizard */
  albaString GetDescriptionTitle();

  /** Set the wizard id */
  void SetId(int id);

  /** Get the wizard id */
  int GetId();

  /** Add a new Block to the wizard 
      Added blocks will be auto-deleted form the wizard*/
  void AddBlock(albaWizardBlock *block);

  /** Event management */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Used to continue wizard execution after operation termination */
  void ContinueExecution(int opSuccess);

  /** Set the selected VME, this function must be called before execution begin*/
  void SetSelectedVME(albaVME *node);

  albaObserver    *m_Listener;

private:

  /** Manages the begin of execution of current block */
  void BlockExecutionBegin();

  /** Manage the end of execution of current block.
      Controls if the user aborted the wizard during block execution
      If necessary starts the execution of next block */
  void BlockExecutionEnd();

  /** Clean up and abort the wizard */
  void AbortWizard();

  /** Return a pointer to the wizard  block */
  albaWizardBlock *GetBlockByName(const char *name);
  
  std::vector <albaWizardBlock *> m_Blocks;
  albaWizardBlock *m_CurrentBlock; 
  wxString        m_MenuPath;
  int							m_Id; ///< Index of the wizard referring to the wizard list.
  wxString				m_Label; ///< Label of the wizard that will appear on menu.
  wxString				m_Name; ///< Label of the wizard that will appear on menu.
  albaVME	        *m_SelectedVME;
  bool            m_ShowProgressBar;

  friend class albaWizardManager; // class albaWizardManager can now access data directly
};
#endif
