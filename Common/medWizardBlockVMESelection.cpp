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

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "mafNode.h"
#include "medWizardBlockVMESelection.h"

wxString globalAccept;


//----------------------------------------------------------------------------
medWizardBlockVMESelection::medWizardBlockVMESelection(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting default values
}

//----------------------------------------------------------------------------
medWizardBlockVMESelection::~medWizardBlockVMESelection()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medWizardBlockVMESelection::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
void medWizardBlockVMESelection::ExcutionBegin()
//----------------------------------------------------------------------------
{
  mafNode *selVME;
  medWizardBlock::ExcutionBegin();

  //Setting global variable witch can be referred by static function
  globalAccept=m_AcceptedVME;

  mafString title = m_Title.c_str();
  mafEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetArg((long)(&VMEAccept)); 
  // accept only Specified VME
  mafEventMacro(e);
  selVME=e.GetVme();

  if(selVME)
  {
    //Select vme 
    m_SelectedVME=selVME;
    mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
  }
  else
    //Abort on user cancel
    Abort();
}

//----------------------------------------------------------------------------
int medWizardBlockVMESelection::VMEAccept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA(globalAccept));
}

//----------------------------------------------------------------------------
void medWizardBlockVMESelection::SetAcceptedVME( const char *VME )
//----------------------------------------------------------------------------
{
  m_AcceptedVME=VME;
}


