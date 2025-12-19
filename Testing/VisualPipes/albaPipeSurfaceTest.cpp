/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaPipeSurfaceTest.h"
#include "albaPipeSurface.h"

#include "albaSceneNode.h"
#include "albaVMESurface.h"
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
#include "vtkActor2DCollection.h"
#include "vtkStructuredPointsReader.h"
#include "albaVMEVolumeGray.h"

#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"

enum PIPE_SURFACE_ACTORS
  {
    PIPE_SURFACE_ACTOR,
    PIPE_SURFACE_ACTOR_WIRED,
    PIPE_SURFACE_ACTOR_OUTLINE_CORNER,
    PIPE_SURFACE_NUMBER_OF_ACTORS,
  };

enum TESTS_PIPE_SURFACE
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
void albaPipeSurfaceTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTest::BeforeTest()
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTest::AfterTest()
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTest::TestPipeExecution()
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeSurface/surface0.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMESurface *surface;
  albaNEW(surface);
  surface->SetData((vtkPolyData*)Importer->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->GetMaterial();

	//Setting standard material to avoid random color selection
	surface->GetMaterial()->m_Diffuse[0]=0.3;
	surface->GetMaterial()->m_Diffuse[1]=0.6;
	surface->GetMaterial()->m_Diffuse[2]=0.9;
	surface->GetMaterial()->UpdateProp();

  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurface *pipeSurface = new albaPipeSurface;
	pipeSurface->m_RenFront = m_Renderer;
  pipeSurface->Create(sceneNode);
  
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
	
  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
		switch ((TESTS_PIPE_SURFACE) i)
		{
		case BASE_TEST:
			break;
		case WIREFRAME_TEST:
			pipeSurface->SetRepresentation(albaPipeGenericPolydata::WIREFRAME_REP);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_REPRESENTATION));
			break;
		case POINTS_TEST:
			pipeSurface->SetRepresentation(albaPipeGenericPolydata::POINTS_REP);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_REPRESENTATION));
			break;
		case CELL_NORMAL_TEST:
				pipeSurface->SetNormalsTypeToCells();
				pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_NORMALS_TYPE));
			break;
		case SCALAR_TEST:
				pipeSurface->SetScalarMapActive(true);
				pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
			break;
		case VTK_PROPERTY_TEST:
				pipeSurface->SetUseVTKProperty(true);
				pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_USE_VTK_PROPERTY));
			break;
		case EDGE_TEST:
				pipeSurface->SetEdgesVisibilityOn();
				pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_EDGE_VISIBILITY));
			break;
		case SCALAR_ACTOR_TEST:
			pipeSurface->Select(true);
			pipeSurface->SetScalarMapActive(true);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
			pipeSurface->ShowScalarBarActor(true);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_ENABLE_SCALAR_BAR));
			break;
		default:
			break;
		}
				
		//Store Actors 2D
		std::vector <vtkActor2D *> act2dList;
		vtkActor2DCollection * actors2d = pipeSurface->m_RenFront->GetActors2D();
		vtkActor2D * actor2D;
		actors2d->InitTraversal();
		while (actor2D = actors2d->GetNextActor2D())
		{
			act2dList.push_back(actor2D);
		}

		//Updating Actor Lists
		pipeSurface->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		m_Renderer->RemoveAllProps();
		while(actor)
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
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
    CPPUNIT_ASSERT(surfaceActor != NULL);

		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
	  printf("\n Visualization: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeExecution", i);

		//Reset Pipe
		switch ((TESTS_PIPE_SURFACE) i)
		{
		case BASE_TEST:
			break;
		case WIREFRAME_TEST:
		case POINTS_TEST:
			pipeSurface->SetRepresentation(albaPipeGenericPolydata::SURFACE_REP);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_REPRESENTATION));
			break;
		case CELL_NORMAL_TEST:
			pipeSurface->SetNormalsTypeToPoints();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_NORMALS_TYPE));
			break;
		case SCALAR_TEST:
			pipeSurface->SetScalarMapActive(false);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
			break;
		case VTK_PROPERTY_TEST:
			pipeSurface->SetUseVTKProperty(false);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_USE_VTK_PROPERTY));
			break;
		case EDGE_TEST:
			pipeSurface->SetEdgesVisibilityOff();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_EDGE_VISIBILITY));
			break;
		case SCALAR_ACTOR_TEST:
			pipeSurface->SetScalarMapActive(false);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
			pipeSurface->ShowScalarBarActor(false);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_ENABLE_SCALAR_BAR));
			pipeSurface->Select(false);
			break;
		default:
			break;
		}
  }

  vtkDEL(actorList);
  delete sceneNode;
  albaDEL(surface);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
void albaPipeSurfaceTest::TestPipeDensityMap()
{

	////// Create VME (import vtkData) ////////////////////
	vtkDataSetReader *Importer;
	vtkNEW(Importer);
	albaString filename = ALBA_DATA_ROOT;
	filename << "/Test_PipeSurface/Surface1-Body.vtk";
	Importer->SetFileName(filename);
	Importer->Update();
	albaVMESurface *surface;
	albaNEW(surface);
	surface->SetData((vtkPolyData*)Importer->GetOutput(), 0.0);
	surface->GetOutput()->Update();
	surface->GetMaterial();

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

	//Setting standard material to avoid random color selection
	surface->GetMaterial()->m_Diffuse[0] = 0.3;
	surface->GetMaterial()->m_Diffuse[1] = 0.6;
	surface->GetMaterial()->m_Diffuse[2] = 0.9;
	surface->GetMaterial()->UpdateProp();

	surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	surface->Update();

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, surface, m_Renderer);

	/////////// Pipe Instance and Creation ///////////
	albaPipeSurface *pipeSurface = new albaPipeSurface;
	pipeSurface->m_RenFront = m_Renderer;
	pipeSurface->Create(sceneNode);

	// Enable DensityMap
	//pipeSurface->ManageScalarOnExecutePipe(polyline->GetOutput()->GetVTKData());
	pipeSurface->SetProbeVolume(volumeInput);
	pipeSurface->SetProbeMapActive(true);

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
		switch ((TESTS_PIPE_SURFACE)i)
		{
		case BASE_TEST:
			break;
		case WIREFRAME_TEST:
			pipeSurface->SetRepresentation(albaPipeGenericPolydata::WIREFRAME_REP);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_REPRESENTATION));
			break;
		case POINTS_TEST:
			pipeSurface->SetRepresentation(albaPipeGenericPolydata::POINTS_REP);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_REPRESENTATION));
			break;
		case CELL_NORMAL_TEST:
			pipeSurface->SetNormalsTypeToCells();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_NORMALS_TYPE));
			break;
		case SCALAR_TEST:
			//pipeSurface->SetScalarMapActive(true);
			//pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
			break;
		case VTK_PROPERTY_TEST:
			pipeSurface->SetUseVTKProperty(true);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_USE_VTK_PROPERTY));
			break;
		case EDGE_TEST:
			pipeSurface->SetEdgesVisibilityOn();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_EDGE_VISIBILITY));
			break;
		case SCALAR_ACTOR_TEST:
// 			pipeSurface->Select(true);
// 			pipeSurface->SetScalarMapActive(true);
// 			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
// 			pipeSurface->ShowScalarBarActor(true);
// 			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_ENABLE_SCALAR_BAR));
			break;
		default:
			break;
		}

		//Store Actors 2D
		std::vector <vtkActor2D *> act2dList;
		vtkActor2DCollection * actors2d = pipeSurface->m_RenFront->GetActors2D();
		vtkActor2D * actor2D;
		actors2d->InitTraversal();
		while (actor2D = actors2d->GetNextActor2D())
		{
			act2dList.push_back(actor2D);
		}

		//Updating Actor Lists
		pipeSurface->GetAssemblyFront()->GetActors(actorList);
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
		surfaceActor = (vtkActor *)SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
		CPPUNIT_ASSERT(surfaceActor != NULL);
		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
		printf("\n Visualization: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeDensityMap", i);

		//Reset Pipe
		switch ((TESTS_PIPE_SURFACE)i)
		{
		case BASE_TEST:
			break;
		case WIREFRAME_TEST:
		case POINTS_TEST:
			pipeSurface->SetRepresentation(albaPipeGenericPolydata::SURFACE_REP);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_REPRESENTATION));
			break;
		case CELL_NORMAL_TEST:
			pipeSurface->SetNormalsTypeToPoints();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_NORMALS_TYPE));
			break;
		case SCALAR_TEST:
// 			pipeSurface->SetScalarMapActive(false);
// 			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
			break;
		case VTK_PROPERTY_TEST:
			pipeSurface->SetUseVTKProperty(false);
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_USE_VTK_PROPERTY));
			break;
		case EDGE_TEST:
			pipeSurface->SetEdgesVisibilityOff();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_EDGE_VISIBILITY));
			break;
		case SCALAR_ACTOR_TEST:
// 			pipeSurface->SetScalarMapActive(false);
// 			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_SCALAR_MAP_ACTIVE));
// 			pipeSurface->ShowScalarBarActor(false);
// 			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurface::ID_ENABLE_SCALAR_BAR));
// 			pipeSurface->Select(false);
			break;
		default:
			break;
		}
	}

	albaDEL(volumeInput);
	vtkDEL(actorList);
	delete sceneNode;
	albaDEL(surface);
	vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeSurfaceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
