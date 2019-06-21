/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockYesNoSelection
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardBlockYesNoSelection_H__
#define __albaWizardBlockYesNoSelection_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: albaWizardSelectionBlock.
  Class for create a multiple choice switch inside wizard graph
*/
class ALBA_EXPORT albaWizardBlockYesNoSelection : public albaWizardBlock
{
public:

  /** Default constructor */
  albaWizardBlockYesNoSelection(const char *name);

  /** Default destructor */
  ~albaWizardBlockYesNoSelection();
    
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Get The title of the information window */
  wxString GetWindowTitle() {return m_Title;};

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Get The description */
  wxString GetDescription() {return m_Description;};

  /** Set The next block associated to yes user answer */
  void SetNextBlockOnYes(const char *block);

  /** Get The next block associated to yes user answer */
  wxString GetNextBlockOnYes(){return m_YesBlock;};

  /** Set The next block associated to no user answer */
  void SetNextBlockOnNo(const char *block);

  /** Set The next block associated to no user answer */
  wxString GetNextBlockOnNo(){return m_NoBlock;};


  /** This method has no sense in this class, use AddChoice() instead*/
  virtual void SetNextBlock(const char *block){};

  /** Enable or Disable Cancel Button */
  void EnableCancelButton(bool cancel=true);

  /** Get Cancel Button enabled status*/
  bool IsCancelButtonEnabled(){return m_CancelEnabled;};

protected:
  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();
private:

  wxString m_Title;
  wxString m_Description;
  wxString m_YesBlock;
  wxString m_NoBlock;
  bool m_CancelEnabled;
  int m_UserYes;
};
#endif
