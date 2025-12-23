/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTrajectoriesTest
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
#include "albaPipeTrajectoriesTest.h"
#include "albaPipeTrajectories.h"

#include "albaSceneNode.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
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
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaPipeTrajectoriesTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeTrajectoriesTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeTrajectoriesTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeTrajectoriesTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME ////////////////////
  albaSmartPointer<albaVMELandmarkCloud> cloud;
	
	//Setting standard material to avoid random color selection
	cloud->GetMaterial()->m_Diffuse[0]=0.3;
	cloud->GetMaterial()->m_Diffuse[1]=0.6;
	cloud->GetMaterial()->m_Diffuse[2]=0.9;
	cloud->GetMaterial()->UpdateProp();

  cloud->AppendLandmark(0.0,0.0,0.0,"first");
  double x = 0;
  double y = 0;
  double z = 0;

  for (int i = 0; i < 15; i++)
  {
    x += 2.0;
    y += 5.5;
    z += 10;

    cloud->SetLandmark("first",x,y,z, i);
  }

  cloud->Update();
  cloud->TestModeOn();

  albaSmartPointer<albaVMELandmark> landmark;
  landmark = cloud->GetLandmark("first");
  landmark->SetTimeStamp(5);
  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,landmark, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeTrajectories *pipeTrajecotries = new albaPipeTrajectories;
  pipeTrajecotries->Create(sceneNode);
  pipeTrajecotries->SetInterval(10);
  pipeTrajecotries->UpdateProperty();
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeTrajecotries->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
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
}

//----------------------------------------------------------------------------
vtkProp *albaPipeTrajectoriesTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
