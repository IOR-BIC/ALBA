/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizard
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
#include "albaWizard.h"
#include "albaObserver.h"
#include "albaWizardBlock.h"



//----------------------------------------------------------------------------
albaWizard::albaWizard(const wxString &label,  const wxString &name)
//----------------------------------------------------------------------------
{
  //Setting default values
  m_Name=name;
  m_Label     = label;
  m_CurrentBlock=NULL;
  m_Listener=NULL;
  m_ShowProgressBar=false;
}

//----------------------------------------------------------------------------
albaWizard::albaWizard()
//----------------------------------------------------------------------------
{
  //Setting default values
  m_Label  = "Default Wizard Name";
  m_CurrentBlock=NULL;
  m_Listener=NULL;
}

//----------------------------------------------------------------------------
albaWizard::~albaWizard()
//----------------------------------------------------------------------------
{
  //Deleting all blocks added to the wizard
  for (int i=0;i<m_Blocks.size();i++)
    albaDEL(m_Blocks[i]);
  //Clearing blocks array
  m_Blocks.clear();
}

//----------------------------------------------------------------------------
void albaWizard::Execute()
//----------------------------------------------------------------------------
{
  //The first block is defined by the name "START"
  albaWizardBlock *start=GetBlockByName("START");

  if (start==NULL)
    albaLogMessage("Wizard Error: Wizard has no starting point");
  else 
  {
    if (m_ShowProgressBar)
    {
      albaEventMacro(albaEvent(this,PROGRESSBAR_SHOW));
    }
    else 
      albaEventMacro(albaEvent(this,PROGRESSBAR_HIDE));

    //setting current block and start execution
    m_CurrentBlock=start;
    BlockExecutionBegin();
  }
}

//----------------------------------------------------------------------------
void albaWizard::AddBlock( albaWizardBlock *block)
//----------------------------------------------------------------------------
{
  //Add a new block to the wizard and setting is listener 
  block->SetListener(this);
  m_Blocks.push_back(block);
}

//----------------------------------------------------------------------------
void albaWizard::SetMenuPath( wxString path )
//----------------------------------------------------------------------------
{
  //Set the menu path for the wizard 
  m_MenuPath=path;
}

//----------------------------------------------------------------------------
void albaWizard::SetListener( albaObserver *Listener )
//----------------------------------------------------------------------------
{
  //Setting the listener 
  m_Listener = Listener;
}

//----------------------------------------------------------------------------
void albaWizard::SetId( int id )
//----------------------------------------------------------------------------
{
  //The id of the wizard
  m_Id=id;
}

//----------------------------------------------------------------------------
wxString albaWizard::GetMenuPath()
//----------------------------------------------------------------------------
{
  //return the menu path
  return m_MenuPath;
}

//----------------------------------------------------------------------------
int albaWizard::GetId()
//----------------------------------------------------------------------------
{
  //return the id
  return m_Id;
}

//----------------------------------------------------------------------------
wxString albaWizard::GetLabel()
  //----------------------------------------------------------------------------
{
  //return the label
  return m_Label;
}

//----------------------------------------------------------------------------
wxString albaWizard::GetName()
//----------------------------------------------------------------------------
{
  //return the label
  return m_Name;
}

//----------------------------------------------------------------------------
albaWizardBlock *albaWizard::GetBlockByName(const char *name )
//----------------------------------------------------------------------------
{
  //search a block in the block list by his name
  //and return it
  for (int i=0;i<m_Blocks.size();i++)
    if (m_Blocks[i]->GetName()==name)
      return m_Blocks[i];

  albaLogMessage("Wizard error: Block:'%s' not fount", name);
  return NULL;
}

//----------------------------------------------------------------------------
bool albaWizard::Accept(albaVME* vme)
//----------------------------------------------------------------------------
{
  //by default accept function return always false
  //you need to re-write this function in your specific wizard
  //to enable wizard in menu
  return false;
}

//----------------------------------------------------------------------------
void albaWizard::BlockExecutionBegin()
//----------------------------------------------------------------------------
{
  wxString requiredOperation;

  albaEventMacro(albaEvent(this,WIZARD_UPDATE_WINDOW_TITLE));

  //Setting selected vme to the block and execute it
  m_CurrentBlock->SetSelectedVME(m_SelectedVME);
  m_CurrentBlock->ExcutionBegin();

  if (m_CurrentBlock)
  {
    requiredOperation=m_CurrentBlock->GetRequiredOperation();
  
    //if there is a not a required operation 
    //the wizard flow continues without interruption and we call BlockExecutionEnd() 
    //elsewere if we had an operation it is run asynchronous and BlockExecutionEnd() 
    //will be called my managers after operation stop
    if (requiredOperation=="")
      ContinueExecution(m_CurrentBlock->Success());
  }
  
}

//----------------------------------------------------------------------------
void albaWizard::AbortWizard()
//----------------------------------------------------------------------------
{
  if (m_ShowProgressBar)
    albaEventMacro(albaEvent(this,PROGRESSBAR_HIDE));

  albaEventMacro(albaEvent(this,WIZARD_RUN_TERMINATED,false));
}

//----------------------------------------------------------------------------
void albaWizard::BlockExecutionEnd()
//----------------------------------------------------------------------------
{
  wxString nextBlock;

  //Execution End manage the tear down operation required by the block
  m_CurrentBlock->ExcutionEnd();

  nextBlock=m_CurrentBlock->GetNextBlock();

  //if the next block is "END" the execution of the wizard is terminated
  if (nextBlock=="END")
  {
    m_CurrentBlock=NULL;

    if (m_ShowProgressBar)
      albaEventMacro(albaEvent(this,PROGRESSBAR_HIDE));

    albaEventMacro(albaEvent(this,WIZARD_RUN_TERMINATED,true));
  }
  //If the next block is in the form WIZARD{<name>} We switch to the wizard <name> and execute it
  else if (nextBlock.StartsWith("WIZARD{"))
  {
    m_CurrentBlock=NULL;
    //checking match bracket 
    if (nextBlock[nextBlock.size()-1] != '}')
    {
      albaLogMessage("WIZARD special keyword error: WIZARD{<name>} wrong format");

      if (m_ShowProgressBar)
        albaEventMacro(albaEvent(this,PROGRESSBAR_HIDE));

      albaEventMacro(albaEvent(this,WIZARD_RUN_TERMINATED,true));
    }
    else 
    {
      //getting the wizard substring
      albaString wizardName = nextBlock.SubString(7,nextBlock.size()-2);
      albaEventMacro(albaEvent(this,WIZARD_SWITCH,&wizardName));
    }
  }
  else
  {
    m_CurrentBlock=GetBlockByName(nextBlock.ToAscii());

    //if the next block is undefined we abort the wizard execution
    if (m_CurrentBlock==NULL)
    {
      albaLogMessage("Wizard Error: undefined block :'%s'",nextBlock.ToAscii());
      AbortWizard();
    }
    //else we start the execution of the next block to continue wizard flow
    else 
      BlockExecutionBegin();
  }
}

//----------------------------------------------------------------------------  
void albaWizard::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------  
{
  //forward up event;
  albaEventMacro(*alba_event);
}


//----------------------------------------------------------------------------  
void albaWizard::SetSelectedVME( albaVME *node )
//----------------------------------------------------------------------------  
{
  //Selecting VME an (if necessary) setting it to the current block
  m_SelectedVME=node;
  if (m_CurrentBlock)
    m_CurrentBlock->SetSelectedVME(node);
}

//----------------------------------------------------------------------------  
void albaWizard::ContinueExecution(int opSuccess)
//----------------------------------------------------------------------------  
{
  //if last operation has success we continue with the flow of the wizard
  if (opSuccess)
    BlockExecutionEnd();
  else 
  {
    int answare = wxMessageBox(_("Do you want to abort this wizard ?"), _("Wizard Abort"), wxYES_NO|wxCENTER);
    if(answare == wxYES)
    {
      if (m_ShowProgressBar)
        albaEventMacro(albaEvent(this,PROGRESSBAR_HIDE));

      //if the operation has aborted by the user we abort the entire wizard
      //this behavior can be updated for error management
      albaEventMacro(albaEvent(this,WIZARD_RUN_TERMINATED,false));
      m_CurrentBlock=NULL;
    }
    else
    {
      m_CurrentBlock->Abort();
      BlockExecutionEnd();
    }
  }
}

//----------------------------------------------------------------------------
albaString albaWizard::GetDescriptionTitle()
//----------------------------------------------------------------------------
{

  if (m_CurrentBlock)
    return albaString("Wizard - ") + m_CurrentBlock->GetDescriptionLabel().GetCStr();
  else 
    return albaString("Wizard - No running Block");

}

