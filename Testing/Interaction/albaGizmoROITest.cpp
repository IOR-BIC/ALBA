/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoROITest
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

#include "albaGizmoROITest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaString.h"
#include "albaRefSys.h"
#include "albaGizmoROI.h"
#include "albaVMERoot.h"
#include "albaSmartPointer.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMEGizmo.h"
#include "albaAbsMatrixPipe.h"
#include "albaObserver.h"
#include "albaTransform.h"

#include "albaInteractorGenericMouse.h"
#include "mmaMaterial.h"

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
#include "vtkProperty.h"

void albaGizmoROITest::BeforeTest()
{
  InitializeRenderWindow();
	CreateTestData();
}

void albaGizmoROITest::AfterTest()
{
  albaDEL(m_GizmoInputSurface);
  albaDEL(m_Root); 
}

void albaGizmoROITest::CreateTestData()
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

void albaGizmoROITest::TestConstructor()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  
  RenderData(m_GizmoInputSurface->GetOutput()->GetVTKData());
  RenderGizmo(gizmoROI);
  cppDEL(gizmoROI);

}


void albaGizmoROITest::TestSetListener()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);

  gizmoROI->SetListener(gizmoROI);
  
  // using friend albaGizmoROI
  CPPUNIT_ASSERT(gizmoROI->m_Listener == gizmoROI);

  gizmoROI->SetListener(NULL);
  CPPUNIT_ASSERT(gizmoROI->m_Listener == NULL);

  cppDEL(gizmoROI);
}

void albaGizmoROITest::TestSetInput()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);

  gizmoROI->SetInput(m_GizmoInputSurface);

  // using friend albaGizmoROI
  CPPUNIT_ASSERT(gizmoROI->GetInput() == m_GizmoInputSurface);

  cppDEL(gizmoROI);
}



void albaGizmoROITest::TestFixture()
{
	
}
  

void albaGizmoROITest::RenderGizmo( albaGizmoROI *gizmoROI )
{
  RenderData(gizmoROI->m_OutlineGizmo->m_BoxGizmo->GetOutput()->GetVTKData());

  for (int i = 0; i < 6; i++)
  {
    RenderData(gizmoROI->m_GHandle[i]->m_BoxGizmo->GetOutput()->GetVTKData());
  }
}

void albaGizmoROITest::TestSetGetBounds()
{
 
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  
  double inBounds[6] = {0,2.5,0,2.5,0,2.5};
  gizmoROI->SetBounds(inBounds);

  double outBounds[6] = {0,0,0,0,0,0};
  gizmoROI->GetBounds(outBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(inBounds[var] == outBounds[var]);
  }

  cppDEL(gizmoROI);
}



void albaGizmoROITest::TestHighlight()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  for (int component = 0; component < 6; component++) 
  { 
    gizmoROI->Highlight(component);
  } 
  
  // TODO: REFACTOR THIS 
  // this is not easy to test since I need to retrieve the component highlight  status.
  // ivars for status could be added

  cppDEL(gizmoROI);

}

void albaGizmoROITest::TestHighlightOff()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  for (int component = 0; component < 6; component++) 
  { 
    gizmoROI->Highlight(component);
  } 
  gizmoROI->HighlightOff();

  // TODO: REFACTOR THIS 
  // this is not easy to test since I need to retrieve the component highlight  status.
  // ivars for status could be added

  cppDEL(gizmoROI);
  
}


void albaGizmoROITest::TestSetConstrainRefSys()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);

  albaMatrix refSys;
  refSys.SetElement(0,3,5);
  refSys.SetElement(1,3,5);
  refSys.SetElement(2,3,5);
  refSys.SetElement(0,3,5);

  gizmoROI->SetConstrainRefSys(&refSys);

  for (int i =0; i < 6; i++)
  {
    albaMatrix *mat = gizmoROI->m_GHandle[i]->m_IsaGen->GetTranslationConstraint()->GetRefSys()->GetMatrix();
    CPPUNIT_ASSERT(mat == &refSys);
  }

  cppDEL(gizmoROI);
}

void albaGizmoROITest::TestReset()
{
  double oldBounds[6] = {0,0,0,0,0,0};
  m_GizmoInputSurface->GetOutput()->GetBounds(oldBounds);

  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  
  double newBounds[6] = {0,1,0,1,0,1};
  gizmoROI->SetBounds(newBounds);
  
  gizmoROI->Reset();

  double getBounds[6] = {0,0,0,0,0,0};
  gizmoROI->GetBounds(getBounds);

  for (int var = 0; var < 6; var++) 
  { 
    CPPUNIT_ASSERT(getBounds[var] == oldBounds[var]);
  }

  cppDEL(gizmoROI);

}

void albaGizmoROITest::TestShow()
{

  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  gizmoROI->Show(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}



void albaGizmoROITest::TestShowHandles()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  gizmoROI->ShowHandles(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

void albaGizmoROITest::TestShowShadingPlane()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  gizmoROI->ShowShadingPlane(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}

void albaGizmoROITest::TestShowROI()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  gizmoROI->ShowROI(true);
  cppDEL(gizmoROI);
  // TODO: REFACTOR THIS 
  // this is not easy to test since I cannot access actors visibility
  // could add ivars to store visibility status
}


void albaGizmoROITest::TestEnableMinimumHandleSize()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  gizmoROI->EnableMinimumHandleSize(true);
  CPPUNIT_ASSERT(gizmoROI->m_EnableMinimumHandleSize == true);
  cppDEL(gizmoROI);
}

void albaGizmoROITest::TestSetMinimumHandleSize()
{
  albaGizmoROI *gizmoROI = new albaGizmoROI(m_GizmoInputSurface);
  gizmoROI->SetMinimumHandleSize(5);
  CPPUNIT_ASSERT(gizmoROI->m_MinimumHandleSize == 5);
  cppDEL(gizmoROI);
}
