/*=========================================================================

 Program: MAF2
 Module: mafPipeGraphTest
 Authors: Roberto Mucci
 
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
#include "mafPipeGraphTest.h"
#include "mafPipeGraph.h"
#include "mafVMEPolylineEditor.h"

#include "mmaMaterial.h"
#include "mafSceneNode.h"

#include "mafVMERoot.h"

#include "mafVMEAnalog.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
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
void mafPipeGraphTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeGraphTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_AlwaysVisibleRenderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_AlwaysVisibleRenderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(320, 240);
	m_RenderWindow->SetPosition(600, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipeGraphTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_AlwaysVisibleRenderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeGraphTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  mafVMEAnalog *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 30;
  int nTimeStamps = 500;
  emgMatrix.set_size(rows-1 , nTimeStamps);

  int i=0;
  int j=0;
  int count = 0;
  for(;i<=rows;i++)
  {
    for(;j<nTimeStamps;j++)
    {
      emgMatrix.put(i,j,count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  mafNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,analog, m_Renderer,NULL,m_AlwaysVisibleRenderer);
  
  /////////// Pipe Instance and Creation ///////////
  mafPipeGraph *pipe = new mafPipeGraph;
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
  mafDEL(analog);
}

//----------------------------------------------------------------------------
vtkProp *mafPipeGraphTest::SelectActorToControl(vtkPropCollection *propList, int index)
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