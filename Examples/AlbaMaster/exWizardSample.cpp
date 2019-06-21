/*=========================================================================
Program:   AlbaMaster
Module:    albaOpInteractionOp.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in ALBA must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "exWizardSample.h"
#include "albaWizardBlockSelection.h"
#include "albaWizardBlockOperation.h"
#include "albaWizardBlockInformation.h"


//--------------------------------------------------------------------------------
exWizardSample::exWizardSample( const wxString &label ):albaWizard(label)
{
	albaWizardBlockInformation *infoBlock=new albaWizardBlockInformation("START");
  infoBlock->SetBoxLabel("Info");
  infoBlock->SetDescription("Import Loop:\nYou can import and show a series of VTK files.");
  infoBlock->SetWindowTitle("information");
  infoBlock->SetNextBlock("Import");
  infoBlock->SetBlockProgress(10);
  
  //Start Block (import a VTK file)   
  albaWizardBlockOperation *opBlock=new albaWizardBlockOperation("Import");
  opBlock->SetRequiredView("");
  opBlock->VmeSelect("root");
  opBlock->SetRequiredOperation("albaOpImporterVTK");
  opBlock->SetNextBlock("opPause");
  opBlock->SetBlockProgress(25);

  //Move block (position the Block)
  albaWizardBlockOperation *opBlock2=new albaWizardBlockOperation("opPause");
  opBlock2->SetRequiredView("VTK view");
  opBlock2->VmeSelect("lastChild");
  opBlock2->SetRequiredOperation("PAUSE");
  opBlock2->SetNextBlock("Select");
  opBlock2->SetBlockProgress(75);
  
  //Select Block ask user if want to import another file
  albaWizardBlockSelection *selectorBlock=new albaWizardBlockSelection("Select");
  selectorBlock->SetWindowTitle("Test Selector");
  selectorBlock->SetDescription("Wizard Selector Test window");
  selectorBlock->AddChoice("Import another VTK","deleteBlock"); 
  selectorBlock->AddChoice("End Wizard", "END");
  selectorBlock->SetBlockProgress(100);

  //Move block (position the Block)
  albaWizardBlockOperation *deleteBlock=new albaWizardBlockOperation("deleteBlock");
  deleteBlock->SetRequiredOperation("DELETE");
  deleteBlock->SetNextBlock("Import");
	
  //Add blocks to the wizard
  AddBlock(infoBlock);
  AddBlock(selectorBlock);
  AddBlock(opBlock);
  AddBlock(opBlock2);
  AddBlock(deleteBlock);

  ShowProgressBar();
}

//--------------------------------------------------------------------------------
bool exWizardSample::Accept(albaVME* vme)
{
  return true;
}

