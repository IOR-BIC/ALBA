/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextOrientatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafDefines.h"

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
  CPPUNIT_ASSERT(strcmp(actor->GetTextLeft(),"L")==0);
  CPPUNIT_ASSERT(strcmp(actor->GetTextDown(),"D")==0);
  CPPUNIT_ASSERT(strcmp(actor->GetTextRight(),"R")==0);
  CPPUNIT_ASSERT(strcmp(actor->GetTextUp(),"U")==0);


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


  std::string controlOriginFile;
  controlOriginFile+=(path.c_str());
  controlOriginFile+=("\\");
  controlOriginFile+=(name.c_str());
  controlOriginFile+=("_");
  controlOriginFile+=("image");
  controlOriginFile+=vtkMAFTextOrientatorTest::ConvertInt(m_TestNumber).c_str();
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

  imageFile+=vtkMAFTextOrientatorTest::ConvertInt(m_TestNumber).c_str();
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
  imageFileOrig+=vtkMAFTextOrientatorTest::ConvertInt(m_TestNumber).c_str();
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
//--------------------------------------------------
std::string vtkMAFTextOrientatorTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}
