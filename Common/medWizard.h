/*=========================================================================

 Program: MAF2Medical
 Module: medWizard
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizard_H__
#define __medWizard_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafObserver.h"
#include "medWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class mafObserver;
class mafNode;

#ifdef MAF_EXPORT
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,medWizardBlock *);
#endif


/**
  Class Name: medWizard.
  Class for wizard creation each wizard must extend this block
*/
class MAF_EXPORT medWizard : mafObserver
{
public:

  /** Default constructor */
  medWizard();

  /** Default constructor */
  medWizard(const wxString &label, const wxString &name="");

  /** Default destructor */
  ~medWizard();


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
  virtual bool Accept(mafNode* vme);

  /** Execute the wizard */
  void Execute();

  /** Sets The event listener */
  void SetListener(mafObserver *Listener);

  /** returns a description about current step in wizard */
  mafString GetDescriptionTitle();

  /** Set the wizard id */
  void SetId(int id);

  /** Get the wizard id */
  int GetId();

  /** Add a new Block to the wizard 
      Added blocks will be auto-deleted form the wizard*/
  void AddBlock(medWizardBlock *block);

  /** Event management */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Used to continue wizard execution after operation termination */
  void ContinueExecution(int opSuccess);

  /** Set the selected VME, this function must be called before execution begin*/
  void SetSelectedVME(mafNode *node);

  mafObserver    *m_Listener;

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
  medWizardBlock *GetBlockByName(const char *name);
  
  std::vector <medWizardBlock *> m_Blocks;
  medWizardBlock *m_CurrentBlock; 
  wxString        m_MenuPath;
  int							m_Id; ///< Index of the wizard referring to the wizard list.
  wxString				m_Label; ///< Label of the wizard that will appear on menu.
  wxString				m_Name; ///< Label of the wizard that will appear on menu.
  mafNode        *m_SelectedVME;
  bool            m_ShowProgressBar;

  friend class medWizardManager; // class medWizardManager can now access data directly
};
#endif
