/*=========================================================================

 Program: MAF2
 Module: mafGizmoRotateTest
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

#include "mafGizmoRotateTest.h"
#include "mafInteractionTests.h"


#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoRotate.h"
#include "mafGizmoRotateCircle.h"
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

void mafGizmoRotateTest::BeforeTest()
{
  CreateTestData();
}

void mafGizmoRotateTest::AfterTest()
{
   mafDEL(m_GizmoInputSurface);
   mafDEL(m_Root); 
}

void mafGizmoRotateTest::CreateTestData()
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

void mafGizmoRotateTest::TestConstructor()
{
  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface, NULL,false);
  cppDEL(gizmoRotate);
}


void mafGizmoRotateTest::TestSetGetListener()
{
  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);

  gizmoRotate->SetListener(gizmoRotate);
  
  // using friend mafGizmoRotate
  CPPUNIT_ASSERT(gizmoRotate->m_Listener == gizmoRotate);

  cppDEL(gizmoRotate);
}

void mafGizmoRotateTest::TestSetGetInput()
{
  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);
  gizmoRotate->SetInput(m_GizmoInputSurface);

  // using friend mafGizmoRotate
  CPPUNIT_ASSERT(gizmoRotate->m_InputVME == m_GizmoInputSurface);
  
  CPPUNIT_ASSERT(gizmoRotate->GetInput() == m_GizmoInputSurface);
  cppDEL(gizmoRotate);
}


void mafGizmoRotateTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);
  gizmoRotate->SetListener(listener);
  gizmoRotate->Show(true);
  
  CPPUNIT_ASSERT(listener->GetEvent()->GetId() == VME_SHOW);
  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoRotate);

  cppDEL(gizmoRotate);
}

void mafGizmoRotateTest::TestSetGetAbsPose()
{
  mafMatrix matrix;
  matrix.SetElement(0,3,10);
  matrix.SetElement(1,3,20);
  matrix.SetElement(2,3,30);

  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);
  gizmoRotate->SetAbsPose(&matrix);
  
  mafMatrix *absPose = gizmoRotate->GetAbsPose();
  CPPUNIT_ASSERT(absPose->Equals(&matrix));
 
  cppDEL(gizmoRotate);
}

void mafGizmoRotateTest::TestSetGetRefSys()
{
  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);
  
  gizmoRotate->SetRefSys(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoRotate->GetRefSys() == m_GizmoInputSurface);

  cppDEL(gizmoRotate);
}


void mafGizmoRotateTest::TestFixture()
{
	
}

void mafGizmoRotateTest::TestSetGetCircleFanRadius()
{
  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);

  gizmoRotate->SetCircleFanRadius(15);
  CPPUNIT_ASSERT(gizmoRotate->GetCircleFanRadius() == 15);

  cppDEL(gizmoRotate);  
}

void mafGizmoRotateTest::TestGetInteractor()
{
  mafGizmoRotate *gizmoRotate = new mafGizmoRotate(m_GizmoInputSurface,NULL,false);

  for (int axis = mafInteractorConstraint::AXIS::X; axis < mafInteractorConstraint::AXIS::NUM_AXES; axis++) 
  { 
    mafInteractorGenericInterface *interactor = gizmoRotate->GetInteractor(axis);
    
    // using test friend
    CPPUNIT_ASSERT(interactor == gizmoRotate->m_GRCircle[axis]->GetInteractor());
  }
  
  cppDEL(gizmoRotate);  
}
  



