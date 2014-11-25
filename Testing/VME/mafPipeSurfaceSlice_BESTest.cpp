/*=========================================================================

 Program: MAF2Medical
 Module: mafPipeSurfaceSlice_BESTest
 Authors: Eleonora Mambrini
 
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
#include "mafPipeSurfaceSlice_BESTest.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mmaMaterial.h"
#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkSphereSource.h"

#include <iostream>

#define NUM_OF_CENTERS 3


//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestCloudClosePipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////

  mafSmartPointer<mafVMELandmark> first;
  first->SetPoint(0.0,0.0,0.0);
  first->SetName("first");
  mafSmartPointer<mafVMELandmark> second;
  second->SetPoint(1.0,0.0,0.0);
  second->SetName("second");
  mafSmartPointer<mafVMELandmark> third;
  third->SetPoint(0.0,0.0,5.0);
  third->SetName("third");
  mafSmartPointer<mafVMELandmarkCloud> ldm;
  ldm->SetLandmark(first);
  ldm->SetLandmark(second);
  ldm->SetLandmark(third);
  ldm->GetMaterial();
  ldm->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;

  ldm->GetOutput()->GetVTKData()->Update();
  ldm->GetOutput()->Update();
  ldm->Update();

  ldm->Close();

  ldm->GetOutput()->GetVTKData()->Update();
  ldm->GetOutput()->Update();
  ldm->Update();

  double bounds[6];
  ldm->GetOutput()->GetVTKData()->GetBounds(bounds);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,ldm, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  double origins[3][NUM_OF_CENTERS] = {{0.0,0.0,0.0},{5.0,5.0,-5.0},{0.0,0.0,5.0}};
  for(int i=3;i<3+NUM_OF_CENTERS;i++)
  {
    pipeSlice->SetSlice(origins[i-3], NULL);

    pipeSlice->GetAssemblyFront()->GetActors(actorList);
    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();

    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_Renderer->ResetCamera(bounds);
    m_RenderWindow->Render();

    CompareImages(i);
    mafSleep(2000);
  }

  mafSleep(2000);

  vtkDEL(actorList);

  delete pipeSlice;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;

  delete wxLog::SetActiveTarget(NULL);

}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestCloudOpenPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////

  mafSmartPointer<mafVMELandmarkCloud> ldm;
  ldm->AppendLandmark(0.0,0.0,0.0,"first");
  ldm->AppendLandmark(1.0,0.0,0.0,"second");
  ldm->AppendLandmark(0.0,0.0,5.0,"third");
  ldm->GetMaterial();
  ldm->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;

  ldm->GetOutput()->GetVTKData()->Update();
  ldm->GetOutput()->Update();
  ldm->Update();

  ldm->Open();

  ldm->GetOutput()->Update();
  ldm->Update();

  double bounds[6];
  ldm->GetOutput()->GetBounds(bounds);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,ldm, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  mafSleep(2000);

  double origins[3][NUM_OF_CENTERS] = {{0.0,0.0,0.0},{5.0,5.0,-5.0},{0.0,0.0,5.0}};
  for(int i=6;i<6+NUM_OF_CENTERS;i++)
  {
    pipeSlice->SetSlice(origins[i-6], NULL);

    pipeSlice->GetAssemblyFront()->GetActors(actorList);
    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();

    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_Renderer->ResetCamera(bounds);
    m_RenderWindow->Render();

    CompareImages(i);
    mafSleep(2000);
  }


  vtkDEL(actorList);

  delete pipeSlice;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestSurfacePipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME ////////////////////

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  mafSleep(2000);

  double origins[3][NUM_OF_CENTERS] = {0.0,0.0,0.0,0.0,0.0,7.0,0.0,0.0,-3.0};
  for(int i=0;i<NUM_OF_CENTERS;i++)
  {
    pipeSlice->SetSlice(origins[i], NULL);

    pipeSlice->GetAssemblyFront()->GetActors(actorList);
    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();

    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_Renderer->ResetCamera(bounds);
    m_RenderWindow->Render();

    CompareImages(i);
    mafSleep(2000);
  }

  vtkDEL(actorList);

  delete pipeSlice;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::CompareImages(int testIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeSurfaceSlice_BES/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<testIndex;
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
    imageFile<<"/Test_PipeSurfaceSlice_BES/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeSurfaceSlice_BES/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

  imageFile<<testIndex;
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
  imageFileOrig<<"/Test_PipeSurfaceSlice_BES/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<testIndex;
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
void mafPipeSurfaceSlice_BESTest::TestSurfacePipeCreation()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME ////////////////////

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
  pipeSlice->Create(sceneNode);

  CPPUNIT_ASSERT(pipeSlice);

  delete pipeSlice;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;

}

//----------------------------------------------------------------------------
void mafPipeSurfaceSlice_BESTest::TestSetGetThickness()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME ////////////////////

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(8.0);
  sphere->Update();

  double bounds[6];
  sphere->GetOutput()->GetBounds(bounds);

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->GetMaterial();
  surface->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  surface->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeSurfaceSlice_BES *pipeSlice = new mafPipeSurfaceSlice_BES;
  pipeSlice->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeSlice->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera(bounds);
  m_RenderWindow->Render();

  double thickness = 1.0;
  pipeSlice->SetThickness(thickness);

  double result_thickness = pipeSlice->GetThickness();

  CPPUNIT_ASSERT(result_thickness == thickness);

  vtkDEL(actorList);

  delete pipeSlice;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;
}