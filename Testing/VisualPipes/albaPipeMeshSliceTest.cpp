/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshSliceTest
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
#include "albaPipeMeshSliceTest.h"
#include "albaPipeMeshSlice.h"

#include "albaSceneNode.h"
#include "albaVMEMesh.h"
#include "albaOpImporterVTK.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "vtkALBAAssembly.h"

#include "vtkPointData.h"
#include "mmaMaterial.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>
#include "vtkActor.h"
#include "vtkMapper.h"
#include "vtkDataSet.h"

enum PIPE_MESH_ACTORS
  {
    PIPE_MESH_ACTOR,
    PIPE_MESH_ACTOR_WIRED,
    PIPE_MESH_ACTOR_OUTLINE_CORNER,
    PIPE_MESH_NUMBER_OF_ACTORS,
  };

//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::BeforeTest()
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::AfterTest()
{
}

enum ID_TEST
{
  ID_TEST_PIPEEXECUTION = 0,
  ID_TEST_PIPEEXECUTION_WIREFRAME = 10,
  ID_TEST_PIPEEXECUTION_WIRED_ACTOR_VISIBILITY = 20,
  ID_TEST_PIPEEXECUTION_FLIP_NORMAL = 30,
  ID_TEST_PIPEEXECUTION_SCALAR_MAP_ACTIVE = 40,
  ID_TEST_PIPEEXECUTION_USE_VTK_PROPERTY = 50,
  ID_TEST_PIPEEXECUTION_THICKNESS = 60,
};

//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestPipeExecution()
{
  ////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterVTK *Importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  albaVMEMesh *mesh;
  mesh =albaVMEMesh::SafeDownCast(Importer->GetOutput());
	mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
  

  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMeshSlice *pipeMeshSlice = new albaPipeMeshSlice;
	pipeMeshSlice->m_RenFront = m_Renderer;
  pipeMeshSlice->SetScalarMapActive(1);
  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  //pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin,normal);
  pipeMeshSlice->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }
	
  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
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
    case 5:
    {
      controlValues[0] = 1.0;
      controlValues[1] = 2.0;
    }
    break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    
    pipeMeshSlice->OnEvent(&albaEvent(this, albaPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();

		albaLogMessage("\n Current Array. index:%d name:%s \n", arrayIndex, pipeMeshSlice->GetScalarName(arrayIndex).ToAscii());		
    
    ProceduralControl(controlValues, meshActor);
		COMPARE_IMAGES("TestPipeExecution", ID_TEST_PIPEEXECUTION + arrayIndex);
  }

  vtkDEL(actorList);

  delete sceneNode;

  cppDEL(Importer);
  albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestPipeExecution_Wireframe()
{
  ////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterVTK *Importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  albaVMEMesh *mesh;
  mesh =albaVMEMesh::SafeDownCast(Importer->GetOutput());
  mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();


  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh,m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMeshSlice *pipeMeshSlice = new albaPipeMeshSlice;
	pipeMeshSlice->m_RenFront = m_Renderer;
  pipeMeshSlice->SetScalarMapActive(1);
  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  //pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin,normal);
  pipeMeshSlice->Create(sceneNode);

  pipeMeshSlice->SetRepresentation(albaPipeGenericPolydata::WIREFRAME_REP);
  pipeMeshSlice->OnEvent(&albaEvent(this, VME_TIME_SET));
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
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
    case 5:
    {
      controlValues[0] = 1.0;
      controlValues[1] = 2.0;
    }
    break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    pipeMeshSlice->OnEvent(&albaEvent(this, albaPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();

		albaLogMessage("\n Current Array. index:%d name:%s \n", arrayIndex, pipeMeshSlice->GetScalarName(arrayIndex).ToAscii());
    
    ProceduralControl(controlValues, meshActor);
		COMPARE_IMAGES("TestPipeExecution_Wireframe", ID_TEST_PIPEEXECUTION_WIREFRAME + arrayIndex);
  }

  vtkDEL(actorList);

  delete sceneNode;

  cppDEL(Importer);
  albaDEL(storage);

}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestPipeExecution_WiredActorVisibility()
{
  ////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterVTK *Importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  albaVMEMesh *mesh;
  mesh =albaVMEMesh::SafeDownCast(Importer->GetOutput());
  mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
	
  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMeshSlice *pipeMeshSlice = new albaPipeMeshSlice;
	pipeMeshSlice->m_RenFront = m_Renderer;
  pipeMeshSlice->SetScalarMapActive(1);
  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  //pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin,normal);
  pipeMeshSlice->Create(sceneNode);

  pipeMeshSlice->SetEdgesVisibilityOn();
	pipeMeshSlice->SetRepresentation(albaPipeGenericPolydata::SURFACE_REP);
  pipeMeshSlice->OnEvent(&albaEvent(this, VME_TIME_SET));
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
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
    case 5:
    {
      controlValues[0] = 1.0;
      controlValues[1] = 2.0;
    }
    break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    pipeMeshSlice->OnEvent(&albaEvent(this, albaPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();

		albaLogMessage("\n Current Array. index:%d name:%s \n", arrayIndex, pipeMeshSlice->GetScalarName(arrayIndex).ToAscii());
    
    ProceduralControl(controlValues, meshActor);
		COMPARE_IMAGES("TestPipeExecution_WiredActorVisibility", ID_TEST_PIPEEXECUTION_WIRED_ACTOR_VISIBILITY + arrayIndex);
  }

  vtkDEL(actorList);

  delete sceneNode;

  cppDEL(Importer);
  albaDEL(storage);

}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestPipeExecution_FlipNormal()
{
  ////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterVTK *Importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  albaVMEMesh *mesh;
  mesh =albaVMEMesh::SafeDownCast(Importer->GetOutput());
  mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
	
  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMeshSlice *pipeMeshSlice = new albaPipeMeshSlice;
	pipeMeshSlice->m_RenFront = m_Renderer;
  pipeMeshSlice->SetScalarMapActive(1);

  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  //pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin,normal);
  pipeMeshSlice->Create(sceneNode);

  pipeMeshSlice->SetFlipNormalOn();
  pipeMeshSlice->OnEvent(&albaEvent(this, VME_TIME_SET));
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
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
    case 5:
    {
      controlValues[0] = 1.0;
      controlValues[1] = 2.0;
    }
    break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    pipeMeshSlice->OnEvent(&albaEvent(this, albaPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    m_Renderer->ResetCamera();
		m_RenderWindow->Render();

		albaLogMessage("\n Current Array. index:%d name:%s \n", arrayIndex, pipeMeshSlice->GetScalarName(arrayIndex).ToAscii());

    ProceduralControl(controlValues, meshActor);
    COMPARE_IMAGES("TestPipeExecution_FlipNormal", ID_TEST_PIPEEXECUTION_FLIP_NORMAL+arrayIndex);
  }

  vtkDEL(actorList);

  delete sceneNode;

  cppDEL(Importer);
  albaDEL(storage);

}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestPipeExecution_UseVTKProperty()
{
  ////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterVTK *Importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  albaVMEMesh *mesh;
  mesh =albaVMEMesh::SafeDownCast(Importer->GetOutput());
  mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
	
	//Setting standard material to avoid random color selection
	mesh->GetMaterial()->m_Diffuse[0]=0.3;
	mesh->GetMaterial()->m_Diffuse[1]=0.6;
	mesh->GetMaterial()->m_Diffuse[2]=0.9;
	mesh->GetMaterial()->UpdateProp();
	
  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMeshSlice *pipeMeshSlice = new albaPipeMeshSlice;
	pipeMeshSlice->m_RenFront = m_Renderer;
  pipeMeshSlice->SetScalarMapActive(1);
  
  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  //pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin,normal);
  pipeMeshSlice->Create(sceneNode);

  pipeMeshSlice->SetUseVTKProperty(1);
  pipeMeshSlice->OnEvent(&albaEvent(this, VME_TIME_SET));
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
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
    case 5:
    {
      controlValues[0] = 1.0;
      controlValues[1] = 2.0;
    }
    break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    pipeMeshSlice->OnEvent(&albaEvent(this, albaPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();

    albaLogMessage("\n Current Array. index:%d name:%s \n", arrayIndex, pipeMeshSlice->GetScalarName(arrayIndex).ToAscii());

		ProceduralControl(controlValues, meshActor);
		COMPARE_IMAGES("TestPipeExecution_UseVTKProperty", ID_TEST_PIPEEXECUTION_USE_VTK_PROPERTY + arrayIndex);
  }

  vtkDEL(actorList);

  delete sceneNode;

  cppDEL(Importer);
  albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::TestPipeExecution_Thickness_PickActor()
{
  ////// Create VME (import vtkData) ////////////////////
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  albaOpImporterVTK *Importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/FEM/pipemesh/hex8.vtk";
  Importer->TestModeOn();
  Importer->SetFileName(filename);
  Importer->SetInput(storage->GetRoot());
  Importer->ImportVTK();
  albaVMEMesh *mesh;
  mesh =albaVMEMesh::SafeDownCast(Importer->GetOutput());
  mesh->GetMaterial();
  mesh->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  mesh->Update();
	
  double center[3];
  mesh->GetOutput()->GetVTKData()->GetCenter(center);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,mesh, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMeshSlice *pipeMeshSlice = new albaPipeMeshSlice;
	pipeMeshSlice->m_RenFront = m_Renderer;
  pipeMeshSlice->SetScalarMapActive(1);
  
  double origin[3], normal[3];

  //set origin and normal value
  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.3;

  normal[0] = 0.0;
  normal[1] = 0.0; 
  normal[2] = 1.0;

  //pipeMeshSlice->SetNormal(normal);
  pipeMeshSlice->SetSlice(origin,normal);
  pipeMeshSlice->Create(sceneNode);

  pipeMeshSlice->SetThickness(3.0);
	pipeMeshSlice->SetRepresentation(albaPipeGenericPolydata::WIREFRAME_REP);
	pipeMeshSlice->SetEdgesVisibilityOn();
  pipeMeshSlice->SetActorPicking(false);
  pipeMeshSlice->OnEvent(&albaEvent(this, VME_TIME_SET));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeshSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  { 
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  for(int arrayIndex=0; arrayIndex<pipeMeshSlice->GetNumberOfArrays(); arrayIndex++)
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
    case 5:
    {
      controlValues[0] = 1.0;
      controlValues[1] = 2.0;
    }
    break;
    }
    pipeMeshSlice->SetActiveScalar(arrayIndex);
    pipeMeshSlice->OnEvent(&albaEvent(this, albaPipeMeshSlice::ID_SCALARS));

    vtkActor *meshActor;
    meshActor = (vtkActor *) SelectActorToControl(actorList, PIPE_MESH_ACTOR);
    CPPUNIT_ASSERT(meshActor != NULL);
    CPPUNIT_ASSERT(meshActor->GetPickable() == false);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();

    albaLogMessage("\n Current Array. index:%d name:%s \n", arrayIndex, pipeMeshSlice->GetScalarName(arrayIndex).ToAscii());
		
		ProceduralControl(controlValues, meshActor);
		COMPARE_IMAGES("TestPipeExecution_Thickness_PickActor", ID_TEST_PIPEEXECUTION_THICKNESS + arrayIndex);
  }

  vtkDEL(actorList);

  delete sceneNode;

  cppDEL(Importer);
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaPipeMeshSliceTest::ProceduralControl(double controlRangeMapper[2],vtkProp *propToControl)
{
  //procedural control
  double sr[2];
  ((vtkActor* )propToControl)->GetMapper()->GetScalarRange(sr);

  if (!(sr[0] == controlRangeMapper[0] && sr[1] == controlRangeMapper[1]))
    albaLogMessage("Control:[%lf,%lf], Range:[%lf,%lf]", controlRangeMapper[0], controlRangeMapper[1], sr[0], sr[1]);

  CPPUNIT_ASSERT(sr[0] == controlRangeMapper[0] && sr[1] == controlRangeMapper[1]);
  //end procedural control
}
//----------------------------------------------------------------------------
vtkProp *albaPipeMeshSliceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
