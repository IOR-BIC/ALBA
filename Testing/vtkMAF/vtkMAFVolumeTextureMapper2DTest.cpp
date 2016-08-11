/*=========================================================================

 Program: MAF2
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
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "mafString.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"

#include <iostream>


//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::BeforeTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);

	m_Renderer->SetBackground(0.1, 0.1, 0.1);
	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(320, 240);
	m_RenderWindow->SetPosition(400, 0);

	m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}
//----------------------------------------------------------------------------
void vtkMAFVolumeTextureMapper2DTest::AfterTest()
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
  ////// import vtkData ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  mafString filename=MAF_DATA_ROOT;
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

	COMPARE_IMAGES("TestPipeExecution");

  vtkDEL(volumeProperty);
  vtkDEL(volume);
  vtkDEL(colorTransferFunction);
  vtkDEL(piecewiseFunction);
  vtkDEL(volumeMapper);
  vtkDEL(Importer);
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
