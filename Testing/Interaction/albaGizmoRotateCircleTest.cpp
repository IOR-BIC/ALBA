/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateCircleTest
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

#include "albaGizmoRotateCircleTest.h"
#include "albaInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaRefSys.h"
#include "albaGizmoRotateCircle.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"

#include "vtkDiskSource.h"
#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkAxes.h"
#include "vtkTubeFilter.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"

void albaGizmoRotateCircleTest::BeforeTest()
{
  InitializeRenderWindow();
	CreateTestData();
}

void albaGizmoRotateCircleTest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoRotateCircleTest::CreateTestData()
//----------------------------------------------------------------------------
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

  // try to set this data to the volume
  int returnValue = -1;

  CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
  albaNEW(m_GizmoInputSurface);

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);
  
  vtkALBASmartPointer<vtkTubeFilter> tube;
  tube->SetInputConnection(axes->GetOutputPort());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
	tube->Update();
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoRotateCircleTest::TestConstructor()
{
  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);
  gizmoRotateCircle->SetAxis(albaInteractorConstraint::X);
  
  gizmoRotateCircle->SetRadius(3);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoRotateCircle);

  gizmoRotateCircle->SetAxis(albaInteractorConstraint::Y);
  RenderGizmo(gizmoRotateCircle);

  gizmoRotateCircle->SetAxis(albaInteractorConstraint::Z);
  RenderGizmo(gizmoRotateCircle);
  
  cppDEL(gizmoRotateCircle);

}

void albaGizmoRotateCircleTest::TestSetGetAxis()
{
  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);
   
  int axis = gizmoRotateCircle->GetAxis();

  CPPUNIT_ASSERT( axis == albaGizmoRotateCircle::AXIS::X);

  gizmoRotateCircle->SetAxis(albaGizmoRotateCircle::AXIS::Y);
  axis = gizmoRotateCircle->GetAxis();
  CPPUNIT_ASSERT(axis == albaGizmoRotateCircle::AXIS::Y);

  gizmoRotateCircle->SetAxis(albaGizmoRotateCircle::AXIS::Z);
  axis = gizmoRotateCircle->GetAxis();
  CPPUNIT_ASSERT(axis == albaGizmoRotateCircle::AXIS::Z);

  cppDEL(gizmoRotateCircle);
}


void albaGizmoRotateCircleTest::TestSetRadius()
{
  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);
  
  gizmoRotateCircle->SetRadius(10);
  CPPUNIT_ASSERT(gizmoRotateCircle->m_Circle->GetInnerRadius() == 10);
  CPPUNIT_ASSERT(gizmoRotateCircle->m_Circle->GetOuterRadius() == 10);

  cppDEL(gizmoRotateCircle);
}

void albaGizmoRotateCircleTest::TestSetRefSysMatrix()
{
  albaMatrix refSys;

  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);

  gizmoRotateCircle->SetRefSysMatrix(&refSys);
  
  // using friend albaGizmoRotateCircle
  albaMatrix *mat = gizmoRotateCircle->m_IsaGen->GetTranslationConstraint()->GetRefSys()->GetMatrix();
  CPPUNIT_ASSERT(mat->Equals(&refSys));

  cppDEL(gizmoRotateCircle);
}


void albaGizmoRotateCircleTest::TestSetListener()
{
  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);

  gizmoRotateCircle->SetListener(gizmoRotateCircle);
  
  // using friend albaGizmoRotateCircle
  CPPUNIT_ASSERT(gizmoRotateCircle->m_Listener == gizmoRotateCircle);

  cppDEL(gizmoRotateCircle);
}

void albaGizmoRotateCircleTest::TestSetInput()
{
  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);

  gizmoRotateCircle->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoRotateCircle
  CPPUNIT_ASSERT(gizmoRotateCircle->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoRotateCircle);
}


void albaGizmoRotateCircleTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);
  gizmoRotateCircle->SetListener(listener);
 
  gizmoRotateCircle->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoRotateCircle);

  cppDEL(gizmoRotateCircle);
}

void albaGizmoRotateCircleTest::TestSetGetAbsPose()
{
  albaMatrix absPose;

  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);
  gizmoRotateCircle->SetAbsPose(&absPose);
  
  albaMatrix *gipo = gizmoRotateCircle->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoRotateCircle);
}


void albaGizmoRotateCircleTest::TestFixture()
{
	
}

void albaGizmoRotateCircleTest::RenderGizmo( albaGizmoRotateCircle * gizmoRotateCircle )
{
  RenderData(gizmoRotateCircle->m_Circle->GetOutput());
}

void albaGizmoRotateCircleTest::TestSetGetIsActive()
{
  albaGizmoRotateCircle *gizmoRotateCircle = new albaGizmoRotateCircle(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoRotateCircle->GetIsActive() == false);
  gizmoRotateCircle->SetIsActive(true);
  CPPUNIT_ASSERT(gizmoRotateCircle->GetIsActive() == true);
  cppDEL(gizmoRotateCircle);
}
