/*=========================================================================

 Program: MAF2Medical
 Module: medWizardSelectionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardBlockVMESelection_H__
#define __medWizardBlockVMESelection_H__

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
class MED_COMMON_EXPORT medWizardBlockVMESelection : public medWizardBlock
{
public:

  /** Default constructor */
  medWizardBlockVMESelection(const char *name);

  /** Default destructor */
  ~medWizardBlockVMESelection();
  
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Get The title of the selection window */
  wxString GetWindowTitle(){return m_Title;};

  /** Set The title of the selection window */
  void SetAcceptedVME(const char *VME);

  /** Set The title of the selection window */
  wxString GetAcceptedVME(){return m_AcceptedVME;};

  
protected:

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  //** VME Accept function */
  static int VMEAccept(mafNode *node);

private:

  wxString m_AcceptedVME;
  wxString m_Title;
  wxString m_Description;
};
#endif
