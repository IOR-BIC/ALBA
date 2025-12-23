/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeRayCastTest
 Authors: Grazia Di Cosmo
 
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
#include "albaPipeRayCastTest.h"
#include "albaPipeRayCast.h"

#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"


//----------------------------------------------------------------------------
void albaPipeRayCastTest::TestFixture()
  //----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeRayCastTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeRayCastTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeRayCastTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRayCastTest.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeRayCast *pipeRayCast = new albaPipeRayCast;
  pipeRayCast->TestModeOn();
  pipeRayCast->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeRayCast->GetAssemblyFront()->GetVolumes(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    actor = actorList->GetNextProp();
  }

  vtkVolume *surfaceActor;
  surfaceActor = (vtkVolume *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(surfaceActor != NULL);

	m_Renderer->ResetCamera();
  m_RenderWindow->Render();
	COMPARE_IMAGES("TestPipeExecution", 0);

  m_Renderer->RemoveVolume(actor);

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(volume);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
void albaPipeRayCastTest::TestPipeExecutionMR()
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRayCastTestMR.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeRayCast *pipeRayCast = new albaPipeRayCast;
  pipeRayCast->TestModeOn();
  pipeRayCast->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeRayCast->GetAssemblyFront()->GetVolumes(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    actor = actorList->GetNextProp();
  }

  vtkVolume *surfaceActor;
  surfaceActor = (vtkVolume *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(surfaceActor != NULL);

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecutionMR", 1);

  m_Renderer->RemoveActor(actor);

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(volume);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeRayCastTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
