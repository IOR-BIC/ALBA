/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFTextActorMeterTest.cpp,v $
Language:  C++
Date:      $Date: 2009-06-08 14:55:32 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "vtkMAFTextActorMeter.h"
#include "vtkMAFTextActorMeterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

#include "mafString.h"



void vtkMAFTextActorMeterTest::setUp()
{
}

void vtkMAFTextActorMeterTest::tearDown()
{
}


void vtkMAFTextActorMeterTest::TestFixture()
{
}
//------------------------------------------------------------
void vtkMAFTextActorMeterTest::RenderData(vtkActor2D *actor)
//------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.0, 0.0, 0.0);

  vtkCamera *camera = renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(100,0);

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor2D(actor);
  renderWindow->Render();

  //renderWindowInteractor->Start();
  CompareImages(renderWindow);
  mafSleep(2000);

}
//------------------------------------------------------------------
void vtkMAFTextActorMeterTest::SetText(vtkMAFTextActorMeter *actor, const char *text, double position[3])
//------------------------------------------------------------------
{
  if(actor)
  {
    actor->SetVisibility(true);
    actor->SetText(text);
    actor->SetTextPosition(position);
  }
}
//------------------------------------------------------------
void vtkMAFTextActorMeterTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFTextActorMeter *to = vtkMAFTextActorMeter::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFTextActorMeterTest::TestTextPos1()
//--------------------------------------------
{
  m_TestNumber = ID_TEXT_TEST_POS1;
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();

  mafString text = "String Text Test 1";
  double position[3] = {0.1,0.1,0.};
  text << " pos->";
  text << position[0];
  text << " ";
  text << position[1];
  text << " ";
  text << position[2];

  SetText(actor, text, position);

  //test GetText
  CPPUNIT_ASSERT(mafString(actor->GetText()).Equals(text));

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextActorMeterTest::TestTextPos2()
//--------------------------------------------
{
  m_TestNumber = ID_TEXT_TEST_POS2;
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();

  mafString text = "String Text Test 2";
  double position[3] = {0.05,0.05,0.};
  text << " pos->";
  text << position[0];
  text << " ";
  text << position[1];
  text << " ";
  text << position[2];

  SetText(actor, text, position);

  //test GetText
  CPPUNIT_ASSERT(mafString(actor->GetText()).Equals(text));

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextActorMeterTest::TestTextColor()
//--------------------------------------------
{
  m_TestNumber = ID_TEXT_COLOR_TEST;
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();

  mafString text = "String Text Test Color";
  double position[3] = {0.05,0.05,0.};
  text << " pos->";
  text << position[0];
  text << " ";
  text << position[1];
  text << " ";
  text << position[2];

  SetText(actor, text, position);
  actor->SetColor(1.0, 0.0, 0.0);

  RenderData(actor);
  actor->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTextActorMeterTest::CompareImages(vtkRenderWindow * renwin)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  std::string path(file);
  int slashIndex =  name.find_last_of('\\');

  
  name = name.substr(slashIndex+1);
  path = path.substr(0,slashIndex);

  int pointIndex =  name.find_last_of('.');

  name = name.substr(0, pointIndex);


  mafString controlOriginFile;
  controlOriginFile<<path.c_str();
  controlOriginFile<<"\\";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<m_TestNumber;
  controlOriginFile<<".jpg";

  fstream controlStream;
  controlStream.open(controlOriginFile.GetCStr()); 

  // visualization control
  renwin->OffScreenRenderingOn();
  vtkWindowToImageFilter *w2i;
  vtkNEW(w2i);
  w2i->SetInput(renwin);
  //w2i->SetMagnification(magnification);
  w2i->Update();
  renwin->OffScreenRenderingOff();

  //write comparing image
  vtkJPEGWriter *w;
  vtkNEW(w);
  w->SetInput(w2i->GetOutput());
  mafString imageFile="";

  if(!controlStream)
  {
    imageFile<<path.c_str();
    imageFile<<"\\";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<path.c_str();
    imageFile<<"\\";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

  imageFile<<m_TestNumber;
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
  mafString imageFileOrig="";
  imageFileOrig<<path.c_str();
  imageFileOrig<<"\\";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<m_TestNumber;
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
  vtkDEL(rO);
  vtkDEL(rC);
  vtkDEL(imageMath);

  vtkDEL(w);
  vtkDEL(w2i);
}
//----------------------------------------------------------------------------
void vtkMAFTextActorMeterTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}