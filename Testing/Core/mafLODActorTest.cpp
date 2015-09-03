/*=========================================================================

 Program: MAF2
 Module: mafLODActorTest
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
#include "mafLODActor.h"
#include "mafLODActorTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"


#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

#include "mafString.h"

#define RENDER_DATA 0 //set 1 to see actor in vtk render window

void mafLODActorTest::TestFixture()
{
}
//------------------------------------------------------------
void mafLODActorTest::RenderData(mafLODActor *actor)
//------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkSphereSource> sphere;
	vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
	mapper->SetInput(sphere->GetOutput());
	actor->SetMapper(mapper);
	actor->Modified();

  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.5, 0.5, 0.5);

  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(100,0);


  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
	actor->Register(renderer);
  renderWindow->Render();

  renderer->RemoveAllProps();
  renderWindow->RemoveRenderer(renderer);
  renderWindowInteractor->SetRenderWindow(NULL);
}
//--------------------------------------------
void mafLODActorTest::TestDynamicAllocation()
//--------------------------------------------
{
  mafLODActor *to = mafLODActor::New();
  to->Delete();
}
//--------------------------------------------
void mafLODActorTest::TestPixelThreshold()
//--------------------------------------------
{
	m_TestNumber = ID_PIXEL_THRESHOLD;

	mafLODActor *actor;
	actor = mafLODActor::New();
	actor->SetPixelThreshold(500);
	CPPUNIT_ASSERT(actor->GetPixelThreshold() == 500);
	CPPUNIT_ASSERT(actor->GetPixelThresholdMinValue() == 1);
	CPPUNIT_ASSERT(actor->GetPixelThresholdMaxValue() == VTK_INT_MAX);

	if(RENDER_DATA) RenderData(actor);

  vtkDEL(actor);
}
//--------------------------------------------
void mafLODActorTest::TestFlagDimension()
//--------------------------------------------
{
	m_TestNumber = ID_FLAG_DIMENSION;

	mafLODActor *actor;
	actor = mafLODActor::New();

	actor->SetPixelThreshold(500);
	actor->SetFlagDimension(300);
	CPPUNIT_ASSERT(actor->GetFlagDimension() == 300);
  CPPUNIT_ASSERT(actor->GetFlagDimensionMinValue() == 1);
	CPPUNIT_ASSERT(actor->GetFlagDimensionMaxValue() == VTK_INT_MAX);

	if(RENDER_DATA) RenderData(actor);

	vtkDEL(actor);
}
//--------------------------------------------
void mafLODActorTest::TestEnableFading()
//--------------------------------------------
{
	m_TestNumber = ID_ENABLE_FADING;

	mafLODActor *actor;
	actor = mafLODActor::New();
	
	actor->EnableFadingOn();
	CPPUNIT_ASSERT(actor->GetEnableFading() == 1);

	actor->EnableFadingOff();
	CPPUNIT_ASSERT(actor->GetEnableFading() == 0);

	CPPUNIT_ASSERT(actor->GetEnableFadingMinValue() == 0);
	CPPUNIT_ASSERT(actor->GetEnableFadingMaxValue() == 1);

	//RenderData(actor);

	actor->Delete();
}
//--------------------------------------------
void mafLODActorTest::TestEnableHighThreshold()
//--------------------------------------------
{
	m_TestNumber = ID_ENABLE_HIGH_THRESHOLD;

	mafLODActor *actor;
	actor = mafLODActor::New();
	
	actor->EnableHighThresholdOn();
	CPPUNIT_ASSERT(actor->GetEnableHighThreshold() == 1);

	actor->EnableHighThresholdOff();
	CPPUNIT_ASSERT(actor->GetEnableHighThreshold() == 0);

	CPPUNIT_ASSERT(actor->GetEnableHighThresholdMinValue() == 0);
	CPPUNIT_ASSERT(actor->GetEnableHighThresholdMaxValue() == 1);

	//RenderData(actor);

	actor->Delete();
}