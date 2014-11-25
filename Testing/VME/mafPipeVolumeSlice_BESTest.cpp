/*=========================================================================

 Program: MAF2Medical
 Module: mafPipeVolumeSlice_BESTest
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
#include "mafPipeVolumeSlice_BESTest.h"
#include "mafPipeVolumeSlice_BES.h"

#include "mafSceneNode.h"
#include "mafVMEVolumeGray.h"
#include "mafGUILutPreset.h"
#include "mmaVolumeMaterial.h"

#include "vtkMAFAssembly.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkStructuredPointsReader.h"
#include "vtkCamera.h"
#include "vtkLookupTable.h"

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
void mafPipeVolumeSlice_BESTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}

enum ID_TEST
{
  ID_TEST_PIPEEXECUTION = 0,
  ID_TEST_PIPEEXECUTION_SLICEOPACITY = 10,
  ID_TEST_PIPEEXECUTION_LUTRANGE = 20,
  ID_TEST_PIPEEXECUTION_COLORLOOKUPTABLE = 30,
  ID_TEST_PIPEEXECUTION_TICKS = 40,
  ID_TEST_PIPEEXECUTION_SHOWSLIDER = 50,
};

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::TestPipeExecution()
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
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice_BES/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeSlice_BES::SLICE_X ; direction<=mafPipeVolumeSlice_BES::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeSlice_BES *pipeSlice = new mafPipeVolumeSlice_BES;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
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
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeSlice_BES::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);
      mafSleep(1000);
      CompareImages(3*direction+i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    delete pipeSlice;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::CompareImages(int imageIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);



  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeVolumeSlice_BES/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<imageIndex;
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
    imageFile<<"/Test_PipeVolumeSlice_BES/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeVolumeSlice_BES/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

  imageFile<<imageIndex;
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
  imageFileOrig<<"/Test_PipeVolumeSlice_BES/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<imageIndex;
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

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::TestPipeExecution_SetSliceOpacity()
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
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice_BES/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeSlice_BES::SLICE_X ; direction<=mafPipeVolumeSlice_BES::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeSlice_BES *pipeSlice = new mafPipeVolumeSlice_BES;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    double opacity = 0.5;
    //opacity = rand()/RAND_MAX;

    pipeSlice->SetSliceOpacity(opacity);

    CPPUNIT_ASSERT(opacity == pipeSlice->GetSliceOpacity());

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
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeSlice_BES::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);
      mafSleep(1000);
      CompareImages(ID_TEST_PIPEEXECUTION_SLICEOPACITY + 3*direction+i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    delete pipeSlice;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);

  delete wxLog::SetActiveTarget(NULL);

}

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::TestPipeExecution_SetLutRange()
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
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice_BES/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeSlice_BES::SLICE_X ; direction<=mafPipeVolumeSlice_BES::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeSlice_BES *pipeSlice = new mafPipeVolumeSlice_BES;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    double range[2], range_out[2];
    range[0] = 0.2;
    range[1] = 0.5;

    pipeSlice->SetLutRange(range[0], range[1]);
    pipeSlice->GetLutRange(range_out);

    CPPUNIT_ASSERT(range[0] == range_out[0] && range[1] == range_out[1]);

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
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeSlice_BES::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);
      mafSleep(1000);
      CompareImages(ID_TEST_PIPEEXECUTION_LUTRANGE + 3*direction+i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    delete pipeSlice;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);

  delete wxLog::SetActiveTarget(NULL);

}

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::TestPipeExecution_SetColorLookupTable()
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
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice_BES/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  for (int direction = mafPipeVolumeSlice_BES::SLICE_X ; direction<=mafPipeVolumeSlice_BES::SLICE_Z;direction++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeSlice_BES *pipeSlice = new mafPipeVolumeSlice_BES;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);

    vtkLookupTable *lut = vtkLookupTable::New();
    lutPreset(6, lut);

    pipeSlice->SetColorLookupTable(lut);

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
    double x,y,z,vx,vy,vz;
    switch(direction)
    {
    case mafPipeVolumeSlice_BES::SLICE_X:
      {
        //x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
        x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Y:
      {
        x=0; y=-1; z=0; vx=0; vy=0; vz=1;
      }
      break;
    case mafPipeVolumeSlice_BES::SLICE_Z:
      {
        //x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
        x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
      }
      break;
    }

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = 0.0;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);
      mafSleep(1000);
      CompareImages(ID_TEST_PIPEEXECUTION_COLORLOOKUPTABLE + 3*direction+i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    vtkDEL(lut);
    delete pipeSlice;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);

  delete wxLog::SetActiveTarget(NULL);

}

//----------------------------------------------------------------------------
void mafPipeVolumeSlice_BESTest::TestPipeExecution_TicksOnOff()
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
  vtkStructuredPointsReader *importer;
  vtkNEW(importer);
  mafString filename1=MAF_DATA_ROOT;
  filename1<<"/Test_PipeVolumeSlice_BES/VolumeSP.vtk";
  importer->SetFileName(filename1.GetCStr());
  importer->Update();

  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkImageData*)importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double zValue[3][3]={{4.0,4.0,0.0},{4.0,4.0,1.0},{4.0,4.0,2.0}};

  int direction = mafPipeVolumeSlice_BES::SLICE_X;

  for (int showticks = 0; showticks<2; showticks++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeVolumeSlice_BES *pipeSlice = new mafPipeVolumeSlice_BES;
    pipeSlice->InitializeSliceParameters(direction,zValue[0],true);
    pipeSlice->Create(sceneNode);
    if(showticks)
      pipeSlice->ShowTICKsOn();
    else
      pipeSlice->ShowTICKsOff();


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
    double x,y,z,vx,vy,vz;

    x=1 ;y=0; z=0; vx=0; vy=0; vz=1;

    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    m_Renderer->GetActiveCamera()->SetFocalPoint(0,0,0);
    m_Renderer->GetActiveCamera()->SetPosition(x*100,y*100,z*100);
    m_Renderer->GetActiveCamera()->SetViewUp(vx,vy,vz);
    m_Renderer->GetActiveCamera()->SetClippingRange(0.1,1000);

    for(int i=0;i<3;i++)
    {
      //
      double normals[3];
      normals[0] = i/10;
      normals[1] = 0.0;
      normals[2] = 0.0;
      pipeSlice->SetSlice(zValue[i], normals);


      m_Renderer->ResetCamera();

      char *strings="Slice";

      m_RenderWindow->Render();
      printf("\n Visualization: %s \n", strings);
      mafSleep(1000);
      CompareImages(ID_TEST_PIPEEXECUTION_TICKS + showticks*3 +i);
    }

    m_Renderer->RemoveAllProps();
    vtkDEL(actorList);
    delete pipeSlice;
  }
  

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(importer);

  delete wxLog::SetActiveTarget(NULL);

}