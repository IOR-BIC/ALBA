/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterRAWVolumeTest
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

#include "albaOpImporterRAWVolumeTest.h"
#include "albaOpImporterRAWVolume.h"

#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVME.h"

#include "vtkImageData.h"

//-----------------------------------------------------------
void albaOpImporterRAWVolumeTest::Test() 
//-----------------------------------------------------------
{
	albaVMEStorage *Storage = albaVMEStorage::New();
  Storage->GetRoot()->SetName("root");
  Storage->GetRoot()->Initialize();

	albaOpImporterRAWVolume *Importer=new albaOpImporterRAWVolume("importer");
	Importer->TestModeOn();
  Importer->SetInput(Storage->GetRoot());
	albaString Filename=ALBA_DATA_ROOT;
  Filename<<"/RAW_Volume/Volume.raw";
	Importer->SetFileName(Filename.GetCStr());
	int Dimensions[3]={512,512,5};
	Importer->SetDataDimensions(Dimensions);
	double Spacing[3]={0.2,0.3,1.0};
	Importer->SetDataSpacing(Spacing);
	Importer->SetScalarType(Importer->SHORT_SCALAR);
	Importer->ScalarSignedOn();
	Importer->SetScalarDataToLittleEndian();
	int VOI[2]={0,5};
	Importer->SetDataVOI(VOI);
	bool ok=Importer->Import();
	CPPUNIT_ASSERT(ok);

	albaVME *VME=Importer->GetOutput();
	vtkImageData *Data=vtkImageData::SafeDownCast(VME->GetOutput()->GetVTKData());
	Data->ComputeBounds();
	CPPUNIT_ASSERT(Data);

	//Check type scalar
	CPPUNIT_ASSERT(Data->GetScalarType()==VTK_SHORT);

	//Check spacing
	double NewSpacing[3];
	Data->GetSpacing(NewSpacing);
	CPPUNIT_ASSERT(NewSpacing[0]==Spacing[0]);
	CPPUNIT_ASSERT(NewSpacing[1]==Spacing[1]);
	CPPUNIT_ASSERT(NewSpacing[2]==Spacing[2]);

	//Check number of points
	int NewDimensions[3];
	Data->GetDimensions(NewDimensions);
	CPPUNIT_ASSERT(NewDimensions[0]==Dimensions[0]);
	CPPUNIT_ASSERT(NewDimensions[1]==Dimensions[1]);
	CPPUNIT_ASSERT(NewDimensions[2]==VOI[1]-VOI[0]);

	cppDEL(Importer);
	albaDEL(Storage);
}
