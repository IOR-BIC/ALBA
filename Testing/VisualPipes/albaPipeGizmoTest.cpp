/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeGizmoTest
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
#include "albaPipeGizmoTest.h"
#include "albaPipeGizmo.h"

#include "albaSceneNode.h"
#include "albaVMEGizmo.h"
#include "mmaMaterial.h"

#include "albaVMERoot.h"
#include "vtkALBAAssembly.h"

#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkCubeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>


//----------------------------------------------------------------------------
void albaPipeGizmoTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeGizmoTest::BeforeTest()
//----------------------------------------------------------------------------
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
void albaPipeGizmoTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipeGizmoTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  double cubeBounds[6]={-5.0, 10.0, 0.0, 2.0, 0.0, 2.0};
  double handlerBounds[6]={10.0, 14.0, -1.0, 3.0, -1.0, 3.0};
  
  vtkCubeSource *cube = vtkCubeSource::New();
  cube->SetBounds(cubeBounds);
  cube->Update();
  vtkCubeSource *handler = vtkCubeSource::New();
  handler->SetBounds(handlerBounds);
  handler->Update();

  vtkAppendPolyData *append = vtkAppendPolyData::New();
  append->AddInputConnection(cube->GetOutputPort());
  append->AddInputConnection(handler->GetOutputPort());
  append->Update();

  vtkTransform *transform = vtkTransform::New(); 
  transform->RotateX(45);
  transform->Update();

  vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New();
  tpdf->SetInputConnection(append->GetOutputPort());
  tpdf->SetTransform(transform);
  tpdf->Update();

  albaVMEGizmo *gizmo;
  albaNEW(gizmo);
  gizmo->SetData(tpdf->GetOutput());
  gizmo->GetOutput()->Update();
  gizmo->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,gizmo, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeGizmo *pipeGizmo = new albaPipeGizmo;
  pipeGizmo->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeGizmo->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  vtkActor *surfaceActor;
  surfaceActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(surfaceActor != NULL);

  m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeExecution", 0);

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(gizmo);
  vtkDEL(transform);
  vtkDEL(tpdf);
  vtkDEL(append);
  vtkDEL(cube);
  vtkDEL(handler);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeGizmoTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
