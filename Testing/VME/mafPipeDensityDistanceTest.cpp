/*=========================================================================

 Program: MAF2
 Module: mafPipeDensityDistanceTest
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
#include "mafPipeDensityDistanceTest.h"
#include "mafPipeDensityDistance.h"

#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"


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


// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>


//----------------------------------------------------------------------------
void mafPipeDensityDistanceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeDensityDistanceTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeDensityDistanceTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeDensityDistanceTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create surface VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Surfaces/surface.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMESurface *surface;
  mafNEW(surface);
  surface->SetData((vtkPolyData*)Importer->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  ////// Create volume VME (import vtkData) ////////////////////
  filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();
  
  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surface, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeDensityDistance *pipeDD = new mafPipeDensityDistance;
  pipeDD->SetVolume(volume);
  pipeDD->Create(sceneNode);
  pipeDD->m_RenFront = m_Renderer;
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeDD->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  vtkActor *cloudActor;
  cloudActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(cloudActor != NULL);

  m_RenderWindow->Render();
  mafSleep(800);
  CompareImages(0);
  vtkDEL(actorList);

  delete pipeDD;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;

  mafDEL(surface);
  mafDEL(volume);
  vtkDEL(Importer);
  
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeDensityDistanceTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeDensityDistance/";
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
    imageFile<<"/Test_PipeDensityDistance/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeDensityDistance/";
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
  imageFileOrig<<"/Test_PipeDensityDistance/";
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
vtkProp *mafPipeDensityDistanceTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
