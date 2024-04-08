/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkXRayVolumeMapperTest
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
#include "vtkXRayVolumeMapperTest.h"
#include "vtkXRayVolumeMapper.h"

#include "vtkXRayVolumeMapper.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDataSetReader.h"
#include "albaString.h"

// render window stuff
#include "vtkRenderWindowInteractor.h"

#include <iostream>
#include "vtkDataSetAttributes.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkImageAlgorithm.h"


//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::BeforeTest()
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
void vtkXRayVolumeMapperTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
  vtkDEL(m_RenderWindow);
  vtkDEL(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestPipeExecution()
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

	vtkXRayVolumeMapper *volumeMapper;
  vtkNEW(volumeMapper);
  volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

  volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  volumeMapper->Update();	
  
  vtkVolume *volume;
  vtkNEW(volume);
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  volume->PickableOff();

  m_Renderer->AddVolume(volume);
  m_RenderWindow->Render();

	COMPARE_IMAGES("TestPipeExecution");

  vtkDEL(volumeProperty);
  vtkDEL(volume);
  vtkDEL(volumeMapper);
  vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::Test_SetInput_GetInput()
//----------------------------------------------------------------------------
{
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	vtkImageData *image = vtkImageData::New();
	image->SetDimensions(1,1,1);
	image->SetSpacing(1,1,1);
	volumeMapper->SetInput(image);
	CPPUNIT_ASSERT(image == volumeMapper->GetInput());
	vtkDEL(image);
	vtkDEL(volumeMapper);	
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestReduceColorReduction()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));
	volumeMapper->ReduceColorResolutionOn();

	CPPUNIT_ASSERT(volumeMapper->GetReduceColorResolution() == true);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();

	COMPARE_IMAGES("TestReduceColorReduction");
	
	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestExposureCorrection()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));
	double *val = new double[2];
	val[0] = 0.1;
	val[1] = 0.4;
	volumeMapper->SetExposureCorrection(val);

	CPPUNIT_ASSERT(volumeMapper->GetExposureCorrection()[0] == val[0] && volumeMapper->GetExposureCorrection()[1] == val[1]);
  
	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();
	
	COMPARE_IMAGES("TestExposureCorrection");

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
  delete val;
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestGamma()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

  volumeMapper->SetGamma(0.15);
	CPPUNIT_ASSERT(volumeMapper->GetGamma() == 0.15);

  volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();

	COMPARE_IMAGES("TestGamma");

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestPerspectiveCorrection()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));
	
	volumeMapper->PerspectiveCorrectionOn();
	CPPUNIT_ASSERT(volumeMapper->GetPerspectiveCorrection() == true);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();

	COMPARE_IMAGES("TestPerspectiveCorrection");

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestColor()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

	volumeMapper->SetColor(1.0,0.,0.);
	CPPUNIT_ASSERT(volumeMapper->GetColor()[0] == 1.0 && volumeMapper->GetColor()[1] == 0.0 && volumeMapper->GetColor()[1] == 0.0);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();

	COMPARE_IMAGES("TestColor");

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestEnableAutoLOD()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

	volumeMapper->EnableAutoLODOn();
	CPPUNIT_ASSERT(volumeMapper->GetEnableAutoLOD() == true);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();

	COMPARE_IMAGES("TestEnableAutoLOD");

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestDataValid()
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
	
	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();
	
	CPPUNIT_ASSERT(volumeMapper->IsDataValid());

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestTextureMemoryAndPercentage()
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


	vtkXRayVolumeMapper *volumeMapper;
	vtkNEW(volumeMapper);
	volumeMapper->SetInput(vtkImageData::SafeDownCast(Importer->GetOutput()));

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();
	
	printf("\nAllocated Texture Memory %d\n", volumeMapper->GetAllocatedTextureMemory());
	printf("\nPercentage Of Resident Textures %d\n", volumeMapper->GetPercentageOfResidentTextures());

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
}

//----------------------------------------------------------------------------
vtkProp *vtkXRayVolumeMapperTest::SelectActorToControl(vtkPropCollection *propList, int index)
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
