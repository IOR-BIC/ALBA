/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorFieldSurfaceTest
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
#include "mafPipeVectorFieldSurfaceTest.h"
#include "mafPipeVectorFieldSurface.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEVolumeGray.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"


//----------------------------------------------------------------------------
void mafPipeVectorFieldSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldSurfaceTest::TestCreate()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ///////////////// render stuff /////////////////////////

  vtkRenderer *frontRenderer;
  vtkNEW(frontRenderer);
  frontRenderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow = vtkRenderWindow::New();
  m_RenderWindow->AddRenderer(frontRenderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  //////////////////////////////////////////////////////////////////////////

  mafVMEVolumeGray *volume;
  mafNEW(volume);
  volume->ReparentTo(storage->GetRoot());

  int volumeDimensions[3];
  volumeDimensions[0] = 2;
  volumeDimensions[1] = 2;
  volumeDimensions[2] = 2;

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetNumberOfComponents(1);
  scalarArray->SetName("Scalar");
  //scalarArray->InsertNextTuple1(1.0);

  vtkMAFSmartPointer<vtkImageData> image;
  image->SetDimensions(volumeDimensions[0],volumeDimensions[1],volumeDimensions[2]);
  image->SetSpacing(1.,1.,1.);

  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);
  vectorArray->SetName("Vector");

  for(int i=0;i< volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2];i++)
  {
    scalarArray->InsertNextTuple1(i%3);
    vectorArray->SetTuple3(i, i, i, i);
  }

  image->GetPointData()->SetScalars(scalarArray);
  image->GetPointData()->SetActiveScalars("Scalar");

  
  volume->SetData(image, 0.);
  volume->Update();

  mafVMEOutput *output = volume->GetOutput();
  output->Update();
  vtkPointData *pointData = output->GetVTKData()->GetPointData();
  pointData->Update();

  pointData->AddArray(vectorArray);
  pointData->SetActiveVectors("Vector");

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL,rootscenenode,volume, frontRenderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeVectorFieldSurface *pipe = new mafPipeVectorFieldSurface;
  pipe->Create(sceneNode);
  pipe->m_RenFront = frontRenderer;

  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    frontRenderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_RenderWindow->Render();
  CompareImage();

  vtkDEL(actorList);

  pipe->Delete();
  sceneNode->m_RenFront = NULL;
  delete sceneNode;
  delete(rootscenenode);

  vtkDEL(renderWindowInteractor);
  vtkDEL(m_RenderWindow);
  vtkDEL(frontRenderer);


  volume->ReparentTo(NULL);
  mafDEL(volume);

  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafPipeVectorFieldSurfaceTest::CompareImage()
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeVectorFieldSurface/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image.jpg";

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
    imageFile<<"/Test_PipeVectorFieldSurface/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeVectorFieldSurface/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

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
  imageFileOrig<<"/Test_PipeVectorFieldSurface/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
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