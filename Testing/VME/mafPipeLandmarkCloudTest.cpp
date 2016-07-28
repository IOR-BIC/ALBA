/*=========================================================================

 Program: MAF2
 Module: mafPipeLandmarkCloudTest
 Authors: Matteo Giacomoni
 
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

#include "mafPipeLandmarkCloudTest.h"
#include "mafPipeLandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkLogLookupTable.h"
#include "vtkCellData.h"
#include "vtkDataSetWriter.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::BeforeTest()
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
void mafPipeLandmarkCloudTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafPipeLandmarkCloud *pipeCloud = new mafPipeLandmarkCloud();

  delete pipeCloud;
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> cloud;
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

  vtkMAFSmartPointer<vtkDoubleArray> scalars;
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

  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,cloud, NULL);

  mafPipeLandmarkCloud *pipe = new mafPipeLandmarkCloud();
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
	
  pipe->SetScalarVisibility(TRUE);
  pipe->OnEvent(&mafEvent(this,mafPipeLandmarkCloud::ID_SCALAR_VISIBILITY));
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

  pipe->SetScalarVisibility(FALSE);
  pipe->OnEvent(&mafEventBase(this,mafPipeLandmarkCloud::ID_SCALAR_VISIBILITY));

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
