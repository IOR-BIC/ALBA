/*=========================================================================

 Program: MAF2Medical
 Module: medWizardOperaiontionBlock
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
#include "medWizardOperationBlock.h"
#include "mafNode.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
medWizardOperaiontionBlock::medWizardOperaiontionBlock(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{
  //Default constructor
}

//----------------------------------------------------------------------------
medWizardOperaiontionBlock::~medWizardOperaiontionBlock()
//----------------------------------------------------------------------------
{
  //Clearing the list of to show and to hide VME
  m_VmeShow.clear();
  m_VmeHide.clear();
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::SetRequiredView( const char *View )
//----------------------------------------------------------------------------
{
  //setting required view name 
  m_RequiredView = View;
}

//----------------------------------------------------------------------------
wxString medWizardOperaiontionBlock::GetRequiredView()
//----------------------------------------------------------------------------
{
  //return the required view
  return m_RequiredView;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeSelect( const char *path )
//----------------------------------------------------------------------------
{
  //Set the path of the vme which was selected before operation start
  m_VmeSelect=path;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeShowAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme showed before operation start
  wxString wxPath;
  wxPath=path;
  m_VmeShow.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeHideAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme hided after operation end
  wxString wxPath;
  wxPath=path;
  m_VmeHide.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::SetNextBlock( const char *block )
//----------------------------------------------------------------------------
{
  //set the name of the block called after block execution
  m_NextBlock=block;
}

//----------------------------------------------------------------------------
wxString medWizardOperaiontionBlock::GetNextBlock()
//----------------------------------------------------------------------------
{
  //return the name of the next block
  return m_NextBlock;
}


//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::ExcutionBegin()
//----------------------------------------------------------------------------
{
  mafString tmpStr;

  medWizardBlock::ExcutionBegin();

  ///////////////////////
  //Ask Wizard for View
  if (m_RequiredView!="")
  {
    //send up the event in order to open/select the required view
    tmpStr=m_RequiredView;
    mafEventMacro(mafEvent(this,WIZARD_REQUIRED_VIEW,&tmpStr));
  }

  ///////////////////////
  //Select the input VME for the operation
  if (m_SelectedVME)
    m_SelectedVME=m_SelectedVME->GetByPath(m_VmeSelect.c_str());
  
  if (m_SelectedVME)
  {
    //forward up vme selection event 
    //for view/logic/operation update
    mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
  }
  else 
  {
    //If we cannot select the correct vme we need to abort the wizard
    mafLogMessage("Wizard Error: unable to select VME, path:\"%s\" base:\"%s\"",m_VmeSelect.c_str(),m_SelectedVME->GetName());
    Abort();
    //we stop execution now
    return;
  }

  //////////////////////////  
  //Show the required VMEs

  //If there is no view required we don't show any vme 
  if (m_RequiredView != "")
  {
    //Showing input vme to ensure visualization in the operation
    mafEventMacro(mafEvent(this,VME_SHOW,m_SelectedVME,true));

    for(int i=0;i<m_VmeShow.size();i++)
    {
      //detecting all other vme starting on selected vme and show it
      mafNode *toShow=m_SelectedVME->GetByPath(m_VmeShow[i].c_str());
      if (toShow != NULL)
        mafEventMacro(mafEvent(this,VME_SHOW,toShow,true));
    }
  }
  
  //////////////////////////  
  //Run Operation
  if (m_Operation!="")
  {
    tmpStr=m_Operation;
    //ask logic for operation run the flow will continue after operation stop
    mafEventMacro(mafEvent(this,WIZARD_RUN_OP,&tmpStr));
  }
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::ExcutionEnd()
//----------------------------------------------------------------------------
{
  medWizardBlock::ExcutionEnd();
  //////////////////////////  
  //Hide the required VMEs
  for(int i=0;i<m_VmeHide.size();i++)
  {
    mafNode *toHide=m_SelectedVME->GetByPath(m_VmeHide[i]);
    mafEventMacro(mafEvent(this,VME_SHOW,toHide,false));
  }
  
}

//----------------------------------------------------------------------------
wxString medWizardOperaiontionBlock::GetRequiredOperation()
//----------------------------------------------------------------------------
{
  //return the name of required operation
  return m_Operation;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::SetRequiredOperation( const const char *name )
//----------------------------------------------------------------------------
{
  //set the name of required operation
  m_Operation=name;
}




