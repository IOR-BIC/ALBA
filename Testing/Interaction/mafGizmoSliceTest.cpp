/*=========================================================================

 Program: MAF2
 Module: mafGizmoSliceTest
 Authors: Roberto Mucci
 
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

#include "mafGizmoSliceTest.h"
#include "mafInteractionTests.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafString.h"
#include "mafGizmoSlice.h"
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

//----------------------------------------------------------------------------
void mafGizmoSliceTest::BeforeTest()
//----------------------------------------------------------------------------
{
  CreateTestData();
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::AfterTest()
//----------------------------------------------------------------------------
{
   mafDEL(m_GizmoInputSurface);
   mafDEL(m_Root); 
}
//----------------------------------------------------------------------------
void mafGizmoSliceTest::CreateTestData()
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
  tube->SetRadius(0.1);
  tube->SetNumberOfSides(20);
  
  m_GizmoInputSurface->SetData(tube->GetOutput(),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  
  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);


  mafNEW(m_Root);
  m_GizmoInputSurface->ReparentTo(m_Root);

  CPPUNIT_ASSERT(m_GizmoInputSurface->GetOutput()->GetVTKData() != NULL);
}
//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestConstructor()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestShow()
//----------------------------------------------------------------------------
{
  mockListener *listener = new mockListener();

  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  gizmoSlice->SetListener(listener);
  gizmoSlice->Show(true);
  
	mafFakeLogicForTest *logic = (mafFakeLogicForTest*)mafServiceLocator::GetLogicManager();
	logic->ClearCalls();
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == mafFakeLogicForTest::FKL_VME_SHOW);

  CPPUNIT_ASSERT(listener->GetEvent()->GetSender() == gizmoSlice);

  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestSetGetAbsPose()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetAbsPose() == NULL);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestSetGetModality()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetModality() == mafGizmoSlice::G_LOCAL);
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestSetGetMovingModality()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  gizmoSlice->SetGizmoMovingModalityToBound();
  CPPUNIT_ASSERT(gizmoSlice->GetGizmoMovingModality() == 0); //mafGizmoSlice::BOUND

  gizmoSlice->SetGizmoMovingModalityToSnap();
  CPPUNIT_ASSERT(gizmoSlice->GetGizmoMovingModality() == 1); //mafGizmoSlice::SNAP
  cppDEL(gizmoSlice);
}

//----------------------------------------------------------------------------
void mafGizmoSliceTest::TestGetGui()
//----------------------------------------------------------------------------
{
  mafGizmoSlice *gizmoSlice = new mafGizmoSlice(m_GizmoInputSurface);
  CPPUNIT_ASSERT(gizmoSlice->GetGui() == NULL);

  cppDEL(gizmoSlice);  
}

void mafGizmoSliceTest::TestFixture()
{

}
  



