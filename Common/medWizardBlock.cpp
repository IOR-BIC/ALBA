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
#include "medWizardBlock.h"




//----------------------------------------------------------------------------
medWizardBlock::medWizardBlock(const char *name)
//----------------------------------------------------------------------------
{
  m_Name = name;

  if (m_Name == "END")
    mafLogMessage("MafWizardBlock: 'END' was reserved");
  else if (m_Name == "END")
    mafLogMessage("MafWizardBlock: 'ABORT' was reserved");

  m_Aborted=false;
}

//----------------------------------------------------------------------------
medWizardBlock::~medWizardBlock()
//----------------------------------------------------------------------------
{
}

int medWizardBlock::isAborted()
{
  return m_Aborted;
}

wxString medWizardBlock::GetName()
{
   return m_Name;
}


//

wxString medWizardBlock::GetNextBlock()
{
  return wxString("");
}

void medWizardBlock::Abort()
{
  m_Aborted=true;
}


void medWizardBlock::ExcutionBegin()
{
  m_Running=true;
}

void medWizardBlock::ExcutionEnd()
{
  m_Running=false;
}

wxString medWizardBlock::GetRequiredOperation()
{
  return wxString("");
}

void medWizardBlock::SetSelectedVME( mafNode *node )
{
  m_SelectedVME=node;
}

//----------------------------------------------------------------------------
void medWizardBlock::SetListener( mafObserver *Listener )
//----------------------------------------------------------------------------
{
  m_Listener = Listener;
}


//----------------------------------------------------------------------------
//void medWizardBlock::Delete()
//----------------------------------------------------------------------------

