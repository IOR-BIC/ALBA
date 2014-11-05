/*=========================================================================

 Program: MAF2Medical
 Module: medPipeVectorFieldGlyphsTest
 Authors: Roberto Muccis
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medPipeVectorFieldGlyphsTest.h"
#include "medPipeVectorFieldGlyphs.h"

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

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include <iostream>
#include <fstream>
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageMathematics.h"


//----------------------------------------------------------------------------
void medPipeVectorFieldGlyphsTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medPipeVectorFieldGlyphsTest::setUp()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medPipeVectorFieldGlyphsTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medPipeVectorFieldGlyphsTest::TestCreate()
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

  mafVMEVolumeGray *testVolume;
  mafNEW(testVolume);
  testVolume->ReparentTo(storage->GetRoot());

  int volumeDimensions[3];
  volumeDimensions[0] = 5;
  volumeDimensions[1] = 5;
  volumeDimensions[2] = 5;

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(volumeDimensions[0],volumeDimensions[1],volumeDimensions[2]);
  imageData->SetSpacing(1.,1.,1.);

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetNumberOfComponents(1);
  scalarArray->SetName("Scalar");
  
  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);
  vectorArray->SetName("Vector");

  for(int i=0;i< volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2];i++)
  {
    scalarArray->InsertNextTuple1(i%3);
    vectorArray->SetTuple3(i, i, i, i);
  }

  imageData->GetPointData()->SetScalars(scalarArray);
  imageData->GetPointData()->SetActiveScalars("Scalar");

  
  testVolume->SetData(imageData, 0.);
  testVolume->Update();

  mafVMEOutput *output = testVolume->GetOutput();
  output->Update();
  vtkPointData *pointData = output->GetVTKData()->GetPointData();
  pointData->Update();

  pointData->AddArray(vectorArray);
  pointData->SetActiveVectors("Vector");

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *rootscenenode = new mafSceneNode(NULL, NULL, storage->GetRoot(), NULL, NULL);
  mafSceneNode *sceneNode = new mafSceneNode(NULL,rootscenenode,testVolume, frontRenderer);

  /////////// Pipe Instance and Creation ///////////
  medPipeVectorFieldGlyphs *pipe = new medPipeVectorFieldGlyphs;
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
  mafSleep(2000);
  CompareImage();

  vtkDEL(actorList);

  pipe->Delete();
  sceneNode->m_RenFront = NULL;
  delete sceneNode;
  delete(rootscenenode);

  vtkDEL(renderWindowInteractor);
  vtkDEL(m_RenderWindow);
  vtkDEL(frontRenderer);


  testVolume->ReparentTo(NULL);
  mafDEL(testVolume);
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void medPipeVectorFieldGlyphsTest::CompareImage()
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MED_DATA_ROOT;
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
  mafString imageFile=MED_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_PipeVectorFieldGlyphs/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeVectorFieldGlyphs/";
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
  mafString imageFileOrig=MED_DATA_ROOT;
  imageFileOrig<<"/Test_PipeVectorFieldGlyphs/";
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