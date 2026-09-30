/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeProjectedTest
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
#include "albaPipeVolumeProjectedTest.h"
#include "albaPipeVolumeProjected.h"

#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkCamera.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <iostream>
#include <fstream>
#include "albaVMEOutputVolume.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
void albaPipeVolumeProjectedTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjectedTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
	m_Renderer->GetActiveCamera()->SetPosition(5, 5, 5);
}
//----------------------------------------------------------------------------
void albaPipeVolumeProjectedTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjectedTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volumeInput;
  albaNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  albaNEW(material);

  albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volumeInput, m_Renderer);
	int testNum = 0;
  for (int camera = CAMERA_RX_FRONT; camera <= CAMERA_RX_RIGHT; camera++)
  {
    /////////// Pipe Instance and Creation ///////////
    albaPipeVolumeProjected *pipeProjected = new albaPipeVolumeProjected;
    pipeProjected->InitializeProjectParameters(camera);
    pipeProjected->Create(sceneNode);

    ////////// ACTORS List ///////////////
    vtkPropCollection *actorList = vtkPropCollection::New();
    pipeProjected->GetAssemblyFront()->GetActors(actorList);

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      actor = actorList->GetNextProp();
    }

    vtkActor *projActor;
    projActor = (vtkActor *) SelectActorToControl(actorList, 0);
    CPPUNIT_ASSERT(projActor != NULL);

    m_Renderer->ResetCamera();
    m_RenderWindow->Render();
		COMPARE_IMAGES("TestPipeExecution", testNum);

    m_Renderer->RemoveAllViewProps();
    vtkDEL(actorList);
		sceneNode->DeletePipe();

		testNum++;
  }
   
  delete sceneNode;

  albaDEL(material);
  albaDEL(volumeInput);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
void albaPipeVolumeProjectedTest::TestProjectionRange()
{
	////// Create VME (import vtkData) ////////////////////
	vtkDataSetReader *Importer;
	vtkNEW(Importer);
	albaString filename = ALBA_DATA_ROOT;
	filename << "/VTK_Volumes/volume.vtk";
	Importer->SetFileName(filename);
	Importer->Update();
	albaVMEVolumeGray *volumeInput;
	albaNEW(volumeInput);
	volumeInput->SetData((vtkImageData*)Importer->GetOutput(), 0.0);
	volumeInput->GetOutput()->Update();
	volumeInput->Update();
	
	//Creating a range valid for all sides
	int *dims = vtkImageData::SafeDownCast(volumeInput->GetOutput()->GetVTKData())->GetDimensions();
	int minDim = MIN(dims[0], MIN(dims[1], dims[2]));
	int prjRange[2];
	prjRange[0] = minDim/4;
	prjRange[1] = prjRange[0] * 3;
	

	mmaVolumeMaterial *material;
	albaNEW(material);

	albaVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL, NULL, volumeInput, m_Renderer);
	int testNum = 0;
	for (int camera = CAMERA_RX_FRONT; camera <= CAMERA_RX_RIGHT; camera++)
	{
		/////////// Pipe Instance and Creation ///////////
		albaPipeVolumeProjected *pipeProjected = new albaPipeVolumeProjected;
		pipeProjected->InitializeProjectParameters(camera);
		pipeProjected->EnableRangeProjection(true);
		pipeProjected->SetProjectionRange(prjRange);
		pipeProjected->Create(sceneNode);

		////////// ACTORS List ///////////////
		vtkPropCollection *actorList = vtkPropCollection::New();
		pipeProjected->GetAssemblyFront()->GetActors(actorList);

		actorList->InitTraversal();
		vtkProp *actor = actorList->GetNextProp();
		while (actor)
		{
			m_Renderer->AddActor(actor);
			actor = actorList->GetNextProp();
		}

		vtkActor *projActor;
		projActor = (vtkActor *)SelectActorToControl(actorList, 0);
		CPPUNIT_ASSERT(projActor != NULL);

		m_Renderer->ResetCamera();
		m_RenderWindow->Render();
		COMPARE_IMAGES("TestProjectionRange", testNum);

		m_Renderer->RemoveAllViewProps();
		vtkDEL(actorList);
		sceneNode->DeletePipe();

		testNum++;
	}

	delete sceneNode;

	albaDEL(material);
	albaDEL(volumeInput);
	vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeVolumeProjectedTest::SelectActorToControl(vtkPropCollection *propList, int index)
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