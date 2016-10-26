/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceSlice_BESTest
 Authors: Eleonora Mambrini, Matteo Giacomoni, Gianluigi Crimi
 
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
#include "mafPipeSurfaceSlice_BESTest.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mmaMaterial.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
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
void mafPipeSurfaceSlice_BESTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::BeforeTest()
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
void mafPipeSurfaceSlice_BESTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestCloudPipeExecution()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmark> first;
  first->SetPoint(0.0,0.0,0.0);
  first->SetName("first");
  mafSmartPointer<mafVMELandmark> second;
  second->SetPoint(1.0,0.0,0.0);
  second->SetName("second");
  mafSmartPointer<mafVMELandmark> third;
  third->SetPoint(0.0,0.0,5.0);
  third->SetName("third");
  mafSmartPointer<mafVMELandmarkCloud> ldm;
  ldm->TestModeOn();
	first->ReparentTo(ldm);
	second->ReparentTo(ldm);
	third->ReparentTo(ldm);
  ldm->GetMaterial();
  ldm->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
	ldm->ShowAllLandmarks();

  ldm->GetOutput()->GetVTKData()->Update();
  ldm->GetOutput()->Update();
  ldm->Update();

  double bounds[6];
  ldm->GetOutput()->GetVTKData()->GetBounds(bounds);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,ldm, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
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
void mafPipeSurfaceSlice_BESTest::TestSurfacePipeExecution()
//----------------------------------------------------------------------------
{
	
  ////// Create VME ////////////////////

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
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
void mafPipeSurfaceSlice_BESTest::TestSurfacePipeCreation()
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

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
  pipeSlice->Create(sceneNode);

  CPPUNIT_ASSERT(pipeSlice);

  delete sceneNode;
}

//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestSetGetThickness()
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

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
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