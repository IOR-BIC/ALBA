/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeWrappedMeterTest
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
#include "albaPipeWrappedMeterTest.h"
#include "albaPipeWrappedMeter.h"
#include "albaTransform.h"

#include "albaSceneNode.h"
#include "albaVMEVector.h"
#include "mmaMaterial.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEWrappedMeter.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkALBASmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkCaptionActor2D.h"
#include "vtkALBATubeFilter.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


#include <iostream>

//----------------------------------------------------------------------------
void albaPipeWrappedMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeWrappedMeterTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeWrappedMeterTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeWrappedMeterTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ////// Create support VMEs ////////////////////
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);	
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();
	
  albaMatrix matrix;
  matrix.SetElement(0,3,4); //set a translation value on x axis of 4.0
  matrix.SetElement(1,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix);

	////// Create VME meter ////////////////////
  albaSmartPointer<albaVMEWrappedMeter> meter;
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
	
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,meter, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  albaPipeWrappedMeter *pipeMeter = new albaPipeWrappedMeter;
  pipeMeter->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeter->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }

  vtkActor *vectorActor;
  vectorActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(vectorActor != NULL);

  m_Renderer->ResetCamera();
  m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecution", 0);

	// Tube representation
	meter->SetMeterRepresentationToTube();
	meter->SetMeterRadius(0.2);
	pipeMeter->m_Tube->SetRadius(meter->GetMeterRadius());
	sceneNode->UpdateProperty();
	m_RenderWindow->Render();
	COMPARE_IMAGES("TestPipeExecution", 1);

	// Tube with capping
	meter->SetMeterCappingOn();
	pipeMeter->m_Tube->SetCapping(meter->GetMeterCapping());
	sceneNode->UpdateProperty();
	m_RenderWindow->Render();
	COMPARE_IMAGES("TestPipeExecution", 2);

	// Color from the distance range (distance is 5, in the middle of the range)
	meter->SetMeterColorModeToRangeColor();
	meter->SetDistanceRange(0, 10);
	sceneNode->UpdateProperty();
	m_RenderWindow->Render();
	COMPARE_IMAGES("TestPipeExecution", 3);

	// Back to line representation, still colored by range
	meter->SetMeterRepresentationToLine();
	sceneNode->UpdateProperty();
	m_RenderWindow->Render();
	COMPARE_IMAGES("TestPipeExecution", 4);

  vtkDEL(actorList);

  delete sceneNode;

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);

  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaPipeWrappedMeterTest::TestTwoPipesOnSameVME()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMESurfaceParametric *vmeStart;
	albaNEW(vmeStart);
	vmeStart->ReparentTo(storage->GetRoot());
	vmeStart->Update();

	albaVMESurfaceParametric *vmeEnd;
	albaNEW(vmeEnd);
	vmeEnd->ReparentTo(storage->GetRoot());
	vmeEnd->Update();

	albaMatrix matrix;
	matrix.SetElement(0, 3, 4);
	matrix.SetElement(1, 3, 3);
	vmeEnd->SetAbsMatrix(matrix);

	albaSmartPointer<albaVMEWrappedMeter> meter;
	meter->SetMeterLink("StartVME", vmeStart);
	meter->SetMeterLink("EndVME1", vmeEnd);
	meter->ReparentTo(storage->GetRoot());
	meter->SetMeterRepresentationToTube();
	meter->Modified();
	meter->Update();

	// Two renderers in the same window, like two views (or a compound view) showing the same VME
	vtkRenderer *renderer2 = vtkRenderer::New();
	m_Renderer->SetViewport(0, 0, 0.5, 1);
	renderer2->SetViewport(0.5, 0, 1, 1);
	m_RenderWindow->AddRenderer(renderer2);

	vtkRenderer *renderers[2] = { m_Renderer, renderer2 };
	albaSceneNode *sceneNodes[2];
	albaPipeWrappedMeter *pipes[2];

	for (int p = 0; p < 2; p++)
	{
		sceneNodes[p] = new albaSceneNode(NULL, NULL, meter, renderers[p]);
		pipes[p] = new albaPipeWrappedMeter;
		pipes[p]->Create(sceneNodes[p]);

		vtkPropCollection *actorList = vtkPropCollection::New();
		pipes[p]->GetAssemblyFront()->GetActors(actorList);
		actorList->InitTraversal();
		for (vtkProp *actor = actorList->GetNextProp(); actor; actor = actorList->GetNextProp())
			renderers[p]->AddActor(actor);
		vtkDEL(actorList);

		renderers[p]->ResetCamera();
	}

	m_RenderWindow->Render();

	// Moving the end point updates the meter while both pipes observe it
	for (int i = 1; i <= 5; i++)
	{
		matrix.SetElement(0, 3, 4 + i);
		matrix.SetElement(2, 3, i);
		vmeEnd->SetAbsMatrix(matrix);
		meter->Modified();
		meter->Update();
		m_RenderWindow->Render();

		for (int p = 0; p < 2; p++)
		{
			CPPUNIT_ASSERT(pipes[p]->m_DataMapper->GetInput() != NULL);
			CPPUNIT_ASSERT(pipes[p]->m_DataMapper->GetInput()->GetNumberOfPoints() > 0);
		}
		CPPUNIT_ASSERT(strcmp(pipes[0]->m_Caption->GetCaption(), pipes[1]->m_Caption->GetCaption()) == 0);
	}

	// Deleting the scene nodes deletes the pipes
	delete sceneNodes[0];
	delete sceneNodes[1];
	m_RenderWindow->Render();

	m_RenderWindow->RemoveRenderer(renderer2);
	vtkDEL(renderer2);

	meter->ReparentTo(NULL);
	vmeStart->ReparentTo(NULL);
	vmeEnd->ReparentTo(NULL);
	albaDEL(vmeStart);
	albaDEL(vmeEnd);

	albaDEL(storage);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeWrappedMeterTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
