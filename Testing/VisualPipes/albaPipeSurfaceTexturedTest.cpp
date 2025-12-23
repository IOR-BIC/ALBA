/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceTexturedTest
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
#include "albaPipeSurfaceTexturedTest.h"
#include "albaPipeSurfaceTextured.h"

#include "albaSceneNode.h"
#include "albaVMESurface.h"
#include "mmaMaterial.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkJPEGReader.h"
#include "vtkDoubleArray.h"

// render window stuff
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkSphereSource.h"
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
  USE_TEXTURE_PLANE_MAPPING,
  USE_TEXTURE_CYLINDER_MAPPING,
  USE_TEXTURE_SPHERE_MAPPING,
  NUMBER_OF_TEST_TEXTURES,
	SCALAR_TEST,
  VTK_PROPERTY_TEST,
  NUMBER_OF_TEST_CLASSIC,
};

//----------------------------------------------------------------------------
void albaPipeSurfaceTexturedTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTexturedTest::BeforeTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTexturedTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTexturedTest::TestPipeTextureExecution()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
	
  ////// Create VME (import vtkData) ////////////////////
	vtkJPEGReader *ImporterImage;
	vtkNEW(ImporterImage);
	albaString filenameImage=ALBA_DATA_ROOT;
	filenameImage<<"/Test_PipeSurfaceTextured/texture1.jpg";
	ImporterImage->SetFileName(filenameImage);
	ImporterImage->Update();

  albaVMESurface *surface;
  albaNEW(surface);

	vtkCubeSource *cube = vtkCubeSource::New();
	cube->Update();
	cube->GetOutput()->Update();

  vtkCylinderSource *cylinder = vtkCylinderSource::New();
  cylinder->SetResolution(64);
  cylinder->Update();
  cylinder->GetOutput()->Update();
	
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetThetaResolution(64);
  sphere->SetPhiResolution(64);
  sphere->Update();
  sphere->GetOutput()->Update();
	  
  surface->SetData((vtkPolyData*)cube->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_TEXTURE;
	surface->GetMaterial()->SetMaterialTexture((vtkImageData*)ImporterImage->GetOutput());
  surface->Update();
	  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurfaceTextured *pipeSurface = new albaPipeSurfaceTextured;
  pipeSurface->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSurface->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }
	
	const char *strings[NUMBER_OF_TEST_TEXTURES];
	
	strings[0] = "USE_TEXTURE_PLANE_MAPPING";
	strings[1] = "USE_TEXTURE_CYLINDER_MAPPING";
	strings[2] = "USE_TEXTURE_SPHERE_MAPPING";
  
  for(int i=0;i<NUMBER_OF_TEST_TEXTURES;i++)
  {
    if(i == USE_TEXTURE_PLANE_MAPPING)
    {
      surface->GetSurfaceOutput()->GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;
      pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_TEXTURE_MAPPING_MODE));
    }
    else if(i == USE_TEXTURE_CYLINDER_MAPPING)
    {
      surface->SetData((vtkPolyData*)cylinder->GetOutput(),0.0);
      surface->GetOutput()->Update();
      surface->Update();

			surface->GetSurfaceOutput()->GetMaterial()->m_TextureMappingMode = mmaMaterial::CYLINDER_MAPPING;
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_TEXTURE_MAPPING_MODE));
    }
    else if(i == USE_TEXTURE_SPHERE_MAPPING)
    {
      surface->SetData((vtkPolyData*)sphere->GetOutput(),0.0);
      surface->GetOutput()->Update();
      surface->Update();

			surface->GetSurfaceOutput()->GetMaterial()->m_TextureMappingMode = mmaMaterial::SPHERE_MAPPING;
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_TEXTURE_MAPPING_MODE));
    }
		
		
    vtkActor *surfaceActor;
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
    CPPUNIT_ASSERT(surfaceActor != NULL);

		m_Renderer->ResetCamera();
    m_RenderWindow->Render();
	  printf("\n Visualization: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeTextureExecution", i);
  }

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(surface);
  vtkDEL(cube);
  vtkDEL(cylinder);
  vtkDEL(sphere);
	vtkDEL(ImporterImage);
}
//----------------------------------------------------------------------------
void albaPipeSurfaceTexturedTest::TestPipeClassicExecution()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
	
	////// Create VME (import vtkData) ////////////////////
	vtkDataSetReader *Importer;
	vtkNEW(Importer);
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Test_PipeSurfaceTextured/surface0.vtk";
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
	albaPipeSurfaceTextured *pipeSurface = new albaPipeSurfaceTextured;
	pipeSurface->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeSurface->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while(actor)
	{   
		m_Renderer->AddActor(actor);
		actor = actorList->GetNextProp();
	}

	const char *strings[2];
	strings[0] = "SCALAR_TEST";
	strings[1] = "VTK_PRPOERTY_TEST";

	for(int i=SCALAR_TEST;i<NUMBER_OF_TEST_CLASSIC;i++)
	{
		if(i == SCALAR_TEST)
		{
			pipeSurface->SetScalarVisibilityOn();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_SCALAR_VISIBILITY));
		}
		else if(i == VTK_PROPERTY_TEST)
		{
			pipeSurface->SetUseVtkPropertyOn();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_USE_VTK_PROPERTY));
		}

		vtkActor *surfaceActor;
		surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_ACTOR);
		CPPUNIT_ASSERT(surfaceActor != NULL);

		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
		printf("\n Visualization: %s \n", strings[i-SCALAR_TEST]);
		COMPARE_IMAGES("TestPipeClassicExecution", i);

		if(i == SCALAR_TEST)
		{
			pipeSurface->SetScalarVisibilityOff();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_SCALAR_VISIBILITY));
		}
		else if(i == VTK_PROPERTY_TEST)
		{
			pipeSurface->SetUseVtkPropertyOff();
			pipeSurface->OnEvent(&albaEvent(this, albaPipeSurfaceTextured::ID_USE_VTK_PROPERTY));
		}
	}

	vtkDEL(actorList);

	delete sceneNode;

	albaDEL(surface);
	vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeSurfaceTexturedTest::SelectActorToControl(vtkPropCollection *propList, int index)
//----------------------------------------------------------------------------
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
