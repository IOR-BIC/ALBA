/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateFanTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
#include "albaInteractionTests.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGizmoRotateFanTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoRotateFan.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"
#include "mmaMaterial.h"

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
#include "vtkProperty.h"
#include "albaFakeLogicForTest.h"
#include "albaServiceLocator.h"


void albaGizmoRotateFanTest::BeforeTest()
{
  InitializeRenderWindow();
	CreateTestData();
}

void albaGizmoRotateFanTest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoRotateFanTest::CreateTestData()
//----------------------------------------------------------------------------
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

  CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
  albaNEW(m_GizmoInputSurface);

  vtkALBASmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);
  
  vtkALBASmartPointer<vtkTubeFilter> tube;
  tube->SetInput(axes->GetOutput());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);

  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoRotateFanTest::TestConstructor()
{
  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoRotateFan);

  cppDEL(gizmoRotateFan);
}


void albaGizmoRotateFanTest::TestSetListener()
{
  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);

  gizmoRotateFan->SetListener(gizmoRotateFan);
  
  // using friend albaGizmoRotateFan
  CPPUNIT_ASSERT(gizmoRotateFan->m_Listener == gizmoRotateFan);

  gizmoRotateFan->SetListener(NULL);
  CPPUNIT_ASSERT(gizmoRotateFan->m_Listener == NULL);

  cppDEL(gizmoRotateFan);
}

void albaGizmoRotateFanTest::TestSetInput()
{
  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);

  gizmoRotateFan->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoRotateFan
  CPPUNIT_ASSERT(gizmoRotateFan->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoRotateFan);
}

void albaGizmoRotateFanTest::TestSetGetAbsPose()
{
  albaMatrix absPose;
  absPose.SetElement(0,3,10);

  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);
  gizmoRotateFan->SetAbsPose(&absPose);
  
  albaMatrix gipo = gizmoRotateFan->m_GizmoFan->GetAbsMatrixPipe()->GetMatrix();
  CPPUNIT_ASSERT(gipo.Equals(&absPose));
 
  cppDEL(gizmoRotateFan);
}


void albaGizmoRotateFanTest::TestSetGetAxis()
{
  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);
  gizmoRotateFan->SetAxis(albaGizmoRotateFan::X);
  CPPUNIT_ASSERT(gizmoRotateFan->m_ActiveAxis == albaGizmoRotateFan::X);

  gizmoRotateFan->SetAxis(albaGizmoRotateFan::Y);
  CPPUNIT_ASSERT(gizmoRotateFan->m_ActiveAxis == albaGizmoRotateFan::Y); 
  
  cppDEL(gizmoRotateFan);

}

void albaGizmoRotateFanTest::TestSetGetRadius()
{
  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);
  gizmoRotateFan->SetRadius(10);
  CPPUNIT_ASSERT(gizmoRotateFan->GetRadius() == 10);

  cppDEL(gizmoRotateFan);
}

void albaGizmoRotateFanTest::TestFixture()
{
	
}

void albaGizmoRotateFanTest::TestShow()
{
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();

  albaGizmoRotateFan *gizmoRotateFan = new albaGizmoRotateFan(m_GizmoInputSurface);

  gizmoRotateFan->Show(true);

	CPPUNIT_ASSERT(logic->GetCall(1).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  cppDEL(gizmoRotateFan);
}

void albaGizmoRotateFanTest::RenderGizmo( albaGizmoRotateFan *gizmoRotateFan )
{
  RenderData(gizmoRotateFan->m_GizmoFan->GetOutput()->GetVTKData());
}
