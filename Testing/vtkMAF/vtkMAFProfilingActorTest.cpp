/*=========================================================================

 Program: MAF2
 Module: vtkMAFProfilingActorTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFGridActor.h"
#include "vtkMAFProfilingActor.h"
#include "vtkMAFProfilingActorTest.h"

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
      int *size = ren->GetSize();
      TextFPS->SetPosition(10,size[1]-40);
      TextFPS->Modified();
      sprintf(TextBuff,"fps: %.1f \nrender time: %.3f s",0, 0);
      TextFPS->SetInput(this->TextBuff);
    };

};

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
  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renderWindow = vtkRenderWindow::New();

  PrepareToRender(renderer,renderWindow);
  
  vtkMAFProfilingActorDummy *profActor = vtkMAFProfilingActorDummy::New();
  
  renderer->AddActor(profActor);
  renderWindow->Render();

  CPPUNIT_ASSERT(profActor->RenderOverlay((vtkViewport*)renderer) == 1);
  renderWindow->Render();
  profActor->FPSUpdate(renderer);
  CompareImages(renderWindow, 0);

  profActor->Delete();

  renderer->Delete();
  renderWindow->Delete();
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderOpaqueGeometry()
//--------------------------------------------
{
  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renderWindow = vtkRenderWindow::New();

  PrepareToRender(renderer,renderWindow);

  vtkMAFProfilingActorDummy *profActor = vtkMAFProfilingActorDummy::New();

  renderer->AddActor(profActor);
  renderWindow->Render();

  CPPUNIT_ASSERT(profActor->RenderOpaqueGeometry((vtkViewport*)renderer) == 0);
  renderWindow->Render();
  profActor->FPSUpdate(renderer);
  CompareImages(renderWindow, 1);

  profActor->Delete();

  renderer->Delete();
  renderWindow->Delete();
}
//--------------------------------------------
void vtkMAFProfilingActorTest::TestRenderTranslucentGeometry()
//--------------------------------------------
{
  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renderWindow = vtkRenderWindow::New();

  PrepareToRender(renderer,renderWindow);

  vtkMAFProfilingActor *profActor = vtkMAFProfilingActor::New();

  renderer->AddActor(profActor);

  CPPUNIT_ASSERT(profActor->RenderTranslucentGeometry((vtkViewport*)renderer) == 0); //This method only returns 0

  profActor->Delete();
  //renderWindow->Render();

  //CompareImages(renderWindow);

  renderer->Delete();
  renderWindow->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFProfilingActorTest::CompareImages(vtkRenderWindow * renwin, int indexTest)
//----------------------------------------------------------------------------
{
  printf("Comparing images...\n");
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
  controlOriginFile+=vtkMAFProfilingActorTest::ConvertInt(indexTest).c_str();
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

  imageFile+=vtkMAFProfilingActorTest::ConvertInt(indexTest).c_str();
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
  imageFileOrig+=vtkMAFProfilingActorTest::ConvertInt(indexTest).c_str();
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
void vtkMAFProfilingActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFProfilingActor *actor;
  actor = vtkMAFProfilingActor::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}
//--------------------------------------------------
std::string vtkMAFProfilingActorTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}