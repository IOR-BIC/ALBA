/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFClipSurfaceBoundingBoxTest.cpp,v $
Language:  C++
Date:      $Date: 2009-08-31 12:52:16 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
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

#include "mafString.h"

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
void vtkMAFClipSurfaceBoundingBoxTest::setUp()
//--------------------------------------------------
{
  vtkNEW(m_SphereInput);
  vtkNEW(m_PlaneMask);
}

//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::tearDown()
//--------------------------------------------------
{
  vtkDEL(m_SphereInput);
  vtkDEL(m_PlaneMask);
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  vtkMAFClipSurfaceBoundingBox *filter2 = vtkMAFClipSurfaceBoundingBox::New();
  vtkDEL(filter2);
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
  mafSleep(2000);

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