/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorFieldSliceTest
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
#include "mafPipeVectorFieldSliceTest.h"
#include "mafPipeVectorFieldSlice.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEStorage.h"

#include "vtkDataSetReader.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPoints.h"
#include "vtkCamera.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"
#include "vtkProp3DCollection.h"


//----------------------------------------------------------------------------
void mafPipeVectorFieldSliceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldSliceTest::setUp()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldSliceTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeVectorFieldSliceTest::TestCreate()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  ///////////////// render stuff /////////////////////////
  vtkRenderer *frontRenderer;
  vtkNEW(frontRenderer);
  frontRenderer->SetBackground(0.1, 0.1, 0.1);

  vtkCamera *cam = frontRenderer->GetActiveCamera();

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

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");

  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("velocity");

  vtkMAFSmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("tensor");


  int x,y,z;
  x = y = z = 10;
  vtkMAFSmartPointer<vtkStructuredPoints> image;
  double origin[3] = {0.0,0.0, -45.};
  image->SetOrigin(origin);
  image->SetDimensions(x,y,z);
  image->SetSpacing(10.,10.,10.);

  int i = 0, size = x*y*z;
  for(;i<size;i++){
    scalarArray->InsertNextTuple1(1.0);
    vectorArray->InsertNextTuple3((float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX);
    tensorArray->InsertNextTuple9((float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX,(float)rand()/(float)RAND_MAX);
  }

  image->GetPointData()->AddArray(vectorArray);
  image->GetPointData()->AddArray(tensorArray);

  image->GetPointData()->SetScalars(scalarArray);
  image->GetPointData()->GetVectors("velocity")->Modified();
  image->GetPointData()->Update();

  image->GetPointData()->SetActiveVectors("velocity");

  image->Update();

  double bb[6];
  image->GetBounds(bb);

  volume->SetDataByReference(image, 0.);
  volume->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL,rootscenenode,volume, frontRenderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipeVectorFieldSlice *pipe = new mafPipeVectorFieldSlice;
  pipe->Create(sceneNode);
  

  double focalPoint[3] = {45.,45.,0.};
  double position[3] = {45.,45.,100.};

  cam->SetFocalPoint(focalPoint);
  cam->SetPosition(position);

  ////////// ACTORS List ///////////////
  vtkProp3DCollection *actorList = pipe->GetAssemblyFront()->GetParts();

  actorList->InitTraversal();
  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    frontRenderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  //m_RenderWindow->Render();
  //mafSleep(2000);
  CompareImage();

  delete pipe;
  sceneNode->m_RenFront = NULL;
  delete sceneNode;
  delete(rootscenenode);

  volume->ReparentTo(NULL);
  mafDEL(volume);
  mafDEL(storage);

  vtkDEL(frontRenderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(renderWindowInteractor);

}

//----------------------------------------------------------------------------
void mafPipeVectorFieldSliceTest::CompareImage()
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeVectorFieldGlyphs/";
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
    imageFile<<"/Test_PipeVectorFieldSlice/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeVectorFieldSlice/";
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
  imageFileOrig<<"/Test_PipeVectorFieldSlice/";
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