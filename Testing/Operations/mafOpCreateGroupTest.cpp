/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCreateGroupTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 12:01:16 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpCreateGroupTest.h"

#include "mafOpCreateGroup.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafOpImporterVTK.h"
#include "mafVMEVolumeGray.h"

//-----------------------------------------------------------
void mafOpCreateGroupTest::Test() 
//-----------------------------------------------------------
{
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	//Create operation
	mafOpImporterVTK *Importer=new mafOpImporterVTK("importer");
	mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_VOI/volume.vtk";
	Importer->TestModeOn();
	Importer->SetFileName(filename);
	Importer->SetInput(storage->GetRoot());
	Importer->ImportVTK();
	mafSmartPointer<mafVMEVolumeGray>Data=mafVMEVolumeGray::SafeDownCast(Importer->GetOutput());
	Data->Update();

	mafOpCreateGroup *CreateGroup=new mafOpCreateGroup("Create Group");
	CreateGroup->SetInput(Data);
	CreateGroup->OpRun();

	//Check if VME created is a group
	CPPUNIT_ASSERT(CreateGroup->GetOutput()->IsA("mafVMEGroup"));

	mafDEL(CreateGroup);
	mafDEL(Importer);
	mafDEL(storage);
}
