/*=========================================================================

 Program: MAF2
 Module: vtkMAFImageFillHolesRemoveIslandsTest
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
#include "vtkMAFImageFillHolesRemoveIslands.h"
#include "vtkMAFImageFillHolesRemoveIslandsTest.h"

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
#include "vtkStructuredPoints.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"

#include "mafString.h"


//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::setUp()
//------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::tearDown()
//------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestFixture()
//------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::RenderData(vtkActor *actor)
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

  vtkTimerLog::CleanupLog();
}

//------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFImageFillHolesRemoveIslands *to = vtkMAFImageFillHolesRemoveIslands::New();
  to->Delete();
}

//--------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestFillHoles()
//--------------------------------------------
{
  m_Algorithm = vtkMAFImageFillHolesRemoveIslands::FILL_HOLES;
  TestAlgorithm();
}
//--------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestRemoveIslands()
//--------------------------------------------
{
  m_Algorithm = vtkMAFImageFillHolesRemoveIslands::REMOVE_ISLANDS;
  TestAlgorithm();
}

//--------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::TestAlgorithm()
//--------------------------------------------
{
  mafString filename = MAF_DATA_ROOT;
  filename<<"/Test_ImageFillHolesRemoveIslands/test.bmp";

  //load an image
  vtkBMPReader *r = vtkBMPReader::New();
  r->SetFileName(filename.GetCStr());
  r->Allow8BitBMPOn();
  r->SetDataScalarTypeToUnsignedChar();
  r->Update();

  //create vtkImageData from zero and set the correct parameters (spacing, dimension) ...
  vtkStructuredPoints *originalImage = vtkStructuredPoints::New();
  originalImage->SetSpacing(r->GetOutput()->GetSpacing());
  originalImage->SetDimensions(r->GetOutput()->GetDimensions());
  //originalImage->AllocateScalars();
  originalImage->SetScalarTypeToUnsignedChar();

  // and scalar
  vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
  scalars->SetNumberOfComponents(1);
  scalars->SetName("SCALARS");
  for(int i=0; i<originalImage->GetNumberOfPoints();i++)
  {
    scalars->InsertNextTuple1(r->GetOutput()->GetPointData()->GetScalars()->GetTuple(i)[0]);
  }
  originalImage->GetPointData()->SetScalars(scalars);
  originalImage->Update();

  r->Delete();

  vtkMAFImageFillHolesRemoveIslands *filter = vtkMAFImageFillHolesRemoveIslands::New();
  filter->SetInput(originalImage);
  filter->SetEdgeSize(1);
  filter->SetAlgorithm(m_Algorithm);
  filter->Update();

  vtkStructuredPoints *outputImage = vtkStructuredPoints::New();
  outputImage->DeepCopy(filter->GetOutput());
  outputImage->Update();

  vtkPlaneSource *imagePlane = vtkPlaneSource::New();
  imagePlane->SetOrigin(0.,0.,0.);
  imagePlane->SetPoint1(1.,0.,0.);
  imagePlane->SetPoint2(0.,1.,0.);

  double w,l, range[2];
  outputImage->GetScalarRange(range);
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
  imageTexture->SetInput(outputImage);


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

  scalars->Delete();
  actor->Delete();
  mapper->Delete();
  imageTexture->Delete();
  imageLUT->Delete();
  imagePlane->Delete();
  outputImage->Delete();
  originalImage->Delete();
  filter->Delete();

}

//----------------------------------------------------------------------------
void vtkMAFImageFillHolesRemoveIslandsTest::CompareImages(vtkRenderWindow * renwin)
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
  controlOriginFile<<m_Algorithm;
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

  imageFile<<m_Algorithm;
  imageFile<<".jpg";
  w->SetFileName(imageFile.GetCStr());
  w->Write();

  if(!controlStream)
  {
    controlStream.close();
    vtkDEL(w2i);
    vtkDEL(w);
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
  imageFileOrig<<m_Algorithm;
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