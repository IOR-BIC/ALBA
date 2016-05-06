/*=========================================================================

 Program: MAF2
 Module: mafPipeLandmarkCloudTest
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

#include "mafPipeLandmarkCloudTest.h"

#include "mafPipeLandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkLogLookupTable.h"
#include "vtkCellData.h"
#include "vtkDataSetWriter.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafPipeLandmarkCloud *pipeCloud = new mafPipeLandmarkCloud();

  delete pipeCloud;
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(200,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  ///////////// end render stuff /////////////////////////

  mafSmartPointer<mafVMELandmarkCloud> cloud;
  cloud->TestModeOn();
  cloud->AppendLandmark(10.0,0.0,0.0,"first");
  cloud->AppendLandmark(20.0,0.0,0.0,"second");
  cloud->AppendLandmark(30.0,0.0,0.0,"third");
  cloud->AppendLandmark(40.0,0.0,0.0,"fourth");

  cloud->GetOutput()->GetVTKData()->Update();
  cloud->GetOutput()->Update();

	//Setting standard material to avoid random color selection
	cloud->GetMaterial()->m_Diffuse[0]=0.3;
	cloud->GetMaterial()->m_Diffuse[1]=0.6;
	cloud->GetMaterial()->m_Diffuse[2]=0.9;
	cloud->GetMaterial()->UpdateProp();

  cloud->Update();

  vtkMAFSmartPointer<vtkDoubleArray> scalars;
  scalars->InsertNextTuple1(0.0);//This is a special value, with this value the pipe will not show the landmark
  scalars->InsertNextTuple1(10.0);
  scalars->InsertNextTuple1(20.0);
  scalars->InsertNextTuple1(30.0);
  scalars->SetName("scalars");

  vtkPolyData *ds = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
  ds->Update();
  int p = ds->GetNumberOfPoints();
  int c = ds->GetNumberOfCells();
  ds->GetPointData()->SetScalars(scalars);
  ds->UpdateData();

  cloud->GetMaterial();
  cloud->GetOutput()->GetVTKData()->Update();
  cloud->GetOutput()->Update();
  cloud->Update();

  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,cloud, NULL);

  mafPipeLandmarkCloud *pipe = new mafPipeLandmarkCloud();
  pipe->Create(sceneNode);
  pipe->Select(true);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

  CompareImages(0);

  m_Renderer->RemoveAllProps();
  m_RenderWindow->Render();


  pipe->SetScalarVisibility(TRUE);
  pipe->OnEvent(&mafEvent(this,mafPipeLandmarkCloud::ID_SCALAR_VISIBILITY));
  actorList->RemoveAllItems();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

  CompareImages(1);

  m_Renderer->RemoveAllProps();
  m_RenderWindow->Render();

  pipe->SetScalarVisibility(FALSE);
  pipe->OnEvent(&mafEventBase(this,mafPipeLandmarkCloud::ID_SCALAR_VISIBILITY));

  actorList->RemoveAllItems();
  pipe->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  m_Renderer->ResetCamera();

  CompareImages(2);

  delete sceneNode;
  actorList->Delete();
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloudTest::CompareImages(int imageIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipeLandmarkcloud/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<imageIndex;
  controlOriginFile<<".jpg";

  fstream controlStream;
  controlStream.open(controlOriginFile.GetCStr()); 

  // visualization control
  m_RenderWindow->OffScreenRenderingOn();
  vtkMAFSmartPointer<vtkWindowToImageFilter> w2i;
  w2i->SetInput(m_RenderWindow);
  //w2i->SetMagnification(magnification);
  w2i->Update();
  m_RenderWindow->OffScreenRenderingOff();

  //write comparing image
  vtkMAFSmartPointer<vtkJPEGWriter> w;
  w->SetInput(w2i->GetOutput());
  mafString imageFile=MAF_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_PipeLandmarkcloud/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipeLandmarkcloud/";
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

    controlStream.close();
    return;
  }
  controlStream.close();

  //read original Image
  vtkMAFSmartPointer<vtkJPEGReader> rO;
  mafString imageFileOrig=MAF_DATA_ROOT;
  imageFileOrig<<"/Test_PipeLandmarkcloud/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<imageIndex;
  imageFileOrig<<".jpg";
  rO->SetFileName(imageFileOrig.GetCStr());
  rO->Update();

  vtkImageData *imDataOrig = rO->GetOutput();

  //read compared image
  vtkMAFSmartPointer<vtkJPEGReader> rC;
  rC->SetFileName(imageFile.GetCStr());
  rC->Update();

  vtkImageData *imDataComp = rC->GetOutput();


  vtkMAFSmartPointer<vtkImageMathematics> imageMath;
  imageMath->SetInput1(imDataOrig);
  imageMath->SetInput2(imDataComp);
  imageMath->SetOperationToSubtract();
  imageMath->Update();

  double srR[2] = {-1,1};
  imageMath->GetOutput()->GetPointData()->GetScalars()->GetRange(srR);

  CPPUNIT_ASSERT(srR[0] == 0.0 && srR[1] == 0.0);
}
