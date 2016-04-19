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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "mafVME.h"
#include "mafWizardBlockTypeCheck.h"




//----------------------------------------------------------------------------
mafWizardBlockTypeCheck::mafWizardBlockTypeCheck(const char *name):mafWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting default values
  m_Title="Bad VME type";
  m_Description="The Vme is of a wrong type,\nWizard will be closed!";
  m_WrongTypeNextBlock="END"; 
  m_ErrorMessageEnabled=true;
}

//----------------------------------------------------------------------------
mafWizardBlockTypeCheck::~mafWizardBlockTypeCheck()
//----------------------------------------------------------------------------
{
  //clearing choices list
  m_AcceptedVmes.clear();
}

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheck::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}



//----------------------------------------------------------------------------
wxString mafWizardBlockTypeCheck::GetNextBlock()
//----------------------------------------------------------------------------
{
  wxString block;
  //Return the next block according on user choice
  if (m_TestPassed)
    block=m_NextBlock;
  else 
    block=m_WrongTypeNextBlock;

  return block;
}

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheck::SetDescription( const char *description )
//----------------------------------------------------------------------------
{
  //set the description showed to the user
  m_Description=description;
}


//----------------------------------------------------------------------------
void mafWizardBlockTypeCheck::ExcutionBegin()
//----------------------------------------------------------------------------
{
  mafWizardBlock::ExcutionBegin();

  m_TestPassed=false;

  //Select the input VME for the operation
  if (m_SelectedVME)
    m_SelectedVME=m_SelectedVME->GetByPath(m_VmeSelect.c_str());
  else
    return;  

  mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
  
  for (int i=0;i<m_AcceptedVmes.size();i++)
    if (m_SelectedVME->IsA(m_AcceptedVmes[i].c_str()))
      m_TestPassed=true;

  if (m_ErrorMessageEnabled && !m_TestPassed)
    //Show Modal window
    wxMessageBox(m_Description,m_Title, wxOK);
}

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheck::VmeSelect( const char *path )
//----------------------------------------------------------------------------
{
  //Set the path of the vme which was selected before operation start
  m_VmeSelect=path;
}


//----------------------------------------------------------------------------
void mafWizardBlockTypeCheck::AddAcceptedType( const char *label )
//----------------------------------------------------------------------------
{
  wxString accept=label;
  m_AcceptedVmes.push_back(accept);
}

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheck::SetWrongTypeNextBlock( const char *block )
//----------------------------------------------------------------------------
{
  m_WrongTypeNextBlock=block;
}



