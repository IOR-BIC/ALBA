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

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medWizardSelectionBlock.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(medWizardSelectionBlock);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
medWizardSelectionBlock::medWizardSelectionBlock()
//----------------------------------------------------------------------------
{
  m_SelectedChoice=0;
}

//----------------------------------------------------------------------------
medWizardSelectionBlock::~medWizardSelectionBlock()
//----------------------------------------------------------------------------
{
  m_Choices.clear();
}

//----------------------------------------------------------------------------
void medWizardSelectionBlock::SetTitle( char *Title )
//----------------------------------------------------------------------------
{
  m_Title=Title;
}

//----------------------------------------------------------------------------
void medWizardSelectionBlock::AddChoice( char *label, char *block )
//----------------------------------------------------------------------------
{
  blockChoice tmpChoice;
  tmpChoice.label=label;
  tmpChoice.block=block;
  m_Choices.push_back(tmpChoice);
}

//----------------------------------------------------------------------------
wxString medWizardSelectionBlock::GetNextBlock()
//----------------------------------------------------------------------------
{
  wxString block;

  block=m_Choices[m_SelectedChoice].block;
  
  return block;
}

//----------------------------------------------------------------------------
void medWizardSelectionBlock::Execute()
//----------------------------------------------------------------------------
{
  //Generating Gui

  //Show Modal window
 
}

//----------------------------------------------------------------------------
void medWizardSelectionBlock::SetDescription( char *description )
//----------------------------------------------------------------------------
{
  m_Description=description;
}

//----------------------------------------------------------------------------
void medWizardSelectionBlock::OnEvent( mafEventBase *maf_event )
//----------------------------------------------------------------------------
{
    //Manage Events
}

void medWizardSelectionBlock::Abort()
{
  
  //This function does nothing because selection blocks run in a modal window
  //so we don't have to manage exteral aborts

}

