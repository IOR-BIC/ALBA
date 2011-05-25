/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFTextActorMeterTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFTextActorMeter.h"
#include "vtkMAFTextActorMeterTest.h"

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
#ifdef WIN32
  Sleep(2000);
#else
  usleep(2000*1000);
#endif

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

  std::string text = "String Text Test 1";
  double position[3] = {0.1,0.1,0.};
  text += " pos->";
  text += ConvertDouble(position[0]);
  text += " ";
  text += ConvertDouble(position[1]);
  text += " ";
  text += ConvertDouble(position[2]);

  SetText(actor, text.c_str(), position);

  //test GetText
  CPPUNIT_ASSERT(strcmp(actor->GetText(),text.c_str())==0);

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

  std::string text = "String Text Test 2";
  double position[3] = {0.05,0.05,0.};
  text += " pos->";
  text += ConvertDouble(position[0]);
  text += " ";
  text += ConvertDouble(position[1]);
  text += " ";
  text += ConvertDouble(position[2]);

  SetText(actor, text.c_str(), position);

  //test GetText
  CPPUNIT_ASSERT(strcmp(actor->GetText(),text.c_str())==0);

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

  std::string text = "String Text Test Color";
  double position[3] = {0.05,0.05,0.};
  text += " pos->";
  text += ConvertDouble(position[0]);
  text += " ";
  text += ConvertDouble(position[1]);
  text += " ";
  text += ConvertDouble(position[2]);

  SetText(actor, text.c_str(), position);
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


  std::string controlOriginFile;
  controlOriginFile+=(path.c_str());
  controlOriginFile+=("\\");
  controlOriginFile+=(name.c_str());
  controlOriginFile+=("_");
  controlOriginFile+=("image");
  controlOriginFile+=vtkMAFTextActorMeterTest::ConvertInt(m_TestNumber).c_str();
  controlOriginFile+=(".jpg");

  fstream controlStream;
  controlStream.open(controlOriginFile.c_str()); 

  // visualization control
  renwin->OffScreenRenderingOn();
  vtkWindowToImageFilter *w2i = vtkWindowToImageFilter::New();
  w2i->SetInput(renwin);
  //w2i->SetMagnification(magnification);
  w2i->Update();
  renwin->OffScreenRenderingOff();

  //write comparing image
  vtkJPEGWriter *w = vtkJPEGWriter::New();
  w->SetInput(w2i->GetOutput());
  std::string imageFile="";

  if(!controlStream)
  {
    imageFile+=(path.c_str());
    imageFile+=("\\");
    imageFile+=(name.c_str());
    imageFile+=("_");
    imageFile+=("image");
  }
  else
  {
    imageFile+=(path.c_str());
    imageFile+=("\\");
    imageFile+=(name.c_str());
    imageFile+=("_");
    imageFile+=("comp");
  }

  imageFile+=vtkMAFTextActorMeterTest::ConvertInt(m_TestNumber).c_str();
  imageFile+=(".jpg");
  w->SetFileName(imageFile.c_str());
  w->Write();

  if(!controlStream)
  {
    controlStream.close();
    w->Delete();
    w2i->Delete();
    return;
  }
  controlStream.close();

  //read original Image
  vtkJPEGReader *rO = vtkJPEGReader::New();
  std::string imageFileOrig="";
  imageFileOrig+=(path.c_str());
  imageFileOrig+=("\\");
  imageFileOrig+=(name.c_str());
  imageFileOrig+=("_");
  imageFileOrig+=("image");
  imageFileOrig+=vtkMAFTextActorMeterTest::ConvertInt(m_TestNumber).c_str();
  imageFileOrig+=(".jpg");
  rO->SetFileName(imageFileOrig.c_str());
  rO->Update();

  vtkImageData *imDataOrig = rO->GetOutput();

  //read compared image
  vtkJPEGReader *rC = vtkJPEGReader::New();
  rC->SetFileName(imageFile.c_str());
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
  rO->Delete();
  rC->Delete();
  imageMath->Delete();

  w->Delete();
  w2i->Delete();
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
//--------------------------------------------------
std::string vtkMAFTextActorMeterTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}
//--------------------------------------------------
std::string vtkMAFTextActorMeterTest::ConvertDouble(double number)
//--------------------------------------------------
{
  std::ostringstream strs;
  strs << number;
  return strs.str();
}
