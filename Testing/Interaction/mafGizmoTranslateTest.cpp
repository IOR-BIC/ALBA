/*=========================================================================

 Program: MAF2
 Module: mafGizmoTranslateTest
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

#include "mafGizmoTranslateTest.h"
#include "mafInteractionTests.h"


#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoTranslate.h"
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

void mafGizmoTranslateTest::setUp()
{
  CreateTestData();
}

void mafGizmoTranslateTest::tearDown()
{
   mafDEL(m_GizmoInputSurface);
   mafDEL(m_Root); 
}

void mafGizmoTranslateTest::CreateTestData()
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
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  mafNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}

void mafGizmoTranslateTest::TestConstructor()
{
  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface, NULL,false);
  cppDEL(gizmoTranslate);
}


void mafGizmoTranslateTest::TestSetGetConstraintModality()
{
  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);
  
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE);

  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::X, mafInteractorConstraint::LOCK);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Y, mafInteractorConstraint::LOCK);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Z, mafInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK);

  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::X, mafInteractorConstraint::BOUNDS);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Y, mafInteractorConstraint::BOUNDS);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Z, mafInteractorConstraint::BOUNDS);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::BOUNDS);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::BOUNDS);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::BOUNDS);

  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::X, mafInteractorConstraint::SNAP_STEP);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Y, mafInteractorConstraint::SNAP_STEP);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Z, mafInteractorConstraint::SNAP_STEP);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::SNAP_STEP);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::SNAP_STEP);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::SNAP_STEP);

  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::X, mafInteractorConstraint::SNAP_ARRAY);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Y, mafInteractorConstraint::SNAP_ARRAY);
  gizmoTranslate->SetConstraintModality(mafInteractorConstraint::Z, mafInteractorConstraint::SNAP_ARRAY);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::SNAP_ARRAY);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::SNAP_ARRAY);
  CPPUNIT_ASSERT(gizmoTranslate->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::SNAP_ARRAY);

  cppDEL(gizmoTranslate);
}

void mafGizmoTranslateTest::TestSetListener()
{
  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);

  gizmoTranslate->SetListener(gizmoTranslate);
  
  // using friend mafGizmoTranslate
  CPPUNIT_ASSERT(gizmoTranslate->m_Listener == gizmoTranslate);

  cppDEL(gizmoTranslate);
}

void mafGizmoTranslateTest::TestSetInput()
{
  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);

  gizmoTranslate->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoTranslate
  CPPUNIT_ASSERT(gizmoTranslate->m_InputVME == m_GizmoInputSurface);

  cppDEL(gizmoTranslate);
}


void mafGizmoTranslateTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);
  gizmoTranslate->SetListener(listener);
 
  gizmoTranslate->Show(true);
  
  CPPUNIT_ASSERT(listener->GetEvent()->GetId() == VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoTranslate);

  cppDEL(gizmoTranslate);
}

void mafGizmoTranslateTest::TestSetGetAbsPose()
{
  mafMatrix matrix;
  matrix.SetElement(0,3,10);
  matrix.SetElement(1,3,20);
  matrix.SetElement(2,3,30);

  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);
  gizmoTranslate->SetAbsPose(&matrix);
  
  mafMatrix *absPose = gizmoTranslate->GetAbsPose();
  CPPUNIT_ASSERT(absPose->Equals(&matrix));
 
  cppDEL(gizmoTranslate);
}

void mafGizmoTranslateTest::TestSetGetRefSys()
{
  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);
  
  gizmoTranslate->SetRefSys(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoTranslate->GetRefSys() == m_GizmoInputSurface);

  cppDEL(gizmoTranslate);
}

void mafGizmoTranslateTest::TestSetGetStep()
{
  mafMatrix absPose;
  double position[3] = {5,0,0};
  mafTransform::SetPosition(absPose, position);

  mafGizmoTranslate *gizmoTranslate = new mafGizmoTranslate(m_GizmoInputSurface,NULL,false);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(mafInteractorConstraint::X) == 1);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(mafInteractorConstraint::Y) == 1);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(mafInteractorConstraint::Z) == 1);
  
  gizmoTranslate->SetStep(mafInteractorConstraint::Y, 2);
  CPPUNIT_ASSERT(gizmoTranslate->GetStep(mafInteractorConstraint::Y) == 2);

  cppDEL(gizmoTranslate);
}

void mafGizmoTranslateTest::TestFixture()
{
	
}

  



