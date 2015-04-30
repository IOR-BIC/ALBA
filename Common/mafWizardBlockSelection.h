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

#ifndef __mafWizardBlockSelection_H__
#define __mafWizardBlockSelection_H__

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
class MAF_EXPORT mafWizardBlockSelection : public mafWizardBlock
{
public:

  /** Default constructor */
  mafWizardBlockSelection(const char *name);

  /** Default destructor */
  ~mafWizardBlockSelection();
    
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Get The title of the selection window */
  wxString GetWindowTitle(){return m_Title;};

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Get The title of the selection window */
  wxString GetDescription(){return m_Description;};

  /** Add a new choice in the selection.
      The choice is composed by the text showed to the user and the correspondent block
      */
  void AddChoice(const char *label, const char *block);

  /** Returns the label of the n-th choice*/
  wxString GetChoiceLabel(int n);

  /** Returns the Next Block of the n-th choice*/
  wxString GetChoiceNextBlock(int n);

  
  /** This method has no sense in this class, use AddChoice() instead*/
  virtual void SetNextBlock(const char *block){};

protected:
  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();
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
