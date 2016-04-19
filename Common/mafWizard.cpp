/*=========================================================================

 Program: MAF2
 Module: mafWizard
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
#include "mafWizard.h"
#include "mafObserver.h"
#include "mafWizardBlock.h"



//----------------------------------------------------------------------------
mafWizard::mafWizard(const wxString &label,  const wxString &name)
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
mafWizard::mafWizard()
//----------------------------------------------------------------------------
{
  //Setting default values
  m_Label  = "Default Wizard Name";
  m_CurrentBlock=NULL;
  m_Listener=NULL;
}

//----------------------------------------------------------------------------
mafWizard::~mafWizard()
//----------------------------------------------------------------------------
{
  //Deleting all blocks added to the wizard
  for (int i=0;i<m_Blocks.size();i++)
    mafDEL(m_Blocks[i]);
  //Clearing blocks array
  m_Blocks.clear();
}

//----------------------------------------------------------------------------
void mafWizard::Execute()
//----------------------------------------------------------------------------
{
  //The first block is defined by the name "START"
  mafWizardBlock *start=GetBlockByName("START");

  if (start==NULL)
    mafLogMessage("Wizard Error: Wizard has no starting point");
  else 
  {
    if (m_ShowProgressBar)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    }
    else 
      mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

    //setting current block and start execution
    m_CurrentBlock=start;
    BlockExecutionBegin();
  }
}

//----------------------------------------------------------------------------
void mafWizard::AddBlock( mafWizardBlock *block)
//----------------------------------------------------------------------------
{
  //Add a new block to the wizard and setting is listener 
  block->SetListener(this);
  m_Blocks.push_back(block);
}

//----------------------------------------------------------------------------
void mafWizard::SetMenuPath( wxString path )
//----------------------------------------------------------------------------
{
  //Set the menu path for the wizard 
  m_MenuPath=path;
}

//----------------------------------------------------------------------------
void mafWizard::SetListener( mafObserver *Listener )
//----------------------------------------------------------------------------
{
  //Setting the listener 
  m_Listener = Listener;
}

//----------------------------------------------------------------------------
void mafWizard::SetId( int id )
//----------------------------------------------------------------------------
{
  //The id of the wizard
  m_Id=id;
}

//----------------------------------------------------------------------------
wxString mafWizard::GetMenuPath()
//----------------------------------------------------------------------------
{
  //return the menu path
  return m_MenuPath;
}

//----------------------------------------------------------------------------
int mafWizard::GetId()
//----------------------------------------------------------------------------
{
  //return the id
  return m_Id;
}

//----------------------------------------------------------------------------
wxString mafWizard::GetLabel()
  //----------------------------------------------------------------------------
{
  //return the label
  return m_Label;
}

//----------------------------------------------------------------------------
wxString mafWizard::GetName()
//----------------------------------------------------------------------------
{
  //return the label
  return m_Name;
}

//----------------------------------------------------------------------------
mafWizardBlock *mafWizard::GetBlockByName(const char *name )
//----------------------------------------------------------------------------
{
  //search a block in the block list by his name
  //and return it
  for (int i=0;i<m_Blocks.size();i++)
    if (m_Blocks[i]->GetName()==name)
      return m_Blocks[i];

  mafLogMessage("Wizard error: Block:'%s' not fount", name);
  return NULL;
}

//----------------------------------------------------------------------------
bool mafWizard::Accept(mafVME* vme)
//----------------------------------------------------------------------------
{
  //by default accept function return always false
  //you need to re-write this function in your specific wizard
  //to enable wizard in menu
  return false;
}

//----------------------------------------------------------------------------
void mafWizard::BlockExecutionBegin()
//----------------------------------------------------------------------------
{
  wxString requiredOperation;

  mafEventMacro(mafEvent(this,WIZARD_UPDATE_WINDOW_TITLE));

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
void mafWizard::AbortWizard()
//----------------------------------------------------------------------------
{
  if (m_ShowProgressBar)
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  mafEventMacro(mafEvent(this,WIZARD_RUN_TERMINATED,false));
}

//----------------------------------------------------------------------------
void mafWizard::BlockExecutionEnd()
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
      mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

    mafEventMacro(mafEvent(this,WIZARD_RUN_TERMINATED,true));
  }
  //If the next block is in the form WIZARD{<name>} We switch to the wizard <name> and execute it
  else if (nextBlock.StartsWith("WIZARD{"))
  {
    m_CurrentBlock=NULL;
    //checking match bracket 
    if (nextBlock[nextBlock.size()-1] != '}')
    {
      mafLogMessage("WIZARD special keyword error: WIZARD{<name>} wrong format");

      if (m_ShowProgressBar)
        mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

      mafEventMacro(mafEvent(this,WIZARD_RUN_TERMINATED,true));
    }
    else 
    {
      //getting the wizard substring
      mafString wizardName = nextBlock.SubString(7,nextBlock.size()-2).c_str();
      mafEventMacro(mafEvent(this,WIZARD_SWITCH,&wizardName));
    }
  }
  else
  {
    m_CurrentBlock=GetBlockByName(nextBlock.c_str());

    //if the next block is undefined we abort the wizard execution
    if (m_CurrentBlock==NULL)
    {
      mafLogMessage("Wizard Error: undefined block :'%s'",nextBlock.c_str());
      AbortWizard();
    }
    //else we start the execution of the next block to continue wizard flow
    else 
      BlockExecutionBegin();
  }
}

//----------------------------------------------------------------------------  
void mafWizard::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------  
{
  //forward up event;
  mafEventMacro(*maf_event);
}


//----------------------------------------------------------------------------  
void mafWizard::SetSelectedVME( mafVME *node )
//----------------------------------------------------------------------------  
{
  //Selecting VME an (if necessary) setting it to the current block
  m_SelectedVME=node;
  if (m_CurrentBlock)
    m_CurrentBlock->SetSelectedVME(node);
}

//----------------------------------------------------------------------------  
void mafWizard::ContinueExecution(int opSuccess)
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
        mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

      //if the operation has aborted by the user we abort the entire wizard
      //this behavior can be updated for error management
      mafEventMacro(mafEvent(this,WIZARD_RUN_TERMINATED,false));
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
mafString mafWizard::GetDescriptionTitle()
//----------------------------------------------------------------------------
{

  if (m_CurrentBlock)
    return mafString("Wizard - ") + m_CurrentBlock->GetDescriptionLabel().GetCStr();
  else 
    return mafString("Wizard - No running Block");

}

