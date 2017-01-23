/*=========================================================================

 Program: MAF2
 Module: mafWizardBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafWizardBlock_H__
#define __mafWizardBlock_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafEvent.h"
#include "mafServiceClient.h"
#include "mafAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: mafWizardBlock.
  Class for the base wizard block, each block on a wizard must extend this block
*/
class MAF_EXPORT mafWizardBlock: public mafServiceClient 
{
public:

  /** Default constructor   
      Requires the name of the block 
      "START","END" are reserved. */
  mafWizardBlock(const char *name);

  /** Default destructor */
  virtual ~mafWizardBlock();

  
  /** Get the name of the block */
  wxString GetName();


  /** Set name of the Block called after operation. 
      There are some special blocks:
      WIZARD{<name>} Switch to the wizard <name> and execute it
      START Goes to the first block of the wizard
      END Exits from the wizard
      */
  virtual void SetNextBlock(const char *block);

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();

  /** Called to clean up memory*/
  virtual void Delete(){delete this;};

  /** Get a Label containing a description of the the current step*/
  mafString GetDescriptionLabel();

  /** Set a Label containing a description of the the current step*/
  void SetDescriptionLabel(const char *label);

  /** Set a Label containing a description of the the current step*/
  void SetNextBlockOnAbort(const char *label);

  /** Abort the execution of the block */
  virtual void Abort();

  /** Set the progress associated to this block */
  void SetBlockProgress(int progress){m_BlockProgress=progress;};

  /** Get the progress associated to this block */
  int GetBlockProgress(){return m_BlockProgress;};

protected:

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Ends the execution of the block */
  virtual void ExcutionEnd();

  /** Set the selected VME, this function must be called before execution begin*/
  void SetSelectedVME(mafVME *node);

  /** Sets The event listener */
  void SetListener(mafObserver *Listener);

  /** Return true if the user has aborted the operation */
  int Success();

  /** Returns the name of the operation required by this block 
      Return an empty string if no operation is required */
  virtual wxString GetRequiredOperation();

  wxString m_Name;
  wxString m_BlockType;
  wxString m_AbortBlock;
  wxString m_NextBlock;
  mafVME	*m_SelectedVME; ///< Pointer to the current selected node.
  mafVME *m_InputVME; ///< The vme selected on operation start.
  int m_Success;
  int m_Running;
  mafObserver    *m_Listener;
  mafString m_DescriptionLabel;
  long m_BlockProgress;

private:

  friend class mafWizard; // class mafWizard can now access data directly
};
#endif
