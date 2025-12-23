/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceEditorTest
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
#include "albaPipeSurfaceEditorTest.h"
#include "albaPipeSurfaceEditor.h"

#include "albaSceneNode.h"
#include "albaVMESurfaceEditor.h"
#include "mmaMaterial.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkLookupTable.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkTimerLog.h"

#include <iostream>
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

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
void albaPipeSurfaceEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditorTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeSurfaceEditorTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeSurfaceEditor/surface0.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMESurfaceEditor *surface;
  albaNEW(surface);
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
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeSurfaceEditor *pipeSurface = new albaPipeSurfaceEditor;
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
    actor = actorList->GetNextProp();
  }
	
	const char *strings[NUMBER_OF_TEST];
	strings[0] = "SCALAR_TEST";


  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
    vtkActor *surfaceActor;
    surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_SURFACE_EDITOR_ACTOR);
    CPPUNIT_ASSERT(surfaceActor != NULL);

		m_Renderer->ResetCamera();
    m_RenderWindow->Render();
	  printf("\n Visualization: %s \n", strings[i]);

		COMPARE_IMAGES("TestPipeExecution", i);
  }

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(surface);
  vtkDEL(Importer);

  vtkTimerLog::CleanupLog();
}

//----------------------------------------------------------------------------
vtkProp *albaPipeSurfaceEditorTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
