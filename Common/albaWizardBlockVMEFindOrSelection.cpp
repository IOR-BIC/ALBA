/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockVMEFindOrSelection
 Authors: Simone Bnà
 
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
#include "albaVME.h"
#include "albaWizardBlockVMEFindOrSelection.h"

wxString globalVMEAccept;


//----------------------------------------------------------------------------
albaWizardBlockVMEFindOrSelection::albaWizardBlockVMEFindOrSelection(const char *name):albaWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting default values
}

//----------------------------------------------------------------------------
albaWizardBlockVMEFindOrSelection::~albaWizardBlockVMEFindOrSelection()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaWizardBlockVMEFindOrSelection::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
void albaWizardBlockVMEFindOrSelection::ExcutionBegin()
//----------------------------------------------------------------------------
{
  albaVME *selVME;
  albaWizardBlock::ExcutionBegin();

  //Setting global variable witch can be referred by static function
  globalVMEAccept=m_AcceptedVME;

  if (m_SelectedVME)
	  m_SelectedVME=m_SelectedVME->GetByPath(m_VmeParentSelect.c_str());

  if (m_SelectedVME)
  {
	  const albaVME::albaChildrenVector *childs;
	  childs=m_SelectedVME->GetChildren();
	  int nnodes=m_SelectedVME->GetNumberOfChildren();
	  int VMENumber=0;
	  int VMEIndex;

	  for (int i=0;i<nnodes;i++)
	  {
		  albaVME *child=(*childs)[i];
		  if (VMEAccept(child))
		  {
			  VMENumber++;
			  VMEIndex = i;
		  }
	  }

	  // if there are many acceptable volume we tell the user to select it
	  if (VMENumber > 1)
	  {
		  albaString title = m_Title;
		  albaEvent e(this,VME_CHOOSE);
		  e.SetString(&title);
		  e.SetPointer(&VMEAccept); 
		  // accept only Specified VME
		  albaEventMacro(e);
		  selVME=e.GetVme();

		  if(selVME)
		  {
			  //Select vme 
			  m_SelectedVME=selVME;
			  albaEventMacro(albaEvent(this,VME_SELECT,m_SelectedVME));
		  }
	  }
	  // if there is only one acceptable volume we select it
	  else if (VMENumber == 1)
	  {
		  selVME = (*childs)[VMEIndex];
		  if (VMEAccept(selVME))
		  {
			  //Select vme 
			  m_SelectedVME=selVME;
			  albaEventMacro(albaEvent(this,VME_SELECT,m_SelectedVME));
		  }
	  }
	  else {
		  //Abort on user cancel
		  Abort();
	  }
  }

}

//----------------------------------------------------------------------------
void albaWizardBlockVMEFindOrSelection::VmeParentSelect( const char *path )
//----------------------------------------------------------------------------
{
	//Set the path of the parent vme of a list of childs
	//The wizard block will look for childs of correct type
	m_VmeParentSelect=path;
}

//----------------------------------------------------------------------------
int albaWizardBlockVMEFindOrSelection::VMEAccept(albaVME *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA(globalVMEAccept));
}

//----------------------------------------------------------------------------
void albaWizardBlockVMEFindOrSelection::SetAcceptedVME( const char *VME )
//----------------------------------------------------------------------------
{
  m_AcceptedVME=VME;
}