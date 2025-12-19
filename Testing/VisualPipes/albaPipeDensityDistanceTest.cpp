/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeDensityDistanceTest
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
#include "albaPipeDensityDistanceTest.h"
#include "albaPipeDensityDistance.h"

#include "albaSceneNode.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include "vtkPropCollection.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
void albaPipeDensityDistanceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeDensityDistanceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeDensityDistanceTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeDensityDistanceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create surface VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Surfaces/surface.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMESurface *surface;
  albaNEW(surface);
  surface->SetData((vtkPolyData*)Importer->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  ////// Create volume VME (import vtkData) ////////////////////
  filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,surface, m_Renderer);
  
  /////////// Pipe Instance and Creation ///////////
  albaPipeDensityDistance *pipeDD = new albaPipeDensityDistance;
  pipeDD->SetVolume(volume);
  pipeDD->Create(sceneNode);
  pipeDD->m_RenFront = m_Renderer;
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeDD->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    actor = actorList->GetNextProp();
  }

  vtkActor *cloudActor;
  cloudActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(cloudActor != NULL);

	m_Renderer->ResetCamera();
  m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeExecution", 0);

	vtkDEL(actorList);

  delete sceneNode;

  albaDEL(surface);
  albaDEL(volume);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeDensityDistanceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
