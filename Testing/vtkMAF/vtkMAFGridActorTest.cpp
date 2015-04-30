/*=========================================================================

 Program: MAF2
 Module: vtkMAFGridActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFGridActor.h"
#include "vtkMAFGridActorTest.h"

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


//------------------------------------------------------------
void vtkMAFGridActorTest::setUp()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFGridActorTest::tearDown()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFGridActorTest::RenderData(vtkActor *actor)
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

  renderer->AddActor(actor);
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
void vtkMAFGridActorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFGridActor *to = vtkMAFGridActor::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestSetGridNormal()
//--------------------------------------------
{
  m_TestNumber = ID_GRID_NORMAL_TEST;
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  actor->SetGridNormal(GRID_Y);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestSetGridPosition()
//--------------------------------------------
{
  m_TestNumber = ID_GRID_POSITION_TEST;
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  actor->SetGridNormal(GRID_X);
  actor->SetGridPosition(0.9);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestSetGridColor()
//--------------------------------------------
{
  m_TestNumber = ID_GRID_COLOR_TEST;
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  actor->SetGridColor(1.0,0.0,0.0);

  RenderData(actor);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestGetLabelActor()
//--------------------------------------------
{
  m_TestNumber = ID_LABEL_ACTOR_TEST;
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  RenderData(actor);

  /*used 0.2 as default value. This value is calculated with the internal algorithm
    of the vtkMAFGridActor:
  
    double vdist[3];
    vdist[0] = focalp[0]-cam[0];
    vdist[1] = focalp[1]-cam[1];
    vdist[2] = focalp[2]-cam[2];
    double dist = focalp[GridNormal] + vtkMath::Norm(vdist); 
    size = 2*dist*tan(camera->GetViewAngle());
    
  */

  CPPUNIT_ASSERT(strcmp("0.2",actor->GetLabelActor()->GetInput())==0);

  actor->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFGridActorTest::CompareImages(vtkRenderWindow * renwin)
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
  controlOriginFile+=vtkMAFGridActorTest::ConvertInt(m_TestNumber).c_str();
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

  imageFile+=vtkMAFGridActorTest::ConvertInt(m_TestNumber).c_str();
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
  imageFileOrig+=vtkMAFGridActorTest::ConvertInt(m_TestNumber).c_str();
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
void vtkMAFGridActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}
//--------------------------------------------------
std::string vtkMAFGridActorTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}