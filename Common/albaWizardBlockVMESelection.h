/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardSelectionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardBlockVMESelection_H__
#define __albaWizardBlockVMESelection_H__

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
class ALBA_EXPORT albaWizardBlockVMESelection : public albaWizardBlock
{
public:

  /** Default constructor */
  albaWizardBlockVMESelection(const char *name);

  /** Default destructor */
  ~albaWizardBlockVMESelection();
  
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
  static int VMEAccept(albaVME*node);

private:

  wxString m_AcceptedVME;
  wxString m_Title;
  wxString m_Description;
};
#endif
