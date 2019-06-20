/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockVMEFindOrSelection
 Authors: Simone Bna
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardBlockVMEFindOrSelection_H__
#define __albaWizardBlockVMEFindOrSelection_H__

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
  Class Name: albaWizardBlockVMEFindOrSelection.
  Class for create a multiple choice switch inside wizard graph
*/
class ALBA_EXPORT albaWizardBlockVMEFindOrSelection : public albaWizardBlock
{
public:

  /** Default constructor */
  albaWizardBlockVMEFindOrSelection(const char *name);

  /** Default destructor */
  ~albaWizardBlockVMEFindOrSelection();
  
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Get The title of the selection window */
  wxString GetWindowTitle(){return m_Title;};

  /** Set The title of the selection window */
  void SetAcceptedVME(const char *VME);

  /** Set The title of the selection window */
  wxString GetAcceptedVME(){return m_AcceptedVME;};

  //Set the path of the parent vme of a list of childs
  //The wizard block will look for childs of correct type
  void VmeParentSelect( const char *path );

   
protected:

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  //** VME Accept function */
  static int VMEAccept(albaVME*node);

  wxString m_AcceptedVME;
  wxString m_Title;
  wxString m_Description;
  wxString m_VmeParentSelect;
};
#endif