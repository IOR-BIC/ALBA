/*=========================================================================

 Program: MAF2Medical
 Module: vtkMAFVolumeTextureMapper2DTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFVolumeTextureMapper2DTest.h"
#include "vtkMAFVolumeTextureMapper2D.h"

#include "vtkMAFVolumeTextureMapper2D.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"


#include "vtkMapper.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "mafString.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"


#include <iostream>


//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::tearDown()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ///////////////// render stuff /////////////////////////

  m_Renderer->SetBackground(0.1, 0.1, 0.1);
  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(320, 240);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);

  
  ///////////// end render stuff /////////////////////////


  ////// import vtkData ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MED_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  
  vtkVolumeProperty *volumeProperty = NULL;
  vtkNEW(volumeProperty);  
  //volumeProperty->SetColor(m_ColorTransferFunction);
  volumeProperty->SetInterpolationTypeToLinear();

  vtkMAFVolumeTextureMapper2D *volumeMapper;
  vtkNEW(volumeMapper);
  volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

  volumeMapper->SetMaximumNumberOfPlanes(100);
  volumeMapper->SetTargetTextureSize(512,512);
  volumeMapper->SetMaximumStorageSize(64*1024*1024);  //BES 2.6.2008 - enable texture saving using up to 64 MB

  volumeMapper->Update();	

  vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
  vtkPiecewiseFunction *piecewiseFunction = vtkPiecewiseFunction::New();

  double MaxR;
  double unsignRange[2];
  Importer->GetOutput()->GetScalarRange(unsignRange);
  if (unsignRange[1] < 256.0)
  {    
    piecewiseFunction->AddPoint(  0, 0.0);
    piecewiseFunction->AddPoint(255,1.0);

    MaxR = 255.0;
  }
  else if (unsignRange[1] < 4096.0)
  {
    piecewiseFunction->AddPoint(   0,0.0);
    piecewiseFunction->AddPoint(128,0.0);    
    piecewiseFunction->AddPoint(4095,1.0);

    MaxR = 4095.0;
  }
  else
  {
    piecewiseFunction->AddPoint(    0,0.0);
    piecewiseFunction->AddPoint(22737,0.0);
    piecewiseFunction->AddPoint(44327,1.0);
    piecewiseFunction->AddPoint(65535,1.0);

    MaxR = 65535.0;  
  }

  colorTransferFunction->AddRGBPoint(    0, 0.00, 0.00, 0.00);
  colorTransferFunction->AddRGBPoint((11655 / 65535.0)*MaxR, 0.74, 0.19, 0.14);
  colorTransferFunction->AddRGBPoint((31908 / 65535.0)*MaxR, 0.96, 0.64, 0.42);
  colorTransferFunction->AddRGBPoint((33818 / 65535.0)*MaxR, 0.76, 0.78, 0.25);
  colorTransferFunction->AddRGBPoint((41843 / 65535.0)*MaxR, 1.00, 1.00, 1.00);
  colorTransferFunction->AddRGBPoint((65535 / 65535.0)*MaxR, 1.00, 1.00, 1.00); 

  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(piecewiseFunction);

  vtkVolume *volume;
  vtkNEW(volume);
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  volume->PickableOff();

  m_Renderer->AddVolume(volume);
  //m_RenderWindow->Render();

  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->Azimuth(60);
  camera->Elevation(30);

  m_RenderWindow->Render();

  //m_RenderWindowInteractor->Start();
  CompareImages(0);

  vtkDEL(volumeProperty);
  vtkDEL(volume);
  vtkDEL(colorTransferFunction);
  vtkDEL(piecewiseFunction);
  vtkDEL(volumeMapper);
  vtkDEL(Importer);
  
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MED_DATA_ROOT;
  controlOriginFile<<"/Test_vtkMAFVolumeTextureMapper2D/";
  controlOriginFile<<name.c_str();
  controlOriginFile<<"_";
  controlOriginFile<<"image";
  controlOriginFile<<scalarIndex;
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
  mafString imageFile=MED_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_vtkMAFVolumeTextureMapper2D/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_vtkMAFVolumeTextureMapper2D/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"comp";
  }
  
  imageFile<<scalarIndex;
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
  mafString imageFileOrig=MED_DATA_ROOT;
  imageFileOrig<<"/Test_vtkMAFVolumeTextureMapper2D/";
  imageFileOrig<<name.c_str();
  imageFileOrig<<"_";
  imageFileOrig<<"image";
  imageFileOrig<<scalarIndex;
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
//----------------------------------------------------------------------------
vtkProp *vtkMAFVolumeTextureMapper2DTest::SelectActorToControl(vtkPropCollection *propList, int index)
//----------------------------------------------------------------------------
{
  propList->InitTraversal();
  vtkProp *actor = propList->GetNextProp();
  int count = 0;
  while(actor)
  {   
    if(count == index) return actor;
    actor = propList->GetNextProp();
    count ++;
  }
  return NULL;
}
