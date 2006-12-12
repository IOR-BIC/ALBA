/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoRAWImporterImagesTest.cpp,v $
Language:  C++
Date:      $Date: 2006-12-12 16:04:17 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoRAWImporterImagesTest.h"
#include "mmoRAWImporterImages.h"

#include "mafVMEStorage.h"
#include "mafVMERoot.h"

#include "vtkDataSet.h"
#include "vtkRectilinearGrid.h"

//-----------------------------------------------------------
void mmoRAWImporterImagesTest::Test() 
//-----------------------------------------------------------
{
	mafVMEStorage *Storage = mafVMEStorage::New();
  Storage->GetRoot()->SetName("root");
  Storage->GetRoot()->Initialize();

	mmoRAWImporterImages *Importer=new mmoRAWImporterImages("importer");
	Importer->TestModeOn();
  Importer->SetInput(Storage->GetRoot());
	mafString Directory=MED_DATA_ROOT;
  Directory<<"/RAW_Images";
	Importer->CreatePipeline();
	Importer->SetRawDirectory(Directory);
	Importer->OnOpenDir();
	Importer->SetSigned(true);
	Importer->SetDataType(2);
	mafString FileZ=MED_DATA_ROOT;
	FileZ<<"/RAW_Images/IMG.txt";
	Importer->SetCoordFile(FileZ.GetCStr());
	mafString Prefix="IMG_512x512_";
	Importer->SetStringPrefix(Prefix.GetCStr());
	Importer->OnStringPrefix();
	Importer->CreateGui();

	mafVME *VME=mafVME::SafeDownCast(Importer->GetOutput());
	VME->Update();

	CPPUNIT_ASSERT(VME->IsA("mafVMEVolumeGray"));
	vtkRectilinearGrid *Data=vtkRectilinearGrid::SafeDownCast(VME->GetOutput()->GetVTKData());
	Data->UpdateData();

	double sr[2];
	Data->GetScalarRange(sr);
	CPPUNIT_ASSERT(sr[0]==-3024 && sr[1]==1808);

	double b[6];
	Data->GetBounds(b);
	CPPUNIT_ASSERT(b[4]==-109.1 && b[5]==-96.1 && b[0]==0.0 && b[1]==511.0 && b[2]==0.0 && b[3]==511.0);

	VME = NULL;
	mafDEL(Importer);
	mafDEL(Storage);
}