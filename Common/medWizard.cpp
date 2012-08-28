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
#include "medWizard.h"
#include "mafObserver.h"
#include "medWizardBlock.h"
#include <math.h>


//----------------------------------------------------------------------------
medWizard::medWizard(const wxString &label)
//----------------------------------------------------------------------------
{
  m_Label     = label;
  m_CurrentBlock=NULL;
  m_Listener=NULL;
}

//----------------------------------------------------------------------------
medWizard::~medWizard()
//----------------------------------------------------------------------------
{
  m_Blocks.clear();
}

void medWizard::Execute()
{
  medWizardBlock *start=GetBlockByName("START");

  if (start==NULL)
    mafLogMessage("Wizard Error: Wizard has no starting point");
  else 
  {
    m_CurrentBlock=start;
    BlockExecutionBegin();
  }
}

void medWizard::AddBlock( medWizardBlock *block)
{
  this->m_Blocks.push_back(block);
}

void medWizard::SetMenuPath( wxString path )
{
  m_MenuPath=path;
}

void medWizard::SetListener( mafObserver *Listener )
{
  m_Listener = Listener;
}

void medWizard::SetId( int id )
{
  m_Id=id;
}

wxString medWizard::GetMenuPath()
{
  return m_MenuPath;
}

int medWizard::GetId()
{
  return m_Id;
}

wxString medWizard::GetLabel()
{
  return m_Label;
}

medWizardBlock *medWizard::GetBlockByName(const char *name )
{
  for (int i=0;i<m_Blocks.size();i++)
    if (m_Blocks[i]->GetName()==name)
      return m_Blocks[i];

  mafLogMessage("Wizard error: Block:'%s' not fount", name);
  return NULL;
}

void medWizard::BlockExecutionBegin()
{
  wxString requiredOperation;
  m_CurrentBlock->ExcutionBegin();

  //there is a required operation 
  //We need to ask to the wizard manager for the operation start
  //the esecution of operation is asyncronous, so we need to suspend the esecution of the block
  //and wait for operation done event to continue execution
  
  if (requiredOperation!="")
  {
    //G,G OPERATION EVENT 
  }
  else
    BlockExecutionEnd();
  
}

void medWizard::AbortWizard()
{

}

void medWizard::BlockExecutionEnd()
{
  wxString nextBlock;

  m_CurrentBlock->ExcutionEnd();

  if (m_CurrentBlock->isAborted())
  {
    AbortWizard();
  }
  else
  {
    nextBlock=m_CurrentBlock->GetNextBlock();

    if (nextBlock=="END")
    {
      //G,G End WIZARD EVENY
    }
    else
    {
      m_CurrentBlock=GetBlockByName(nextBlock.c_str());

      if (m_CurrentBlock==NULL)
      {
        mafLogMessage("Wizard Error: undefined block :'%s'",nextBlock.c_str());
        AbortWizard();
      }
      else 
        BlockExecutionBegin();
    }
  }
}
