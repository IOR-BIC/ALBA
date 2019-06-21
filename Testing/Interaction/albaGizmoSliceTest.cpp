/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoSliceTest
 Authors: Roberto Mucci
 
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

#include "albaGizmoSliceTest.h"
#include "albaInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaGizmoSlice.h"
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

//----------------------------------------------------------------------------
void albaGizmoSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  CreateTestData();
}

//----------------------------------------------------------------------------
void albaGizmoSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
   albaDEL(m_GizmoInputSurface);
   albaDEL(m_Root); 
}
//----------------------------------------------------------------------------
void albaGizmoSliceTest::CreateTestData()
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
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  albaNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}
//----------------------------------------------------------------------------
void albaGizmoSliceTest::TestConstructor()
//----------------------------------------------------------------------------
{
  albaGizmoSlice *gizmoSlice = new albaGizmoSlice(m_GizmoInputSurface);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void albaGizmoSliceTest::TestShow()
//----------------------------------------------------------------------------
{
  mockListener *listener = new mockListener();

  albaGizmoSlice *gizmoSlice = new albaGizmoSlice(m_GizmoInputSurface);
  gizmoSlice->SetListener(listener);
  gizmoSlice->Show(true);
  
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoSlice);

  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void albaGizmoSliceTest::TestSetGetAbsPose()
//----------------------------------------------------------------------------
{
  albaGizmoSlice *gizmoSlice = new albaGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetAbsPose() == NULL);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void albaGizmoSliceTest::TestSetGetModality()
//----------------------------------------------------------------------------
{
  albaGizmoSlice *gizmoSlice = new albaGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetModality() == albaGizmoSlice::G_LOCAL);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void albaGizmoSliceTest::TestSetGetMovingModality()
//----------------------------------------------------------------------------
{
  albaGizmoSlice *gizmoSlice = new albaGizmoSlice(m_GizmoInputSurface);
  gizmoSlice->SetGizmoMovingModalityToBound();
  CPPUNIT_ASSERT(gizmoSlice->GetGizmoMovingModality() == 0); //albaGizmoSlice::BOUND

  gizmoSlice->SetGizmoMovingModalityToSnap();
  CPPUNIT_ASSERT(gizmoSlice->GetGizmoMovingModality() == 1); //albaGizmoSlice::SNAP
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void albaGizmoSliceTest::TestGetGui()
//----------------------------------------------------------------------------
{
  albaGizmoSlice *gizmoSlice = new albaGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetGui() == NULL);

  cppDEL(gizmoSlice);  
}

void albaGizmoSliceTest::TestFixture()
{

}
  



