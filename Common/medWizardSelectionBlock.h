/*=========================================================================

 Program: MAF2Medical
 Module: medVect3d
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardSelectionBlock_H__
#define __medWizardSelectionBlock_H__

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
  Class Name: medVect3d.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medWizardSelectionBlock : medWizardBlock
{
public:

  mafTypeMacro(medWizardSelectionBlock, medWizardBlock);

  /** Default constructor */
  medWizardSelectionBlock();

  /** Default destructor */
  ~medWizardSelectionBlock();
    
  /** Set The title of the selection window */
  void SetTitle(char *Title);

  /** Set The title of the selection window */
  void SetDescription(char *description);

  /** Add a new choice in the selection.
      The choice is composed by the text showed to the user and the correspondent block
      */
  void AddChoice(char *label, char *block);

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

private:

  //Choices struct definition
  typedef struct 
  {
    wxString label;
    wxString block;
  } blockChoice;

  wxString m_Title;
  wxString m_Description;
  std::vector < blockChoice > m_Choices;
  int m_SelectedChoice;
};
#endif
