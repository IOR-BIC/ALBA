/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlockVMEFindOrSelection
 Authors: Simone Bna
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardBlockVMEFindOrSelection_H__
#define __medWizardBlockVMEFindOrSelection_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "medWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medWizardBlockVMEFindOrSelection.
  Class for create a multiple choice switch inside wizard graph
*/
class MAF_EXPORT medWizardBlockVMEFindOrSelection : public medWizardBlock
{
public:

  /** Default constructor */
  medWizardBlockVMEFindOrSelection(const char *name);

  /** Default destructor */
  ~medWizardBlockVMEFindOrSelection();
  
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
  static int VMEAccept(mafNode *node);

  wxString m_AcceptedVME;
  wxString m_Title;
  wxString m_Description;
  wxString m_VmeParentSelect;
};
#endif