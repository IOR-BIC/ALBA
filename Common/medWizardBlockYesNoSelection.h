/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlockYesNoSelection
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardBlockYesNoSelection_H__
#define __medWizardBlockYesNoSelection_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medWizardSelectionBlock.
  Class for create a multiple choice switch inside wizard graph
*/
class MED_COMMON_EXPORT medWizardBlockYesNoSelection : public medWizardBlock
{
public:

  /** Default constructor */
  medWizardBlockYesNoSelection(const char *name);

  /** Default destructor */
  ~medWizardBlockYesNoSelection();
    
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Set The next block associated to yes user answer */
  void SetNextBlockOnYes(const char *block);

  /** Set The next block associated to no user answer */
  void SetNextBlockOnNo(const char *block);

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();

  /** This method has no sense in this class, use AddChoice() instead*/
  virtual void SetNextBlock(const char *block){};

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Enable or Disable Cancel Button */
  void EnableCancelButton(bool cancel=true);

protected:


private:

  wxString m_Title;
  wxString m_Description;
  wxString m_YesBlock;
  wxString m_NoBlock;
  bool m_CancelEnabled;
  int m_UserYes;
};
#endif
