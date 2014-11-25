/*=========================================================================

 Program: MAF2Medical
 Module: vtkXRayVolumeMapperTest
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
#include "vtkXRayVolumeMapperTest.h"
#include "vtkXRayVolumeMapper.h"

#include "vtkXRayVolumeMapper.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"


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


#include <iostream>


//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::setUp()
//----------------------------------------------------------------------------
{
  vtkNEW(m_Renderer);
  vtkNEW(m_RenderWindow);
  vtkNEW(m_RenderWindowInteractor);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::tearDown()
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
  mafString filename=MAF_DATA_ROOT;
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
  CompareImages(ID_TEST_PIPE_EXECUTION);

  vtkDEL(volumeProperty);
  vtkDEL(volume);
  vtkDEL(volumeMapper);
  vtkDEL(Importer);
  
  delete wxLog::SetActiveTarget(NULL);
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
	mafString filename=MAF_DATA_ROOT;
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

	CPPUNIT_ASSERT(volumeMapper->GetReduceColorResolution() == TRUE);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();
	CompareImages(ID_TEST_REDUCE_COLOR_REDUCTION);

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestExposureCorrection()
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
	mafString filename=MAF_DATA_ROOT;
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
	CompareImages(ID_TEST_EXPOSURE_CORRECTION);

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);
  delete val;
	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestGamma()
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
	mafString filename=MAF_DATA_ROOT;
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
	CompareImages(ID_TEST_GAMMA);

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestPerspectiveCorrection()
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
	mafString filename=MAF_DATA_ROOT;
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
	CPPUNIT_ASSERT(volumeMapper->GetPerspectiveCorrection() == TRUE);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();
	CompareImages(ID_TEST_PERSPECTIVE_CORRECTION);

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestColor()
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
	mafString filename=MAF_DATA_ROOT;
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
	CompareImages(ID_TEST_COLOR);

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestEnableAutoLOD()
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
	mafString filename=MAF_DATA_ROOT;
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
	CPPUNIT_ASSERT(volumeMapper->GetEnableAutoLOD() == TRUE);

	volumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
	volumeMapper->Update();	

	vtkVolume *volume;
	vtkNEW(volume);
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->PickableOff();

	m_Renderer->AddVolume(volume);
	m_RenderWindow->Render();
	CompareImages(ID_TEST_ENABLE_AUTOLOAD);

	vtkDEL(volumeProperty);
	vtkDEL(volume);
	vtkDEL(volumeMapper);
	vtkDEL(Importer);

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestDataValid()
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
	mafString filename=MAF_DATA_ROOT;
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

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::TestTextureMemoryAndPercentage()
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
	mafString filename=MAF_DATA_ROOT;
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

	delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void vtkXRayVolumeMapperTest::CompareImages(int scalarIndex)
//----------------------------------------------------------------------------
{
  char *file = __FILE__;
  std::string name(file);
  int slashIndex =  name.find_last_of('\\');

  name = name.substr(slashIndex+1);

  int pointIndex =  name.find_last_of('.');
  name = name.substr(0, pointIndex);

  mafString controlOriginFile=MAF_DATA_ROOT;
  controlOriginFile<<"/Test_vtkXRayVolumeMapper/";
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
  mafString imageFile=MAF_DATA_ROOT;

  if(!controlStream)
  {
    imageFile<<"/Test_vtkXRayVolumeMapper/";
    imageFile<<name.c_str();
    imageFile<<"_";
    imageFile<<"image";
  }
  else
  {
    imageFile<<"/Test_vtkXRayVolumeMapper/";
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
  mafString imageFileOrig=MAF_DATA_ROOT;
  imageFileOrig<<"/Test_vtkXRayVolumeMapper/";
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
