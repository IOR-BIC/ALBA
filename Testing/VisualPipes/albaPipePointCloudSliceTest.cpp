/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointCloudSliceTest
 Authors: Gianluigi Crimi

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
#include "albaPipePointCloudSliceTest.h"

#include "albaSceneNode.h"
#include "albaVMERoot.h"
#include "albaPipePointCloudSlice.h"
#include "albaVMEPointCloud.h"
#include "mmaMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkVertex.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>

#define NUM_OF_CENTERS 3

//----------------------------------------------------------------------------
vtkPolyData *CreatePointCloudWithVertexCells(int numPoints, double spacing)
//----------------------------------------------------------------------------
{
  vtkPolyData *polyData = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *vertices = vtkCellArray::New();

  // Create points
  for (int i = 0; i < numPoints; i++)
  {
    double x = i * spacing;
    double y = (i % 3) * spacing;
    double z = (i / 3) * spacing;
    points->InsertNextPoint(x, y, z);

    // Create vertex cell for each point
    vtkVertex *vertex = vtkVertex::New();
    vertex->GetPointIds()->SetId(0, i);
    vertices->InsertNextCell(vertex);
    vertex->Delete();
  }

  polyData->SetPoints(points);
  polyData->SetVerts(vertices);

  points->Delete();
  vertices->Delete();

  return polyData;
}

//----------------------------------------------------------------------------
void albaPipePointCloudSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipePointCloudSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}

//----------------------------------------------------------------------------
void albaPipePointCloudSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipePointCloudSliceTest::TestPointCloudPipeCreation()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  vtkPolyData *pointCloudData = CreatePointCloudWithVertexCells(5, 1.0);

  albaSmartPointer<albaVMEPointCloud> pointCloud;
  pointCloud->SetData(pointCloudData, 0.0);
  pointCloud->GetOutput()->GetVTKData()->Update();
  pointCloud->GetOutput()->Update();
  pointCloud->GetMaterial();
  pointCloud->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  pointCloud->Update();

  double bounds[6];
  pointCloud->GetOutput()->GetVTKData()->GetBounds(bounds);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL, NULL, pointCloud, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipePointCloudSlice *pipeSlice = new albaPipePointCloudSlice;
  pipeSlice->Create(sceneNode);

  CPPUNIT_ASSERT(pipeSlice);

  vtkDEL(pointCloudData);
  delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipePointCloudSliceTest::TestPointCloudPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  vtkPolyData *pointCloudData = CreatePointCloudWithVertexCells(10, 1.0);

  albaSmartPointer<albaVMEPointCloud> pointCloud;
  pointCloud->SetData(pointCloudData, 0.0);
  pointCloud->GetOutput()->GetVTKData()->Update();
  pointCloud->GetOutput()->Update();
  pointCloud->GetMaterial();
  pointCloud->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  pointCloud->Update();

  double bounds[6];
  pointCloud->GetOutput()->GetVTKData()->GetBounds(bounds);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL, NULL, pointCloud, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipePointCloudSlice *pipeSlice = new albaPipePointCloudSlice;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while (actor)
  {
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  double origins[3][NUM_OF_CENTERS] = { {0.0, 0.0, 0.0}, {2.0, 2.0, 2.0}, {5.0, 0.0, 0.0} };
  for (int i = 0; i < NUM_OF_CENTERS; i++)
  {
    pipeSlice->SetSlice(origins[i], NULL);

    pipeSlice->GetAssemblyFront()->GetActors(actorList);
    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();

    while (actor)
    {
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }

    m_Renderer->ResetCamera(bounds);
    m_RenderWindow->Render();
    COMPARE_IMAGES("TestPointCloudPipeSliceExecution", i);
  }

  vtkDEL(actorList);
  vtkDEL(pointCloudData);
  delete sceneNode;
}

//----------------------------------------------------------------------------
void albaPipePointCloudSliceTest::TestSetGetThickness()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  vtkPolyData *pointCloudData = CreatePointCloudWithVertexCells(5, 1.0);

  albaSmartPointer<albaVMEPointCloud> pointCloud;
  pointCloud->SetData(pointCloudData, 0.0);
  pointCloud->GetOutput()->GetVTKData()->Update();
  pointCloud->GetOutput()->Update();
  pointCloud->Update();

  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL, NULL, pointCloud, m_Renderer);

  albaPipePointCloudSlice *pipeSlice = new albaPipePointCloudSlice;
  pipeSlice->Create(sceneNode);

  double thickness = 5.0;
  pipeSlice->SetThickness(thickness);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(thickness, pipeSlice->GetThickness(), 0.001);

  vtkDEL(pointCloudData);
  delete sceneNode;
}