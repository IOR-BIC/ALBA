/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointCloudTest
 Authors: Gianluigi Crimi
 
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
#include "albaPipePointCloudTest.h"
#include "albaPipePointCloud.h"

#include "albaSceneNode.h"
#include "albaVMEPointCloud.h"
#include "mmaMaterial.h"

#include "albaVMERoot.h"
#include "vtkALBAAssembly.h"

#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include "vtkCollection.h"
#include "vtkActor2DCollection.h"
#include "vtkActor2D.h"
#include "vtkLookupTable.h"

enum PIPE_POINT_CLOUD_ACTORS
  {
    PIPE_POINT_CLOUD_ACTOR,
    PIPE_POINT_CLOUD_ACTOR_WIRED,
    PIPE_POINT_CLOUD_ACTOR_OUTLINE_CORNER,
    PIPE_POINT_CLOUD_NUMBER_OF_ACTORS,
  };

enum TESTS_PIPE_POINT_CLOUD
{
  BASE_TEST,
  SCALAR_TEST,
  VTK_PROPERTY_TEST,
	THICKNESS_TEST,
	SCALAR_ACTOR_TEST,
	LABEL_NUM_TEST,
	SCALAR_ACTOR_RANGE_TEST,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaPipePointCloudTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaPipePointCloudTest::BeforeTest()
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(320, 240);
	m_RenderWindow->SetPosition(400, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void albaPipePointCloudTest::AfterTest()
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipePointCloudTest::TestPipeExecution()
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipePointCloud/PointCloud.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEPointCloud *pointCloud;
  albaNEW(pointCloud);
  pointCloud->SetData((vtkPolyData*)Importer->GetOutput(),0.0);
  pointCloud->GetOutput()->Update();
  pointCloud->GetMaterial();

	//Setting standard material to avoid random color selection
	pointCloud->GetMaterial()->m_Diffuse[0]=0.3;
	pointCloud->GetMaterial()->m_Diffuse[1]=0.6;
	pointCloud->GetMaterial()->m_Diffuse[2]=0.9;
	pointCloud->GetMaterial()->UpdateProp();

  pointCloud->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  pointCloud->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,pointCloud, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipePointCloud *pipePointCloud = new albaPipePointCloud;
	pipePointCloud->m_RenFront = m_Renderer;
  pipePointCloud->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();

	const char *strings[NUMBER_OF_TEST];
	strings[0] = "BASE_TEST";
	strings[1] = "SCALAR_TEST";
	strings[2] = "VTK_PRPOERTY_TEST";
	strings[3] = "THICKNESS_TEST";
	strings[4] = "SCALAR_ACTOR_TEST";
	strings[5] = "LABEL_NUM_TEST";
	strings[6] = "SCALAR_ACTOR_RANGE_TEST";

  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
		switch ((TESTS_PIPE_POINT_CLOUD) i)
		{
		case BASE_TEST:
			break;
		case SCALAR_TEST:
				pipePointCloud->SetScalarMapActive(true);
				pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			break;
		case VTK_PROPERTY_TEST:
				pipePointCloud->SetUseVTKProperty(true);
				pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_USE_VTK_PROPERTY));
			break;
		case THICKNESS_TEST:
				pipePointCloud->SetThickness(5);
			break;
		case SCALAR_ACTOR_TEST:
			pipePointCloud->Select(true);
			pipePointCloud->SetScalarMapActive(true);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			pipePointCloud->ShowScalarBarActor(true);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
			break;

		case LABEL_NUM_TEST:
			pipePointCloud->Select(true);
			pipePointCloud->SetScalarMapActive(true);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			pipePointCloud->ShowScalarBarActor(true);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
			pipePointCloud->SetScalarBarLabNum(7);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_BAR_LAB_N));
			break;

		case SCALAR_ACTOR_RANGE_TEST:
			pipePointCloud->Select(true);
			pipePointCloud->SetScalarMapActive(true);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			pipePointCloud->ShowScalarBarActor(true);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
			pipePointCloud->GetLookupTable()->SetTableRange(0.16, 0.161);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_LUT));

			break;
		default:
			break;
		}
				
		//Store Actors 2D
		std::vector <vtkActor2D *> act2dList;
		vtkActor2DCollection * actors2d = pipePointCloud->m_RenFront->GetActors2D();
		vtkActor2D * actor2D;
		actors2d->InitTraversal();
		while (actor2D = actors2d->GetNextActor2D())
		{
			act2dList.push_back(actor2D);
		}

		//Updating Actor Lists
		pipePointCloud->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		m_Renderer->RemoveAllViewProps();
		while (actor)
		{
			m_Renderer->AddActor(actor);
			actor = actorList->GetNextProp();
		}

		//Restore 2d actor list
		for (int j = 0; j < act2dList.size(); j++)
		{
			m_Renderer->AddActor2D(act2dList[j]);
		}
		
		// Rendering - check images 
    vtkActor *pointCloudActor;
    pointCloudActor = (vtkActor *) SelectActorToControl(actorList, PIPE_POINT_CLOUD_ACTOR);
    CPPUNIT_ASSERT(pointCloudActor != NULL);

    m_RenderWindow->Render();
	  printf("\n Visualization: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeExecution", i);

		//Reset Pipe
		switch ((TESTS_PIPE_POINT_CLOUD) i)
		{
		case BASE_TEST:
			break;
		case SCALAR_TEST:
			pipePointCloud->SetScalarMapActive(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			break;
		case VTK_PROPERTY_TEST:
			pipePointCloud->SetUseVTKProperty(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_USE_VTK_PROPERTY));
			break;
		case THICKNESS_TEST:
			pipePointCloud->SetThickness(1);
			break;
		case SCALAR_ACTOR_TEST:
			pipePointCloud->SetScalarMapActive(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			pipePointCloud->ShowScalarBarActor(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
			pipePointCloud->Select(false);
			break;
		case LABEL_NUM_TEST:
			pipePointCloud->SetScalarMapActive(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			pipePointCloud->ShowScalarBarActor(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
			pipePointCloud->SetScalarBarLabNum(5);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_BAR_LAB_N));
			pipePointCloud->Select(false);
			break;
		case SCALAR_ACTOR_RANGE_TEST:
			pipePointCloud->SetScalarMapActive(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
			pipePointCloud->ShowScalarBarActor(false);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
			pipePointCloud->GetLookupTable()->SetRange(0.16, 0.165);
			pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_LUT));
			pipePointCloud->Select(false);
			break;
		default:
			break;
		}
  }

  vtkDEL(actorList);
  delete sceneNode;
  albaDEL(pointCloud);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
void albaPipePointCloudTest::TestScalarActorPos()
{
	////// Create VME (import vtkData) ////////////////////
	vtkDataSetReader *Importer;
	vtkNEW(Importer);
	albaString filename = ALBA_DATA_ROOT;
	filename << "/Test_PipePointCloud/PointCloud.vtk";
	Importer->SetFileName(filename);
	Importer->Update();
	albaVMEPointCloud *pointCloud;
	albaNEW(pointCloud);
	pointCloud->SetData((vtkPolyData*)Importer->GetOutput(), 0.0);
	pointCloud->GetOutput()->Update();
	pointCloud->GetMaterial();

	//Setting standard material to avoid random color selection
	pointCloud->GetMaterial()->m_Diffuse[0] = 0.3;
	pointCloud->GetMaterial()->m_Diffuse[1] = 0.6;
	pointCloud->GetMaterial()->m_Diffuse[2] = 0.9;
	pointCloud->GetMaterial()->UpdateProp();

	pointCloud->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	pointCloud->Update();

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, pointCloud, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	albaPipePointCloud *pipePointCloud = new albaPipePointCloud;
	pipePointCloud->m_RenFront = m_Renderer;
	pipePointCloud->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();

	const char *strings[4];
	strings[0] = "UP";
	strings[1] = "DOWN";
	strings[2] = "LEFT";
	strings[3] = "RIGHT";
	

	for (int i = 0; i < 4; i++)
	{
		
		pipePointCloud->Select(true);
		pipePointCloud->SetScalarMapActive(true);
		pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_SCALAR_MAP_ACTIVE));
		pipePointCloud->ShowScalarBarActor(true);
		pipePointCloud->OnEvent(&albaEvent(this, albaPipePointCloud::ID_ENABLE_SCALAR_BAR));
		pipePointCloud->SetScalarBarPos(i);

		//Store Actors 2D
		std::vector <vtkActor2D *> act2dList;
		vtkActor2DCollection * actors2d = pipePointCloud->m_RenFront->GetActors2D();
		vtkActor2D * actor2D;
		actors2d->InitTraversal();
		while (actor2D = actors2d->GetNextActor2D())
		{
			act2dList.push_back(actor2D);
		}

		//Updating Actor Lists
		pipePointCloud->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		m_Renderer->RemoveAllViewProps();
		while (actor)
		{
			m_Renderer->AddActor(actor);
			actor = actorList->GetNextProp();
		}

		//Restore 2d actor list
		for (int j = 0; j < act2dList.size(); j++)
		{
			m_Renderer->AddActor2D(act2dList[j]);
		}

		// Rendering - check images 
		vtkActor *pointCloudActor;
		pointCloudActor = (vtkActor *)SelectActorToControl(actorList, PIPE_POINT_CLOUD_ACTOR);
		CPPUNIT_ASSERT(pointCloudActor != NULL);

		m_RenderWindow->Render();
		printf("\n ScalarPos: %s \n", strings[i]);

		COMPARE_IMAGES("TestScalarPos", i);
	}

	vtkDEL(actorList);
	delete sceneNode;
	albaDEL(pointCloud);
	vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipePointCloudTest::SelectActorToControl(vtkPropCollection *propList, int index)
{
  propList->InitTraversal();
  vtkProp *actor = propList->GetNextProp();
  int count = 0;
  while(actor)
  {   
    if(count == index) return actor;
    actor = propList->GetNextProp();
    count ++;
  }
  return NULL;
}
