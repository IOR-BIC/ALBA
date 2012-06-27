/*=========================================================================

 Program: MAF2
 Module: vtkMAFClipSurfaceBoundingBoxTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFClipSurfaceBoundingBoxTest.h"
#include "vtkMAFClipSurfaceBoundingBox.h"
#include "vtkMAFSmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkCamera.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"

enum ID_TEST_LIST
{
  ID_EXECUTION_TEST = 0,
};
//--------------------------------------------------
std::string vtkMAFClipSurfaceBoundingBoxTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::setUp()
//--------------------------------------------------
{
  m_SphereInput = vtkSphereSource::New();
  m_PlaneMask = vtkPlaneSource::New();
}

//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::tearDown()
//--------------------------------------------------
{
  m_SphereInput->Delete();
  m_PlaneMask->Delete();
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  vtkMAFClipSurfaceBoundingBox *filter2 = vtkMAFClipSurfaceBoundingBox::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestSetGetMask()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  filter->SetMask(m_PlaneMask->GetOutput());

  CPPUNIT_ASSERT(filter->GetMask() == m_PlaneMask->GetOutput());
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestSetGetClipInside()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  filter->SetClipInside(TRUE);

  CPPUNIT_ASSERT(filter->GetClipInside() == TRUE);  
  filter->SetClipInside(FALSE);
  CPPUNIT_ASSERT(filter->GetClipInside() == FALSE);  
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestExecutionClipInsideOff()
//--------------------------------------------------
{
  TestExecution(FALSE);
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestExecutionClipInsideOn()
//--------------------------------------------------
{
  TestExecution(TRUE);
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestExecution(int clipInside)
//--------------------------------------------------
{
  m_TestNumber = ID_EXECUTION_TEST + clipInside;

  m_SphereInput->SetRadius(5.);
  m_SphereInput->SetCenter(0.,0.,0.);
  m_SphereInput->Update();

  int numberOfPoints = m_SphereInput->GetOutput()->GetNumberOfPoints();

  //set correct bounds
  m_PlaneMask->SetOrigin(0.,0.,0.);
  m_PlaneMask->SetPoint1(5.0,0.0,0.0);
  m_PlaneMask->SetPoint2(0.0,0.0,5.0);
  m_PlaneMask->Update();

  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  filter->SetInput(m_SphereInput->GetOutput());
  filter->SetMask(m_PlaneMask->GetOutput());
  filter->SetClipInside(clipInside);
  filter->Update();

  vtkPolyData *result = filter->GetOutput();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(result);
  mapper->Update();

  vtkPolyDataMapper *mapperMask = vtkPolyDataMapper::New();
  mapperMask->SetInput(m_PlaneMask->GetOutput());
  mapperMask->Update();

  vtkActor *actor;
  actor = vtkActor::New();
  actor->SetMapper(mapper);

  vtkActor *actorMask;
  actorMask = vtkActor::New();
  actorMask->SetMapper(mapperMask);

  vtkActorCollection *coll = vtkActorCollection::New();
  coll->AddItem(actor);
  coll->AddItem(actorMask);
  RenderData(coll);

  coll->Delete();
  actor->Delete();
  mapper->Delete();

  actorMask->Delete();
  mapperMask->Delete();
}
//------------------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::RenderData(vtkActorCollection *actorCollection)
//------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.0, 0.0, 0.0);

  vtkCamera *camera = renderer->GetActiveCamera();
  camera->SetPosition(20.0,3.0,20.0);
  camera->Modified();

  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(100,0);

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  actorCollection->InitTraversal();
  for (vtkProp *actor = actorCollection->GetNextProp(); actor != NULL; actor = actorCollection->GetNextProp()) 
  {
    renderer->AddActor(actor);  
  }

  renderWindow->Render();

  //renderWindowInteractor->Start();
  CompareImages(renderWindow);

#ifdef WIN32
  Sleep(2000);
#else
  usleep(2000*1000);
#endif

  vtkTimerLog::CleanupLog();
}
//----------------------------------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::CompareImages(vtkRenderWindow * renwin)
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
  controlOriginFile+=vtkMAFClipSurfaceBoundingBoxTest::ConvertInt(m_TestNumber).c_str();
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

  imageFile+=vtkMAFClipSurfaceBoundingBoxTest::ConvertInt(m_TestNumber).c_str();
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
  imageFileOrig+=vtkMAFClipSurfaceBoundingBoxTest::ConvertInt(m_TestNumber).c_str();
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