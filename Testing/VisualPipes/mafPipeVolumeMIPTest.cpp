/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeMIPTest
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
#include "mafPipeVolumeMIPTest.h"
#include "mafPipeVolumeMIP.h"

#include "mafSceneNode.h"
#include "mafVMEVolumeGray.h"
#include "mafVMERoot.h"

#include "vtkMAFAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>

//----------------------------------------------------------------------------
void mafPipeVolumeMIPTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeVolumeMIPTest::BeforeTest()
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
void mafPipeVolumeMIPTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeMIPTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();
  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeVolumeMIP *pipeGizmo = new mafPipeVolumeMIP;
  pipeGizmo->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeGizmo->GetAssemblyFront()->GetVolumes(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  vtkVolume *surfaceActor;
  surfaceActor = (vtkVolume *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(surfaceActor != NULL);

  m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeExecution", 0);

  vtkDEL(actorList);

  delete sceneNode;

  mafDEL(volume);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *mafPipeVolumeMIPTest::SelectActorToControl(vtkPropCollection *propList, int index)
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