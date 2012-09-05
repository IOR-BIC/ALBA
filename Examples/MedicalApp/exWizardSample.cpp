/*=========================================================================

 Program: MAF2Medical
 Module: medOpInteractionOp
 Authors: Marco Petrone
 
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

#include "exWizardSample.h"
#include "medWizardSelectionBlock.h"
#include "medWizardOperationBlock.h"


//--------------------------------------------------------------------------------
exWizardSample::exWizardSample( const wxString &label ):medWizard(label)
//--------------------------------------------------------------------------------
{
  
  //Start Block (import a VTK file)   
  medWizardOperaiontionBlock *opBlock=new medWizardOperaiontionBlock("START");
  opBlock->SetRequiredView("");
  opBlock->VmeSelect("root");
  opBlock->SetRequiredOperation("mafOpImporterVTK");
  opBlock->SetNextBlock("opMove");

  //Move block (position the Block)
  medWizardOperaiontionBlock *opBlock2=new medWizardOperaiontionBlock("opMove");
  opBlock2->SetRequiredView("VTK view");
  opBlock2->VmeSelect("lastChild");
  opBlock2->SetRequiredOperation("medOpMove");
  opBlock2->SetNextBlock("Select");
  
  //Select Block ask user if want to import another file
  medWizardSelectionBlock *selectorBlock=new medWizardSelectionBlock("Select");
  selectorBlock->SetWindowTitle("Test Selector");
  selectorBlock->SetDescription("Wizard Selector Test window");
  selectorBlock->AddChoice("Import another VTK","START");
  selectorBlock->AddChoice("End Wizard", "END");

  //add blocks to the wizard
  AddBlock(selectorBlock);
  AddBlock(opBlock);
  AddBlock(opBlock2);
}

//--------------------------------------------------------------------------------
bool exWizardSample::Accept(mafNode* vme)
//--------------------------------------------------------------------------------
{
  return true;
}

