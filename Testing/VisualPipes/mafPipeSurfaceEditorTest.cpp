/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceEditorTest
 Authors: Daniele Giunchi
 
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
#include "mafPipeSurfaceEditorTest.h"
#include "mafPipeSurfaceEditor.h"

#include "mafSceneNode.h"
#include "mafVMESurfaceEditor.h"
#include "mmaMaterial.h"
#include "mafVMERoot.h"

#include "vtkMAFAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkLookupTable.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkTimerLog.h"

#include <iostream>

enum PIPE_SURFACE_ACTORS
  {
    PIPE_SURFACE_EDITOR_ACTOR,
    PIPE_SURFACE_NUMBER_OF_ACTORS,
  };

enum TESTS_PIPE_SURFACE
{
  SCALAR_TEST,
  NUMBER_OF_TEST,
};

//----------------------------------------------------------------------------
void mafPipeSurfaceEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeSurfaceEditorTest::BeforeTest()
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
void mafPipeSurfaceEditorTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceEditorTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeSurfaceEditor/surface0.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMESurfaceEditor *surface;
  mafNEW(surface);
  surface->SetData((vtkPolyData*)Importer->GetOutput(),0.0);
  surface->GetOutput()->Update();
  //surface->GetMaterial();
  //surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  double range[2];
  surface->GetOutput()->GetVTKData()->GetScalarRange(range);
  surface->GetMaterial()->m_ColorLut->SetNumberOfTableValues(4);
  surface->GetMaterial()->m_ColorLut->SetTableValue(range[0], 1.0, 1.0, 1.0, 1.0);
  surface->GetMaterial()->m_ColorLut->SetTableValue(range[0]+ 1., 0.0, 0.0, 1.0, 1.0);
  surface->GetMaterial()->m_ColorLut->SetTableValue(range[0]+ 2., 0.0, 1.0, 0.0, 1.0);
  surface->GetMaterial()->m_ColorLut->SetTableValue(range[1], 1.0, 0.0, 0.0, 1.0);
  surface->GetMaterial()->m_ColorLut->Build();
  surface->Update();
  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceEditor *pipeSurface = new mafPipeSurfaceEditor;
  pipeSurface->Create(sceneNode);
  pipeSurface->m_RenFront = m_Renderer;
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSurface->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }
	
	const char *strings[NUMBER_OF_TEST];
	strings[0] = "SCALAR_TEST";


  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
    vtkActor *surfaceActor;
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_EDITOR_ACTOR);
    CPPUNIT_ASSERT(surfaceActor != NULL);

    m_RenderWindow->Render();
	  printf("\n Visualizzazione: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeExecution", i);
  }

  vtkDEL(actorList);

  delete sceneNode;

  mafDEL(surface);
  vtkDEL(Importer);

  vtkTimerLog::CleanupLog();
}

//----------------------------------------------------------------------------
vtkProp *mafPipeSurfaceEditorTest::SelectActorToControl(vtkPropCollection *propList, int index)
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