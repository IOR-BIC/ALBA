/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFTextOrientatorTest.cpp,v $
Language:  C++
Date:      $Date: 2007-11-26 12:53:07 $
Version:   $Revision: 1.1 $
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
#include "vtkMAFTextOrientator.h"
#include "vtkMAFTextOrientatorTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor2D.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

#include "mafString.h"



void vtkMAFTextOrientatorTest::setUp()
{
}

void vtkMAFTextOrientatorTest::tearDown()
{
}


void vtkMAFTextOrientatorTest::TestFixture()
{
}
//------------------------------------------------------------
void vtkMAFTextOrientatorTest::RenderData(vtkActor2D *actor)
//------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(1.0, 1.0, 1.0);

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
void vtkMAFTextOrientatorTest::SetText(vtkMAFTextOrientator *actor)
//------------------------------------------------------------------
{
  if(actor)
  {
    actor->SetVisibility(true);
    actor->SetTextLeft("L");
    actor->SetTextDown("D");
    actor->SetTextRight("R");
    actor->SetTextUp("U");
  }
}
//------------------------------------------------------------
void vtkMAFTextOrientatorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFTextOrientator *to = vtkMAFTextOrientator::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestText()
//--------------------------------------------
{
  m_TestNumber = ID_TEXT_TEST;
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);

  //test GetText
  CPPUNIT_ASSERT(mafString(actor->GetTextLeft()).Equals("L"));
  CPPUNIT_ASSERT(mafString(actor->GetTextDown()).Equals("D"));
  CPPUNIT_ASSERT(mafString(actor->GetTextRight()).Equals("R"));
  CPPUNIT_ASSERT(mafString(actor->GetTextUp()).Equals("U"));


  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestSingleActorVisibility()
//--------------------------------------------
{
  m_TestNumber = ID_SINGLE_VISIBILITY_TEST;
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetSingleActorVisibility(vtkMAFTextOrientator::ID_ACTOR_LEFT, false); //left label is not visible

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestTextColor()
//--------------------------------------------
{
  m_TestNumber = ID_TEXT_COLOR_TEST;
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetTextColor(1.0, 0.0, 0.0);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestBackgroundColor()
//--------------------------------------------
{
  m_TestNumber = ID_BACKGROUND_COLOR_TEST;
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetBackgroundColor(1.0, 0.0, 0.0);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestBackgroundVisibility()
//--------------------------------------------
{
  m_TestNumber = ID_BACKGROUND_VISIBILITY_TEST;
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetTextColor(0.0,0.0,0.0);
  actor->SetBackgroundVisibility(false);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestScale()
//--------------------------------------------
{
  m_TestNumber = ID_SCALE_TEST;
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetScale(2);

  RenderData(actor);
  actor->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientatorTest::CompareImages(vtkRenderWindow * renwin)
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