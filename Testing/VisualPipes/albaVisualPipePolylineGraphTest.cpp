/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipePolylineGraphTest
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
#include "albaVisualPipePolylineGraphTest.h"
#include "albaVisualPipePolylineGraph.h"
#include "albaVMEPolylineGraph.h"

#include "mmaMaterial.h"
#include "albaSceneNode.h"
#include "albaVMERoot.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkSphereSource.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


#include <iostream>

enum TESTS_PIPE_POLYLINE_GRAPH_EDITOR
{
  BASE_TEST,
  SELECT_TEST,
  CUT_TEST,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaVisualPipePolylineGraphTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVisualPipePolylineGraphTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Polydata);
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaVisualPipePolylineGraphTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Polydata);
}
//----------------------------------------------------------------------------
void albaVisualPipePolylineGraphTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  CreateExamplePolydata();

  albaSmartPointer<albaVMEPolylineGraph> poly;
  poly->SetData(m_Polydata,0.0);
  poly->GetOutput()->Update();
  poly->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,poly, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaVisualPipePolylineGraph *pipe = new albaVisualPipePolylineGraph;
  pipe->Create(sceneNode);
  pipe->m_RenFront = m_Renderer;

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);
	
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    actor = actorList->GetNextProp();
  }

  vtkActor *surfaceActor;
  surfaceActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(surfaceActor != NULL);

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecution", 1);
	  
  vtkDEL(actorList);

  delete sceneNode;
}

//----------------------------------------------------------------------------
vtkProp *albaVisualPipePolylineGraphTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
//------------------------------------------------------------------------------
/* Create example polydata.  This is a connected tree of lines and polylines.

                        17
                        |
                        16
                        |
                        15 - 18 - 19 - 20 - 21
                        |
                        14
   2                    |
  /                     13
 /                      |                        
1 - 3 - 6 - 7 - 8 - 9 - 10 - 11 - 12
|   |\
|   | \
0   5  4

*/  
void albaVisualPipePolylineGraphTest::CreateExamplePolydata()
//------------------------------------------------------------------------------
{
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *lines = vtkCellArray::New() ;

  int i ;

  // coordinates of vertices
  static double vertices[22][3] ={
    {0,0,0},
    {1,2,0},
    {2,4,0},
    {2,1,0},
    {3,1,0},
    {3,0,0},

    {3,2,0},
    {4,1,0},
    {5,2,0},
    {6,1,0},
    {7,2,0},
    {8,1,0},
    {9,2,0},

    {7,4,0},
    {9,6,0},
    {10,8,0},
    {13,10,0},
    {14,12,0},

    {11,9,0},
    {12,8,0},
    {13,8,0},
    {16,10,0}
  } ;

  // indices of simple lines and polylines
  static vtkIdType lineids[7][10] = {
    {0,1},
    {1,2},
    {3,4},
    {3,5},
    {1, 3, 6, 7, 8, 9, 10, 11, 12},
    {10, 13, 14, 15, 16, 17},
    {15, 18, 19, 20, 21}
  };

  // insert points
  for (i = 0 ;  i < 22 ;  i++)
    points->InsertNextPoint(vertices[i]) ;

  // insert lines and polylines
  lines->InsertNextCell(2, lineids[0]) ;
  lines->InsertNextCell(2, lineids[1]) ;
  lines->InsertNextCell(2, lineids[2]) ;
  lines->InsertNextCell(2, lineids[3]) ;
  lines->InsertNextCell(9, lineids[4]) ;
  lines->InsertNextCell(6, lineids[5]) ;
  lines->InsertNextCell(5, lineids[6]) ;

  // put points and cells in polydata
  m_Polydata->SetPoints(points) ;
  m_Polydata->SetLines(lines) ;

  points->Delete() ;
  lines->Delete() ;
}
