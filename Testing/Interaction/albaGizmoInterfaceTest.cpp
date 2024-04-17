/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInterfaceTest
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

#include "albaGizmoInterfaceTest.h"
#include "albaInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoInterface.h"
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


void albaGizmoInterfaceTest::BeforeTest()
{
  CreateTestData();
}

void albaGizmoInterfaceTest::AfterTest()
{
   albaDEL(m_GizmoInputSurface);
   albaDEL(m_Root); 
}

void albaGizmoInterfaceTest::CreateTestData()
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

void albaGizmoInterfaceTest::TestConstructor()
{
  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();
  cppDEL(gizmoInterface);
}


void albaGizmoInterfaceTest::TestSetGetListener()
{
  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();

  CPPUNIT_ASSERT(gizmoInterface->GetListener() == NULL);

  gizmoInterface->SetListener(gizmoInterface);
  CPPUNIT_ASSERT(gizmoInterface->GetListener() == gizmoInterface);

  cppDEL(gizmoInterface);
}

void albaGizmoInterfaceTest::TestSetGetInput()
{
  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();

  CPPUNIT_ASSERT(gizmoInterface->GetInput() == NULL);

  gizmoInterface->SetInput(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoInterface->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoInterface);
}


void albaGizmoInterfaceTest::TestShow()
{
  mockListener *listener = new mockListener();

  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();
  gizmoInterface->SetListener(listener);
  gizmoInterface->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoInterface);

  cppDEL(gizmoInterface);
}

void albaGizmoInterfaceTest::TestSetGetAbsPose()
{
  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();
  CPPUNIT_ASSERT(gizmoInterface->GetAbsPose() == NULL);
  cppDEL(gizmoInterface);
}

void albaGizmoInterfaceTest::TestSetGetModality()
{
  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();
  CPPUNIT_ASSERT(gizmoInterface->GetModality() == albaGizmoInterface::G_LOCAL);

  gizmoInterface->SetModalityToGlobal();
  CPPUNIT_ASSERT(gizmoInterface->GetModality() == albaGizmoInterface::G_GLOBAL);

  cppDEL(gizmoInterface);
}

void albaGizmoInterfaceTest::TestGetGui()
{
  albaGizmoInterface *gizmoInterface = new albaGizmoInterface();
  CPPUNIT_ASSERT(gizmoInterface->GetGui() == NULL);

  cppDEL(gizmoInterface);  
}

void albaGizmoInterfaceTest::TestFixture()
{

}
  



