/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterRAWImagesTest
 Authors: Matteo Giacomoni
 
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

#include "albaOpImporterRAWImagesTest.h"
#include "albaOpImporterRAWImages.h"

#include "albaVMEStorage.h"
#include "albaVMERoot.h"

#include "vtkDataSet.h"
#include "vtkRectilinearGrid.h"

//-----------------------------------------------------------
void albaOpImporterRAWImagesTest::Test() 
//-----------------------------------------------------------
{
	albaVMEStorage *Storage = albaVMEStorage::New();
  Storage->GetRoot()->SetName("root");
  Storage->GetRoot()->Initialize();

	albaOpImporterRAWImages *Importer=new albaOpImporterRAWImages("importer");
	Importer->TestModeOn();
  Importer->SetInput(Storage->GetRoot());
	albaString Directory=ALBA_DATA_ROOT;
  Directory<<"/RAW_Images";
	Importer->CreatePipeline();
	Importer->SetRawDirectory(Directory);
	Importer->OnOpenDir();
	Importer->SetSigned(true);
	Importer->SetDataType(2);
	albaString FileZ=ALBA_DATA_ROOT;
	FileZ<<"/RAW_Images/IMG.txt";
	Importer->SetCoordFile(FileZ.GetCStr());
	albaString Prefix="IMG_512x512_";
	Importer->SetStringPrefix(Prefix.GetCStr());
	Importer->OnStringPrefix();
	Importer->CreateGui();

	albaVME *VME=Importer->GetOutput();
	VME->Update();

	CPPUNIT_ASSERT(VME->IsA("albaVMEVolumeGray"));
	vtkRectilinearGrid *Data=vtkRectilinearGrid::SafeDownCast(VME->GetOutput()->GetVTKData());

	double sr[2];
	Data->GetScalarRange(sr);
	CPPUNIT_ASSERT(sr[0]==-3024 && sr[1]==1808);

	double b[6];
	Data->GetBounds(b);
	CPPUNIT_ASSERT(b[4]==-109.1 && b[5]==-96.1 && b[0]==0.0 && b[1]==511.0 && b[2]==0.0 && b[3]==511.0);

	VME = NULL;
	albaDEL(Importer);
	albaDEL(Storage);
}
