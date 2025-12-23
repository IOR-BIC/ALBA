/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorTest
 Authors: Roberto Mucci
 
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
#include "albaPipeVectorTest.h"
#include "albaPipeVector.h"

#include "albaSceneNode.h"
#include "albaVMEVector.h"
#include "mmaMaterial.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCamera.h"
#include "vtkALBASmartPointer.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaPipeVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVectorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeVectorTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeVectorTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  vtkALBASmartPointer<vtkPolyData> poly;
  vtkALBASmartPointer<vtkPoints> points1;
  vtkALBASmartPointer<vtkCellArray> cellArray1;
  vtkIdType pointId1[2];
  double force1X = 12.5;
  double force1Y = 201.7;
  double force1Z = 20.6;

  points1->InsertPoint(0, 0, 0, 0);
  points1->InsertPoint(1, force1X, force1Y, force1Z);
  pointId1[0] = 0;
  pointId1[1] = 1;
  cellArray1->InsertNextCell(2, pointId1);  
  poly->SetPoints(points1);
  poly->SetLines(cellArray1);

  albaVMEVector *vector;
  albaNEW(vector);
  vector->SetData(poly, 0);
  vector->Modified();
  vector->Update();
  vector->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  vector->SetTimeStamp(0);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,vector, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeVector *pipeVector = new albaPipeVector;
  pipeVector->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeVector->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
    break;
  }

  vtkActor *vectorActor;
  vectorActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(vectorActor != NULL);

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
	COMPARE_IMAGES("TestPipeExecution", 0);

  vtkDEL(actorList);
  delete sceneNode;
  albaDEL(vector);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeVectorTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
