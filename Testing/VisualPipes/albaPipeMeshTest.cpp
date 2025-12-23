/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshTest
 Authors: Daniele Giunchi, Gianluigi Crimi
 
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
#include "albaPipeMeshTest.h"
#include "albaPipeMesh.h"

#include "albaSceneNode.h"
#include "albaVMEMesh.h"

#include "albaVMERoot.h"
#include "vtkALBAAssembly.h"


#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "mmaMaterial.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>
#include "vtkActor2DCollection.h"
#include <vector>
#include "albaVMEVolumeGray.h"
#include "vtkStructuredPointsReader.h"
#include "vtkViewport.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"

enum PIPE_MESH_ACTORS
  {
    PIPE_MESH_ACTOR,
    PIPE_MESH_ACTOR_WIRED,
    PIPE_MESH_ACTOR_OUTLINE_CORNER,
    PIPE_MESH_NUMBER_OF_ACTORS,
  };

enum TESTS_PIPE_MESH
{
	BASE_TEST,
	WIREFRAME_TEST,
	POINTS_TEST,
	CELL_NORMAL_TEST,
	SCALAR_TEST,
	VTK_PROPERTY_TEST,
	EDGE_TEST,
	SCALAR_ACTOR_TEST,
	NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaPipeMeshTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaPipeMeshTest::BeforeTest()
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeMeshTest::AfterTest()
{
}

//----------------------------------------------------------------------------
void albaPipeMeshTest::TestPipeExecution()
{
	////// Create VME (import vtkData) ////////////////////
	vtkDataSetReader *Importer;
	vtkNEW(Importer);
	albaString filename = ALBA_DATA_ROOT;
	filename << "/FEM/pipemesh/hex8.vtk";
	Importer->SetFileName(filename);
	Importer->Update();
	albaVMEMesh *mesh;
	albaNEW(mesh);
	mesh->SetData(Importer->GetOutput(), 0.0);
	mesh->GetOutput()->Update();
	mesh->GetMaterial();

	//Setting standard material to avoid random color selection
	mesh->GetMaterial()->m_Diffuse[0] = 0.3;
	mesh->GetMaterial()->m_Diffuse[1] = 0.6;
	mesh->GetMaterial()->m_Diffuse[2] = 0.9;
	mesh->GetMaterial()->UpdateProp();

	mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	mesh->Update();

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, mesh, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	albaPipeMesh *pipeMesh = new albaPipeMesh;
	pipeMesh->m_RenFront = m_Renderer;
	pipeMesh->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();

	const char *strings[NUMBER_OF_TEST];
	strings[0] = "BASE_TEST";
	strings[1] = "WIREFRAME_TEST";
	strings[2] = "POINTS_TEST";
	strings[3] = "CELL_NORMAL_TEST";
	strings[4] = "SCALAR_TEST";
	strings[5] = "VTK_PRPOERTY_TEST";
	strings[6] = "EDGE_TEST";
	strings[7] = "SCALAR_ACTOR_TEST";

	for (int i = 0; i < NUMBER_OF_TEST; i++)
	{
		switch ((TESTS_PIPE_MESH)i)
		{
			case BASE_TEST:
				break;
			case WIREFRAME_TEST:
				pipeMesh->SetRepresentation(albaPipeGenericPolydata::WIREFRAME_REP);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_REPRESENTATION));
				break;
			case POINTS_TEST:
				pipeMesh->SetRepresentation(albaPipeGenericPolydata::POINTS_REP);
				pipeMesh->SetThickness(5);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_REPRESENTATION));
				break;
			case CELL_NORMAL_TEST:
				pipeMesh->SetNormalsTypeToCells();
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_NORMALS_TYPE));
				break;
			case SCALAR_TEST:
				pipeMesh->SetScalarMapActive(true);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_SCALAR_MAP_ACTIVE));
				break;
			case VTK_PROPERTY_TEST:
				pipeMesh->SetUseVTKProperty(true);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_USE_VTK_PROPERTY));
				break;
			case EDGE_TEST:
				pipeMesh->SetEdgesVisibilityOn();
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_EDGE_VISIBILITY));
				break;
			case SCALAR_ACTOR_TEST:
				pipeMesh->Select(true);
				pipeMesh->SetScalarMapActive(true);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_SCALAR_MAP_ACTIVE));
				pipeMesh->ShowScalarBarActor(true);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_ENABLE_SCALAR_BAR));
				break;
			default:
				break;
		}

		//Store Actors 2D
		std::vector <vtkActor2D *> act2dList;
		vtkActor2DCollection * actors2d = pipeMesh->m_RenFront->GetActors2D();
		vtkActor2D * actor2D;
		actors2d->InitTraversal();
		while (actor2D = actors2d->GetNextActor2D())
		{
			act2dList.push_back(actor2D);
		}

		//Updating Actor Lists
		pipeMesh->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		m_Renderer->RemoveAllProps();
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
		vtkActor *surfaceActor;
		surfaceActor = (vtkActor *)SelectActorToControl(actorList, PIPE_MESH_ACTOR);
		CPPUNIT_ASSERT(surfaceActor != NULL);

		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
		printf("\n Visualization: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeExecution", i);

		//Reset Pipe
		switch ((TESTS_PIPE_MESH)i)
		{
			case BASE_TEST:
				break;
			case WIREFRAME_TEST:
			case POINTS_TEST:
				pipeMesh->SetRepresentation(albaPipeGenericPolydata::SURFACE_REP);
				pipeMesh->SetThickness(1);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_REPRESENTATION));
				break;
			case CELL_NORMAL_TEST:
				pipeMesh->SetNormalsTypeToPoints();
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_NORMALS_TYPE));
				break;
			case SCALAR_TEST:
				pipeMesh->SetScalarMapActive(false);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_SCALAR_MAP_ACTIVE));
				break;
			case VTK_PROPERTY_TEST:
				pipeMesh->SetUseVTKProperty(false);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_USE_VTK_PROPERTY));
				break;
			case EDGE_TEST:
				pipeMesh->SetEdgesVisibilityOff();
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_EDGE_VISIBILITY));
				break;
			case SCALAR_ACTOR_TEST:
				pipeMesh->SetScalarMapActive(false);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_SCALAR_MAP_ACTIVE));
				pipeMesh->ShowScalarBarActor(false);
				pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_ENABLE_SCALAR_BAR));
				pipeMesh->Select(false);
				break;
			default:
				break;
		}
	}

	vtkDEL(actorList);
	delete sceneNode;
	albaDEL(mesh);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void albaPipeMeshTest::TestScalarVisualization()
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEMesh *mesh;
  albaNEW(mesh);
  mesh->SetData(Importer->GetOutput(),0.0);
  mesh->GetOutput()->Update();
	mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMesh *pipeMesh = new albaPipeMesh;
	pipeMesh->m_RenFront = m_Renderer;
  pipeMesh->SetScalarMapActive(1);
  pipeMesh->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMesh->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }
	
	const char *strings[5];
	strings[0] = "Id"; //point 

	strings[1] = "Material"; //cell 
	strings[2] = "EX";
	strings[3] = "NUXY";
	strings[4] = "DENS";

  for(int arrayIndex=0; arrayIndex<pipeMesh->GetNumberOfArrays(); arrayIndex++)
  {
    double controlValues[2] = {-9999,-9999};
    switch(arrayIndex)
    {
    case 0:
      {
        controlValues[0] = 1.0;
        controlValues[1] = 12.0;
      }
      break;
    case 1:
      {
        controlValues[0] = 2.0; 
        controlValues[1] = 3.0;
      }
      break;
    case 2:
      {
        controlValues[0] = 1000.0;
        controlValues[1] = 200000.0;
      }
      break;
    case 3:
      {
        controlValues[0] = 0.33;
        controlValues[1] =  0.39 ;
      }
      break;
    case 4:
      {
        controlValues[0] = 0.107;
        controlValues[1] = 1.07;
      }
      break;
    }
    pipeMesh->SetActiveScalar(arrayIndex);
    pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    ProceduralControl(controlValues, meshActor);

		m_Renderer->ResetCamera();
    m_RenderWindow->Render();
		printf("\n visualization: %s \n", strings[arrayIndex]);

		COMPARE_IMAGES("TestScalarVisualization", arrayIndex);
  }

  vtkDEL(actorList);
  albaDEL(mesh);
  vtkDEL(Importer);

	delete sceneNode;
}
//----------------------------------------------------------------------------
void albaPipeMeshTest::TestPipeDensityMap()
{
	////// Create VME (import vtkData) ////////////////////
	vtkDataSetReader *Importer;
	vtkNEW(Importer);
	albaString filename = ALBA_DATA_ROOT;
	filename << "/FEM/pipemesh/hex8-1.vtk";
	Importer->SetFileName(filename);
	Importer->Update();
	albaVMEMesh *mesh;
	albaNEW(mesh);
	mesh->SetData(Importer->GetOutput(), 0.0);
	mesh->GetOutput()->Update();
	mesh->GetMaterial();
	mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	mesh->Update();

	// Create VME (import Volume) ////////////////////
	vtkStructuredPointsReader *volumeImporter;
	vtkNEW(volumeImporter);
	albaString filename1 = ALBA_DATA_ROOT;
	filename1 << "/VTK_Volumes/volume.vtk";
	volumeImporter->SetFileName(filename1.GetCStr());
	volumeImporter->Update();

	albaVMEVolumeGray *volumeInput;
	albaNEW(volumeInput);
	volumeInput->SetData((vtkImageData*)volumeImporter->GetOutput(), 0.0);
	volumeInput->GetOutput()->GetVTKData()->Update();
	volumeInput->GetOutput()->Update();
	volumeInput->Update();

	vtkDEL(volumeImporter);

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, mesh, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	albaPipeMesh *pipeMesh = new albaPipeMesh;
	pipeMesh->m_RenFront = m_Renderer;
	pipeMesh->SetScalarMapActive(1);
	pipeMesh->Create(sceneNode);

	// Enable DensityMap
	//pipeMesh->ManageScalarOnExecutePipe(polyline->GetOutput()->GetVTKData());
	pipeMesh->SetProbeVolume(volumeInput);
	pipeMesh->SetProbeMapActive(true);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeMesh->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while (actor)
	{
		m_Renderer->AddActor(actor);
		actor = actorList->GetNextProp();
	}

	const char *strings[5];
	strings[0] = "Id"; //point 

	strings[1] = "Material"; //cell 
	strings[2] = "EX";
	strings[3] = "NUXY";
	strings[4] = "DENS";

	for (int arrayIndex = 0; arrayIndex < pipeMesh->GetNumberOfArrays(); arrayIndex++)
	{
		double controlValues[2] = { -9999,-9999 };
		switch (arrayIndex)
		{
		case 0:
		{
			controlValues[0] = 1.0;
			controlValues[1] = 12.0;
		}
		break;
		case 1:
		{
			controlValues[0] = 2.0;
			controlValues[1] = 3.0;
		}
		break;
		case 2:
		{
			controlValues[0] = 1000.0;
			controlValues[1] = 200000.0;
		}
		break;
		case 3:
		{
			controlValues[0] = 0.33;
			controlValues[1] = 0.39;
		}
		break;
		case 4:
		{
			controlValues[0] = 0.107;
			controlValues[1] = 1.07;
		}
		break;
		}
		pipeMesh->SetActiveScalar(arrayIndex);
		pipeMesh->OnEvent(&albaEvent(this, albaPipeMesh::ID_SCALARS));

		vtkActor *meshActor;
		meshActor = (vtkActor *)SelectActorToControl(actorList, PIPE_MESH_ACTOR);
		CPPUNIT_ASSERT(meshActor != NULL);

		//ProceduralControl(controlValues, meshActor);
		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
		printf("\n visualization: %s \n", strings[arrayIndex]);

		COMPARE_IMAGES("TestPipeDensityMap", arrayIndex);
	}

	albaDEL(volumeInput);
	vtkDEL(actorList);
	albaDEL(mesh);
	vtkDEL(Importer);

	delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipeMeshTest::ProceduralControl(double controlRangeMapper[2],vtkProp *propToControl)
{
  //procedural control

  double sr[2];
  ((vtkActor* )propToControl)->GetMapper()->GetScalarRange(sr);
  CPPUNIT_ASSERT(sr[0] == controlRangeMapper[0] && sr[1] == controlRangeMapper[1]);
  //end procedural control
}
//----------------------------------------------------------------------------
vtkProp *albaPipeMeshTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
