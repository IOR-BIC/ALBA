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
#include "albaWizardBlockSelection.h"




//----------------------------------------------------------------------------
albaWizardBlockSelection::albaWizardBlockSelection(const char *name):albaWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting default values
  m_SelectedChoice=0;
}

//----------------------------------------------------------------------------
albaWizardBlockSelection::~albaWizardBlockSelection()
//----------------------------------------------------------------------------
{
  //clearing choices list
  m_Choices.clear();
}

//----------------------------------------------------------------------------
void albaWizardBlockSelection::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}

//----------------------------------------------------------------------------
void albaWizardBlockSelection::AddChoice( const char *label, const char *block )
//----------------------------------------------------------------------------
{
  //Creating a choice struct and push it in the choices array
  blockChoice tmpChoice;
  tmpChoice.label=label;
  tmpChoice.block=block;
  m_Choices.push_back(tmpChoice);
}

//----------------------------------------------------------------------------
wxString albaWizardBlockSelection::GetNextBlock()
//----------------------------------------------------------------------------
{
  wxString block;
  //Return the next block according on user choice
  if (!m_Success)
    block=m_AbortBlock;
  else if (m_SelectedChoice>=0 || m_SelectedChoice >= m_Choices.size())
    block=m_Choices[m_SelectedChoice].block;
  else 
    //if the selection is outside the range we return a fake block
    block="Selection problem";
  return block;
}

//----------------------------------------------------------------------------
void albaWizardBlockSelection::SetDescription( const char *description )
//----------------------------------------------------------------------------
{
  //set the description showed to the user
  m_Description=description;
}



//----------------------------------------------------------------------------
void albaWizardBlockSelection::ExcutionBegin()
//----------------------------------------------------------------------------
{
  albaWizardBlock::ExcutionBegin();

  //Generating required wxstring choice array
  wxString *choices = new wxString[m_Choices.size()];

  for(int i=0;i<m_Choices.size();i++)
    choices[i]=m_Choices[i].label;

  //Show Modal window
  m_SelectedChoice = wxGetSingleChoiceIndex(m_Description,m_Title,m_Choices.size(), choices);

  //User has pessed cancel
  if (m_SelectedChoice<0)
    Abort();

  //free mem 
  delete[] choices;  
}

//----------------------------------------------------------------------------
wxString albaWizardBlockSelection::GetChoiceLabel( int n )
//----------------------------------------------------------------------------
{
  //Bound check return n-th label if bounds are ok 
  //or an empty string elsewhere
  if (n>=0 && n<m_Choices.size())
    return m_Choices[n].label;
  else
    return "";
}

//----------------------------------------------------------------------------
wxString albaWizardBlockSelection::GetChoiceNextBlock( int n )
//----------------------------------------------------------------------------
{
  //Bound check return n-th label if bounds are ok 
  //or an empty string elsewhere
  if (n>=0 && n<m_Choices.size())
    return m_Choices[n].block;
  else
    return "";
}




