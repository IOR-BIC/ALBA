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

#ifndef __medWizardTypeCheckBlock_H__
#define __medWizardTypeCheckBlock_H__

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
  Class Name: medWizardTypeCheckBlock.
  Class for create a type check block, if the selected vme is wrong the wizard
  will be aborted.
*/
class MED_COMMON_EXPORT medWizardTypeCheckBlock : public medWizardBlock
{
public:

  /** Default constructor */
  medWizardTypeCheckBlock(const char *name);

  /** Default destructor */
  ~medWizardTypeCheckBlock();
    
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Set the path for VME selection */
  void VmeSelect(const char *path);

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Add a new accepted type.
      Uses the string of VME type.
      */
  void AddAcceptedType(const char *label);

  /** Set name of the Block called after wrong check.
      By default is set to "END" and the wizard will closed on wrong check. */     
  void SetWrongTypeNextBlock(const char *block);

  /** Set name of the Block called after check. */
  void SetNextBlock(const char *block);

  /** Return the name of the Block witch will be executed after this */
  wxString GetNextBlock();

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

protected:


private:

  wxString m_Title;
  wxString m_Description;

  wxString m_VmeSelect;
  
  std::vector <wxString> m_AcceptedVmes;
  int m_TestPassed;
  
  wxString m_NextBlock;
  wxString m_WrongTypeNextBlock;
};
#endif
