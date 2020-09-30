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
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaPipeSurfaceTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTest::BeforeTest()
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
void albaPipeSurfaceTest::AfterTest()
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
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
  sceneNode = new albaSceneNode(NULL,NULL,surface, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurface *pipeSurface = new albaPipeSurface;
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
		default:
			break;
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
		
		// Rendering - check images 
    vtkActor *surfaceActor;
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
    CPPUNIT_ASSERT(surfaceActor != NULL);

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
