/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlock
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

#include "medDecl.h"
#include "medWizardBlock.h"




//----------------------------------------------------------------------------
medWizardBlock::medWizardBlock(const char *name)
//----------------------------------------------------------------------------
{

  //Setting default values
  m_Name = name;

  if (m_Name == "END")
    mafLogMessage("MafWizardBlock: 'END' was reserved");
  
  m_Success=true;


  //by default on abort cancel operation the operation will be recalled
  m_AbortBlock=name;

  m_BlockProgress=-1;
}

//----------------------------------------------------------------------------
medWizardBlock::~medWizardBlock()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
int medWizardBlock::Success()
//----------------------------------------------------------------------------
{
  //return true if the execution of the block havent problems
  return m_Success;
}

//----------------------------------------------------------------------------
wxString medWizardBlock::GetName()
//----------------------------------------------------------------------------
{
  //return the block name
  return m_Name;
}


//----------------------------------------------------------------------------
void medWizardBlock::SetNextBlock( const char *block )
//----------------------------------------------------------------------------
{
  //set the name of the block called after block execution
  m_NextBlock=block;
}

//----------------------------------------------------------------------------
wxString medWizardBlock::GetNextBlock()
//----------------------------------------------------------------------------
{
  if (m_Success)
  {
    //return the name of the next block
    return m_NextBlock;
  }
  else
  {
    //return the name of the abort block;
    return m_AbortBlock;
  }
}

//----------------------------------------------------------------------------
void medWizardBlock::Abort()
//----------------------------------------------------------------------------
{
  //Set the success to false
  m_Success=false;
}

//----------------------------------------------------------------------------
void medWizardBlock::ExcutionBegin()
//----------------------------------------------------------------------------
{
  //Starting execution
  m_Running=true;
  //Setting success to true on starting if some problem occurs this value
  //will be stetted to false later
  m_Success=true;

  //Setting the input VME to reselect it on op cancel 
  m_InputVME=m_SelectedVME;

  if (m_BlockProgress>=0)
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,m_BlockProgress));
}

//----------------------------------------------------------------------------
void medWizardBlock::ExcutionEnd()
//----------------------------------------------------------------------------
{
  //Setting back the select vme to the input on user abort
  if (!m_Success)
     mafEventMacro(mafEvent(this,VME_SELECT,m_InputVME));
  //Stopping execution
  m_Running=false;
}

//----------------------------------------------------------------------------
wxString medWizardBlock::GetRequiredOperation()
//----------------------------------------------------------------------------
{
  //No operation required by default 
  return wxString("");
}

//----------------------------------------------------------------------------
void medWizardBlock::SetSelectedVME( mafNode *node )
//----------------------------------------------------------------------------
{
  //setting the selected vme
  m_SelectedVME=node;
}

//----------------------------------------------------------------------------
void medWizardBlock::SetListener( mafObserver *Listener )
//----------------------------------------------------------------------------
{
  //setting the event listener
  m_Listener = Listener;
}

//----------------------------------------------------------------------------
mafString medWizardBlock::GetDescriptionLabel()
//----------------------------------------------------------------------------
{
  //return description label
  return m_DescriptionLabel;
}

//----------------------------------------------------------------------------
void medWizardBlock::SetDescriptionLabel( const char *label)
//----------------------------------------------------------------------------
{
  //setting description label
  m_DescriptionLabel=label;
}

void medWizardBlock::SetNextBlockOnAbort( const char *label )
{
  //setting description label
  m_AbortBlock=label;
}


