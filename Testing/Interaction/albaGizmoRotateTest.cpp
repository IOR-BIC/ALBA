/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateTest
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

#include "albaGizmoRotateTest.h"
#include "albaInteractionTests.h"


#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoRotate.h"
#include "albaGizmoRotateCircle.h"
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
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"

void albaGizmoRotateTest::BeforeTest()
{
  CreateTestData();
}

void albaGizmoRotateTest::AfterTest()
{
   albaDEL(m_GizmoInputSurface);
   albaDEL(m_Root); 
}

void albaGizmoRotateTest::CreateTestData()
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

void albaGizmoRotateTest::TestConstructor()
{
  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface, NULL,false);
  cppDEL(gizmoRotate);
}


void albaGizmoRotateTest::TestSetGetListener()
{
  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);

  gizmoRotate->SetListener(gizmoRotate);
  
  // using friend albaGizmoRotate
  CPPUNIT_ASSERT(gizmoRotate->m_Listener == gizmoRotate);

  cppDEL(gizmoRotate);
}

void albaGizmoRotateTest::TestSetGetInput()
{
  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);
  gizmoRotate->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoRotate
  CPPUNIT_ASSERT(gizmoRotate->m_InputVME == m_GizmoInputSurface);
  
  CPPUNIT_ASSERT(gizmoRotate->GetInput() == m_GizmoInputSurface);
  cppDEL(gizmoRotate);
}


void albaGizmoRotateTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);
  gizmoRotate->SetListener(listener);
  gizmoRotate->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoRotate);

  cppDEL(gizmoRotate);
}

void albaGizmoRotateTest::TestSetGetAbsPose()
{
  albaMatrix matrix;
  matrix.SetElement(0,3,10);
  matrix.SetElement(1,3,20);
  matrix.SetElement(2,3,30);

  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);
  gizmoRotate->SetAbsPose(&matrix);
  
  albaMatrix *absPose = gizmoRotate->GetAbsPose();
  CPPUNIT_ASSERT(absPose->Equals(&matrix));
 
  cppDEL(gizmoRotate);
}

void albaGizmoRotateTest::TestSetGetRefSys()
{
  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);
  
  gizmoRotate->SetRefSys(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoRotate->GetRefSys() == m_GizmoInputSurface);

  cppDEL(gizmoRotate);
}


void albaGizmoRotateTest::TestFixture()
{
	
}

void albaGizmoRotateTest::TestSetGetCircleFanRadius()
{
  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);

  gizmoRotate->SetCircleFanRadius(15);
  CPPUNIT_ASSERT(gizmoRotate->GetCircleFanRadius() == 15);

  cppDEL(gizmoRotate);  
}

void albaGizmoRotateTest::TestGetInteractor()
{
  albaGizmoRotate *gizmoRotate = new albaGizmoRotate(m_GizmoInputSurface,NULL,false);

  for (int axis = albaInteractorConstraint::AXIS::X; axis < albaInteractorConstraint::AXIS::NUM_AXES; axis++) 
  { 
    albaInteractorGenericInterface *interactor = gizmoRotate->GetInteractor(axis);
    
    // using test friend
    CPPUNIT_ASSERT(interactor == gizmoRotate->m_GRCircle[axis]->GetInteractor());
  }
  
  cppDEL(gizmoRotate);  
}
  



