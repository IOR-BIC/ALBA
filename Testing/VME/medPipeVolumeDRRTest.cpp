/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeVolumeDRRTest.cpp,v $
Language:  C++
Date:      $Date: 2008-11-13 11:41:17 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2008
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medPipeVolumeDRRTest.h"
#include "medPipeVolumeDRR.h"

#include "mafSceneNode.h"
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

enum TESTS_PIPE_SURFACE
{
  BASE_TEST,
  COLOR_TEST,
  EXPOSURE_TEST,
  GAMMA_TEST,
  CAMERA_ANGLE_TEST,
  CAMERA_POSITION_TEST,
  CAMERA_FOCUS_TEST,
  CAMERA_ROLL_TEST,
  RESAMPLE_FACTOR_TEST,
  NUMBER_OF_TEST,
};



//----------------------------------------------------------------------------
void medPipeVolumeDRRTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medPipeVolumeDRRTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void medPipeVolumeDRRTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void medPipeVolumeDRRTest::TestPipeExecution()
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
  ////// Create VME (import vtkData) ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MED_DATA_ROOT;
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
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  medPipeVolumeDRR *pipeDRR = new medPipeVolumeDRR;
  pipeDRR->Create(sceneNode);
  pipeDRR->m_RenFront = m_Renderer;
  pipeDRR->SetGamma(2.2);
  double position[3] = {70.,111,213};
  double focus[3] = {70.,71., 51.};
  pipeDRR->SetCameraPosition(position);
  pipeDRR->SetCameraFocus(focus);
  pipeDRR->SetCameraAngle(20);
  pipeDRR->SetCameraRoll(0);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeDRR->GetAssemblyFront()->GetVolumes(actorList);

  const char *strings[NUMBER_OF_TEST];
  strings[0] = "BASE_TEST";
  strings[1] = "COLOR_TEST";
  strings[2] = "EXPOSURE_TEST";
  strings[3] = "GAMMA_TEST";
  strings[4] = "CAMERA_ANGLE_TEST";
  strings[5] = "CAMERA_POSITION_TEST";
  strings[6] = "CAMERA_FOCUS_TEST";
  strings[7] = "CAMERA_ROLL_TEST";
  strings[8] = "RESAMPLE_FACTOR_TEST";

  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
    if(i == BASE_TEST)
    {
      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == COLOR_TEST)
    {
      pipeDRR->SetColor(wxColour(200,0,0));
      
      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == EXPOSURE_TEST)
    {
      double value[2] = {-0.6,-0.5};
      pipeDRR->SetExposureCorrection(value);
      
      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == GAMMA_TEST)
    {
      pipeDRR->SetGamma(0.1);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == CAMERA_ANGLE_TEST)
    {
      pipeDRR->SetCameraAngle(10);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == CAMERA_POSITION_TEST)
    {
      double newPosition[3] = {40,40,100};
      pipeDRR->SetCameraPosition(newPosition);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == CAMERA_FOCUS_TEST)
    {
      double newFocus[3] = {70,71,31};
      pipeDRR->SetCameraFocus(newFocus);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == CAMERA_ROLL_TEST)
    {
      double newFocus[3] = {70,71,31};
      pipeDRR->SetCameraRoll(20);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    else if(i == RESAMPLE_FACTOR_TEST)
    {
      pipeDRR->SetResampleFactor(0.5);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddVolume(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }

    }
    
    vtkVolume *volumeActor;
    volumeActor = (vtkVolume *) SelectActorToControl(actorList, 0);
    CPPUNIT_ASSERT(volumeActor != NULL);

    m_RenderWindow->Render();
    printf("\n Visualizzazione: %s \n", strings[i]);
    mafSleep(800);
    CompareImages(i);

    if(i == COLOR_TEST)
    {
      pipeDRR->SetColor(wxColour(128,128,128));
    }
    else if(i == EXPOSURE_TEST)
    {
      double value[2] = {0,0};
      pipeDRR->SetExposureCorrection(value);
    }
    else if(i == GAMMA_TEST)
    {
      pipeDRR->SetGamma(2.2);
    }
    else if(i == CAMERA_ANGLE_TEST)
    {
      pipeDRR->SetCameraAngle(20);
    }
    else if(i == CAMERA_POSITION_TEST)
    {
      pipeDRR->SetCameraPosition(position);
    }
    else if(i == CAMERA_FOCUS_TEST)
    {
      pipeDRR->SetCameraFocus(focus);
    }
    else if(i == CAMERA_ROLL_TEST)
    {
      pipeDRR->SetCameraRoll(0);
    }
    else if(i == RESAMPLE_FACTOR_TEST)
    {
      pipeDRR->SetResampleFactor(1.);
    }


  }



	

  vtkDEL(actorList);

  delete pipeDRR;
  delete sceneNode;

  mafDEL(volume);
  vtkDEL(Importer);
  
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void medPipeVolumeDRRTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MED_DATA_ROOT;
  controlOriginFile<<"/Test_PipeVolumeDRR/";
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
  mafString imageFile=MED_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_PipeVolumeDRR/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeVolumeDRR/";
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
  mafString imageFileOrig=MED_DATA_ROOT;
  imageFileOrig<<"/Test_PipeVolumeDRR/";
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
vtkProp *medPipeVolumeDRRTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
