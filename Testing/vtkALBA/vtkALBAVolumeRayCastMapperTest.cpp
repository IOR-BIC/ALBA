/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeRayCastMapperTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBAVolumeRayCastMapperTest.h"
#include "vtkALBAVolumeRayCastMapper.h"

#include "vtkALBAVolumeRayCastMapper.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "albaString.h"
#include <iostream>
#include "vtkImageData.h"


//----------------------------------------------------------------------------
void vtkALBAVolumeRayCastMapperTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkALBAVolumeRayCastMapperTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void vtkALBAVolumeRayCastMapperTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkALBAVolumeRayCastMapperTest::TestPipeExecution()
//----------------------------------------------------------------------------
{
  ////// import vtkData ////////////////////
  vtkDataSetReader *Importer;
  vtkNEW(Importer);
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  Importer->SetFileName(filename);
  Importer->Update();
  
  vtkVolumeProperty *volumeProperty = NULL;
  vtkNEW(volumeProperty);  
  //volumeProperty->SetColor(m_ColorTransferFunction);
  volumeProperty->SetInterpolationTypeToLinear();

  vtkVolumeRayCastMIPFunction *MIPFunction = NULL;
  vtkNEW(MIPFunction);
  MIPFunction->SetMaximizeMethodToOpacity();

  vtkALBAVolumeRayCastMapper *volumeMapper;
  vtkNEW(volumeMapper);
  volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

  volumeMapper->SetVolumeRayCastFunction(MIPFunction);
  volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  volumeMapper->SetImageSampleDistance(1);
  volumeMapper->SetMaximumImageSampleDistance(10);
  volumeMapper->SetMinimumImageSampleDistance(1);
  volumeMapper->SetNumberOfThreads(1);
  volumeMapper->SetSampleDistance(1);

  volumeMapper->Update();	
  
  vtkVolume *volume;
  vtkNEW(volume);
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  volume->PickableOff();

  m_Renderer->AddVolume(volume);
	
  m_Renderer->ResetCamera();
	m_RenderWindow->Render();
  COMPARE_IMAGES("TestPipeExecution");

  vtkDEL(volumeProperty);
  vtkDEL(MIPFunction);
  vtkDEL(volume);
  vtkDEL(volumeMapper);
  vtkDEL(MIPFunction);
  vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *vtkALBAVolumeRayCastMapperTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
