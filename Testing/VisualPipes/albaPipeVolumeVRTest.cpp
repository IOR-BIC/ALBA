/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: medPipeVolumeVRTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medPipeVolumeVRTest.h"
#include "medPipeVolumeVR.h"

#include "albaSceneNode.h"
#include "albaVMEVolumeGray.h"


#include "albaVMERoot.h"
#include "vtkALBAAssembly.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkProp3DCollection.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>


//----------------------------------------------------------------------------
void medPipeVolumeVRTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medPipeVolumeVRTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void medPipeVolumeVRTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void medPipeVolumeVRTest::TestPipeExecution()
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

  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=MED_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  albaVMEVolumeGray *volume;
  albaNEW(volume);
  volume->SetData((vtkImageData*)Importer->GetOutput(),0.0);
  volume->GetOutput()->Update();
  volume->Update();

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  medPipeVolumeVR *pipeVolume = new medPipeVolumeVR;
  pipeVolume->Create(sceneNode);
  pipeVolume->SetResampleFactor(0.5);
  
  ////////// ACTORS List ///////////////
  vtkProp3DCollection *actorList = vtkProp3DCollection::New();
  actorList = pipeVolume->GetAssemblyFront()->GetParts();
    
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddVolume(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  vtkVolume *volumeActor;
  volumeActor = (vtkVolume *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(volumeActor != NULL);

  m_RenderWindow->Render();
  albaSleep(5000);
  CompareImages(0);
  
  delete pipeVolume;
  delete sceneNode;

  albaDEL(volume);
  vtkDEL(Importer);
  
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void medPipeVolumeVRTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  albaString controlOriginFile=MED_DATA_ROOT;
  controlOriginFile<<"/Test_PipeVolumeVR/";
  controlOriginFile<<name.char_str();
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
  albaString imageFile=MED_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_PipeVolumeVR/";
    imageFile<<name.char_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeVolumeVR/";
    imageFile<<name.char_str();
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
  albaString imageFileOrig=MED_DATA_ROOT;
  imageFileOrig<<"/Test_PipeVolumeVR/";
  imageFileOrig<<name.char_str();
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
  imageMath->GetOutput()->Update();

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
vtkProp *medPipeVolumeVRTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
