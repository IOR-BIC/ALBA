/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterImageTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaOpImporterImageTest.h"
#include "albaOpImporterImage.h"
#include "albaString.h"

#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaVMEIterator.h"


#include "vtkDataSet.h"
#include "vtkImageData.h"

#include <iostream>

void albaOpImporterImageTest::Test_Single()
{
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	albaOpImporterImage *importer=new albaOpImporterImage("importer");
	importer->TestModeOn();
  importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/Image/imageTest.jpg";
	importer->AddFileName(filename.GetCStr());
	importer->OpRun();
	albaVME *node=importer->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("albaVMEGroup"));
	CPPUNIT_ASSERT(node->GetNumberOfChildren() == 1);

  albaVMEImage* img;
	img = albaVMEImage::SafeDownCast(node->GetChild(0));

	vtkDataSet* data = img->GetOutput()->GetVTKData();


	data->Update();
  albaString name = img->GetName();
  CPPUNIT_ASSERT(!name.Compare("imageTest")); //compare returns 0 if equal

  int dim[3];
  ((vtkImageData *) data)->GetDimensions(dim);

   CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 1); 

	 albaDEL(importer);
	 albaDEL(storage);
  
}
void albaOpImporterImageTest::Test_Multi_No_Volume()
{
  albaVMEStorage* storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterImage* importer = new albaOpImporterImage("importer");
  importer->TestModeOn();
  importer->SetInput(storage->GetRoot());

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Image/pre0.jpg";
  importer->AddFileName(filename.GetCStr());

  filename = ALBA_DATA_ROOT;
  filename << "/Image/pre1.jpg";
  importer->AddFileName(filename.GetCStr());

  importer->OpRun();
  importer->Import();
  albaVME* node = importer->GetOutput();

  CPPUNIT_ASSERT(node->IsA("albaVMEGroup"));
  CPPUNIT_ASSERT(node->GetNumberOfChildren() == 2);

  albaVMEImage *img1, *img2;
  img1 = albaVMEImage::SafeDownCast(node->GetChild(0));
  img2 = albaVMEImage::SafeDownCast(node->GetChild(1));

  vtkDataSet* data = img1->GetOutput()->GetVTKData();
  data->Update();
  albaString name = img1->GetName();
  CPPUNIT_ASSERT(!name.Compare("pre0")); //compare returns 0 if equal*/
  int dim[3];
  ((vtkImageData*)data)->GetDimensions(dim);
  CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 1);


	data = img2->GetOutput()->GetVTKData();
	data->Update();
	name = img2->GetName();
	CPPUNIT_ASSERT(!name.Compare("pre1")); //compare returns 0 if equal*/
	((vtkImageData*)data)->GetDimensions(dim);
	CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 1);

  albaDEL(importer);
  albaDEL(storage);

}

void albaOpImporterImageTest::Test_Multi_Volume()
{

  albaVMEStorage* storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterImage* importer = new albaOpImporterImage("importer");
  importer->TestModeOn();
  importer->SetInput(storage->GetRoot());

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Image/pre0.jpg";
  importer->AddFileName(filename.GetCStr());

  filename = ALBA_DATA_ROOT;
  filename << "/Image/pre1.jpg";
  importer->AddFileName(filename.GetCStr());

  double spacing[3] = { 1,1,1 };
  importer->OpRun();
  importer->SetBuildVolumeFlag(true);
  importer->SetSpacing(spacing);


  importer->Import();
  albaVME* node = importer->GetOutput();

  CPPUNIT_ASSERT(node->IsA("albaVMEVolumeGray"));
  vtkDataSet* data = node->GetOutput()->GetVTKData();
  data->Update();
  albaString name = node->GetName();
  CPPUNIT_ASSERT(!name.Compare("Imported Volume")); //compare returns 0 if equal

  int dim[3];
  ((vtkImageData*)data)->GetDimensions(dim);

  CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 2);

  albaDEL(importer);
  albaDEL(storage);
}

