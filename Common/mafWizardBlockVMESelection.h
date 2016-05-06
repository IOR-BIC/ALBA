/*=========================================================================

 Program: MAF2
 Module: mafWizardSelectionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafWizardBlockVMESelection_H__
#define __mafWizardBlockVMESelection_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: mafWizardSelectionBlock.
  Class for create a multiple choice switch inside wizard graph
*/
class MAF_EXPORT mafWizardBlockVMESelection : public mafWizardBlock
{
public:

  /** Default constructor */
  mafWizardBlockVMESelection(const char *name);

  /** Default destructor */
  ~mafWizardBlockVMESelection();
  
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
  static int VMEAccept(mafVME*node);

private:

  wxString m_AcceptedVME;
  wxString m_Title;
  wxString m_Description;
};
#endif
