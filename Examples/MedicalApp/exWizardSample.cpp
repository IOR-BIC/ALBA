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


exWizardSample::exWizardSample( const wxString &label ):medWizard(label)
{
  
  medWizardSelectionBlock *selectorBlock=new medWizardSelectionBlock("START");
  selectorBlock->SetWindowTitle("Test Selector");
  selectorBlock->SetDescription("Wizard Selector Test window");
  selectorBlock->AddChoice("Import VTK","vtkImport");
  selectorBlock->AddChoice("End Wizard", "END");
  
  medWizardOperaiontionBlock *opBlock=new medWizardOperaiontionBlock("vtkImport");
  opBlock->SetRequiredView("");
  opBlock->VmeShow("lastChild");
  opBlock->VmeSelect("");
  opBlock->SetRequiredOperation("mafOpImporterVTK");
  opBlock->VmeHide("");
  opBlock->SetNextBlock("opMove");

  medWizardOperaiontionBlock *opBlock2=new medWizardOperaiontionBlock("opMove");
  opBlock2->SetRequiredView("VTK view");
  opBlock2->VmeSelect("lastChild");
  opBlock2->SetRequiredOperation("medOpMove");
  opBlock2->VmeHide("");
  opBlock2->SetNextBlock("START");
  
  AddBlock(selectorBlock);
  AddBlock(opBlock);
  AddBlock(opBlock2);


}

bool exWizardSample::Accept(mafNode* vme)
{
  return true;
}

