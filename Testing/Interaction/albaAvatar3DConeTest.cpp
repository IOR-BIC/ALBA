/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar3DConeTest
 Authors: Alberto Losi
 
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
#include "albaAvatar3DConeTest.h"
#include "albaAvatar3DCone.h"
#include "albaSmartPointer.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"

#include "vtkTimerLog.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkIdentityTransform.h"
#include "vtkDiskSource.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataReader.h"
#include "albaVMESurface.h"
#include "albaSceneNode.h"
#include "albaPipeBox.h"
#include "vtkALBAAssembly.h"

#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


//----------------------------------------------------------------------------
void albaAvatar3DConeTest::BeforeTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaAvatar3DConeTest::AfterTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaAvatar3DConeTest::TestFixture()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
void albaAvatar3DConeTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaAvatar3DCone *avatar = NULL;
  avatar = albaAvatar3DCone::New();
  CPPUNIT_ASSERT(avatar);
  avatar->Delete();
  
  // a sample smart pointer constructor to check for leaks 
  albaSmartPointer<albaAvatar3DCone> avatarSmartPointer;
}
//----------------------------------------------------------------------------
void albaAvatar3DConeTest::TestPick()
//----------------------------------------------------------------------------
{
  vtkTimerLog::SetLogging(0); // Must shutdown logging otherwise it will generate leaks
  albaMatrix identity;
  identity.Identity();

	InitializeRenderWindow();

  vtkALBASmartPointer<vtkIdentityTransform> ctransform;
  m_Renderer->GetActiveCamera()->GetViewTransformObject()->SetInput(ctransform);

  albaSmartPointer<albaAvatar3DCone> avatar;
  albaSmartPointer<albaDeviceButtonsPadTracker> device;
  
  //device->SetAvatar(avatar); NOT USED BECAUSE IT CREATES A GUI
  avatar->SetRendererAndView(m_Renderer,NULL);
  avatar->SetDevice(device);

	m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  CPPUNIT_ASSERT(0 == avatar->Pick(identity)); // no actors picked!!!

  // Add an actor to the scene

  vtkALBASmartPointer<vtkDiskSource> disk;

  disk->SetOuterRadius( .2 );
  disk->SetInnerRadius(0);
  disk->SetRadialResolution(80);
  disk->Update();
  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(disk->GetOutputPort());
  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->SetPosition(0,0,0);
  m_Renderer->AddActor(actor);

  m_Renderer->Render();

  CPPUNIT_ASSERT(1 == avatar->Pick(identity));

	COMPARE_IMAGES("TestPick");
}