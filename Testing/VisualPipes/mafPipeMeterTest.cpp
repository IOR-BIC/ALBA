/*=========================================================================

 Program: MAF2
 Module: mafPipeMeterTest
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
#include "mafPipeMeterTest.h"
#include "mafPipeMeter.h"
#include "mafTransform.h"

#include "mafSceneNode.h"
#include "mafVMEVector.h"
#include "mmaMaterial.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEMeter.h"

#include "vtkMAFAssembly.h"

#include "vtkMapper.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkMAFSmartPointer.h"

#include "vtkSphereSource.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>


//----------------------------------------------------------------------------
void mafPipeMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(600, 600);
	m_RenderWindow->SetPosition(0, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	////// Create VME ////////////////////
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);	
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

	mafMatrix matrix;
  matrix.SetElement(0,3,4); //set a translation value on x axis of 4.0
  matrix.SetElement(1,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix);

  mafSmartPointer<mafVMEMeter> meter;
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();

	//Setting standard material to avoid random color selection
	meter->GetMaterial()->m_Diffuse[0]=0.3;
	meter->GetMaterial()->m_Diffuse[1]=0.6;
	meter->GetMaterial()->m_Diffuse[2]=0.9;
	meter->GetMaterial()->UpdateProp();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,meter, m_Renderer);
  
  /////////// Pipe Instance and Creation ///////////
  mafPipeMeter *pipeMeter = new mafPipeMeter;
  pipeMeter->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeter->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
    break;
  }

  vtkActor *vectorActor;
  vectorActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(vectorActor != NULL);

  m_RenderWindow->Render();

  COMPARE_IMAGES("TestPipeExecution", 0);

  vtkDEL(actorList);

  delete sceneNode;

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);

  mafDEL(storage);
}

//----------------------------------------------------------------------------
vtkProp *mafPipeMeterTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
