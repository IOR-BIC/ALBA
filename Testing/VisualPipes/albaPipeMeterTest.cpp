/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeterTest
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
#include "albaPipeMeterTest.h"
#include "albaPipeMeter.h"
#include "albaTransform.h"

#include "albaSceneNode.h"
#include "albaVMEVector.h"
#include "mmaMaterial.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEMeter.h"

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

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>


//----------------------------------------------------------------------------
void albaPipeMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeMeterTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeMeterTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeMeterTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	////// Create VME ////////////////////
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);	
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();


	albaMatrix matrix;
  matrix.SetElement(0,3,4); //set a translation value on x axis of 4.0
  matrix.SetElement(1,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix);

  albaSmartPointer<albaVMEMeter> meter;
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->ReparentTo(storage->GetRoot());
  meter->GetOutput()->GetVTKData()->Update();
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
  albaPipeMeter *pipeMeter = new albaPipeMeter;
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
vtkProp *albaPipeMeterTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
