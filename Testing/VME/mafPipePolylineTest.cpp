/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipePolylineTest.cpp,v $
Language:  C++
Date:      $Date: 2009-05-20 15:03:26 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2008
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
#include "mafPipePolylineTest.h"

#include "mafSceneNode.h"
#include "mafVMERoot.h"
#include "mafPipePolyline.h"
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
  TEST_COLOR,
  TEST_POLYLINE,
  TEST_SCALAR,
  TEST_POLYLINE_SPLINE_MODE,
  TEST_TUBE,
  TEST_TUBE_SPLINE_MODE,
  TEST_TUBE_CHANGE_VALUES,
  TEST_TUBE_CAPPING,
  TEST_GLYPH,
  TEST_GLYPH_CHANGE_VALUES,
  TEST_GLYPH_UNCONNECTED,
  TEST_GLYPH_SPLINE_MODE,
  NUMBER_OF_TEST,
};


//----------------------------------------------------------------------------
void mafPipePolylineTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::TestPipeExecution()
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
  filename<<"/Test_PipePolyline/polyline.vtk";
  importer->SetFileName(filename);
  importer->Update();
  mafSmartPointer<mafVMEPolyline> polyline;
  polyline->SetData((vtkPolyData*)importer->GetOutput(),0.0);
  polyline->GetOutput()->Update();
  polyline->GetMaterial();
  polyline->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
  polyline->Update();

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,polyline, NULL);

  /////////// Pipe Instance and Creation ///////////
  mafPipePolyline *pipePolyline = new mafPipePolyline;
  pipePolyline->Create(sceneNode);

  ////////// ACTORS List ///////////////
  vtkPropCollection *actorList = vtkPropCollection::New();
  pipePolyline->GetAssemblyFront()->GetActors(actorList);

  actorList->InitTraversal();

  vtkProp *actor = actorList->GetNextProp();
  while(actor)
  {   
    m_Renderer->AddActor(actor);
    m_RenderWindow->Render();

    actor = actorList->GetNextProp();
  }

  for(int i=0;i<NUMBER_OF_TEST;i++)
  {
    if(i == TEST_COLOR)
    {
      double col[3]={1.0,0.0,0.0};
      pipePolyline->SetColor(col);

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_POLYLINE)
    {
      pipePolyline->SetRepresentation(mafPipePolyline::POLYLINE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));
      
      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_POLYLINE_SPLINE_MODE)
    {
      pipePolyline->SetSplineMode(TRUE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_TUBE)
    {
      pipePolyline->SetSplineMode(FALSE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
      pipePolyline->SetRepresentation(mafPipePolyline::TUBE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_TUBE_SPLINE_MODE)
    {
      pipePolyline->SetSplineMode(TRUE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_TUBE_CHANGE_VALUES)
    {
      pipePolyline->SetRadius(2.0);
      pipePolyline->SetTubeResolution(5.0);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_TUBE_RADIUS));
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_TUBE_RESOLUTION));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_TUBE_CAPPING)
    {
      pipePolyline->SetTubeCapping(TRUE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_TUBE_CAPPING));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_GLYPH)
    {
      pipePolyline->SetRepresentation(mafPipePolyline::GLYPH);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_GLYPH_CHANGE_VALUES)
    {
      pipePolyline->SetRadius(2.0);
      pipePolyline->SetGlyphResolution(5.0);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RADIUS));
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RESOLUTION));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_GLYPH_UNCONNECTED)
    {
      pipePolyline->SetSplineMode(FALSE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
      pipePolyline->SetRepresentation(mafPipePolyline::GLYPH_UNCONNECTED);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_POLYLINE_REPRESENTATION));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }
    else if(i == TEST_GLYPH_SPLINE_MODE)
    {
      pipePolyline->SetSplineMode(TRUE);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPLINE));
      pipePolyline->SetRadius(1.0);
      pipePolyline->SetGlyphResolution(10.0);
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RADIUS));
      pipePolyline->OnEvent(&mafEvent(this, mafPipePolyline::ID_SPHERE_RESOLUTION));

      pipePolyline->GetAssemblyFront()->GetActors(actorList);

      actorList->InitTraversal();
      vtkProp *actor = actorList->GetNextProp();
      while(actor)
      {   
        m_Renderer->AddActor(actor);
        m_RenderWindow->Render();

        actor = actorList->GetNextProp();
      }
    }

    //CompareImages(i);
    //mafSleep(1800);
  }

  vtkDEL(actorList);

  delete pipePolyline;
  delete sceneNode;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafPipePolylineTest::CompareImages(int testIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_PipePolyline/";
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
    imageFile<<"/Test_PipePolyline/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_PipePolyline/";
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
  imageFileOrig<<"/Test_PipePolyline/";
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