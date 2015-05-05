/*=========================================================================

 Program: MAF2
 Module: mafGizmoScaleAxisTest
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

#include "mafGizmoScaleAxisTest.h"
#include "mafInteractionTests.h"


#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafRefSys.h"
#include "mafGizmoScaleAxis.h"
#include "mafVMERoot.h"
#include "mafSmartPointer.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"
#include "mafObserver.h"
#include "mafTransform.h"

#include "mafInteractorGenericMouse.h"

#include "vtkMAFSmartPointer.h"
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


void mafGizmoScaleAxisTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoScaleAxisTest::tearDown()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoScaleAxisTest::CreateTestData()
//----------------------------------------------------------------------------
{
  m_Root = NULL;
  m_GizmoInputSurface = NULL;

  // try to set this data to the volume
  int returnValue = -1;

  CPPUNIT_ASSERT(m_GizmoInputSurface == NULL);
  mafNEW(m_GizmoInputSurface);

  vtkMAFSmartPointer<vtkAxes> axes;
  axes->SetScaleFactor(2.5);
  
  vtkMAFSmartPointer<vtkTubeFilter> tube;
  tube->SetInput(axes->GetOutput());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  mafNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void mafGizmoScaleAxisTest::TestConstructor()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);
  gizmoScaleAxis->SetAxis(mafInteractorConstraint::X);

  gizmoScaleAxis->SetCylinderLength(5);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoScaleAxis);

  gizmoScaleAxis->SetAxis(mafInteractorConstraint::Y);
  RenderGizmo(gizmoScaleAxis);

  gizmoScaleAxis->SetAxis(mafInteractorConstraint::Z);
  RenderGizmo(gizmoScaleAxis);

  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoScaleAxis);

}

void mafGizmoScaleAxisTest::TestSetGetAxis()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);
   
  int axis = gizmoScaleAxis->GetAxis();

  CPPUNIT_ASSERT( axis == mafGizmoScaleAxis::AXIS::X);

  gizmoScaleAxis->SetAxis(mafGizmoScaleAxis::AXIS::Y);
  axis = gizmoScaleAxis->GetAxis();
  CPPUNIT_ASSERT(axis == mafGizmoScaleAxis::AXIS::Y);

  gizmoScaleAxis->SetAxis(mafGizmoScaleAxis::AXIS::Z);
  axis = gizmoScaleAxis->GetAxis();
  CPPUNIT_ASSERT(axis == mafGizmoScaleAxis::AXIS::Z);

  cppDEL(gizmoScaleAxis);
}


void mafGizmoScaleAxisTest::TestSetGetCubeLength()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);
  
  gizmoScaleAxis->SetCylinderLength(10);
  double coneLength = gizmoScaleAxis->GetCylinderLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoScaleAxis);
}

void mafGizmoScaleAxisTest::TestSetGetCylinderLength()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetCubeLength(10);
  double coneLength = gizmoScaleAxis->GetCubeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoScaleAxis);
}

void mafGizmoScaleAxisTest::TestSetRefSysMatrix()
{
  mafMatrix *refSysMatrix = mafMatrix::New();

  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetRefSysMatrix(refSysMatrix);
  
  // using friend mafGizmoScaleAxis
  for (int i = 0; i < 2; i++)
  {
    mafMatrix *mat = gizmoScaleAxis->m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == refSysMatrix);
  }

  cppDEL(gizmoScaleAxis);
}


void mafGizmoScaleAxisTest::TestSetListener()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetListener(gizmoScaleAxis);
  
  // using friend mafGizmoScaleAxis
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Listener == gizmoScaleAxis);

  cppDEL(gizmoScaleAxis);
}

void mafGizmoScaleAxisTest::TestSetGetInput()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);

  gizmoScaleAxis->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoScaleAxis
  CPPUNIT_ASSERT(gizmoScaleAxis->m_InputVme == m_GizmoInputSurface);

  CPPUNIT_ASSERT(gizmoScaleAxis->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoScaleAxis);
}


void mafGizmoScaleAxisTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface, listener);
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Show == false);

  gizmoScaleAxis->SetListener(listener);
   
  gizmoScaleAxis->Show(true);
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Show == true);

  cppDEL(gizmoScaleAxis);
  cppDEL(listener);
}

void mafGizmoScaleAxisTest::TestSetGetAbsPose()
{
  mafMatrix *absPose = mafMatrix::New();

  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);
  gizmoScaleAxis->SetAbsPose(absPose);
  
  mafMatrix *gipo = gizmoScaleAxis->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(absPose));
 
  cppDEL(gizmoScaleAxis);
}


void mafGizmoScaleAxisTest::TestFixture()
{
	
}

void mafGizmoScaleAxisTest::RenderData( vtkDataSet *data )
{
  vtkDataSetMapper *mapper = vtkDataSetMapper::New();
  mapper->ScalarVisibilityOn();
  mapper->SetInput(data);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  m_Renderer->AddActor(actor);
  
  m_RenderWindow->Render();
  
  mapper->Delete();
  actor->Delete();

} 
  

void mafGizmoScaleAxisTest::CreateRenderStuff()
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

void mafGizmoScaleAxisTest::RenderGizmo( mafGizmoScaleAxis * gizmoScaleAxis )
{
  RenderData(gizmoScaleAxis->m_CylGizmo->GetOutput()->GetVTKData());
  RenderData(gizmoScaleAxis->m_CubeGizmo->GetOutput()->GetVTKData());
}

void mafGizmoScaleAxisTest::TestHighlight()
{
  mafGizmoScaleAxis *gizmoScaleAxis = new mafGizmoScaleAxis(m_GizmoInputSurface);
  
  CPPUNIT_ASSERT(gizmoScaleAxis->m_Highlight == false);
  
  gizmoScaleAxis->Highlight(true);

  CPPUNIT_ASSERT(gizmoScaleAxis->m_Highlight == true);
  
  cppDEL(gizmoScaleAxis);
}


