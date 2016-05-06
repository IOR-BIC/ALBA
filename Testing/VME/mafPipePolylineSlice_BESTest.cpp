/*=========================================================================

 Program: MAF2
 Module: mafPipePolylineSlice_BESTest
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
#include "mafPipePolylineSlice_BESTest.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafVMEPolyline.h"
#include "mmaMaterial.h"
#include "vtkMAFAssembly.h"

#include "vtkMAFSmartPointer.h"

#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"

#include <iostream>

enum TESTS_PIPE_SURFACE
{
  TEST_RADIUS,
  TEST_THICKNESS,
  TEST_SPLINE,
  NUMBER_OF_TEST,
};


//----------------------------------------------------------------------------
void mafPipePolylineSlice_BESTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice_BESTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice_BESTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice_BESTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
  ///////////// end render stuff /////////////////////////


  ////// Create VME (import vtkData) ////////////////////
  vtkMAFSmartPointer<vtkDataSetReader> importer;
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipePolylineSlice_BES/polydata.vtk";
  importer->SetFileName(filename);
  importer->Update();
  mafSmartPointer<mafVMEPolyline> polyline;
  polyline->SetData((vtkPolyData*)importer->GetOutput(),0.0);
  polyline->GetOutput()->Update();
  polyline->GetMaterial();
  polyline->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  polyline->Update();

	//Setting standard material to avoid random color selection
	polyline->GetMaterial()->m_Diffuse[0]=0.3;
	polyline->GetMaterial()->m_Diffuse[1]=0.6;
	polyline->GetMaterial()->m_Diffuse[2]=0.9;
	polyline->GetMaterial()->UpdateProp();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,polyline, m_Renderer);

  /////////// Pipe Instance and Creation ///////////
  mafPipePolylineSlice_BES *pipePolylineSlice = new mafPipePolylineSlice_BES;
  pipePolylineSlice->Create(sceneNode);
  

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

  double slice[3];
  slice[0] = slice[1] = slice[2] = 0.0;
  
  double normal[3];
  normal[0] = 0;
  normal[1] = 0;
  normal[2] = 1;
  pipePolylineSlice->SetSlice(slice, normal);

  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
    if(i == TEST_RADIUS)
    {
      pipePolylineSlice->SetRadius(10);

      pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
      CompareImages(i);
      m_Renderer->RemoveAllProps();
    }
    else if(i == TEST_THICKNESS)
    {
      pipePolylineSlice->SetThickness(10);
      
      pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
      CompareImages(i);

      m_Renderer->RemoveAllProps();
    }
    else if(i == TEST_SPLINE)
    {
      pipePolylineSlice->SetSplineMode(true);
      pipePolylineSlice->UpdateProperty();

      pipePolylineSlice->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
      CompareImages(i);

      m_Renderer->RemoveAllProps();
    }
  }

  vtkDEL(actorList);

  delete sceneNode;
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice_BESTest::CompareImages(int testIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipePolylineSlice_BES/";
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
    imageFile<<"/Test_PipePolylineSlice_BES/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipePolylineSlice_BES/";
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
  imageFileOrig<<"/Test_PipePolylineSlice_BES/";
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