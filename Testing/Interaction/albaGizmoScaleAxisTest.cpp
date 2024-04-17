/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleAxisTest
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

#include "albaGizmoScaleAxisTest.h"
#include "albaInteractionTests.h"


#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaRefSys.h"
#include "albaGizmoScaleAxis.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"

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


void albaGizmoScaleAxisTest::BeforeTest()
{
  CreateRenderStuff();
	CreateTestData();
}

void albaGizmoScaleAxisTest::AfterTest()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoScaleAxisTest::CreateTestData()
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

void albaGizmoScaleAxisTest::TestConstructor()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);
  gizmoScaleAxis->SetAxis(albaInteractorConstraint::X);

  gizmoScaleAxis->SetCylinderLength(5);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoScaleAxis);

  gizmoScaleAxis->SetAxis(albaInteractorConstraint::Y);
  RenderGizmo(gizmoScaleAxis);

  gizmoScaleAxis->SetAxis(albaInteractorConstraint::Z);
  RenderGizmo(gizmoScaleAxis);

  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoScaleAxis);

}

void albaGizmoScaleAxisTest::TestSetGetAxis()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);
   
  int axis = gizmoScaleAxis->GetAxis();

  CPPUNIT_ASSERT( axis == albaGizmoScaleAxis::AXIS::X);

  gizmoScaleAxis->SetAxis(albaGizmoScaleAxis::AXIS::Y);
  axis = gizmoScaleAxis->GetAxis();
  CPPUNIT_ASSERT(axis == albaGizmoScaleAxis::AXIS::Y);

  gizmoScaleAxis->SetAxis(albaGizmoScaleAxis::AXIS::Z);
  axis = gizmoScaleAxis->GetAxis();
  CPPUNIT_ASSERT(axis == albaGizmoScaleAxis::AXIS::Z);

  cppDEL(gizmoScaleAxis);
}


void albaGizmoScaleAxisTest::TestSetGetCubeLength()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);
  
  gizmoScaleAxis->SetCylinderLength(10);
  double coneLength = gizmoScaleAxis->GetCylinderLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoScaleAxis);
}

void albaGizmoScaleAxisTest::TestSetGetCylinderLength()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetCubeLength(10);
  double coneLength = gizmoScaleAxis->GetCubeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoScaleAxis);
}

void albaGizmoScaleAxisTest::TestSetRefSysMatrix()
{
  albaMatrix *refSysMatrix = albaMatrix::New();

  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetRefSysMatrix(refSysMatrix);
  
  // using friend albaGizmoScaleAxis
  for (int i = 0; i < 2; i++)
  {
    albaMatrix *mat = gizmoScaleAxis->m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == refSysMatrix);
  }

  cppDEL(gizmoScaleAxis);
}


void albaGizmoScaleAxisTest::TestSetListener()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetListener(gizmoScaleAxis);
  
  // using friend albaGizmoScaleAxis
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Listener == gizmoScaleAxis);

  cppDEL(gizmoScaleAxis);
}

void albaGizmoScaleAxisTest::TestSetGetInput()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoScaleAxis
  CPPUNIT_ASSERT(gizmoScaleAxis->m_InputVme == m_GizmoInputSurface);

  CPPUNIT_ASSERT(gizmoScaleAxis->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoScaleAxis);
}


void albaGizmoScaleAxisTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface, listener);
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Show == false);

  gizmoScaleAxis->SetListener(listener);
   
  gizmoScaleAxis->Show(true);
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Show == true);

  cppDEL(gizmoScaleAxis);
  cppDEL(listener);
}

void albaGizmoScaleAxisTest::TestSetGetAbsPose()
{
  albaMatrix *absPose = albaMatrix::New();

  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);
  gizmoScaleAxis->SetAbsPose(absPose);
  
  albaMatrix *gipo = gizmoScaleAxis->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(absPose));
 
  cppDEL(gizmoScaleAxis);
}


void albaGizmoScaleAxisTest::TestFixture()
{
	
}

void albaGizmoScaleAxisTest::RenderData( vtkDataSet *data )
{
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->ScalarVisibilityOn();
  mapper->SetInputData(data);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  
  m_RenderWindow->Render();
  
  mapper->Delete();
  actor->Delete();

} 
  

void albaGizmoScaleAxisTest::CreateRenderStuff()
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

void albaGizmoScaleAxisTest::RenderGizmo( albaGizmoScaleAxis * gizmoScaleAxis )
{
  RenderData(gizmoScaleAxis->m_CylGizmo->GetOutput()->GetVTKData());
  RenderData(gizmoScaleAxis->m_CubeGizmo->GetOutput()->GetVTKData());
}

void albaGizmoScaleAxisTest::TestHighlight()
{
  albaGizmoScaleAxis *gizmoScaleAxis = new albaGizmoScaleAxis(m_GizmoInputSurface);
  
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Highlight == false);
  
  gizmoScaleAxis->Highlight(true);

  CPPUNIT_ASSERT(gizmoScaleAxis->m_Highlight == true);
  
  cppDEL(gizmoScaleAxis);
}


