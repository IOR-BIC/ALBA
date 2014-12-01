/*=========================================================================

 Program: MAF2Medical
 Module: mafOpImporterRAWImagesTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterRAWImagesTest.h"
#include "mafOpImporterRAWImages.h"

#include "mafVMEStorage.h"
#include "mafVMERoot.h"

#include "vtkDataSet.h"
#include "vtkRectilinearGrid.h"

//-----------------------------------------------------------
void mafOpImporterRAWImagesTest::Test() 
//-----------------------------------------------------------
{
	mafVMEStorage *Storage = mafVMEStorage::New();
  Storage->GetRoot()->SetName("root");
  Storage->GetRoot()->Initialize();

	mafOpImporterRAWImages *Importer=new mafOpImporterRAWImages("importer");
	Importer->TestModeOn();
  Importer->SetInput(Storage->GetRoot());
	mafString Directory=MAF_DATA_ROOT;
  Directory<<"/RAW_Images";
	Importer->CreatePipeline();
	Importer->SetRawDirectory(Directory);
	Importer->OnOpenDir();
	Importer->SetSigned(true);
	Importer->SetDataType(2);
	mafString FileZ=MAF_DATA_ROOT;
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
