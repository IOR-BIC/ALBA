/*=========================================================================

 Program: MAF2
 Module: vtkMAFRulerActor2DTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFRulerActor2D.h"
#include "vtkMAFRulerActor2DTest.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

//------------------------------------------------------------
void vtkMAFRulerActor2DTest::setUp()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFRulerActor2DTest::tearDown()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFRulerActor2DTest::RenderData(vtkActor2D *actor)
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
//------------------------------------------------------------
void vtkMAFRulerActor2DTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFRulerActor2D *to = vtkMAFRulerActor2D::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetColor()
//--------------------------------------------
{
  m_TestNumber = ID_SET_COLOR_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->SetColor(0.5,0.5,0.5);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetLabelScaleVisibility()
//--------------------------------------------
{
  m_TestNumber = ID_SET_LABEL_SCALE_VISIBILITY_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetLabelScaleVisibility(false);
  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetLabelAxesVisibility()
//--------------------------------------------
{
  m_TestNumber = ID_SET_LABEL_AXES_VISIBILITY_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetLabelAxesVisibility(false);
  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetAxesVisibility()
//--------------------------------------------
{
  m_TestNumber = ID_SET_AXES_VISIBILITY_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetAxesVisibility(false);
  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetTickVisibility()
//--------------------------------------------
{
  m_TestNumber = ID_SET_TICK_VISIBILITY_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetTickVisibility(false);
  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetLegend()
//--------------------------------------------
{
  m_TestNumber = ID_SET_LEGEND_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetLegend("Test Legend");
  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetGetScaleFactor()
//--------------------------------------------
{
  m_TestNumber = ID_SET_GET_SCALE_FACTOR_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetScaleFactor(0.5);
  RenderData(actor);

  CPPUNIT_ASSERT(actor->GetScaleFactor() == 0.5);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestUseGlobalAxes()
//--------------------------------------------
{
  m_TestNumber = ID_USE_GLOBAL_AXES_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->UseGlobalAxes(true);

  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetInverseTicks()
//--------------------------------------------
{
  m_TestNumber = ID_SET_INVERSE_TICKS_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->SetInverseTicks(true);

  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetAttachPosition()
//--------------------------------------------
{
  m_TestNumber = ID_SET_ATTACH_POSITION_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->SetAttachPositionFlag(true);
  double position[3] = {1,2,0};
  actor->SetAttachPosition(position);
  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestChangeRulerMarginsAndLengths()
//--------------------------------------------
{
  m_TestNumber = ID_CHANGE_RULER_MARGINS_AND_LENGHTS_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->ChangeRulerMarginsAndLengths(2,3,5,7,2,3);

  RenderData(actor);

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetText()
//--------------------------------------------
{
  m_TestNumber = ID_SET_TEXT_TEST;
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  const char *label[3]={"Label 1","Label 2","Label 3"};
  actor->SetText(label);

  RenderData(actor);

  actor->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2DTest::CompareImages(vtkRenderWindow * renwin)
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
  controlOriginFile+=vtkMAFRulerActor2DTest::ConvertInt(m_TestNumber).c_str();
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

  imageFile+=vtkMAFRulerActor2DTest::ConvertInt(m_TestNumber).c_str();
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
  imageFileOrig+=vtkMAFRulerActor2DTest::ConvertInt(m_TestNumber).c_str();
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
  //CPPUNIT_ASSERT(ComparingImagesDetailed(imDataOrig,imDataComp));

  // end visualization control
  rO->Delete();
  rC->Delete();
  imageMath->Delete();

  w->Delete();
  w2i->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFRulerActor2DTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}
//--------------------------------------------------
std::string vtkMAFRulerActor2DTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}
