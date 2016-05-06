/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMSFTest
 Authors: Roberto Mucci
 
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

#include "mafOpImporterMSFTest.h"
#include "mafVMEFactory.h"
#include "mafOpImporterMSF.h"
#include "mafString.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEIterator.h"
#include <iostream>

//----------------------------------------------------------------------------
void mafOpImporterMSFTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpImporterMSFTest::TestConstructor()
//----------------------------------------------------------------------------
{
  //check memory leaks mad by constructor
  mafOpImporterMSF *importerMSF = new mafOpImporterMSF("importer MSF");
  mafDEL(importerMSF);
}

//----------------------------------------------------------------------------
void mafOpImporterMSFTest::TestImporter()
//----------------------------------------------------------------------------
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

	mafOpImporterMSF *importer=new mafOpImporterMSF("importer");
	importer->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_MSFImporter/Test_MSFImporter.msf";
  importer->TestModeOn();
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

  mafVME *node = importer->GetOutput();

  //check the name of the Group imported by the importerMSF
  mafString name = node->GetName();
  CPPUNIT_ASSERT(name.Compare("imported from Test_MSFImporter.msf") == 0);
  
  //Check the number and type of children nodes
	int i = node->GetNumberOfChildren();
	CPPUNIT_ASSERT(i == 5); 

	for(int j=0;j<i;j++)
	{
		mafVME *child = node->GetChild(j);
		switch (j)
		{
		case 0:
			CPPUNIT_ASSERT(child->IsA("mafVMEVolumeGray"));
			break;
		case 1:
			CPPUNIT_ASSERT(child->IsA("mafVMEMesh"));
      break;
    case 2:
      CPPUNIT_ASSERT(child->IsA("mafVMESurface"));
      break;
    case 3:
      CPPUNIT_ASSERT(child->IsA("mafVMEVector"));
      break;
    case 4:
      CPPUNIT_ASSERT(child->IsA("mafVMELandmarkCloud"));
      break;
   		default:
			CPPUNIT_ASSERT(false);
			break;
		}
    
    if(child->IsA("mafVMEVolumeGray"))
		{
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_volume") == 0);
			vtkDataSet *data=child->GetOutput()->GetVTKData();
			data->Update();
			double range[2];

      //check the range of the volume
			data->GetScalarRange(range);
			CPPUNIT_ASSERT(range[0]==-137 && range[1]==1738);

      //check the number of children
			int n=child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
    if(child->IsA("mafVMEMesh"))
    {
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_mesh_tetra10") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      data->Update();

      //check the number of cells
      int cells=data->GetNumberOfCells();
      CPPUNIT_ASSERT(cells==2);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
		if(child->IsA("mafVMESurface"))
		{
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_surface") == 0);
			vtkDataSet *data=child->GetOutput()->GetVTKData();
			data->Update();

      //check the number of triangles of the surface
			int cells=data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells==12);
			int n=child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
    if(child->IsA("mafVMEVector"))
    {
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_vector_GRFtimevar") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      data->Update();

      //check the length of the vector
      double length = data->GetLength();
      CPPUNIT_ASSERT(fabs(length - 824.56) < 0.01);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("mafVMELandmarkCloud"))
    {
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_ landmark cloud") == 0);
      mafVMELandmarkCloud *lmCloud = (mafVMELandmarkCloud *)child;

      //check the number children of the cloud
      int n = lmCloud->GetNumberOfLandmarks();
      CPPUNIT_ASSERT(n==1);
      double xyz[3], rot[3];
      mafVMELandmark *lm = lmCloud->GetLandmark(0);

      //check the name of the landmark
      name = lm->GetName();
      CPPUNIT_ASSERT(name.Compare("test_landmark") == 0);

      //check the position of the landmark
      lm->GetOutput()->GetPose(xyz,rot);
      CPPUNIT_ASSERT( fabs(xyz[0] - 0.0) < 0.01 &&
        fabs(xyz[1] - 50.0) < 0.01 &&
        fabs(xyz[2] - 100.0) < 0.01 );
    }
  }
  mafDEL(storage);
  mafDEL(importer);
}
  
//----------------------------------------------------------------------------
void mafOpImporterMSFTest::TestImporterZMSF()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafOpImporterMSF *importer=new mafOpImporterMSF("importer");
  importer->SetInput(storage->GetRoot());
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_MSFImporterZMSF.zmsf";
  importer->TestModeOn();
  importer->SetFileName(filename.GetCStr());
  importer->OpRun();

  mafVME *node = importer->GetOutput();

  mafString name = node->GetName();
  CPPUNIT_ASSERT(name.Compare("imported from Test_MSFImporterZMSF.zmsf") == 0);

  int i = node->GetNumberOfChildren();
  CPPUNIT_ASSERT(i == 5); 

  for(int j=0;j<i;j++)
  {
    mafVME *child = node->GetChild(j);
    switch (j)
    {
    case 0:
      CPPUNIT_ASSERT(child->IsA("mafVMEVolumeGray"));
      break;
    case 1:
      CPPUNIT_ASSERT(child->IsA("mafVMEMesh"));
      break;
    case 2:
      CPPUNIT_ASSERT(child->IsA("mafVMESurface"));
      break;
    case 3:
      CPPUNIT_ASSERT(child->IsA("mafVMEVector"));
      break;
    case 4:
      CPPUNIT_ASSERT(child->IsA("mafVMELandmarkCloud"));
      break;
    default:
      CPPUNIT_ASSERT(false);
      break;
    }

    if(child->IsA("mafVMEVolumeGray"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_volume") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      data->Update();
      double range[2];
      data->GetScalarRange(range);
      CPPUNIT_ASSERT(range[0]==-137 && range[1]==1738);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("mafVMEMesh"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_mesh_tetra10") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      data->Update();
      int cells=data->GetNumberOfCells();
      CPPUNIT_ASSERT(cells==2);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("mafVMESurface"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_surface") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      data->Update();
      int cells=data->GetNumberOfCells();
      CPPUNIT_ASSERT(cells==12);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("mafVMEVector"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_vector_GRFtimevar") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      data->Update();
      double length = data->GetLength();
      CPPUNIT_ASSERT(fabs(length - 824.56) < 0.01);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("mafVMELandmarkCloud"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_ landmark cloud") == 0);
      mafVMELandmarkCloud *lmCloud = (mafVMELandmarkCloud *)child;
      int n = lmCloud->GetNumberOfLandmarks();
      CPPUNIT_ASSERT(n==1);
      double xyz[3], rot[3];
      mafVMELandmark *lm = lmCloud->GetLandmark(0);
      name = lm->GetName();
      CPPUNIT_ASSERT(name.Compare("test_landmark") == 0);
      lm->GetOutput()->GetPose(xyz,rot);
      CPPUNIT_ASSERT( fabs(xyz[0] - 0.0) < 0.01 &&
        fabs(xyz[1] - 50.0) < 0.01 &&
        fabs(xyz[2] - 100.0) < 0.01 );
    }
  }
  mafDEL(storage);
  mafDEL(importer);
}
//----------------------------------------------------------------------------
void mafOpImporterMSFTest::TestImportedVmeId()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafOpImporterMSF *importer1=new mafOpImporterMSF("importer");
  importer1->SetInput(storage->GetRoot());
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_MSFImporter/Test_MSFImporter.msf";
  importer1->TestModeOn();
  importer1->SetFileName(filename.GetCStr());
  importer1->OpRun();

  mafOpImporterMSF *importer2=new mafOpImporterMSF("importer");
  importer2->SetInput(storage->GetRoot());
  importer2->TestModeOn();
  importer2->SetFileName(filename.GetCStr());
  // Import twice the same MSF
  importer2->OpRun();

  mafVMERoot *root = storage->GetRoot();
  CPPUNIT_ASSERT(root->GetNumberOfChildren()==2);

  std::vector<mafID> vectorID;

  mafVMEIterator *iter = root->NewIterator();
  for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    mafID lastId = node->GetId();
    for (int i = 0; i < vectorID.size(); i++)
    {
      CPPUNIT_ASSERT(vectorID[i] != lastId);
    }
    vectorID.push_back(lastId);
  }
  iter->Delete();

  mafDEL(storage);
  mafDEL(importer1);
  mafDEL(importer2);
}
