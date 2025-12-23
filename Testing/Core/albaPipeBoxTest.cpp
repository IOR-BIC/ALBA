/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeBoxTest
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
#include "albaPipeBoxTest.h"
#include "albaPipeBox.h"

#include "albaSceneNode.h"
#include "albaVMESurface.h"

#include "vtkALBAAssembly.h"
#include "vtkDataSet.h"

#include "vtkMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

// render window stuff

#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>

enum PIPE_BOX_ACTORS
{
  PIPE_BOX_ACTOR,
  PIPE_BOX_ACTOR_WIRED,
  PIPE_BOX_ACTOR_OUTLINE_CORNER,
  PIPE_BOX_NUMBER_OF_ACTORS,
};

//----------------------------------------------------------------------------
void albaPipeBoxTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeBoxTest::BeforeTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeBoxTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeBoxTest::TestPipe3DExecution()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();


  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMESurface *surfaceParent;
  albaNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  albaString filename2=ALBA_DATA_ROOT;
  filename2<<"/PipeBox/VTK/Cube2T0.vtk";
  importer->SetFileName(filename2.GetCStr());
  importer->Update();

  albaVMESurface *surfaceChild;
  albaNEW(surfaceChild);
  surfaceChild->SetData(importer->GetOutput(),0.0);
  surfaceChild->GetOutput()->Update();
  surfaceChild->Update();

  surfaceChild->ReparentTo(surfaceParent);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeBox *pipeBox = new albaPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(albaPipeBox::BOUNDS_3D);
  pipeBox->OnEvent(&albaEvent(this, albaPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  char *strings="3D";

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);

	COMPARE_IMAGES(strings);
  vtkDEL(actorList);
  
  delete sceneNode;

  albaDEL(surfaceChild);
  albaDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void albaPipeBoxTest::TestPipe3DSubtreeExecution()
//----------------------------------------------------------------------------
{

  InitializeRenderWindow();


  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMESurface *surfaceParent;
  albaNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  albaString filename2=ALBA_DATA_ROOT;
  filename2<<"/PipeBox/VTK/Cube2T0.vtk";
  importer->SetFileName(filename2.GetCStr());
  importer->Update();

  albaVMESurface *surfaceChild;
  albaNEW(surfaceChild);
  surfaceChild->SetData(importer->GetOutput(),0.0);
  surfaceChild->GetOutput()->Update();
  surfaceChild->Update();

  surfaceChild->ReparentTo(surfaceParent);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeBox *pipeBox = new albaPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(albaPipeBox::BOUNDS_3D_SUBTREE);
  pipeBox->OnEvent(&albaEvent(this, albaPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  char *strings="3DSubtree";

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);

	COMPARE_IMAGES(strings);

	vtkDEL(actorList);

  delete sceneNode;

  albaDEL(surfaceChild);
  albaDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void albaPipeBoxTest::TestPipe4DExecution()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();

  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  albaString filename1=ALBA_DATA_ROOT;
  filename1<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  albaVMESurface *surfaceParent;
  albaNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  albaString filename2=ALBA_DATA_ROOT;
  filename2<<"/PipeBox/VTK/Cube1T1.vtk";
  importer->SetFileName(filename2.GetCStr());
  importer->Update();

  surfaceParent->SetData(importer->GetOutput(),1.0);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeBox *pipeBox = new albaPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(albaPipeBox::BOUNDS_4D);
  pipeBox->OnEvent(&albaEvent(this, albaPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  char *strings="4D";

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);

	COMPARE_IMAGES(strings);

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void albaPipeBoxTest::TestPipe4DSubtreeExecution()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();

  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  albaVMESurface *surfaceParent;
  albaNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  filename=ALBA_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube1T1.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  surfaceParent->SetData(importer->GetOutput(),1.0);

  filename=ALBA_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube2T0.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  albaVMESurface *surfaceChild;
  albaNEW(surfaceChild);
  surfaceChild->SetData(importer->GetOutput(),0.0);
  surfaceChild->GetOutput()->Update();
  surfaceChild->Update();

  filename=ALBA_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube2T1.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  surfaceChild->SetData(importer->GetOutput(),1.0);

  surfaceChild->ReparentTo(surfaceParent);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeBox *pipeBox = new albaPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(albaPipeBox::BOUNDS_4D_SUBTREE);
  pipeBox->OnEvent(&albaEvent(this, albaPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  char *strings="4DSubtree";

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);

	COMPARE_IMAGES(strings);

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(surfaceChild);
  albaDEL(surfaceParent);
  vtkDEL(importer);
}