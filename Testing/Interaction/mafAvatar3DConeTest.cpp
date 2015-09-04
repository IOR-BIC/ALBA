/*=========================================================================

 Program: MAF2
 Module: mafAvatar3DConeTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafAvatar3DConeTest.h"
#include "mafAvatar3DCone.h"
#include "mafSmartPointer.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafDeviceButtonsPadTracker.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkIdentityTransform.h"
#include "vtkDiskSource.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"

//----------------------------------------------------------------------------
void mafAvatar3DConeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafAvatar3DConeTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
}
//----------------------------------------------------------------------------
void mafAvatar3DConeTest::TestFixture()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void mafAvatar3DConeTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafAvatar3DCone *avatar = NULL;
  avatar = mafAvatar3DCone::New();
  CPPUNIT_ASSERT(avatar);
  avatar->Delete();
  
  // a sample smart pointer constructor to check for leaks 
  mafSmartPointer<mafAvatar3DCone> avatarSmartPointer;
}
//----------------------------------------------------------------------------
void mafAvatar3DConeTest::TestPick()
//----------------------------------------------------------------------------
{
  vtkTimerLog::SetLogging(0); // Must shutdown logging otherwise it will generate leaks
  mafMatrix identity;
  identity.Identity();

  // Render stuff
  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(400, 400);
  m_RenderWindow->SetPosition(400,0);

  vtkMAFSmartPointer<vtkIdentityTransform> ctransform;
  m_Renderer->GetActiveCamera()->GetViewTransformObject()->SetInput(ctransform);

  mafSmartPointer<mafAvatar3DCone> avatar;
  mafSmartPointer<mafDeviceButtonsPadTracker> device;
  
  //device->SetAvatar(avatar); NOT USED BECAUSE IT CREATES A GUI
  avatar->SetRenderer(m_Renderer);
  avatar->SetDevice(device);

  m_RenderWindow->Render();

  CPPUNIT_ASSERT(0 == avatar->Pick(identity)); // no actors picked!!!

  // Add an actor to the scene

  vtkMAFSmartPointer<vtkDiskSource> disk;

  disk->SetOuterRadius( .2 );
  disk->SetInnerRadius(0);
  disk->SetRadialResolution(80);
  disk->Update();
  vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInput(disk->GetOutput());
  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->SetPosition(0,0,0);
  m_Renderer->AddActor(actor);

  m_Renderer->Render();

  CPPUNIT_ASSERT(1 == avatar->Pick(identity));

  CompareImages();

}
//----------------------------------------------------------------------------
void mafAvatar3DConeTest::CompareImages()
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";

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
    imageFile<<"/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }

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
  imageFileOrig<<"/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
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