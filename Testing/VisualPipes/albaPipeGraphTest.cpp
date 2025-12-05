/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeGraphTest
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
#include "albaPipeGraphTest.h"
#include "albaPipeGraph.h"
#include "albaVMEPolylineEditor.h"

#include "mmaMaterial.h"
#include "albaSceneNode.h"

#include "albaVMERoot.h"

#include "albaVMEAnalog.h"

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
#include "vtkActor2DCollection.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>

enum TESTS_PIPE_POLYLINE_GRAPH_EDITOR
{
  BASE_TEST,
  SELECT_TEST,
  CUT_TEST,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void albaPipeGraphTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeGraphTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_AlwaysVisibleRenderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_AlwaysVisibleRenderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(800, 600);
	m_RenderWindow->SetPosition(600, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void albaPipeGraphTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_AlwaysVisibleRenderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipeGraphTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  albaVMEAnalog *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 30;
  int nTimeStamps = 500;
  emgMatrix.set_size(rows , nTimeStamps);
   
  int count = 0;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < nTimeStamps; j++)
    {
      emgMatrix.put(i, j, count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  albaNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,analog, m_Renderer,NULL,m_AlwaysVisibleRenderer);
  
  /////////// Pipe Instance and Creation ///////////
  albaPipeGraph *pipe = new albaPipeGraph;
  pipe->Create(sceneNode);
  pipe->SetSignalToPlot(1, true);
  pipe->SetSignalToPlot(25, true);
  pipe->SetTitleX("Test plot");
  pipe->UpdateGraph();

  ////////// ACTORS List ///////////////
  
  vtkActor2DCollection *actorList = pipe->m_RenFront->GetActors2D();

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  vtkActor *graphActor;

  graphActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(graphActor != NULL);
  m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeExecution", 0);
 
  delete sceneNode;
  albaDEL(analog);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeGraphTest::SelectActorToControl(vtkPropCollection *propList, int index)
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