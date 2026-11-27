/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardOperaiontionBlock
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
#include "albaWizardBlockOperation.h"
#include "albaVME.h"
#include "albaEvent.h"
#include "albaWizard.h"
#include "albaWizardManager.h"

//----------------------------------------------------------------------------
albaWizardBlockOperation::albaWizardBlockOperation(const char *name):albaWizardBlock(name)
//----------------------------------------------------------------------------
{
  //Default constructor
  m_AutoShowSelectedVME=true;
  m_viewtodelete = "";
  m_viewhastobedeleted = false;
}

//----------------------------------------------------------------------------
albaWizardBlockOperation::~albaWizardBlockOperation()
//----------------------------------------------------------------------------
{
  //Clearing the list of to show and to hide VME
  m_VmeShow.clear();
  m_VmeHide.clear();
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetRequiredView( const char *View )
//----------------------------------------------------------------------------
{
  //setting required view name 
  m_RequiredView = View;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetViewToDelete( const char *View )
	//----------------------------------------------------------------------------
{
  m_viewhastobedeleted = true;
  m_viewtodelete = View;
}

//----------------------------------------------------------------------------
wxString albaWizardBlockOperation::GetRequiredView()
//----------------------------------------------------------------------------
{
  //return the required view
  return m_RequiredView;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::VmeSelect( const char *path )
//----------------------------------------------------------------------------
{
  //Set the path of the vme which was selected before operation start
  m_VmeSelect=path;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::VmeShowAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme showed before operation start
  wxString wxPath;
  wxPath=path;
  m_VmeShow.push_back(wxPath);
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::VmeHideAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme hided after operation end
  wxString wxPath;
  wxPath=path;
  m_VmeHide.push_back(wxPath);
}


//----------------------------------------------------------------------------
void albaWizardBlockOperation::ExcutionBegin()
//----------------------------------------------------------------------------
{
  albaWizardBlock::ExcutionBegin();

  ///////////////////////
  //Ask Wizard for View
  if (m_RequiredView!="")
  {
    //open or select the required view
    GetViewManager()->ViewOpen(m_RequiredView.ToAscii());
  }

  ///////////////////////
  //Select the input VME for the operation
  if (m_SelectedVME)
    m_SelectedVME=m_SelectedVME->GetByPath(m_VmeSelect.ToAscii());
  
  if (m_SelectedVME)
  {
    //forward up vme selection event 
    //for view/logic/operation update
		GetLogicManager()->VmeSelect(m_SelectedVME);
  }
  else 
  {
    //If we cannot select the correct vme we need to abort the wizard
    albaLogMessage("Wizard Error: unable to select VME, path:\"%s\" base:\"%s\"",m_VmeSelect.ToAscii(),m_SelectedVME->GetName());
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
    if (m_AutoShowSelectedVME)
			GetLogicManager()->VmeShow(m_SelectedVME, true);

    for(int i=0;i<m_VmeShow.size();i++)
    {
      //detecting all other vme starting on selected vme and show it
      albaVME *toShow=m_SelectedVME->GetByPath(m_VmeShow[i].ToAscii());
      if (toShow != NULL)
        GetLogicManager()->VmeShow(toShow, true);
    }
  }
  
  //////////////////////////  
  //Run Operation
  if (m_Operation!="")
  {
    albaString tmpStr = m_Operation;
    //ask logic for operation run the flow will continue after operation stop
    if (m_Wizard && m_Wizard->GetManager())
      m_Wizard->GetManager()->RunOp(tmpStr.GetCStr());
  }
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::ExcutionEnd()
//----------------------------------------------------------------------------
{
  albaWizardBlock::ExcutionEnd();
  //////////////////////////  
  //Hide the required VMEs
  for(int i=0;i<m_VmeHide.size();i++)
  {
    albaVME *toHide=m_SelectedVME->GetByPath(m_VmeHide[i]);
    GetLogicManager()->VmeShow(toHide, false);
  }

  if(m_viewhastobedeleted) 
  {
      GetViewManager()->ViewClose(m_viewtodelete.GetCStr());
  }
 
}

//----------------------------------------------------------------------------
wxString albaWizardBlockOperation::GetRequiredOperation()
//----------------------------------------------------------------------------
{
  //return the name of required operation
  return m_Operation;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetRequiredOperation( const char *name )
//----------------------------------------------------------------------------
{
  //set the name of required operation
  m_Operation=name;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetAutoShowSelectedVME( bool autoShow )
//----------------------------------------------------------------------------
{
  m_AutoShowSelectedVME=autoShow;
}

//----------------------------------------------------------------------------
bool albaWizardBlockOperation::GetAutoShowSelectedVME()
//----------------------------------------------------------------------------
{
  return m_AutoShowSelectedVME;
}




