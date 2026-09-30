/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslateTest
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

#include "albaGizmoTranslateTest.h"
#include "albaInteractionTests.h"


#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoTranslate.h"
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
#include "albaFakeLogicForTest.h"
#include "albaServiceLocator.h"

void albaGizmoTranslateTest::BeforeTest()
{
  CreateTestData();
}

void albaGizmoTranslateTest::AfterTest()
{
   albaDEL(m_GizmoInputSurface);
   albaDEL(m_Root); 
}

void albaGizmoTranslateTest::CreateTestData()
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
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
	tube->Update();
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoTranslateTest::TestConstructor()
{
  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface, NULL,false);
  cppDEL(gizmoTranslate);
}


void albaGizmoTranslateTest::TestSetGetConstraintModality()
{
  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);
  
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE);

  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::X, albaInteractorConstraint::LOCK);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Y, albaInteractorConstraint::LOCK);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Z, albaInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK);

  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::X, albaInteractorConstraint::BOUNDS);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Y, albaInteractorConstraint::BOUNDS);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Z, albaInteractorConstraint::BOUNDS);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::BOUNDS);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::BOUNDS);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::BOUNDS);

  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::X, albaInteractorConstraint::SNAP_STEP);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Y, albaInteractorConstraint::SNAP_STEP);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Z, albaInteractorConstraint::SNAP_STEP);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::SNAP_STEP);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::SNAP_STEP);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::SNAP_STEP);

  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::X, albaInteractorConstraint::SNAP_ARRAY);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Y, albaInteractorConstraint::SNAP_ARRAY);
  gizmoTranslate->SetConstraintModality(albaInteractorConstraint::Z, albaInteractorConstraint::SNAP_ARRAY);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::SNAP_ARRAY);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::SNAP_ARRAY);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::SNAP_ARRAY);

  cppDEL(gizmoTranslate);
}

void albaGizmoTranslateTest::TestSetListener()
{
  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);

  gizmoTranslate->SetListener(gizmoTranslate);
  
  // using friend albaGizmoTranslate
  CPPUNIT_ASSERT(gizmoTranslate->m_Listener == gizmoTranslate);

  cppDEL(gizmoTranslate);
}

void albaGizmoTranslateTest::TestSetInput()
{
  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);

  gizmoTranslate->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoTranslate
  CPPUNIT_ASSERT(gizmoTranslate->m_InputVME == m_GizmoInputSurface);

  cppDEL(gizmoTranslate);
}


void albaGizmoTranslateTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);
  gizmoTranslate->SetListener(listener);
 
  gizmoTranslate->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoTranslate);

  cppDEL(gizmoTranslate);
}

void albaGizmoTranslateTest::TestSetGetAbsPose()
{
  albaMatrix matrix;
  matrix.SetElement(0,3,10);
  matrix.SetElement(1,3,20);
  matrix.SetElement(2,3,30);

  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);
  gizmoTranslate->SetAbsPose(&matrix);
  
  albaMatrix *absPose = gizmoTranslate->GetAbsPose();
  CPPUNIT_ASSERT(absPose->Equals(&matrix));
 
  cppDEL(gizmoTranslate);
}

void albaGizmoTranslateTest::TestSetGetRefSys()
{
  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);
  
  gizmoTranslate->SetRefSys(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoTranslate->GetRefSys() == m_GizmoInputSurface);

  cppDEL(gizmoTranslate);
}

void albaGizmoTranslateTest::TestSetGetStep()
{
  albaMatrix absPose;
  double position[3] = {5,0,0};
  albaTransform::SetPosition(absPose, position);

  albaGizmoTranslate *gizmoTranslate = new albaGizmoTranslate(m_GizmoInputSurface,NULL,false);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(albaInteractorConstraint::X) == 1);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(albaInteractorConstraint::Y) == 1);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(albaInteractorConstraint::Z) == 1);
  
  gizmoTranslate->SetStep(albaInteractorConstraint::Y, 2);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(albaInteractorConstraint::Y) == 2);

  cppDEL(gizmoTranslate);
}

void albaGizmoTranslateTest::TestFixture()
{
	
}

  



