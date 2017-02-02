/*=========================================================================

 Program: MAF2
 Module: mafGizmoInterfaceTest
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

#include "mafGizmoInterfaceTest.h"
#include "mafInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoInterface.h"
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
#include "mafServiceLocator.h"
#include "mafFakeLogicForTest.h"


void mafGizmoInterfaceTest::BeforeTest()
{
  CreateTestData();
}

void mafGizmoInterfaceTest::AfterTest()
{
   mafDEL(m_GizmoInputSurface);
   mafDEL(m_Root); 
}

void mafGizmoInterfaceTest::CreateTestData()
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

void mafGizmoInterfaceTest::TestConstructor()
{
  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();
  cppDEL(gizmoInterface);
}


void mafGizmoInterfaceTest::TestSetGetListener()
{
  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();

  CPPUNIT_ASSERT(gizmoInterface->GetListener() == NULL);

  gizmoInterface->SetListener(gizmoInterface);
  CPPUNIT_ASSERT(gizmoInterface->GetListener() == gizmoInterface);

  cppDEL(gizmoInterface);
}

void mafGizmoInterfaceTest::TestSetGetInput()
{
  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();

  CPPUNIT_ASSERT(gizmoInterface->GetInput() == NULL);

  gizmoInterface->SetInput(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoInterface->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoInterface);
}


void mafGizmoInterfaceTest::TestShow()
{
  mockListener *listener = new mockListener();

  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();
  gizmoInterface->SetListener(listener);
  gizmoInterface->Show(true);
  
	mafFakeLogicForTest *logic = (mafFakeLogicForTest*)mafServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == mafFakeLogicForTest::VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoInterface);

  cppDEL(gizmoInterface);
}

void mafGizmoInterfaceTest::TestSetGetAbsPose()
{
  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();
  CPPUNIT_ASSERT(gizmoInterface->GetAbsPose() == NULL);
  cppDEL(gizmoInterface);
}

void mafGizmoInterfaceTest::TestSetGetModality()
{
  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();
  CPPUNIT_ASSERT(gizmoInterface->GetModality() == mafGizmoInterface::G_LOCAL);

  gizmoInterface->SetModalityToGlobal();
  CPPUNIT_ASSERT(gizmoInterface->GetModality() == mafGizmoInterface::G_GLOBAL);

  cppDEL(gizmoInterface);
}

void mafGizmoInterfaceTest::TestGetGui()
{
  mafGizmoInterface *gizmoInterface = new mafGizmoInterface();
  CPPUNIT_ASSERT(gizmoInterface->GetGui() == NULL);

  cppDEL(gizmoInterface);  
}

void mafGizmoInterfaceTest::TestFixture()
{

}
  



