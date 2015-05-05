/*=========================================================================

 Program: MAF2
 Module: mafGizmoTranslateAxisTest
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
#include "mafInteractionTests.h"

#include "mafGizmoTranslateAxisTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafRefSys.h"
#include "mafString.h"
#include "mafGizmoTranslateAxis.h"
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



void mafGizmoTranslateAxisTest::setUp()
{
  CreateRenderStuff();
	CreateTestData();
}

void mafGizmoTranslateAxisTest::tearDown()
{
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  m_RenderWindowInteractor->Delete();

  mafDEL(m_GizmoInputSurface);
  mafDEL(m_Root); 
}

void mafGizmoTranslateAxisTest::CreateTestData()
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

void mafGizmoTranslateAxisTest::TestConstructor()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetAxis(mafInteractorConstraint::X);

  gizmoTranslateAxis->SetCylinderLength(5);
  gizmoTranslateAxis->SetConeLength(5);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoTranslateAxis);

  gizmoTranslateAxis->SetAxis(mafInteractorConstraint::Y);
  RenderGizmo(gizmoTranslateAxis);

  gizmoTranslateAxis->SetAxis(mafInteractorConstraint::Z);
  RenderGizmo(gizmoTranslateAxis);

  // m_RenderWindowInteractor->Start();

  cppDEL(gizmoTranslateAxis);

}

void mafGizmoTranslateAxisTest::TestSetGetAxis()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);
   
  int axis = gizmoTranslateAxis->GetAxis();

  CPPUNIT_ASSERT( axis == mafGizmoTranslateAxis::AXIS::X);

  gizmoTranslateAxis->SetAxis(mafGizmoTranslateAxis::AXIS::Y);
  axis = gizmoTranslateAxis->GetAxis();
  CPPUNIT_ASSERT(axis == mafGizmoTranslateAxis::AXIS::Y);

  gizmoTranslateAxis->SetAxis(mafGizmoTranslateAxis::AXIS::Z);
  axis = gizmoTranslateAxis->GetAxis();
  CPPUNIT_ASSERT(axis == mafGizmoTranslateAxis::AXIS::Z);

  cppDEL(gizmoTranslateAxis);
}


void mafGizmoTranslateAxisTest::TestSetGetConeLength()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);
  
  gizmoTranslateAxis->SetConeLength(10);
  double coneLength = gizmoTranslateAxis->GetConeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetGetCylinderLength()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetCylinderLength(10);
  double coneLength = gizmoTranslateAxis->GetCylinderLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetRefSysMatrix()
{
  mafMatrix refSys;

  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetRefSysMatrix(&refSys);
  
  // using friend mafGizmoTranslateAxis
  for (int i = 0; i < 2; i++)
  {
    mafMatrix *mat = gizmoTranslateAxis->m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == &refSys);
  }

  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetConstraintModality()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetAxis(mafInteractorConstraint::X);
  gizmoTranslateAxis->SetConstraintModality(mafInteractorConstraint::X, mafInteractorConstraint::LOCK);
  
  // using friend mafGizmoTranslateAxis
  for (int i = 0; i < 2; i++)
  {
    int constrainModality = gizmoTranslateAxis->m_IsaGen[i]->GetTranslationConstraint()->GetConstraintModality(mafInteractorConstraint::X);
    CPPUNIT_ASSERT(constrainModality  ==  mafInteractorConstraint::LOCK);
  } 

  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetListener()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetListener(gizmoTranslateAxis);
  
  // using friend mafGizmoTranslateAxis
  CPPUNIT_ASSERT(gizmoTranslateAxis->m_Listener == gizmoTranslateAxis);

  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetInput()
{
  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoTranslateAxis
  CPPUNIT_ASSERT(gizmoTranslateAxis->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoTranslateAxis);
}


void mafGizmoTranslateAxisTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetListener(listener);
 
  gizmoTranslateAxis->Show(true);
  
  CPPUNIT_ASSERT(listener->GetEvent()->GetId() == VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoTranslateAxis);

  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetGetAbsPose()
{
  mafMatrix absPose;

  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetAbsPose(&absPose);
  
  mafMatrix *gipo = gizmoTranslateAxis->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestSetStep()
{
  mafMatrix absPose;
  double position[3] = {5,0,0};
  mafTransform::SetPosition(absPose, position);

  mafGizmoTranslateAxis *gizmoTranslateAxis = new mafGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetStep(mafInteractorConstraint::Y, 2);

  for (int i = 0; i < 2; i++)
  {
    CPPUNIT_ASSERT(
      gizmoTranslateAxis->m_IsaGen[i]->GetTranslationConstraint()->GetStep(mafInteractorConstraint::Y)
      == 2);
  }

  cppDEL(gizmoTranslateAxis);
}

void mafGizmoTranslateAxisTest::TestFixture()
{
	
}

void mafGizmoTranslateAxisTest::RenderData( vtkDataSet *data )
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
  

void mafGizmoTranslateAxisTest::CreateRenderStuff()
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

void mafGizmoTranslateAxisTest::RenderGizmo( mafGizmoTranslateAxis * gizmoTranslateAxis )
{
  RenderData(gizmoTranslateAxis->m_CylGizmo->GetOutput()->GetVTKData());
  RenderData(gizmoTranslateAxis->m_ConeGizmo->GetOutput()->GetVTKData());
}

