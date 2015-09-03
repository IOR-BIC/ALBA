/*=========================================================================

 Program: MAF2
 Module: mafAttachCameraTest
 Authors: Stefano Perticoni
 
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

#include "mafAttachCameraTest.h"
#include "mafDecl.h"
#include "mafGUISettingsStorage.h"
#include "mafAttachCamera.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"
#include "mafNode.h"

#include "vtkMAFSmartPointer.h"
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

void mafAttachCameraTest::TestFixture()
{ 

}


void mafAttachCameraTest::BeforeTest()
{
  CreateRenderStuff();
  CreateTestData();
}

void mafAttachCameraTest::AfterTest()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_TestSurface);
  mafDEL(m_Root); 
}


void mafAttachCameraTest::TestConstructorDestructor()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  cppDEL(attachCamera);
}

void mafAttachCameraTest::TestRenderStuff()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  // RenderVMESurface(m_TestSurface);
  cppDEL(attachCamera);
}

void mafAttachCameraTest::TestEnableDisableAttachCamera()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == FALSE);
  attachCamera->EnableAttachCamera();
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == TRUE);
  cppDEL(attachCamera);
}

void mafAttachCameraTest::TestSetGetEnableAttachCamera()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == FALSE);
  attachCamera->SetEnableAttachCamera(TRUE);
  CPPUNIT_ASSERT(attachCamera->GetEnableAttachCamera() == TRUE);
  cppDEL(attachCamera);
}

void mafAttachCameraTest::TestSetGetVme()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetVme() == NULL);
  attachCamera->SetVme(m_TestSurface);
  CPPUNIT_ASSERT(attachCamera->GetVme() == m_TestSurface);
  cppDEL(attachCamera);
}

void mafAttachCameraTest::TestSetGetListener()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  CPPUNIT_ASSERT(attachCamera->GetListener() == NULL);
  attachCamera->SetListener(m_TestSurface);
  CPPUNIT_ASSERT(attachCamera->GetListener() == m_TestSurface);
  cppDEL(attachCamera);
}

void mafAttachCameraTest::TestSetGetStartingMatrix()
{
  mafAttachCamera *attachCamera=new mafAttachCamera(NULL, NULL, NULL);
  mafMatrix startingMatrix = attachCamera->GetStartingMatrix();
  mafMatrix identity;

  CPPUNIT_ASSERT(startingMatrix.Equals(&identity) == true);
  
  mafMatrix testMatrix;
  testMatrix.SetElement(0,0,3.0);
  testMatrix.SetElement(1,0,2.0);
  testMatrix.SetElement(2,2,5.0);
  
  attachCamera->SetStartingMatrix(&testMatrix);
  startingMatrix = attachCamera->GetStartingMatrix();
  
  CPPUNIT_ASSERT(startingMatrix.Equals(&testMatrix) == true);

  cppDEL(attachCamera);
}

void mafAttachCameraTest::CreateTestData()
{
  m_Root = NULL;
  m_TestSurface = NULL;

  int returnValue = -1;

  CPPUNIT_ASSERT(m_TestSurface == NULL);
  mafNEW(m_TestSurface);

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);

  vtkMAFSmartPointer<vtkTubeFilter> tube;
  tube->SetInput(axes->GetOutput());
  tube->SetRadius(0.1);
  tube->CappingOn();
  tube->SetNumberOfSides(20);

  m_TestSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);

  CPPUNIT_ASSERT(m_TestSurface->GetOutput()->GetVTKData() != NULL);


  mafNEW(m_Root);
  m_TestSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_TestSurface->GetOutput()->GetVTKData() != NULL);
}

void mafAttachCameraTest::RenderVMESurface( mafVMESurface *vme )
{
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->ScalarVisibilityOn();
  mapper->SetInput(vme->GetOutput()->GetVTKData());

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->SetUserMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  m_Renderer->AddActor(actor);
  
  m_RenderWindow->Render();
  m_RenderWindowInteractor->Start();

  mapper->Delete();
  actor->Delete();

} 

void mafAttachCameraTest::CreateRenderStuff()
{
  m_Renderer = vtkRenderer::New();
  m_RenderWindow = vtkRenderWindow::New();
  m_RenderWindowInteractor = vtkRenderWindowInteractor::New() ;

  m_Renderer->SetBackground(0.1, 0.1, 0.1);

  m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetSize(640, 480);
  m_RenderWindow->SetPosition(400,0);

  m_RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
}

