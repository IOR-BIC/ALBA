/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeLandmarkCloudTest
 Authors: Matteo Giacomoni
 
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

#include "albaPipeLandmarkCloudTest.h"
#include "albaPipeLandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaSceneNode.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkLogLookupTable.h"
#include "vtkCellData.h"
#include "vtkDataSetWriter.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void albaPipeLandmarkCloudTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(200, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloudTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloudTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaPipeLandmarkCloud *pipeCloud = new albaPipeLandmarkCloud();

  delete pipeCloud;
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloudTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> cloud;
  cloud->TestModeOn();
  cloud->AppendLandmark(10.0,0.0,0.0,"first");
  cloud->AppendLandmark(20.0,0.0,0.0,"second");
  cloud->AppendLandmark(30.0,0.0,0.0,"third");
  cloud->AppendLandmark(40.0,0.0,0.0,"fourth");

  cloud->GetOutput()->GetVTKData()->Update();
  cloud->GetOutput()->Update();

	//Setting standard material to avoid random color selection
	cloud->GetMaterial()->m_Diffuse[0]=0.3;
	cloud->GetMaterial()->m_Diffuse[1]=0.6;
	cloud->GetMaterial()->m_Diffuse[2]=0.9;
	cloud->GetMaterial()->UpdateProp();

  cloud->Update();

  vtkALBASmartPointer<vtkDoubleArray> scalars;
  scalars->InsertNextTuple1(0.0);//This is a special value, with this value the pipe will not show the landmark
  scalars->InsertNextTuple1(10.0);
  scalars->InsertNextTuple1(20.0);
  scalars->InsertNextTuple1(30.0);
  scalars->SetName("scalars");

  vtkPolyData *ds = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
  ds->Update();
  int p = ds->GetNumberOfPoints();
  int c = ds->GetNumberOfCells();
  ds->GetPointData()->SetScalars(scalars);
  ds->UpdateData();

  cloud->GetMaterial();
  cloud->GetOutput()->GetVTKData()->Update();
  cloud->GetOutput()->Update();
  cloud->Update();

  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,cloud, NULL);

  albaPipeLandmarkCloud *pipe = new albaPipeLandmarkCloud();
  pipe->Create(sceneNode);
  pipe->Select(true);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

	COMPARE_IMAGES("TestPipeExecution", 0);

  m_Renderer->RemoveAllProps();
  m_RenderWindow->Render();
	
  pipe->SetScalarVisibility(true);
  pipe->OnEvent(&albaEvent(this,albaPipeLandmarkCloud::ID_SCALAR_VISIBILITY));
  actorList->RemoveAllItems();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

	COMPARE_IMAGES("TestPipeExecution", 1);

  m_Renderer->RemoveAllProps();
  m_RenderWindow->Render();

  pipe->SetScalarVisibility(false);
  pipe->OnEvent(&albaEventBase(this,albaPipeLandmarkCloud::ID_SCALAR_VISIBILITY));

  actorList->RemoveAllItems();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

	COMPARE_IMAGES("TestPipeExecution", 2);

  delete sceneNode;
  actorList->Delete();
}
