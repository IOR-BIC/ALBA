/*=========================================================================

 Program: MAF2
 Module: mafPipeMeterTest
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
#include "mafPipeMeterTest.h"
#include "mafPipeMeter.h"
#include "mafTransform.h"

#include "mafSceneNode.h"
#include "mafVMEVector.h"
#include "mmaMaterial.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEMeter.h"

#include "mafVMERoot.h"
#include "vtkMAFAssembly.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkMAFSmartPointer.h"

#include "vtkSphereSource.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include <iostream>


//----------------------------------------------------------------------------
void mafPipeMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(600, 600);
  m_RenderWindow->SetPosition(0,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
  ///////////// end render stuff /////////////////////////


  ////// Create VME ////////////////////
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);	
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();


  mafMatrix *matrix = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
  matrix->SetElement(0,3,4); //set a translation value on x axis of 4.0
  matrix->SetElement(1,3,3); //set a translation value on x axis of 3.0


  mafSmartPointer<mafVMEMeter> meter;
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetParent(storage->GetRoot());
  meter->GetOutput()->GetVTKData()->Update();
  meter->Modified();
  meter->Update();

	//Setting standard material to avoid random color selection
	meter->GetMaterial()->m_Diffuse[0]=0.3;
	meter->GetMaterial()->m_Diffuse[1]=0.6;
	meter->GetMaterial()->m_Diffuse[2]=0.9;
	meter->GetMaterial()->UpdateProp();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,meter, NULL);
  sceneNode->m_RenFront = m_Renderer;

  /////////// Pipe Instance and Creation ///////////
  mafPipeMeter *pipeMeter = new mafPipeMeter;
  pipeMeter->Create(sceneNode);
  
  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeMeter->GetAssemblyFront()->GetActors(actorList);
  
  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
    break;
  }

  vtkActor *vectorActor;
  vectorActor = (vtkActor *) SelectActorToControl(actorList, 0);
  CPPUNIT_ASSERT(vectorActor != NULL);

  m_RenderWindow->Render();
  mafSleep(800);
  CompareImages(0);

  vtkDEL(actorList);

  delete pipeMeter;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);

  mafDEL(storage);


  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeMeterTest::CompareImages(int testIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeMeter/";
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
    imageFile<<"/Test_PipeMeter/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeMeter/";
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
  imageFileOrig<<"/Test_PipeMeter/";
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
vtkProp *mafPipeMeterTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
