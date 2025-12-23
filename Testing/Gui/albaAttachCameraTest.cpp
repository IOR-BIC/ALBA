/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttachCameraTest
 Authors: Stefano Perticoni
 
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

#include "albaAttachCameraTest.h"
#include "albaDecl.h"
#include "albaAttachCamera.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"
#include "albaVME.h"

#include "vtkALBASmartPointer.h"
#include "vtkMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkCamera.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <iostream>

void albaAttachCameraTest::TestFixture()
{ 

}


void albaAttachCameraTest::BeforeTest()
{
  InitializeRenderWindow();
  CreateTestData();
}

void albaAttachCameraTest::AfterTest()
{
  albaDEL(m_TestSurface);
  albaDEL(m_Root); 
}


void albaAttachCameraTest::TestConstructorDestructor()
{
  albaAttachCamera *attachCamera=new albaAttachCamera(NULL, NULL, NULL);
  cppDEL(attachCamera);
}

void albaAttachCameraTest::TestRenderStuff()
{
  albaAttachCamera *attachCamera=new albaAttachCamera(NULL, NULL, NULL);
  RenderVMESurface(m_TestSurface);
  cppDEL(attachCamera);
}

void albaAttachCameraTest::TestEnableDisableAttachCamera()
{
  albaAttachCamera *attachCamera=new albaAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == false);
  attachCamera->EnableAttachCamera();
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == true);
  cppDEL(attachCamera);
}

void albaAttachCameraTest::TestSetGetEnableAttachCamera()
{
  albaAttachCamera *attachCamera=new albaAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == false);
  attachCamera->SetEnableAttachCamera(true);
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == true);
  cppDEL(attachCamera);
}

void albaAttachCameraTest::TestSetGetVme()
{
  albaAttachCamera *attachCamera=new albaAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetVme() == NULL);
  attachCamera->SetVme(m_TestSurface);
  CPPUNIT_ASSERT(attachCamera->GetVme() == m_TestSurface);
  cppDEL(attachCamera);
}

void albaAttachCameraTest::TestSetGetListener()
{
  albaAttachCamera *attachCamera=new albaAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetListener() == NULL);
  attachCamera->SetListener(m_TestSurface);
  CPPUNIT_ASSERT(attachCamera->GetListener() == m_TestSurface);
  cppDEL(attachCamera);
}



void albaAttachCameraTest::CreateTestData()
{
  m_Root = NULL;
  m_TestSurface = NULL;

  int returnValue = -1;

  CPPUNIT_ASSERT(m_TestSurface == NULL);
  albaNEW(m_TestSurface);

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);

  vtkALBASmartPointer<vtkTubeFilter> tube;
  tube->SetInputConnection(axes->GetOutputPort());
  tube->SetRadius(0.1);
  tube->CappingOn();
  tube->SetNumberOfSides(20);
	tube->Update();

  m_TestSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);

  CPPUNIT_ASSERT(m_TestSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_TestSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_TestSurface->GetOutput()->GetVTKData() != NULL);
}

void albaAttachCameraTest::RenderVMESurface( albaVMESurface *vme )
{
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->ScalarVisibilityOn();
  mapper->SetInputData(vme->GetOutput()->GetVTKData());

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->SetUserMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  m_Renderer->AddActor(actor);
  
	m_Renderer->ResetCamera();
  m_RenderWindow->Render();

  mapper->Delete();
  actor->Delete();

}
