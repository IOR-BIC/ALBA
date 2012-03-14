/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeIsosurfaceGPUTest.cpp,v $
Language:  C++
Date:      $Date: 2009-04-16 13:36:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafPipeIsosurfaceGPUTest.h"
#include "mafPipeIsosurfaceGPU.h"

#include "mafSceneNode.h"
#include "mafVMEVolumeGray.h"
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
#include "vtkRectilinearGrid.h"
#include "vtkProp3DCollection.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include <fstream>

enum ID_TEST_FUNCTIONS
{
  ID_CONTOUR_COMPARING = 0,
  ID_OPACITY_COMPARING = 2,
};

//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestPipeExecutionCountour()
//----------------------------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double scalarValue[2] = {1.0,0.0};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeIsosurfaceGPU *pipeIso = new mafPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&mafEvent(this, mafPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(scalarValue[v]);

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_RenderWindow->Render();

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();
    
    printf("\n Visualization: \n");
    CompareImages(v + ID_CONTOUR_COMPARING);
    mafSleep(1000);

    m_Renderer->RemoveAllProps();
    //vtkDEL(actorList);
    delete pipeIso;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(Importer);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestPipeExecutionOpacity()
//----------------------------------------------------------------------------
{

  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double opacity[2] = {1.0,0.8};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeIsosurfaceGPU *pipeIso = new mafPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&mafEvent(this, mafPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(1.0);
    pipeIso->SetAlphaValue(opacity[v]); //opacity

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_RenderWindow->Render();

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();

    printf("\n Visualization: \n");
    CompareImages(v + ID_OPACITY_COMPARING);
    mafSleep(1000);

    m_Renderer->RemoveAllProps();
    //vtkDEL(actorList);
    delete pipeIso;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(Importer);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::TestExtractIsosurface()
//----------------------------------------------------------------------------
{
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeIsosurfaceGPU/volumeRG.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  mafVMEVolumeGray *volumeInput;
  mafNEW(volumeInput);
  volumeInput->SetData((vtkRectilinearGrid*)Importer->GetOutput(),0.0);
  volumeInput->GetOutput()->GetVTKData()->Update();
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////

  mmaVolumeMaterial *material;
  mafNEW(material);

  mafVMEOutputVolume::SafeDownCast(volumeInput->GetOutput())->SetMaterial(material);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volumeInput, NULL);

  double opacity[2] = {1.0,0.8};

  for (int v = 0 ; v<2;v++)
  {
    /////////// Pipe Instance and Creation ///////////
    mafPipeIsosurfaceGPU *pipeIso = new mafPipeIsosurfaceGPU;
    pipeIso->Create(sceneNode);
    pipeIso->OnEvent(&mafEvent(this, mafPipeIsosurfaceGPU::ID_ENABLE_GPU));
    pipeIso->SetContourValue(1.0);
    pipeIso->SetAlphaValue(opacity[v]); //opacity

    ////////// ACTORS List ///////////////
    vtkProp3DCollection *actorList = pipeIso->GetAssemblyFront()->GetParts();

    actorList->InitTraversal();
    vtkProp *actor = actorList->GetNextProp();
    while(actor)
    {   
      m_Renderer->AddActor(actor);
      m_RenderWindow->Render();

      actor = actorList->GetNextProp();
    }

    m_RenderWindow->Render();

    double b[6];
    volumeInput->GetOutput()->GetVTKData()->GetBounds(b);
    m_Renderer->ResetCamera(b);
    m_RenderWindow->Render();

    printf("\n Visualization and Isosurface Extraction. \n");
    pipeIso->ExctractIsosurface();

    vtkPolyData *isoSurface = vtkPolyData::SafeDownCast(mafVME::SafeDownCast(volumeInput->GetChild(0))->GetOutput()->GetVTKData());
    isoSurface->Update();

    CPPUNIT_ASSERT(isoSurface && isoSurface->GetNumberOfPoints() != 0);

    mafSleep(1000);

    m_Renderer->RemoveAllProps();
    //vtkDEL(actorList);
    delete pipeIso;
  }

  delete sceneNode;

  mafDEL(material);
  mafDEL(volumeInput);
  vtkDEL(Importer);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipeIsosurfaceGPUTest::CompareImages(int imageIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);



  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeIsosurfaceGPU/";
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
    imageFile<<"/Test_PipeIsosurfaceGPU/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeIsosurfaceGPU/";
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
  imageFileOrig<<"/Test_PipeIsosurfaceGPU/";
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