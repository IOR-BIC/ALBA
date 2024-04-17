/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSFTest
 Authors: Roberto Mucci
 
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

#include "albaOpImporterMSFTest.h"
#include "albaVMEFactory.h"
#include "albaOpImporterMSF.h"
#include "albaString.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEIterator.h"
#include <iostream>

//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestConstructor()
//----------------------------------------------------------------------------
{
  //check memory leaks mad by constructor
  albaOpImporterMSF *importerMSF = new albaOpImporterMSF("importer MSF");
  albaDEL(importerMSF);
}

//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestImporter()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

	albaOpImporterMSF *importer=new albaOpImporterMSF("importer");
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_MSFImporter/Test_MSFImporter.msf";
  importer->TestModeOn();
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

  albaVME *node = importer->GetOutput();

  //check the name of the Group imported by the importerMSF
  albaString name = node->GetName();
  CPPUNIT_ASSERT(name.Compare("imported from Test_MSFImporter.msf") == 0);
  
  //Check the number and type of children nodes
	int i = node->GetNumberOfChildren();
	CPPUNIT_ASSERT(i == 5); 

	for(int j=0;j<i;j++)
	{
		albaVME *child = node->GetChild(j);
		switch (j)
		{
		case 0:
			CPPUNIT_ASSERT(child->IsA("albaVMEVolumeGray"));
			break;
		case 1:
			CPPUNIT_ASSERT(child->IsA("albaVMEMesh"));
      break;
    case 2:
      CPPUNIT_ASSERT(child->IsA("albaVMESurface"));
      break;
    case 3:
      CPPUNIT_ASSERT(child->IsA("albaVMEVector"));
      break;
    case 4:
      CPPUNIT_ASSERT(child->IsA("albaVMELandmarkCloud"));
      break;
   		default:
			CPPUNIT_ASSERT(false);
			break;
		}
    
    if(child->IsA("albaVMEVolumeGray"))
		{
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_volume") == 0);
			vtkDataSet *data=child->GetOutput()->GetVTKData();
			double range[2];

      //check the range of the volume
			data->GetScalarRange(range);
			CPPUNIT_ASSERT(range[0]==-137 && range[1]==1738);

      //check the number of children
			int n=child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
    if(child->IsA("albaVMEMesh"))
    {
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_mesh_tetra10") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();

      //check the number of cells
      int cells=data->GetNumberOfCells();
      CPPUNIT_ASSERT(cells==2);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
		if(child->IsA("albaVMESurface"))
		{
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_surface") == 0);
			vtkDataSet *data=child->GetOutput()->GetVTKData();

      //check the number of triangles of the surface
			int cells=data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells==12);
			int n=child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
    if(child->IsA("albaVMEVector"))
    {
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_vector_GRFtimevar") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();

      //check the length of the vector
      double length = data->GetLength();
      CPPUNIT_ASSERT(fabs(length - 824.56) < 0.01);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("albaVMELandmarkCloud"))
    {
      //check the name of the node
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_ landmark cloud") == 0);
      albaVMELandmarkCloud *lmCloud = (albaVMELandmarkCloud *)child;

      //check the number children of the cloud
      int n = lmCloud->GetNumberOfLandmarks();
      CPPUNIT_ASSERT(n==1);
      double xyz[3], rot[3];
      albaVMELandmark *lm = lmCloud->GetLandmark(0);

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
  albaDEL(storage);
  albaDEL(importer);
}

//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestImporterMAF()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");

	// in order to create VME from storage we need the factory to initialize 
	albaVMEFactory::Initialize();

	albaOpImporterMSF *importer = new albaOpImporterMSF("importer");
	importer->SetInput(storage->GetRoot());
	albaString filename = ALBA_DATA_ROOT;
	filename << "/Test_MSFImporter/MAF/Test_MSFImporter.msf";
	importer->TestModeOn();
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	albaVME *node = importer->GetOutput();

	//check the name of the Group imported by the importerMSF
	albaString name = node->GetName();
	CPPUNIT_ASSERT(name.Compare("imported from Test_MSFImporter.msf") == 0);

	//Check the number and type of children nodes
	int i = node->GetNumberOfChildren();
	CPPUNIT_ASSERT(i == 5);

	for (int j = 0; j < i; j++)
	{
		albaVME *child = node->GetChild(j);
		switch (j)
		{
		case 0:
			CPPUNIT_ASSERT(child->IsA("albaVMEVolumeGray"));
			break;
		case 1:
			CPPUNIT_ASSERT(child->IsA("albaVMEMesh"));
			break;
		case 2:
			CPPUNIT_ASSERT(child->IsA("albaVMESurface"));
			break;
		case 3:
			CPPUNIT_ASSERT(child->IsA("albaVMEVector"));
			break;
		case 4:
			CPPUNIT_ASSERT(child->IsA("albaVMELandmarkCloud"));
			break;
		default:
			CPPUNIT_ASSERT(false);
			break;
		}

		if (child->IsA("albaVMEVolumeGray"))
		{
			//check the name of the node
			name = child->GetName();
			CPPUNIT_ASSERT(name.Compare("test_volume") == 0);
			vtkDataSet *data = child->GetOutput()->GetVTKData();
			double range[2];

			//check the range of the volume
			data->GetScalarRange(range);
			CPPUNIT_ASSERT(range[0] == -137 && range[1] == 1738);

			//check the number of children
			int n = child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n == 0);
		}
		if (child->IsA("albaVMEMesh"))
		{
			//check the name of the node
			name = child->GetName();
			CPPUNIT_ASSERT(name.Compare("test_mesh_tetra10") == 0);
			vtkDataSet *data = child->GetOutput()->GetVTKData();

			//check the number of cells
			int cells = data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells == 2);
			int n = child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n == 0);
		}
		if (child->IsA("albaVMESurface"))
		{
			//check the name of the node
			name = child->GetName();
			CPPUNIT_ASSERT(name.Compare("test_surface") == 0);
			vtkDataSet *data = child->GetOutput()->GetVTKData();

			//check the number of triangles of the surface
			int cells = data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells == 12);
			int n = child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n == 0);
		}
		if (child->IsA("albaVMEVector"))
		{
			//check the name of the node
			name = child->GetName();
			CPPUNIT_ASSERT(name.Compare("test_vector_GRFtimevar") == 0);
			vtkDataSet *data = child->GetOutput()->GetVTKData();

			//check the length of the vector
			double length = data->GetLength();
			CPPUNIT_ASSERT(fabs(length - 824.56) < 0.01);
			int n = child->GetNumberOfChildren();
			CPPUNIT_ASSERT(n == 0);
		}
		if (child->IsA("albaVMELandmarkCloud"))
		{
			//check the name of the node
			name = child->GetName();
			CPPUNIT_ASSERT(name.Compare("test_ landmark cloud") == 0);
			albaVMELandmarkCloud *lmCloud = (albaVMELandmarkCloud *)child;

			//check the number children of the cloud
			int n = lmCloud->GetNumberOfLandmarks();
			CPPUNIT_ASSERT(n == 1);
			double xyz[3], rot[3];
			albaVMELandmark *lm = lmCloud->GetLandmark(0);

			//check the name of the landmark
			name = lm->GetName();
			CPPUNIT_ASSERT(name.Compare("test_landmark") == 0);

			//check the position of the landmark
			lm->GetOutput()->GetPose(xyz, rot);
			CPPUNIT_ASSERT(fabs(xyz[0] - 0.0) < 0.01 &&
				fabs(xyz[1] - 50.0) < 0.01 &&
				fabs(xyz[2] - 100.0) < 0.01);
		}
	}
	albaDEL(storage);
	albaDEL(importer);
}

//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestImporterCompareALBA_MAF()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");

	albaVMEStorage *storage2 = albaVMEStorage::New();
	storage2->GetRoot()->SetName("root");

	// in order to create VME from storage we need the factory to initialize 
	albaVMEFactory::Initialize();

	albaOpImporterMSF *importer = new albaOpImporterMSF("importer");
	importer->SetInput(storage->GetRoot());
	albaString filename = ALBA_DATA_ROOT;
	filename << "/Test_MSFImporter/Test_MSFImporter.msf";
	importer->TestModeOn();
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();

	albaVME *node = importer->GetOutput();

	//
	albaOpImporterMSF *importerMaf = new albaOpImporterMSF("importer");
	importerMaf->SetInput(storage2->GetRoot());
	albaString filename2 = ALBA_DATA_ROOT;
	filename2 << "/Test_MSFImporter/MAF/Test_MSFImporter.msf";
	importerMaf->TestModeOn();
	importerMaf->SetFileName(filename2.GetCStr());
	importerMaf->OpRun();

	albaVME *nodeMaf = importerMaf->GetOutput();
	
	//check the name of the Group imported by the importerMSF
	CPPUNIT_ASSERT(strcmp(node->GetName(),nodeMaf->GetName()) == 0);

	//Check the number and type of children nodes
	CPPUNIT_ASSERT(node->GetNumberOfChildren() == nodeMaf->GetNumberOfChildren());

	for (int j = 0; j < node->GetNumberOfChildren(); j++)
	{
		albaVME *child = node->GetChild(j);
		albaVME *childMaf = nodeMaf->GetChild(j);

		CPPUNIT_ASSERT(strcmp(child->GetName(), childMaf->GetName()) == 0);

		switch (j)
		{
		case 0:
			CPPUNIT_ASSERT(child->IsA("albaVMEVolumeGray") && childMaf->IsA("albaVMEVolumeGray"));
			break;
		case 1:
			CPPUNIT_ASSERT(child->IsA("albaVMEMesh") && childMaf->IsA("albaVMEMesh"));
			break;
		case 2:
			CPPUNIT_ASSERT(child->IsA("albaVMESurface") && childMaf->IsA("albaVMESurface"));
			break;
		case 3:
			CPPUNIT_ASSERT(child->IsA("albaVMEVector") && childMaf->IsA("albaVMEVector"));
			break;
		case 4:
			CPPUNIT_ASSERT(child->IsA("albaVMELandmarkCloud") && childMaf->IsA("albaVMELandmarkCloud"));
			break;
		default:
			CPPUNIT_ASSERT(false);
			break;
		}
	}

	albaDEL(storage);
	albaDEL(storage2);
	albaDEL(importer);
	albaDEL(importerMaf);
}


//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestImporterZMSF()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaOpImporterMSF *importer=new albaOpImporterMSF("importer");
  importer->SetInput(storage->GetRoot());
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_MSFImporterZMSF.zmsf";
  importer->TestModeOn();
  importer->SetFileName(filename.GetCStr());
  importer->OpRun();

  albaVME *node = importer->GetOutput();

  albaString name = node->GetName();
  CPPUNIT_ASSERT(name.Compare("imported from Test_MSFImporterZMSF.zmsf") == 0);

  int i = node->GetNumberOfChildren();
  CPPUNIT_ASSERT(i == 5); 

  for(int j=0;j<i;j++)
  {
    albaVME *child = node->GetChild(j);
    switch (j)
    {
    case 0:
      CPPUNIT_ASSERT(child->IsA("albaVMEVolumeGray"));
      break;
    case 1:
      CPPUNIT_ASSERT(child->IsA("albaVMEMesh"));
      break;
    case 2:
      CPPUNIT_ASSERT(child->IsA("albaVMESurface"));
      break;
    case 3:
      CPPUNIT_ASSERT(child->IsA("albaVMEVector"));
      break;
    case 4:
      CPPUNIT_ASSERT(child->IsA("albaVMELandmarkCloud"));
      break;
    default:
      CPPUNIT_ASSERT(false);
      break;
    }

    if(child->IsA("albaVMEVolumeGray"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_volume") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      double range[2];
      data->GetScalarRange(range);
      CPPUNIT_ASSERT(range[0]==-137 && range[1]==1738);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("albaVMEMesh"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_mesh_tetra10") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      int cells=data->GetNumberOfCells();
      CPPUNIT_ASSERT(cells==2);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("albaVMESurface"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_surface") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      int cells=data->GetNumberOfCells();
      CPPUNIT_ASSERT(cells==12);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("albaVMEVector"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_vector_GRFtimevar") == 0);
      vtkDataSet *data=child->GetOutput()->GetVTKData();
      double length = data->GetLength();
      CPPUNIT_ASSERT(fabs(length - 824.56) < 0.01);
      int n=child->GetNumberOfChildren();
      CPPUNIT_ASSERT(n==0);
    }
    if(child->IsA("albaVMELandmarkCloud"))
    {
      name = child->GetName();
      CPPUNIT_ASSERT(name.Compare("test_ landmark cloud") == 0);
      albaVMELandmarkCloud *lmCloud = (albaVMELandmarkCloud *)child;
      int n = lmCloud->GetNumberOfLandmarks();
      CPPUNIT_ASSERT(n==1);
      double xyz[3], rot[3];
      albaVMELandmark *lm = lmCloud->GetLandmark(0);
      name = lm->GetName();
      CPPUNIT_ASSERT(name.Compare("test_landmark") == 0);
      lm->GetOutput()->GetPose(xyz,rot);
      CPPUNIT_ASSERT( fabs(xyz[0] - 0.0) < 0.01 &&
        fabs(xyz[1] - 50.0) < 0.01 &&
        fabs(xyz[2] - 100.0) < 0.01 );
    }
  }
  albaDEL(storage);
  albaDEL(importer);
}
//----------------------------------------------------------------------------
void albaOpImporterMSFTest::TestImportedVmeId()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaOpImporterMSF *importer1=new albaOpImporterMSF("importer");
  importer1->SetInput(storage->GetRoot());
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_MSFImporter/Test_MSFImporter.msf";
  importer1->TestModeOn();
  importer1->SetFileName(filename.GetCStr());
  importer1->OpRun();

  albaOpImporterMSF *importer2=new albaOpImporterMSF("importer");
  importer2->SetInput(storage->GetRoot());
  importer2->TestModeOn();
  importer2->SetFileName(filename.GetCStr());
  // Import twice the same MSF
  importer2->OpRun();

  albaVMERoot *root = storage->GetRoot();
  CPPUNIT_ASSERT(root->GetNumberOfChildren()==2);

  std::vector<albaID> vectorID;

  albaVMEIterator *iter = root->NewIterator();
  for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    albaID lastId = node->GetId();
    for (int i = 0; i < vectorID.size(); i++)
    {
      CPPUNIT_ASSERT(vectorID[i] != lastId);
    }
    vectorID.push_back(lastId);
  }
  iter->Delete();

  albaDEL(storage);
  albaDEL(importer1);
  albaDEL(importer2);
}
