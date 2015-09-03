/*=========================================================================

 Program: MAF2
 Module: mafPipeImage3DTest
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
#include "mafPipeImage3DTest.h"
#include "mafPipeImage3D.h"

#include "mafSceneNode.h"
#include "mafVMEImage.h"
#include "mmaMaterial.h"

#include "mafVMERoot.h"
#include "vtkMAFAssembly.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkPolyData.h"
#include "vtkJPEGReader.h"
#include "vtkDoubleArray.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include <iostream>

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
void mafPipeImage3DTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeImage3DTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeImage3DTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeImage3DTest::TestPipeRGBImageExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME (import vtkData) ////////////////////
	vtkJPEGReader *ImporterImage;
	vtkNEW(ImporterImage);
	mafString filenameImage=MAF_DATA_ROOT;
	filenameImage<<"/Test_PipeImage3D/colour_blocks.jpg";
	ImporterImage->SetFileName(filenameImage);
	ImporterImage->Update();

  mafVMEImage *image;
  mafNEW(image);

  image->SetData((vtkImageData*)ImporterImage->GetOutput(),0.0);
  image->GetOutput()->Update();
  image->Update();

  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,image, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeImage3D *pipeImage3D = new mafPipeImage3D;
  pipeImage3D->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeImage3D->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }
	
		
  vtkActor *surfaceActor;
  surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_IMAGE_ACTOR);
  CPPUNIT_ASSERT(surfaceActor != NULL);

  m_RenderWindow->Render();
	printf("\n-> RGB <-\n");
  CompareImages(0);

	CPPUNIT_ASSERT(!pipeImage3D->IsGrayImage());

  vtkDEL(actorList);

  delete pipeImage3D;
  delete sceneNode;

  mafDEL(image);
	vtkDEL(ImporterImage);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeImage3DTest::TestPipeGrayImageExecution()
//----------------------------------------------------------------------------
{
	///////////////// render stuff /////////////////////////

	m_Renderer->SetBackground(0.1, 0.1, 0.1);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(320, 240);
	m_RenderWindow->SetPosition(400,0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

	///////////// end render stuff /////////////////////////


	////// Create VME (import vtkData) ////////////////////
	vtkJPEGReader *ImporterImage;
	vtkNEW(ImporterImage);
	mafString filenameImage=MAF_DATA_ROOT;
	filenameImage<<"/Test_PipeImage3D/gray_blocks.jpg";
	ImporterImage->SetFileName(filenameImage);
	ImporterImage->Update();

	mafVMEImage *image;
	mafNEW(image);

	image->SetData((vtkImageData*)ImporterImage->GetOutput(),0.0);
	image->GetOutput()->Update();
	image->Update();


	//Assembly will be create when instancing mafSceneNode
	mafSceneNode *sceneNode;
	sceneNode = new mafSceneNode(NULL,NULL,image, NULL);

	/////////// Pipe Instance and Creation ///////////
	mafPipeImage3D *pipeImage3D = new mafPipeImage3D;
	pipeImage3D->Create(sceneNode);

	////////// ACTORS List ///////////////
	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeImage3D->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while(actor)
	{   
		m_Renderer->AddActor(actor);
		m_RenderWindow->Render();

		actor = actorList->GetNextProp();
	}


	vtkActor *surfaceActor;
	surfaceActor = (vtkActor *) SelectActorToControl(actorList, PIPE_IMAGE_ACTOR);
	CPPUNIT_ASSERT(surfaceActor != NULL);

	m_RenderWindow->Render();
	printf("\n-> Gray <-\n");
	CompareImages(1);

	CPPUNIT_ASSERT(pipeImage3D->IsGrayImage());

	vtkDEL(actorList);

	delete pipeImage3D;
	delete sceneNode;

	mafDEL(image);
	vtkDEL(ImporterImage);

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeImage3DTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeImage3D/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<scalarIndex;
  controlOriginFile<<".jpg";

  fstream controlStream;
  controlStream.open(controlOriginFile.GetCStr()); 

  // visualization control
	m_RenderWindow->OffScreenRenderingOn();
  vtkWindowToImageFilter *w2i;
  vtkNEW(w2i);
  w2i->SetInput(m_RenderWindow);
  //w2i->SetMagnification(magnification);
  w2i->Update();
	m_RenderWindow->OffScreenRenderingOff();

  //write comparing image
  vtkJPEGWriter *w;
  vtkNEW(w);
  w->SetInput(w2i->GetOutput());
  mafString imageFile=MAF_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_PipeImage3D/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeImage3D/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }
  
  imageFile<<scalarIndex;
  imageFile<<".jpg";
  w->SetFileName(imageFile.GetCStr());
  w->Write();

  if(!controlStream)
  {
    controlStream.close();
    vtkDEL(w);
    vtkDEL(w2i);
    return;
  }
  controlStream.close();

  //read original Image
  vtkJPEGReader *rO;
  vtkNEW(rO);
  mafString imageFileOrig=MAF_DATA_ROOT;
  imageFileOrig<<"/Test_PipeImage3D/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<scalarIndex;
  imageFileOrig<<".jpg";
  rO->SetFileName(imageFileOrig.GetCStr());
  rO->Update();

  vtkImageData *imDataOrig = rO->GetOutput();

  //read compared image
  vtkJPEGReader *rC;
  vtkNEW(rC);
  rC->SetFileName(imageFile.GetCStr());
  rC->Update();

  vtkImageData *imDataComp = rC->GetOutput();


  vtkImageMathematics *imageMath = vtkImageMathematics::New();
  imageMath->SetInput1(imDataOrig);
  imageMath->SetInput2(imDataComp);
  imageMath->SetOperationToSubtract();
  imageMath->Update();

  double srR[2] = {-1,1};
  imageMath->GetOutput()->GetPointData()->GetScalars()->GetRange(srR);

  CPPUNIT_ASSERT(srR[0] == 0.0 && srR[1] == 0.0);

  // end visualization control
  vtkDEL(imageMath);
  vtkDEL(rC);
  vtkDEL(rO);

  vtkDEL(w);
  vtkDEL(w2i);
}
//----------------------------------------------------------------------------
vtkProp *mafPipeImage3DTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
