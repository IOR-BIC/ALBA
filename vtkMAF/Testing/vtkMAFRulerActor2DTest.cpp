/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFRulerActor2DTest.cpp,v $
Language:  C++
Date:      $Date: 2009-10-19 14:41:56 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi

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

#include "vtkMAFRulerActor2D.h"
#include "vtkMAFRulerActor2DTest.h"

#include <cppunit/config/SourcePrefix.h>

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

#include "mafString.h"

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
  mafSleep(2000);

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
  //CPPUNIT_ASSERT(ComparingImagesDetailed(imDataOrig,imDataComp));

  // end visualization control
  vtkDEL(rO);
  vtkDEL(rC);
  vtkDEL(imageMath);

  vtkDEL(w);
  vtkDEL(w2i);
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