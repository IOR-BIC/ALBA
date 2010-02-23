/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFHistogramTest.cpp,v $
Language:  C++
Date:      $Date: 2010-02-23 14:20:49 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi

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
#include "vtkMAFHistogram.h"
#include "vtkMAFHistogramTest.h"

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
#include "vtkDataSetReader.h"
#include "vtkPointData.h"

#include "mafString.h"


//------------------------------------------------------------
void vtkMAFHistogramTest::setUp()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFHistogramTest::tearDown()
//------------------------------------------------------------
{
}
//------------------------------------------------------------
void vtkMAFHistogramTest::RenderData(vtkActor2D *actor)
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
void vtkMAFHistogramTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFHistogram *to = vtkMAFHistogram::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramPointRepresentation()
//--------------------------------------------
{
  m_TestNumber = ID_HISTOGRAM_POINT_REPRESENTATION_TEST;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::POINT_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("POINT REPRESENTATION");

  RenderData(actor);
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramLineRepresentation()
//--------------------------------------------
{
  m_TestNumber = ID_HISTOGRAM_LINE_REPRESENTATION_TEST;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::LINE_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("LINE REPRESENTATION");

  RenderData(actor);
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramBarRepresentation()
//--------------------------------------------
{
  m_TestNumber = ID_HISTOGRAM_BAR_REPRESENTATION_TEST;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("BAR REPRESENTATION");

  RenderData(actor);
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramLogaritmicProperties()
//--------------------------------------------
{
  m_TestNumber = ID_HISTOGRAM_LOGARITMIC_PROPERTIES_TEST;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST LOGARITMIC PROPERTIES");

  actor->SetLogHistogram(TRUE);
  actor->SetLogScaleConstant(TRUE);

  RenderData(actor);
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramScaleFactorColorProperties()
//--------------------------------------------
{
  m_TestNumber = ID_HISTOGRAM_SCALE_COLOR_PROPERTIES;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST SCALE PROPERTIES");

  actor->AutoscaleHistogramOff();
  actor->SetScaleFactor(0.1);
  actor->SetColor(1.0,0.0,0.0);
  actor->Modified();

  RenderData(actor);
  actor->Delete();
  reader->Delete();
}
//--------------------------------------------
void vtkMAFHistogramTest::TestHistogramGetAttributesAndUpdateLines()
//--------------------------------------------
{
  m_TestNumber = ID_HISTOGRAM_GET_ATTRIBUTES_AND_UPDATE_LINES;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/VTK_Volumes/CropTestVolumeSP" << ".vtk" << std::ends ;

  // read the data
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(fname.str());

  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  vtkDataArray *data = reader->GetOutput()->GetPointData()->GetScalars();

  actor->SetInputData(data);
  actor->SetHisctogramRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);

  actor->LabelVisibilityOn();
  actor->SetLabel("TEST ATTRIBUTES AND UPDATE LINES");

  actor->UpdateLines(50,80);
  actor->ShowLinesOn();

  RenderData(actor);

  int hv[2] = {10,20};
  int value = actor->GetHistogramValue(hv[0], hv[1]);
  CPPUNIT_ASSERT(value >= 0 && value <= data->GetSize()) ;
  value = actor->GetScalarValue(hv[0], hv[1]);
  double range[2];
  data->GetRange(range);
  CPPUNIT_ASSERT(value >= range[0] && value <= range[1]) ;

  actor->Delete();
  reader->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFHistogramTest::CompareImages(vtkRenderWindow * renwin)
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
void vtkMAFHistogramTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFHistogram *actor;
  actor = vtkMAFHistogram::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}