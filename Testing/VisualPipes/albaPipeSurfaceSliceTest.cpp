/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceSliceTest
 Authors: Eleonora Mambrini, Matteo Giacomoni, Gianluigi Crimi
 
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
#include "albaPipeSurfaceSliceTest.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMESurface.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "mmaMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkSphereSource.h"

#include <iostream>

#define NUM_OF_CENTERS 3

//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(400, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::TestCloudPipeExecution()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmark> first;
  first->SetPoint(0.0,0.0,0.0);
  first->SetName("first");
  albaSmartPointer<albaVMELandmark> second;
  second->SetPoint(1.0,0.0,0.0);
  second->SetName("second");
  albaSmartPointer<albaVMELandmark> third;
  third->SetPoint(0.0,0.0,5.0);
  third->SetName("third");
  albaSmartPointer<albaVMELandmarkCloud> ldm;
  ldm->TestModeOn();
	first->ReparentTo(ldm);
	second->ReparentTo(ldm);
	third->ReparentTo(ldm);
  ldm->GetMaterial();
  ldm->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	ldm->ShowAllLandmarks();

  ldm->GetOutput()->Update();
  ldm->Update();

  double bounds[6];
  ldm->GetOutput()->GetVTKData()->GetBounds(bounds);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,ldm, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurfaceSlice *pipeSlice = new albaPipeSurfaceSlice;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  double origins[3][NUM_OF_CENTERS] = {{0.0,0.0,0.0},{5.0,5.0,-5.0},{0.0,0.0,5.0}};
  for(int i=3;i<3+NUM_OF_CENTERS;i++)
  {
    pipeSlice->SetSlice(origins[i-3], NULL);

    pipeSlice->GetAssemblyFront()->GetActors(actorList);
    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();

    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_Renderer->ResetCamera(bounds);
    m_RenderWindow->Render();

		COMPARE_IMAGES("TestCloudClosePipeExecution", i);
  }

  vtkDEL(actorList);
  delete sceneNode;
}
//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::TestSurfacePipeExecution()
//----------------------------------------------------------------------------
{
	
  ////// Create VME ////////////////////

  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurfaceSlice *pipeSlice = new albaPipeSurfaceSlice;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  double origins[3][NUM_OF_CENTERS] = {0.0,0.0,0.0,0.0,0.0,7.0,0.0,0.0,-3.0};
  for(int i=0;i<NUM_OF_CENTERS;i++)
  {
    pipeSlice->SetSlice(origins[i], NULL);

    pipeSlice->GetAssemblyFront()->GetActors(actorList);
    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();

    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_Renderer->ResetCamera(bounds);
    m_RenderWindow->Render();

		COMPARE_IMAGES("TestSurfacePipeExecution", i);
  }

  vtkDEL(actorList);

  delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::TestSurfacePipeCreation()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////
	
  ////// Create VME ////////////////////

  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurfaceSlice *pipeSlice = new albaPipeSurfaceSlice;
  pipeSlice->Create(sceneNode);

  CPPUNIT_ASSERT(pipeSlice);

  delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipeSurfaceSliceTest::TestSetGetThickness()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////
	
  ////// Create VME ////////////////////

  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurfaceSlice *pipeSlice = new albaPipeSurfaceSlice;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  double thickness = 1.0;
  pipeSlice->SetThickness(thickness);

  double result_thickness = pipeSlice->GetThickness();

  CPPUNIT_ASSERT(result_thickness == thickness);

  vtkDEL(actorList);

  delete sceneNode;
}