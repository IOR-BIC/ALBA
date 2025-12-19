/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeImage3DTest
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
#include "albaPipeImage3DTest.h"
#include "albaPipeImage3D.h"

#include "albaSceneNode.h"
#include "albaVMEImage.h"
#include "mmaMaterial.h"
#include "albaVMERoot.h"

#include "vtkALBAAssembly.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include <iostream>
#include "vtkJPEGReader.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

enum PIPE_IMAGE_ACTORS
  {
    PIPE_IMAGE_ACTOR,
  };

enum TESTS_PIPE_IMAGE
{
  USE_GRAY_IMAGE,
  USE_RGB_IMAGE,
};


//----------------------------------------------------------------------------
void albaPipeImage3DTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeImage3DTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void albaPipeImage3DTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeImage3DTest::TestPipeRGBImageExecution()
//----------------------------------------------------------------------------
{
  ////// Create VME (import vtkData) ////////////////////
	vtkJPEGReader *ImporterImage;
	vtkNEW(ImporterImage);
	albaString filenameImage=ALBA_DATA_ROOT;
	filenameImage<<"/Test_PipeImage3D/colour_blocks.jpg";
	ImporterImage->SetFileName(filenameImage);
	ImporterImage->Update();

  albaVMEImage *image;
  albaNEW(image);

  image->SetData((vtkImageData*)ImporterImage->GetOutput(),0.0);
  image->GetOutput()->Update();
  image->Update();

  
  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,image, NULL);

  /////////// Pipe Instance and Creation ///////////
  albaPipeImage3D *pipeImage3D = new albaPipeImage3D;
  pipeImage3D->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeImage3D->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    actor = actorList->GetNextProp();
  }
	
		
  vtkActor *surfaceActor;
  surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_IMAGE_ACTOR);
  CPPUNIT_ASSERT(surfaceActor != NULL);

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
	
	COMPARE_IMAGES("TestPipeRGBImageExecution", 0);

	CPPUNIT_ASSERT(!pipeImage3D->IsGrayImage());

  vtkDEL(actorList);

  delete sceneNode;

  albaDEL(image);
	vtkDEL(ImporterImage);
}
//----------------------------------------------------------------------------
void albaPipeImage3DTest::TestPipeGrayImageExecution()
//----------------------------------------------------------------------------
{
	////// Create VME (import vtkData) ////////////////////
	vtkJPEGReader *ImporterImage;
	vtkNEW(ImporterImage);
	albaString filenameImage=ALBA_DATA_ROOT;
	filenameImage<<"/Test_PipeImage3D/gray_blocks.jpg";
	ImporterImage->SetFileName(filenameImage);
	ImporterImage->Update();

	albaVMEImage *image;
	albaNEW(image);

	image->SetData((vtkImageData*)ImporterImage->GetOutput(),0.0);
	image->GetOutput()->Update();
	image->Update();


	//Assembly will be create when instancing albaSceneNode
	albaSceneNode *sceneNode;
	sceneNode = new albaSceneNode(NULL,NULL,image, NULL);

	/////////// Pipe Instance and Creation ///////////
	albaPipeImage3D *pipeImage3D = new albaPipeImage3D;
	pipeImage3D->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeImage3D->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while(actor)
	{   
		m_Renderer->AddActor(actor);
		actor = actorList->GetNextProp();
	}


	vtkActor *surfaceActor;
	surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_IMAGE_ACTOR);
	CPPUNIT_ASSERT(surfaceActor != NULL);

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeGrayImageExecution", 1);

	CPPUNIT_ASSERT(pipeImage3D->IsGrayImage());

	vtkDEL(actorList);

	delete sceneNode;

	albaDEL(image);
	vtkDEL(ImporterImage);
}

//----------------------------------------------------------------------------
vtkProp *albaPipeImage3DTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
