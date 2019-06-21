/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlock
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
#include "albaWizardBlock.h"




//----------------------------------------------------------------------------
albaWizardBlock::albaWizardBlock(const char *name)
//----------------------------------------------------------------------------
{

  //Setting default values
  m_Name = name;

  if (m_Name == "END")
    albaLogMessage("MafWizardBlock: 'END' was reserved");
  
  m_Success=true;


  //by default on abort cancel operation the operation will be recalled
  m_AbortBlock=name;

  m_BlockProgress=-1;
}

//----------------------------------------------------------------------------
albaWizardBlock::~albaWizardBlock()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
int albaWizardBlock::Success()
//----------------------------------------------------------------------------
{
  //return true if the execution of the block havent problems
  return m_Success;
}

//----------------------------------------------------------------------------
wxString albaWizardBlock::GetName()
//----------------------------------------------------------------------------
{
  //return the block name
  return m_Name;
}


//----------------------------------------------------------------------------
void albaWizardBlock::SetNextBlock( const char *block )
//----------------------------------------------------------------------------
{
  //set the name of the block called after block execution
  m_NextBlock=block;
}

//----------------------------------------------------------------------------
wxString albaWizardBlock::GetNextBlock()
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
void albaWizardBlock::Abort()
//----------------------------------------------------------------------------
{
  //Set the success to false
  m_Success=false;
}

//----------------------------------------------------------------------------
void albaWizardBlock::ExcutionBegin()
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
    albaEventMacro(albaEvent(this,PROGRESSBAR_SET_VALUE,m_BlockProgress));
}

//----------------------------------------------------------------------------
void albaWizardBlock::ExcutionEnd()
//----------------------------------------------------------------------------
{
	//Setting back the select vme to the input on user abort
	if (!m_Success)
		GetLogicManager()->VmeSelect(m_InputVME);
	//Stopping execution
	m_Running = false;
}

//----------------------------------------------------------------------------
wxString albaWizardBlock::GetRequiredOperation()
//----------------------------------------------------------------------------
{
  //No operation required by default 
  return wxString("");
}

//----------------------------------------------------------------------------
void albaWizardBlock::SetSelectedVME( albaVME *node )
//----------------------------------------------------------------------------
{
  //setting the selected vme
  m_SelectedVME=node;
}

//----------------------------------------------------------------------------
void albaWizardBlock::SetListener( albaObserver *Listener )
//----------------------------------------------------------------------------
{
  //setting the event listener
  m_Listener = Listener;
}

//----------------------------------------------------------------------------
albaString albaWizardBlock::GetDescriptionLabel()
//----------------------------------------------------------------------------
{
  //return description label
  return m_DescriptionLabel;
}

//----------------------------------------------------------------------------
void albaWizardBlock::SetDescriptionLabel( const char *label)
//----------------------------------------------------------------------------
{
  //setting description label
  m_DescriptionLabel=label;
}

void albaWizardBlock::SetNextBlockOnAbort( const char *label )
{
  //setting description label
  m_AbortBlock=label;
}


