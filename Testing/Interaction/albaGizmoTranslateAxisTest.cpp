/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslateAxisTest
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
#include "albaInteractionTests.h"

#include "albaGizmoTranslateAxisTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaRefSys.h"
#include "albaString.h"
#include "albaGizmoTranslateAxis.h"
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
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"



void albaGizmoTranslateAxisTest::BeforeTest()
{
  InitializeRenderWindow();
	CreateTestData();
}

void albaGizmoTranslateAxisTest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoTranslateAxisTest::CreateTestData()
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
  tube->SetInput(axes->GetOutput());
  tube->SetRadius(0.5);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoTranslateAxisTest::TestConstructor()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetAxis(albaInteractorConstraint::X);

  gizmoTranslateAxis->SetCylinderLength(5);
  gizmoTranslateAxis->SetConeLength(5);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoTranslateAxis);

  gizmoTranslateAxis->SetAxis(albaInteractorConstraint::Y);
  RenderGizmo(gizmoTranslateAxis);

  gizmoTranslateAxis->SetAxis(albaInteractorConstraint::Z);
  RenderGizmo(gizmoTranslateAxis);

  cppDEL(gizmoTranslateAxis);

}

void albaGizmoTranslateAxisTest::TestSetGetAxis()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);
   
  int axis = gizmoTranslateAxis->GetAxis();

  CPPUNIT_ASSERT( axis == albaGizmoTranslateAxis::AXIS::X);

  gizmoTranslateAxis->SetAxis(albaGizmoTranslateAxis::AXIS::Y);
  axis = gizmoTranslateAxis->GetAxis();
  CPPUNIT_ASSERT(axis == albaGizmoTranslateAxis::AXIS::Y);

  gizmoTranslateAxis->SetAxis(albaGizmoTranslateAxis::AXIS::Z);
  axis = gizmoTranslateAxis->GetAxis();
  CPPUNIT_ASSERT(axis == albaGizmoTranslateAxis::AXIS::Z);

  cppDEL(gizmoTranslateAxis);
}


void albaGizmoTranslateAxisTest::TestSetGetConeLength()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);
  
  gizmoTranslateAxis->SetConeLength(10);
  double coneLength = gizmoTranslateAxis->GetConeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetGetCylinderLength()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetCylinderLength(10);
  double coneLength = gizmoTranslateAxis->GetCylinderLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetRefSysMatrix()
{
  albaMatrix refSys;

  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetRefSysMatrix(&refSys);
  
  // using friend albaGizmoTranslateAxis
  for (int i = 0; i < 2; i++)
  {
    albaMatrix *mat = gizmoTranslateAxis->m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == &refSys);
  }

  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetConstraintModality()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetAxis(albaInteractorConstraint::X);
  gizmoTranslateAxis->SetConstraintModality(albaInteractorConstraint::X, albaInteractorConstraint::LOCK);
  
  // using friend albaGizmoTranslateAxis
  for (int i = 0; i < 2; i++)
  {
    int constrainModality = gizmoTranslateAxis->m_IsaGen[i]->GetTranslationConstraint()->GetConstraintModality(albaInteractorConstraint::X);
    CPPUNIT_ASSERT(constrainModality  ==  albaInteractorConstraint::LOCK);
  } 

  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetListener()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetListener(gizmoTranslateAxis);
  
  // using friend albaGizmoTranslateAxis
  CPPUNIT_ASSERT(gizmoTranslateAxis->m_Listener == gizmoTranslateAxis);

  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetInput()
{
  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);

  gizmoTranslateAxis->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoTranslateAxis
  CPPUNIT_ASSERT(gizmoTranslateAxis->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoTranslateAxis);
}


void albaGizmoTranslateAxisTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetListener(listener);
 
  gizmoTranslateAxis->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoTranslateAxis);

  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetGetAbsPose()
{
  albaMatrix absPose;

  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetAbsPose(&absPose);
  
  albaMatrix *gipo = gizmoTranslateAxis->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestSetStep()
{
  albaMatrix absPose;
  double position[3] = {5,0,0};
  albaTransform::SetPosition(absPose, position);

  albaGizmoTranslateAxis *gizmoTranslateAxis = new albaGizmoTranslateAxis(m_GizmoInputSurface);
  gizmoTranslateAxis->SetStep(albaInteractorConstraint::Y, 2);

  for (int i = 0; i < 2; i++)
  {
    CPPUNIT_ASSERT(
      gizmoTranslateAxis->m_IsaGen[i]->GetTranslationConstraint()->GetStep(albaInteractorConstraint::Y)
      == 2);
  }

  cppDEL(gizmoTranslateAxis);
}

void albaGizmoTranslateAxisTest::TestFixture()
{
	
}

void albaGizmoTranslateAxisTest::RenderGizmo( albaGizmoTranslateAxis * gizmoTranslateAxis )
{
  RenderData(gizmoTranslateAxis->m_CylGizmo->GetOutput()->GetVTKData());
  RenderData(gizmoTranslateAxis->m_ConeGizmo->GetOutput()->GetVTKData());
}

