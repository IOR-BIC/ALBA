/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslatePlaneTest
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

#include "albaGizmoTranslatePlaneTest.h"
#include "albaInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaRefSys.h"
#include "albaGizmoTranslatePlane.h"
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
#include "vtkLineSource.h"
#include "vtkPlaneSource.h"
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"

void albaGizmoTranslatePlaneTest::BeforeTest()
{
  InitializeRenderWindow();
	CreateTestData();
}

void albaGizmoTranslatePlaneTest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoTranslatePlaneTest::CreateTestData()
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
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
	tube->Update();
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void albaGizmoTranslatePlaneTest::TestConstructor()
{
  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::XY);

  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  
  RenderGizmo(gizmoTranslatePlane);

  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::XZ);
  RenderGizmo(gizmoTranslatePlane);
  
  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::YZ);
  RenderGizmo(gizmoTranslatePlane);

  cppDEL(gizmoTranslatePlane);

}

void albaGizmoTranslatePlaneTest::TestSetGetPlane()
{
  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);
   
  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::XY);
  int axis = gizmoTranslatePlane->GetPlane();

  CPPUNIT_ASSERT( axis == albaGizmoTranslatePlane::XY);

  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::XZ);
  axis = gizmoTranslatePlane->GetPlane();

  CPPUNIT_ASSERT(axis == albaGizmoTranslatePlane::XZ);

  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::YZ);
  axis = gizmoTranslatePlane->GetPlane();
  CPPUNIT_ASSERT(axis == albaGizmoTranslatePlane::YZ);

  cppDEL(gizmoTranslatePlane);
}


void albaGizmoTranslatePlaneTest::TestSetGetLength()
{
  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);
  
  gizmoTranslatePlane->SetSizeLength(10);
  double coneLength = gizmoTranslatePlane->GetSizeLength();
  CPPUNIT_ASSERT(coneLength == 10);
  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestSetConstrainRefSys()
{
  albaMatrix refSys;

  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetConstrainRefSys(&refSys);
  
  // using friend albaGizmoTranslatePlane
  for (int i = 0; i < 2; i++)
  {
    albaMatrix *mat = gizmoTranslatePlane->m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == &refSys);
  }

  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestSetConstraintModality()
{
  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetPlane(albaGizmoTranslatePlane::XY);
  gizmoTranslatePlane->SetConstraintModality(albaGizmoTranslatePlane::XY, albaInteractorConstraint::LOCK);
  
  // using friend albaGizmoTranslatePlane
  for (int i = 0; i < 2; i++)
  {
    int constrainModality = gizmoTranslatePlane->m_IsaGen[i]->GetTranslationConstraint()->GetConstraintModality(albaGizmoTranslatePlane::XY);
    CPPUNIT_ASSERT(constrainModality  ==  albaInteractorConstraint::LOCK);
  } 

  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestSetListener()
{
  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetListener(gizmoTranslatePlane);
  
  // using friend albaGizmoTranslatePlane
  CPPUNIT_ASSERT(gizmoTranslatePlane->m_Listener == gizmoTranslatePlane);

  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestSetInput()
{
  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);

  gizmoTranslatePlane->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoTranslatePlane
  CPPUNIT_ASSERT(gizmoTranslatePlane->m_InputVme == m_GizmoInputSurface);

  cppDEL(gizmoTranslatePlane);
}


void albaGizmoTranslatePlaneTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetListener(listener);
 
  gizmoTranslatePlane->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoTranslatePlane);

  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestSetGetAbsPose()
{
  albaMatrix absPose;

  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetAbsPose(&absPose);
  
  albaMatrix *gipo = gizmoTranslatePlane->GetAbsPose();
  CPPUNIT_ASSERT(gipo->Equals(&absPose));
 
  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestSetStep()
{
  albaMatrix absPose;
  double position[3] = {5,0,0};
  albaTransform::SetPosition(absPose, position);

  albaGizmoTranslatePlane *gizmoTranslatePlane = new albaGizmoTranslatePlane(m_GizmoInputSurface);
  gizmoTranslatePlane->SetStep(albaInteractorConstraint::Y, 2);

  for (int i = 0; i < 2; i++)
  {
    CPPUNIT_ASSERT(
      gizmoTranslatePlane->m_IsaGen[i]->GetTranslationConstraint()->GetStep(albaInteractorConstraint::Y)
      == 2);
  }

  cppDEL(gizmoTranslatePlane);
}

void albaGizmoTranslatePlaneTest::TestFixture()
{
	
}


void albaGizmoTranslatePlaneTest::RenderGizmo( albaGizmoTranslatePlane * gizmoTranslatePlane )
{
  for (int i = 0; i < 3; i++) 
  { 
    RenderData(gizmoTranslatePlane->m_Gizmo[i]->GetOutput()->GetVTKData()); 
  }
}

