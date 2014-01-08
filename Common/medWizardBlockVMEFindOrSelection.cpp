/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlockVMEFindOrSelection
 Authors: Simone Bnà
 
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
#include "mafNode.h"
#include "medWizardBlockVMEFindOrSelection.h"

wxString globalVMEAccept;


//----------------------------------------------------------------------------
medWizardBlockVMEFindOrSelection::medWizardBlockVMEFindOrSelection(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting default values
}

//----------------------------------------------------------------------------
medWizardBlockVMEFindOrSelection::~medWizardBlockVMEFindOrSelection()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medWizardBlockVMEFindOrSelection::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
void medWizardBlockVMEFindOrSelection::ExcutionBegin()
//----------------------------------------------------------------------------
{
  mafNode *selVME;
  medWizardBlock::ExcutionBegin();

  //Setting global variable witch can be referred by static function
  globalVMEAccept=m_AcceptedVME;

  if (m_SelectedVME)
	  m_SelectedVME=m_SelectedVME->GetByPath(m_VmeParentSelect.c_str());

  if (m_SelectedVME)
  {
	  const mafNode::mafChildrenVector *childs;
	  childs=m_SelectedVME->GetChildren();
	  int nnodes=m_SelectedVME->GetNumberOfChildren();
	  int VMENumber=0;
	  int VMEIndex;

	  for (int i=0;i<nnodes;i++)
	  {
		  mafNode *child=(*childs)[i];
		  if (VMEAccept(child))
		  {
			  VMENumber++;
			  VMEIndex = i;
		  }
	  }

	  // if there are many acceptable volume we tell the user to select it
	  if (VMENumber > 1)
	  {
		  mafString title = m_Title.c_str();
		  mafEvent e(this,VME_CHOOSE);
		  e.SetString(&title);
		  e.SetArg((long)(&VMEAccept)); 
		  // accept only Specified VME
		  mafEventMacro(e);
		  selVME=e.GetVme();

		  if(selVME)
		  {
			  //Select vme 
			  m_SelectedVME=selVME;
			  mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
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
			  mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
		  }
	  }
	  else {
		  //Abort on user cancel
		  Abort();
	  }
  }

}

//----------------------------------------------------------------------------
void medWizardBlockVMEFindOrSelection::VmeParentSelect( const char *path )
//----------------------------------------------------------------------------
{
	//Set the path of the parent vme of a list of childs
	//The wizard block will look for childs of correct type
	m_VmeParentSelect=path;
}

//----------------------------------------------------------------------------
int medWizardBlockVMEFindOrSelection::VMEAccept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA(globalVMEAccept));
}

//----------------------------------------------------------------------------
void medWizardBlockVMEFindOrSelection::SetAcceptedVME( const char *VME )
//----------------------------------------------------------------------------
{
  m_AcceptedVME=VME;
}