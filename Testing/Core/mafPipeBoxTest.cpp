/*=========================================================================

 Program: MAF2
 Module: mafPipeBoxTest
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
#include "mafPipeBoxTest.h"
#include "mafPipeBox.h"

#include "mafSceneNode.h"
#include "mafVMESurface.h"

#include "vtkMAFAssembly.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPolyDataReader.h"
#include "vtkCamera.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>

enum PIPE_BOX_ACTORS
{
  PIPE_BOX_ACTOR,
  PIPE_BOX_ACTOR_WIRED,
  PIPE_BOX_ACTOR_OUTLINE_CORNER,
  PIPE_BOX_NUMBER_OF_ACTORS,
};

//----------------------------------------------------------------------------
void mafPipeBoxTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::TestPipe3DExecution()
//----------------------------------------------------------------------------
{

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMESurface *surfaceParent;
  mafNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->GetVTKData()->Update();
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  mafString filename2=MAF_DATA_ROOT;
  filename2<<"/PipeBox/VTK/Cube2T0.vtk";
  importer->SetFileName(filename2.GetCStr());
  importer->Update();

  mafVMESurface *surfaceChild;
  mafNEW(surfaceChild);
  surfaceChild->SetData(importer->GetOutput(),0.0);
  surfaceChild->GetOutput()->GetVTKData()->Update();
  surfaceChild->GetOutput()->Update();
  surfaceChild->Update();

  surfaceChild->ReparentTo(surfaceParent);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeBox *pipeBox = new mafPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(mafPipeBox::BOUNDS_3D);
  pipeBox->OnEvent(&mafEvent(this, mafPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  char *strings="3D";

  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);
  CompareImages(strings);

  vtkDEL(actorList);
  
  delete pipeBox;
  delete sceneNode;

  mafDEL(surfaceChild);
  mafDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::TestPipe3DSubtreeExecution()
//----------------------------------------------------------------------------
{

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMESurface *surfaceParent;
  mafNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->GetVTKData()->Update();
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  mafString filename2=MAF_DATA_ROOT;
  filename2<<"/PipeBox/VTK/Cube2T0.vtk";
  importer->SetFileName(filename2.GetCStr());
  importer->Update();

  mafVMESurface *surfaceChild;
  mafNEW(surfaceChild);
  surfaceChild->SetData(importer->GetOutput(),0.0);
  surfaceChild->GetOutput()->GetVTKData()->Update();
  surfaceChild->GetOutput()->Update();
  surfaceChild->Update();

  surfaceChild->ReparentTo(surfaceParent);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeBox *pipeBox = new mafPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(mafPipeBox::BOUNDS_3D_SUBTREE);
  pipeBox->OnEvent(&mafEvent(this, mafPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  char *strings="3DSubtree";

  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);
  CompareImages(strings);

  vtkDEL(actorList);

  delete pipeBox;
  delete sceneNode;

  mafDEL(surfaceChild);
  mafDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::TestPipe4DExecution()
//----------------------------------------------------------------------------
{

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMESurface *surfaceParent;
  mafNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->GetVTKData()->Update();
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  mafString filename2=MAF_DATA_ROOT;
  filename2<<"/PipeBox/VTK/Cube1T1.vtk";
  importer->SetFileName(filename2.GetCStr());
  importer->Update();

  surfaceParent->SetData(importer->GetOutput(),1.0);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeBox *pipeBox = new mafPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(mafPipeBox::BOUNDS_4D);
  pipeBox->OnEvent(&mafEvent(this, mafPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  char *strings="4D";

  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);
  CompareImages(strings);

  vtkDEL(actorList);

  delete pipeBox;
  delete sceneNode;

  mafDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::TestPipe4DSubtreeExecution()
//----------------------------------------------------------------------------
{

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////


  ////// Create VME (import vtkData) ////////////////////
  vtkPolyDataReader *importer;
  vtkNEW(importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube1T0.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  mafVMESurface *surfaceParent;
  mafNEW(surfaceParent);
  surfaceParent->SetData(importer->GetOutput(),0.0);
  surfaceParent->GetOutput()->GetVTKData()->Update();
  surfaceParent->GetOutput()->Update();
  surfaceParent->Update();

  filename=MAF_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube1T1.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  surfaceParent->SetData(importer->GetOutput(),1.0);

  filename=MAF_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube2T0.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  mafVMESurface *surfaceChild;
  mafNEW(surfaceChild);
  surfaceChild->SetData(importer->GetOutput(),0.0);
  surfaceChild->GetOutput()->GetVTKData()->Update();
  surfaceChild->GetOutput()->Update();
  surfaceChild->Update();

  filename=MAF_DATA_ROOT;
  filename<<"/PipeBox/VTK/Cube2T1.vtk";
  importer->SetFileName(filename.GetCStr());
  importer->Update();

  surfaceChild->SetData(importer->GetOutput(),1.0);

  surfaceChild->ReparentTo(surfaceParent);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,surfaceParent, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipeBox *pipeBox = new mafPipeBox;
  pipeBox->Create(sceneNode);
  pipeBox->SetBoundsMode(mafPipeBox::BOUNDS_4D_SUBTREE);
  pipeBox->OnEvent(&mafEvent(this, mafPipeBox::ID_BOUNDS_MODE));

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipeBox->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  char *strings="4DSubtree";

  m_RenderWindow->Render();
  printf("\n Visualization: %s \n", strings);
  CompareImages(strings);

  vtkDEL(actorList);

  delete pipeBox;
  delete sceneNode;

  mafDEL(surfaceChild);
  mafDEL(surfaceParent);
  vtkDEL(importer);
}
//----------------------------------------------------------------------------
void mafPipeBoxTest::CompareImages(char *type)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);



  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/PipeBox/ImageToCompare/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<type;
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
    imageFile<<"/PipeBox/ImageToCompare/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/PipeBox/ImageToCompare/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

  imageFile<<type;
  imageFile<<".jpg";
  w->SetFileName(imageFile.GetCStr());
  w->Write();

  if(!controlStream)
  {
    vtkDEL(w);
    vtkDEL(w2i);

    controlStream.close();
    return;
  }
  controlStream.close();

  //read original Image
  vtkJPEGReader *rO;
  vtkNEW(rO);
  mafString imageFileOrig=MAF_DATA_ROOT;
  imageFileOrig<<"/PipeBox/ImageToCompare/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<type;
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


  vtkImageMathematics *imageMath;
  vtkNEW(imageMath);
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