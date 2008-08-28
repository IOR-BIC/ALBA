/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDImageUnsharpFilterTest.cpp,v $
Language:  C++
Date:      $Date: 2008-08-28 10:37:40 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "vtkMEDImageUnsharpFilter.h"
#include "vtkMEDImageUnsharpFilterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "vtkBMPReader.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

#include "mafString.h"



void vtkMEDImageUnsharpFilterTest::setUp()
{
}

void vtkMEDImageUnsharpFilterTest::tearDown()
{
}


void vtkMEDImageUnsharpFilterTest::TestFixture()
{
}
//------------------------------------------------------------
void vtkMEDImageUnsharpFilterTest::RenderData(vtkActor *actor)
//------------------------------------------------------------
{
  vtkRenderer *renderer = vtkRenderer::New();

  vtkRenderWindow *renderWindow = vtkRenderWindow::New();
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(100,0);

  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderWindow->Render();

  //renderWindowInteractor->Start();
  CompareImages(renderWindow);
  mafSleep(2000);

  renderWindowInteractor->Delete();
  renderWindow->Delete();
  renderer->Delete();
  
  

}
//------------------------------------------------------------
void vtkMEDImageUnsharpFilterTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMEDImageUnsharpFilter *to = vtkMEDImageUnsharpFilter::New();
  to->Delete();
}
//--------------------------------------------
void vtkMEDImageUnsharpFilterTest::TestUnsharp()
//--------------------------------------------
{
  m_TestNumber = ID_UNSHARP_TEST;
  
  mafString filename = MED_DATA_ROOT;
  filename<<"/ImageFilters/unsharpTest.bmp";

  //load an image
  vtkBMPReader *r = vtkBMPReader::New();
  r->SetFileName(filename.GetCStr());
  r->Allow8BitBMPOn();
  r->SetDataScalarTypeToDouble();
  r->Update();
  
  //create vtkImageData from zero and set the correct parameters (spacing, dimension) ...
  vtkImageData *originalImage = vtkImageData::New();
  originalImage->SetSpacing(r->GetOutput()->GetSpacing());
  originalImage->SetDimensions(r->GetOutput()->GetDimensions());
  originalImage->AllocateScalars();
  
  // and scalars
  for(int i=0; i<originalImage->GetNumberOfPoints();i++)
  {
    originalImage->GetPointData()->GetScalars()->SetTuple1(i, r->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i));
  }
  originalImage->Update();

  r->Delete();

  //create unsharp image as copy of original
  vtkImageData *unsharpImage = vtkImageData::New();
  unsharpImage->DeepCopy(originalImage);
  unsharpImage->Update();

  vtkMEDImageUnsharpFilter *filter = vtkMEDImageUnsharpFilter::New();
  filter->SetInput(originalImage);
  filter->SetOutput(unsharpImage);
  filter->Update();

  //for(int i=0; i< originalImage->GetNumberOfPoints(); i++)
  //{
  //  double  original = originalImage->GetPointData()->GetScalars()->GetTuple1(i);
  //  double  unsharp = unsharpImage->GetPointData()->GetScalars()->GetTuple1(i);
  //
  //  if(original != unsharp)
  //    printf("%d-> %f - uns: %f - diff : %f\n", i, original , unsharp , unsharp -original);
  //  
  //}

  vtkPlaneSource *imagePlane = vtkPlaneSource::New();
  imagePlane->SetOrigin(0.,0.,0.);
  imagePlane->SetPoint1(1.,0.,0.);
  imagePlane->SetPoint2(0.,1.,0.);

  double w,l, range[2];
  unsharpImage->GetScalarRange(range);
  w = range[1] - range[0];
  l = (range[1] + range[0]) * .5;

  vtkWindowLevelLookupTable *imageLUT = vtkWindowLevelLookupTable::New();
  imageLUT->SetWindow(w);
  imageLUT->SetLevel(l);
  //m_ImageLUT->InverseVideoOn();
  imageLUT->Build();

  vtkTexture *imageTexture = vtkTexture::New();
  imageTexture->RepeatOff();
  imageTexture->InterpolateOn();
  imageTexture->SetQualityTo32Bit();
  imageTexture->SetInput(unsharpImage);

  
  imageTexture->SetLookupTable(imageLUT);
  imageTexture->MapColorScalarsThroughLookupTableOn();
  
  imageTexture->Modified();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(imagePlane->GetOutput());
  mapper->ImmediateModeRenderingOff();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->SetTexture(imageTexture);

  RenderData(actor);
  
  actor->Delete();
  mapper->Delete();
  imageTexture->Delete();
  imageLUT->Delete();
  imagePlane->Delete();
  unsharpImage->Delete();
  filter->Delete();
  originalImage->Delete();
  
}

//----------------------------------------------------------------------------
void vtkMEDImageUnsharpFilterTest::CompareImages(vtkRenderWindow * renwin)
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