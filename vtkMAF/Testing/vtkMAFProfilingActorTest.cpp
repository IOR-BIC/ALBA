/*==============================================================================

  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkMAFProfilingActorTest.cpp,v $
  Language:  C++
  Date:      $Date: 2009-10-21 10:57:58 $
  Version:   $Revision: 1.1.2.3 $
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

#include "vtkMAFGridActor.h"
#include "vtkMAFProfilingActor.h"
#include "vtkMAFProfilingActorTest.h"

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
#include "vtkObjectFactory.h"

#include "mafString.h"

class vtkMAFProfilingActorDummy : public vtkMAFProfilingActor
{
  public:
    //vtkTypeRevisionMacro(vtkMAFProfilingActorDummy,vtkMAFProfilingActor);
    
    static vtkMAFProfilingActorDummy *New()
    {
      vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMAFProfilingActorDummy");
      if(ret)
      { 
      return static_cast<vtkMAFProfilingActorDummy*>(ret); 
      } 
      return new vtkMAFProfilingActorDummy;
    };

    void FPSUpdate(vtkRenderer *ren)
    {
      sprintf(TextBuff,"fps: %.1f \nrender time: %.3f s",0, 0);
      TextFPS->SetInput(this->TextBuff);
    };

};

//------------------------------------------------------------
void vtkMAFProfilingActorTest::setUp()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFProfilingActorTest::tearDown()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFProfilingActorTest::PrepareToRender(vtkRenderer *renderer, vtkRenderWindow *renderWindow)
//------------------------------------------------------------
{
  renderer->SetBackground(0.0, 0.0, 0.0);

  vtkCamera *camera = renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(100,0);

}
//------------------------------------------------------------
void vtkMAFProfilingActorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFProfilingActor *profActor = vtkMAFProfilingActor::New();
  profActor->Delete();
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderOverlay()
//--------------------------------------------
{
  vtkRenderer *renderer;
  vtkRenderWindow *renderWindow;

  vtkNEW(renderer);
  vtkNEW(renderWindow);

  PrepareToRender(renderer,renderWindow);
  
  vtkMAFProfilingActorDummy *profActor = vtkMAFProfilingActorDummy::New();
  
  renderer->AddActor(profActor);
  renderWindow->Render();

  CPPUNIT_ASSERT(profActor->RenderOverlay((vtkViewport*)renderer) == 1);
  profActor->FPSUpdate(renderer);
  renderWindow->Render();

  CompareImages(renderWindow, 0);
  mafSleep(2000);
  profActor->Delete();

  vtkDEL(renderer);
  vtkDEL(renderWindow);
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderOpaqueGeometry()
//--------------------------------------------
{
  vtkRenderer *renderer;
  vtkRenderWindow *renderWindow;

  vtkNEW(renderer);
  vtkNEW(renderWindow);

  PrepareToRender(renderer,renderWindow);

  vtkMAFProfilingActorDummy *profActor = vtkMAFProfilingActorDummy::New();

  renderer->AddActor(profActor);
  renderWindow->Render();

  CPPUNIT_ASSERT(profActor->RenderOpaqueGeometry((vtkViewport*)renderer) == 0);
  profActor->FPSUpdate(renderer);
  renderWindow->Render();

  CompareImages(renderWindow, 1);
  mafSleep(2000);
  profActor->Delete();

  vtkDEL(renderer);
  vtkDEL(renderWindow);
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderTranslucentGeometry()
//--------------------------------------------
{
  vtkRenderer *renderer;
  vtkRenderWindow *renderWindow;

  vtkNEW(renderer);
  vtkNEW(renderWindow);

  PrepareToRender(renderer,renderWindow);

  vtkMAFProfilingActor *profActor = vtkMAFProfilingActor::New();

  renderer->AddActor(profActor);

  CPPUNIT_ASSERT(profActor->RenderTranslucentGeometry((vtkViewport*)renderer) == 0); //This method only returns 0

  profActor->Delete();
  //renderWindow->Render();

  //CompareImages(renderWindow);
  //mafSleep(2000);

  vtkDEL(renderer);
  vtkDEL(renderWindow);
}
//----------------------------------------------------------------------------
void vtkMAFProfilingActorTest::CompareImages(vtkRenderWindow * renwin, int indexTest)
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
  controlOriginFile<<indexTest;
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

  imageFile<<indexTest;
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
  imageFileOrig<<indexTest;
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
void vtkMAFProfilingActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFProfilingActor *actor;
  actor = vtkMAFProfilingActor::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}