/*=========================================================================

 Program: MAF2
 Module: mafOpImporterImageTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpImporterImageTest.h"
#include "mafOpImporterImage.h"
#include "mafString.h"

#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVME.h"
#include "mafVMEImage.h"
#include "mafNodeIterator.h"


#include "vtkDataSet.h"
#include "vtkImageData.h"

#include <iostream>

void mafOpImporterImageTest::Test_Single()
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterImage *importer=new mafOpImporterImage("importer");
	importer->TestModeOn();
  importer->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
  filename<<"/Image/imageTest.jpg";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	mafNode *node=importer->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("mafVMEImage"));
	vtkDataSet *data=((mafVME *)node)->GetOutput()->GetVTKData();
	data->Update();
  mafString name = node->GetName();
  CPPUNIT_ASSERT(!name.Compare("imageTest")); //compare returns 0 if equal

  int dim[3];
  ((vtkImageData *) data)->GetDimensions(dim);

   CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 1); 

	 mafDEL(importer);
	 mafDEL(storage);
  
}
void mafOpImporterImageTest::Test_Multi_No_Volume()
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterImage *importer=new mafOpImporterImage("importer");
	importer->TestModeOn();
  importer->SetInput(storage->GetRoot());

	mafString filename=MAF_DATA_ROOT;
  filename<<"/Image/pre0.jpg";
	importer->SetFileName(filename.GetCStr());
	
  filename=MAF_DATA_ROOT;
  filename<<"/Image/pre1.jpg";
	importer->SetFileName(filename.GetCStr());

  importer->OpRun();
  importer->ImportImage();
	mafNode *node=importer->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("mafVMEImage"));
	vtkDataSet *data=((mafVME *)node)->GetOutput()->GetVTKData();
	data->Update();
  mafString name = node->GetName();
  CPPUNIT_ASSERT(!name.Compare("Imported Images")); //compare returns 0 if equal*/

  int dim[3];
  ((vtkImageData *) data)->GetDimensions(dim);

   CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 1); 
	
	 mafDEL(importer);
	 mafDEL(storage);

}

void mafOpImporterImageTest::Test_Multi_Volume()
{
  
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterImage *importer=new mafOpImporterImage("importer");
	importer->TestModeOn();
  importer->SetInput(storage->GetRoot());

	mafString filename=MAF_DATA_ROOT;
  filename<<"/Image/pre0.jpg";
	importer->SetFileName(filename.GetCStr());
	
  filename=MAF_DATA_ROOT;
  filename<<"/Image/pre1.jpg";
	importer->SetFileName(filename.GetCStr());
  

  importer->OpRun();
  importer->SetBuildVolumeFlag(true);
  importer->SetFileExtension("jpg");
  importer->SetFileOffset(0);
  importer->SetFilePattern("%s%d");
  importer->SetFilePrefix("pre");
  importer->SetFileSpacing(1);
  importer->SetImageZSpacing(1);

  
  importer->ImportImage();
	mafNode *node=importer->GetOutput();
	
	CPPUNIT_ASSERT(node->IsA("mafVMEVolumeRGB"));
	vtkDataSet *data=((mafVME *)node)->GetOutput()->GetVTKData();
	data->Update();
  mafString name = node->GetName();
  CPPUNIT_ASSERT(!name.Compare("Imported Volume")); //compare returns 0 if equal

  int dim[3];
  ((vtkImageData *) data)->GetDimensions(dim);
  
   CPPUNIT_ASSERT(dim[0] == 600 && dim[1] == 344 && dim[2] == 2); 

	 mafDEL(importer);
	 mafDEL(storage);

}

