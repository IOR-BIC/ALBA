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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaVME.h"
#include "albaWizardBlockVMESelection.h"

wxString globalAccept;


//----------------------------------------------------------------------------
albaWizardBlockVMESelection::albaWizardBlockVMESelection(const char *name):albaWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting default values
}

//----------------------------------------------------------------------------
albaWizardBlockVMESelection::~albaWizardBlockVMESelection()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaWizardBlockVMESelection::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
void albaWizardBlockVMESelection::ExcutionBegin()
//----------------------------------------------------------------------------
{
  albaVME *selVME;
  albaWizardBlock::ExcutionBegin();

  //Setting global variable witch can be referred by static function
  globalAccept=m_AcceptedVME;

  albaString title = m_Title;
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&VMEAccept); 
  // accept only Specified VME
  albaEventMacro(e);
  selVME=e.GetVme();

  if(selVME)
  {
    //Select vme 
    m_SelectedVME=selVME;
    albaEventMacro(albaEvent(this,VME_SELECT,m_SelectedVME));
  }
  else
    //Abort on user cancel
    Abort();
}

//----------------------------------------------------------------------------
int albaWizardBlockVMESelection::VMEAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA(globalAccept));
}

//----------------------------------------------------------------------------
void albaWizardBlockVMESelection::SetAcceptedVME( const char *VME )
//----------------------------------------------------------------------------
{
  m_AcceptedVME=VME;
}


