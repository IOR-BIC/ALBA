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
#include "medWizardOperationBlock.h"
#include "mafNode.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
medWizardOperaiontionBlock::medWizardOperaiontionBlock(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
medWizardOperaiontionBlock::~medWizardOperaiontionBlock()
//----------------------------------------------------------------------------
{
  m_VmeShow.clear();
  m_VmeHide.clear();
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::SetRequiredView( const char *View )
//----------------------------------------------------------------------------
{
  m_RequiredView = View;
}

wxString medWizardOperaiontionBlock::GetRequiredView()
{
  return m_RequiredView;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeSelect( const char *path )
//----------------------------------------------------------------------------
{
  m_VmeSelect=path;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeShow( const char *path )
//----------------------------------------------------------------------------
{
  wxString wxPath;
  wxPath=path;
  m_VmeShow.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeHide( const char *path )
//----------------------------------------------------------------------------
{
  wxString wxPath;
  wxPath=path;
  m_VmeHide.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::SetNextBlock( const char *block )
//----------------------------------------------------------------------------
{
  m_NextBlock=block;
}

//----------------------------------------------------------------------------
wxString medWizardOperaiontionBlock::GetNextBlock()
//----------------------------------------------------------------------------
{
  return m_NextBlock;
}


void medWizardOperaiontionBlock::Abort()
{
    //Abort The Operation
}

void medWizardOperaiontionBlock::ExcutionBegin()
{
  mafString tmpStr;
  //Ask Wizard for View
  if (m_RequiredView!="")
  {
    tmpStr=m_RequiredView;
    mafEventMacro(mafEvent(this,WIZARD_REQUIRED_VIEW,&tmpStr));
  }

  //Select the input VME for the operation
  if (m_SelectedVME)
    m_SelectedVME=m_SelectedVME->GetByPath(m_VmeSelect.c_str());
  
  if (m_SelectedVME)
  {
    mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
  }
  else 
    mafLogMessage("Wizard Error: unable to select VME, path:\"%s\"",m_VmeSelect.c_str());

  //////////////////////////  
  //Show the required VMEs

  //If there is no view required we don't show any vme
  if (m_RequiredView != "")
  {
    mafEventMacro(mafEvent(this,VME_SHOW,m_SelectedVME,true));

    for(int i=0;i<m_VmeShow.size();i++)
    {
      mafNode *toShow=m_SelectedVME->GetByPath(m_VmeShow[i].c_str());
      if (toShow != NULL)
        mafEventMacro(mafEvent(this,VME_SHOW,toShow,true));
    }
  }
  
  //Run Operation
  if (m_Operation!="")
  {
    tmpStr=m_Operation;
    mafEventMacro(mafEvent(this,WIZARD_RUN_OP,&tmpStr));
  }
}

void medWizardOperaiontionBlock::ExcutionEnd()
{

  //Hide the required VMEs
  for(int i=0;i<m_VmeHide.size();i++)
  {
    mafNode *toHide=m_SelectedVME->GetByPath(m_VmeHide[i]);
    mafEventMacro(mafEvent(this,VME_SHOW,toHide,false));
  }
  
}

wxString medWizardOperaiontionBlock::GetRequiredOperation()
{
  return m_Operation;
}

void medWizardOperaiontionBlock::SetRequiredOperation( const const char *name )
{
  m_Operation=name;
}



