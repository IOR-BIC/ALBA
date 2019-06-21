/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshTest
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

enum PIPE_MESH_ACTORS
  {
    PIPE_MESH_ACTOR,
    PIPE_MESH_ACTOR_WIRED,
    PIPE_MESH_ACTOR_OUTLINE_CORNER,
    PIPE_MESH_NUMBER_OF_ACTORS,
  };

//----------------------------------------------------------------------------
void albaPipeMeshTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeMeshTest::BeforeTest()
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
void albaPipeMeshTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipeMeshTest::TestPipeExecution()
//----------------------------------------------------------------------------
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
  sceneNode = new albaSceneNode(NULL,NULL,mesh, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeMesh *pipeMesh = new albaPipeMesh;
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
    m_RenderWindow->Render();

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
    m_RenderWindow->Render();
		printf("\n Visualizzazione: %s \n", strings[arrayIndex]);

		COMPARE_IMAGES("TestPipeExecution", arrayIndex);
  }

  vtkDEL(actorList);
  albaDEL(mesh);
  vtkDEL(Importer);

	delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipeMeshTest::ProceduralControl(double controlRangeMapper[2],vtkProp *propToControl)
//----------------------------------------------------------------------------
{
  //procedural control

  double sr[2];
  ((vtkActor* )propToControl)->GetMapper()->GetScalarRange(sr);
  CPPUNIT_ASSERT(sr[0] == controlRangeMapper[0] && sr[1] == controlRangeMapper[1]);
  //end procedural control
}
//----------------------------------------------------------------------------
vtkProp *albaPipeMeshTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
